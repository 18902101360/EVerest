from __future__ import annotations

import asyncio
from typing import Any

from ocpp_csms.catalog import from_csms, req_for
from ocpp_csms.config import DONE_VENDOR, CsmsConfig
from ocpp_csms.dispatch import Dispatcher
from ocpp_csms.outbound import reset_payload
from ocpp_csms import rpc
from ocpp_csms.store import ChargePointRecord, ChargePointStore
from ocpp_csms.ws import WsConnection


class ChargePointSession:
    def __init__(
        self,
        ws: WsConnection,
        store: ChargePointStore,
        config: CsmsConfig,
    ) -> None:
        self.ws = ws
        self.cp_id = ws.cp_id
        self.protocol = ws.protocol
        self.heartbeat_interval = config.heartbeat_interval
        self.config = config
        self.record: ChargePointRecord = store.upsert(self.cp_id, self.protocol)
        self.store = store
        self.dispatcher = Dispatcher(self.protocol)
        self._csms_seq = 1
        self._reset_sent = False
        self._probed = False
        self.probe_requested = False
        self._waiters: dict[str, asyncio.Event] = {}
        self._waiter_ok: dict[str, bool] = {}

    def log(self, *args: Any) -> None:
        if self.config.log_frames:
            print("[csms]", *args, flush=True)

    async def run(self) -> None:
        self.log("connected", self.cp_id, self.protocol)
        try:
            while True:
                text = await self.ws.recv_text()
                if text is None:
                    break
                if self.config.log_frames:
                    print("[csms rx]", self.cp_id, text[:240], flush=True)
                try:
                    msg = rpc.parse(text)
                except rpc.RpcError as exc:
                    self.log("bad rpc", exc)
                    continue
                if msg.msg_type == rpc.CALL:
                    await self._on_call(msg)
                elif msg.msg_type in (rpc.CALLRESULT, rpc.CALLERROR):
                    self._finish_waiter(msg.unique_id, ok=msg.msg_type == rpc.CALLRESULT)
                    self.log("reply", msg.unique_id, msg.error_code or "ok")
        finally:
            self.store.disconnect(self.cp_id)
            self.ws.close()
            self.log("disconnected", self.cp_id)

    def _finish_waiter(self, uid: str, ok: bool) -> None:
        self._waiter_ok[uid] = ok
        ev = self._waiters.get(uid)
        if ev is not None:
            ev.set()

    async def _on_call(self, msg: rpc.RpcMessage) -> None:
        action = msg.action or ""
        try:
            payload = self.dispatcher.handle(self, action, msg.payload or {})
            out = rpc.pack_result(msg.unique_id, payload)
        except Exception as exc:  # noqa: BLE001
            self.log("handler error", action, exc)
            out = rpc.pack_error(msg.unique_id, "InternalError", str(exc))
        if self.config.log_frames:
            print("[csms tx]", self.cp_id, out[:240], flush=True)
        await self.ws.send_text(out)
        if action == "BootNotification" and self.config.send_reset_after_boot and not self._reset_sent:
            self._reset_sent = True
            await self.send_call("Reset", reset_payload(self.protocol))
        if self.config.probe_all and self.probe_requested and not self._probed:
            self._probed = True
            self.probe_requested = False
            asyncio.create_task(self._probe_all_csms_calls())

    async def send_call(self, action: str, payload: dict) -> str:
        uid = f"csms-{self._csms_seq}"
        self._csms_seq += 1
        ev = asyncio.Event()
        self._waiters[uid] = ev
        frame = rpc.pack_call(uid, action, payload)
        if self.config.log_frames:
            print("[csms tx]", self.cp_id, frame[:240], flush=True)
        await self.ws.send_text(frame)
        return uid

    async def _probe_all_csms_calls(self) -> None:
        actions = from_csms(self.protocol)
        self.log("probe CSMS CALLs", self.protocol, len(actions))
        for action in actions:
            uid = await self.send_call(action, req_for(self.protocol, action))
            try:
                await asyncio.wait_for(self._waiters[uid].wait(), timeout=8)
            except asyncio.TimeoutError as exc:
                raise RuntimeError(f"no CALLRESULT for {action}") from exc
            if not self._waiter_ok.get(uid):
                raise RuntimeError(f"CallError for CSMS {action}")
        await self.send_call(
            "DataTransfer",
            {"vendorId": DONE_VENDOR, "messageId": "probe", "data": "ok"},
        )
        self.log("probe done", self.cp_id)
