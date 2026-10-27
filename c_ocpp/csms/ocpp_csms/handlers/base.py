from __future__ import annotations

from typing import Any, Callable, Protocol

from ocpp_csms.store import ChargePointRecord


class SessionView(Protocol):
    cp_id: str
    protocol: str
    heartbeat_interval: int
    probe_requested: bool
    record: ChargePointRecord

    def log(self, *args: Any) -> None: ...


Handler = Callable[[SessionView, dict[str, Any]], dict[str, Any]]


def empty(_session: SessionView, _payload: dict[str, Any]) -> dict[str, Any]:
    return {}


def accepted_status(_session: SessionView, _payload: dict[str, Any]) -> dict[str, Any]:
    return {"status": "Accepted"}
