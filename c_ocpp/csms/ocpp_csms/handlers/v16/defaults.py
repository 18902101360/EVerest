from __future__ import annotations

from typing import Any

from ocpp_csms.catalog import conf_for
from ocpp_csms.config import DONE_VENDOR, PROBE_VENDOR
from ocpp_csms.handlers.base import SessionView


def data_transfer(session: SessionView, payload: dict[str, Any]) -> dict[str, Any]:
    vendor = str(payload.get("vendorId") or "")
    session.log("dataTransfer 1.6", session.cp_id, vendor)
    if vendor == PROBE_VENDOR:
        session.probe_requested = True
    return conf_for(session.protocol, "DataTransfer")


def sign_certificate(session: SessionView, _payload: dict[str, Any]) -> dict[str, Any]:
    session.log("signCertificate 1.6", session.cp_id)
    return conf_for(session.protocol, "SignCertificate")
