"""CSMS-originated CALL payloads (Reset after Boot in the demo path)."""

from __future__ import annotations


def reset_payload(protocol: str) -> dict:
    if protocol in ("ocpp2.0.1", "ocpp2.1"):
        return {"type": "Immediate"}
    return {"type": "Soft"}
