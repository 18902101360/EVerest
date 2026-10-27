from __future__ import annotations

from dataclasses import dataclass


PROTO_16 = "ocpp1.6"
PROTO_201 = "ocpp2.0.1"
PROTO_21 = "ocpp2.1"
SUPPORTED = (PROTO_16, PROTO_201, PROTO_21)

PROBE_VENDOR = "PROBE_CSMS"
DONE_VENDOR = "ALL_CSMS_DONE"


@dataclass
class CsmsConfig:
    host: str = "0.0.0.0"
    port: int = 9000
    heartbeat_interval: int = 300
    send_reset_after_boot: bool = False
    probe_all: bool = True
    log_frames: bool = True
