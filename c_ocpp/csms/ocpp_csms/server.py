from __future__ import annotations

import asyncio

from ocpp_csms.config import SUPPORTED, CsmsConfig
from ocpp_csms.session import ChargePointSession
from ocpp_csms.store import ChargePointStore
from ocpp_csms.ws import handshake


async def serve(config: CsmsConfig, store: ChargePointStore | None = None) -> None:
    store = store or ChargePointStore()

    async def on_client(reader: asyncio.StreamReader, writer: asyncio.StreamWriter) -> None:
        peer = writer.get_extra_info("peername")
        try:
            ws = await handshake(reader, writer, SUPPORTED)
        except Exception as exc:  # noqa: BLE001
            print("[csms] handshake failed", peer, exc, flush=True)
            writer.close()
            return
        session = ChargePointSession(ws, store, config)
        await session.run()

    server = await asyncio.start_server(on_client, config.host, config.port)
    sockets = server.sockets or []
    bound = sockets[0].getsockname() if sockets else (config.host, config.port)
    print(f"[csms] listening ws://{bound[0]}:{bound[1]}/{{chargePointId}}", flush=True)
    print("[csms] subprotocols", ", ".join(SUPPORTED), flush=True)
    async with server:
        await server.serve_forever()


def run_server(config: CsmsConfig) -> None:
    asyncio.run(serve(config))
