from __future__ import annotations

from ocpp_csms.catalog import conf_for
from ocpp_csms.handlers.base import Handler, empty
from ocpp_csms.handlers.v16 import registry as registry_v16
from ocpp_csms.handlers.v201 import registry as registry_v201


class Dispatcher:
    def __init__(self, protocol: str) -> None:
        self.protocol = protocol
        if protocol in ("ocpp2.0.1", "ocpp2.1"):
            self._handlers = registry_v201()
        else:
            self._handlers = registry_v16()

    def handle(self, session, action: str, payload: dict) -> dict:
        fn: Handler | None = self._handlers.get(action)
        if fn is not None:
            return fn(session, payload if isinstance(payload, dict) else {})
        conf = conf_for(self.protocol, action)
        if conf:
            return conf
        return empty(session, payload if isinstance(payload, dict) else {})
