from __future__ import annotations

from typing import Any

from ocpp_csms.handlers.base import SessionView


def authorize(session: SessionView, payload: dict[str, Any]) -> dict[str, Any]:
    token = payload.get("idToken") or {}
    session.log("authorize 2.0.1", session.cp_id, token.get("idToken"))
    return {"idTokenInfo": {"status": "Accepted"}}
