from __future__ import annotations

from ocpp_csms.catalog import conf_for
from ocpp_csms.config import PROBE_VENDOR
from ocpp_csms.handlers.v201 import boot as boot_mod


def data_transfer(session, payload):
    vendor = str(payload.get("vendorId") or "")
    session.log("dataTransfer 2.0.1", session.cp_id, vendor)
    if vendor == PROBE_VENDOR:
        session.probe_requested = True
    return conf_for(session.protocol, "DataTransfer")


def registry() -> dict:
    return {
        "BootNotification": boot_mod.boot,
        "DataTransfer": data_transfer,
    }
