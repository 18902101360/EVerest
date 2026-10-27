from __future__ import annotations

from typing import Any

from ocpp_csms.clock import utc_now_iso
from ocpp_csms.handlers.base import SessionView


def heartbeat(session: SessionView, _payload: dict[str, Any]) -> dict[str, Any]:
    session.log("heartbeat 2.0.1", session.cp_id)
    return {"currentTime": utc_now_iso()}
