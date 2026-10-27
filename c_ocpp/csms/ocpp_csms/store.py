"""In-memory charge-point registry (no database)."""

from __future__ import annotations

from dataclasses import dataclass, field
from typing import Any


@dataclass
class ChargePointRecord:
    cp_id: str
    protocol: str
    vendor: str = ""
    model: str = ""
    last_status: dict[str, Any] = field(default_factory=dict)
    last_boot: dict[str, Any] = field(default_factory=dict)
    next_tx_id: int = 1
    connected: bool = False


class ChargePointStore:
    def __init__(self) -> None:
        self._cps: dict[str, ChargePointRecord] = {}

    def upsert(self, cp_id: str, protocol: str) -> ChargePointRecord:
        rec = self._cps.get(cp_id)
        if rec is None:
            rec = ChargePointRecord(cp_id=cp_id, protocol=protocol)
            self._cps[cp_id] = rec
        rec.protocol = protocol
        rec.connected = True
        return rec

    def get(self, cp_id: str) -> ChargePointRecord | None:
        return self._cps.get(cp_id)

    def disconnect(self, cp_id: str) -> None:
        rec = self._cps.get(cp_id)
        if rec is not None:
            rec.connected = False

    def alloc_tx_id(self, cp_id: str) -> int:
        rec = self._cps[cp_id]
        tid = rec.next_tx_id
        rec.next_tx_id += 1
        return tid
