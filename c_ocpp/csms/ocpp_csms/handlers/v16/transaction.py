from __future__ import annotations

from typing import Any

from ocpp_csms.handlers.base import SessionView


def start_transaction(session: SessionView, payload: dict[str, Any]) -> dict[str, Any]:
    tid = session.record.next_tx_id
    session.record.next_tx_id += 1
    session.log("start tx 1.6", session.cp_id, "connector", payload.get("connectorId"), "id", tid)
    return {"transactionId": tid, "idTagInfo": {"status": "Accepted"}}


def stop_transaction(session: SessionView, payload: dict[str, Any]) -> dict[str, Any]:
    session.log("stop tx 1.6", session.cp_id, payload.get("transactionId"))
    return {"idTagInfo": {"status": "Accepted"}}
