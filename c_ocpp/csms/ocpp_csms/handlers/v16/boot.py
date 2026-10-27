from __future__ import annotations

from typing import Any

from ocpp_csms.clock import utc_now_iso
from ocpp_csms.handlers.base import SessionView


def boot(session: SessionView, payload: dict[str, Any]) -> dict[str, Any]:
    session.record.vendor = str(payload.get("chargePointVendor", ""))
    session.record.model = str(payload.get("chargePointModel", ""))
    session.record.last_boot = dict(payload)
    session.log("boot 1.6", session.cp_id, session.record.vendor, session.record.model)
    return {
        "status": "Accepted",
        "currentTime": utc_now_iso(),
        "interval": session.heartbeat_interval,
    }
