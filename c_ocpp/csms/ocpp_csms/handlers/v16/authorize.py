from __future__ import annotations

from typing import Any

from ocpp_csms.handlers.base import SessionView


def authorize(session: SessionView, payload: dict[str, Any]) -> dict[str, Any]:
    tag = str(payload.get("idTag", ""))
    session.log("authorize 1.6", session.cp_id, tag)
    return {"idTagInfo": {"status": "Accepted"}}
