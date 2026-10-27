from __future__ import annotations

from typing import Any

from ocpp_csms.handlers.base import SessionView


def status_notification(session: SessionView, payload: dict[str, Any]) -> dict[str, Any]:
    session.record.last_status = dict(payload)
    session.log("status 1.6", session.cp_id, payload.get("connectorId"), payload.get("status"))
    return {}


def meter_values(session: SessionView, payload: dict[str, Any]) -> dict[str, Any]:
    session.log("meter 1.6", session.cp_id, payload.get("connectorId"))
    return {}
