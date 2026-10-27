from __future__ import annotations

from typing import Any

from ocpp_csms.handlers.base import SessionView


def transaction_event(session: SessionView, payload: dict[str, Any]) -> dict[str, Any]:
    info = payload.get("transactionInfo") or {}
    session.log(
        "tx event 2.0.1",
        session.cp_id,
        payload.get("eventType"),
        info.get("transactionId"),
    )
    return {}


def status_notification(session: SessionView, payload: dict[str, Any]) -> dict[str, Any]:
    session.record.last_status = dict(payload)
    session.log("status 2.0.1", session.cp_id, payload.get("evseId"), payload.get("connectorStatus"))
    return {}
