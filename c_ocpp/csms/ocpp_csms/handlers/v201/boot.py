from __future__ import annotations

from typing import Any

from ocpp_csms.clock import utc_now_iso
from ocpp_csms.handlers.base import SessionView


def boot(session: SessionView, payload: dict[str, Any]) -> dict[str, Any]:
    station = payload.get("chargingStation") or {}
    session.record.vendor = str(station.get("vendorName", ""))
    session.record.model = str(station.get("model", ""))
    session.record.last_boot = dict(payload)
    session.log("boot 2.0.1", session.cp_id, payload.get("reason"), session.record.vendor)
    return {
        "status": "Accepted",
        "currentTime": utc_now_iso(),
        "interval": session.heartbeat_interval,
    }
