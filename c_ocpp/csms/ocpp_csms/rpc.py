"""OCPP JSON-RPC over WebSocket: CALL=2, CALLRESULT=3, CALLERROR=4."""

from __future__ import annotations

import json
from dataclasses import dataclass
from typing import Any


CALL = 2
CALLRESULT = 3
CALLERROR = 4


@dataclass
class RpcMessage:
    msg_type: int
    unique_id: str
    action: str | None = None
    payload: Any = None
    error_code: str | None = None
    error_description: str = ""


class RpcError(ValueError):
    pass


def parse(text: str) -> RpcMessage:
    try:
        arr = json.loads(text)
    except json.JSONDecodeError as exc:
        raise RpcError("invalid json") from exc
    if not isinstance(arr, list) or len(arr) < 2:
        raise RpcError("not an OCPP array")
    msg_type = arr[0]
    uid = str(arr[1])
    if msg_type == CALL:
        if len(arr) < 4:
            raise RpcError("CALL needs action and payload")
        payload = arr[3] if arr[3] is not None else {}
        return RpcMessage(CALL, uid, action=str(arr[2]), payload=payload)
    if msg_type == CALLRESULT:
        payload = arr[2] if len(arr) > 2 and arr[2] is not None else {}
        return RpcMessage(CALLRESULT, uid, payload=payload)
    if msg_type == CALLERROR:
        code = str(arr[2]) if len(arr) > 2 else "GenericError"
        desc = str(arr[3]) if len(arr) > 3 else ""
        extra = arr[4] if len(arr) > 4 else {}
        return RpcMessage(CALLERROR, uid, payload=extra, error_code=code, error_description=desc)
    raise RpcError("unknown message type")


def pack_call(unique_id: str, action: str, payload: dict | None) -> str:
    return json.dumps([CALL, unique_id, action, payload or {}], separators=(",", ":"))


def pack_result(unique_id: str, payload: dict | None) -> str:
    return json.dumps([CALLRESULT, unique_id, payload or {}], separators=(",", ":"))


def pack_error(unique_id: str, code: str, description: str, details: dict | None = None) -> str:
    return json.dumps(
        [CALLERROR, unique_id, code, description or "", details or {}],
        separators=(",", ":"),
    )
