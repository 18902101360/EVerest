from __future__ import annotations

from ocpp_csms.handlers.v16 import boot as boot_mod
from ocpp_csms.handlers.v16.defaults import data_transfer


def registry() -> dict:
    return {
        "BootNotification": boot_mod.boot,
        "DataTransfer": data_transfer,
    }
