from __future__ import annotations

import unittest

from ocpp_csms import rpc
from ocpp_csms.catalog import from_cp, from_csms, req_for
from ocpp_csms.dispatch import Dispatcher
from ocpp_csms.outbound import reset_payload
from ocpp_csms.store import ChargePointStore
from ocpp_csms.ws import accept_key, pick_protocol


class FakeSession:
    def __init__(self, protocol: str) -> None:
        self.cp_id = "CP001"
        self.protocol = protocol
        self.heartbeat_interval = 60
        self.probe_requested = False
        self.record = ChargePointStore().upsert("CP001", protocol)

    def log(self, *args) -> None:
        pass


class RpcTests(unittest.TestCase):
    def test_call_roundtrip(self) -> None:
        raw = rpc.pack_call("1", "Heartbeat", {})
        msg = rpc.parse(raw)
        self.assertEqual(msg.msg_type, rpc.CALL)
        self.assertEqual(msg.action, "Heartbeat")

    def test_result(self) -> None:
        raw = rpc.pack_result("1", {"status": "Accepted"})
        msg = rpc.parse(raw)
        self.assertEqual(msg.payload["status"], "Accepted")


class HandlerTests(unittest.TestCase):
    def test_v16_boot(self) -> None:
        d = Dispatcher("ocpp1.6")
        s = FakeSession("ocpp1.6")
        conf = d.handle(s, "BootNotification", {"chargePointVendor": "Pionix", "chargePointModel": "C-OCPP"})
        self.assertEqual(conf["status"], "Accepted")
        self.assertEqual(conf["interval"], 60)
        self.assertIn("currentTime", conf)

    def test_v16_start_tx(self) -> None:
        d = Dispatcher("ocpp1.6")
        s = FakeSession("ocpp1.6")
        conf = d.handle(s, "StartTransaction", {"connectorId": 1, "idTag": "TAG001"})
        self.assertEqual(conf["transactionId"], 42)
        self.assertEqual(conf["idTagInfo"]["status"], "Accepted")

    def test_v201_boot_and_auth(self) -> None:
        d = Dispatcher("ocpp2.0.1")
        s = FakeSession("ocpp2.0.1")
        boot = d.handle(
            s,
            "BootNotification",
            {"reason": "PowerUp", "chargingStation": {"model": "C-OCPP", "vendorName": "Pionix"}},
        )
        self.assertEqual(boot["status"], "Accepted")
        auth = d.handle(s, "Authorize", {"idToken": {"idToken": "TAG001", "type": "ISO14443"}})
        self.assertEqual(auth["idTokenInfo"]["status"], "Accepted")

    def test_catalog_covers_tables(self) -> None:
        from ocpp_csms.catalog import CONF_16, CONF_201, FROM_CP_16, FROM_CP_201, FROM_CSMS_16, FROM_CSMS_201, REQ_16, REQ_201

        self.assertEqual(len(CONF_16), 39)
        self.assertEqual(len(REQ_16), 39)
        self.assertEqual(len(CONF_201), 64)
        self.assertEqual(len(REQ_201), 64)
        self.assertEqual(len(FROM_CP_16), 14)
        self.assertEqual(len(FROM_CSMS_16), 26)
        self.assertEqual(len(FROM_CP_201), 25)
        self.assertEqual(len(FROM_CSMS_201), 40)
        for a in FROM_CP_16:
            self.assertIn(a, CONF_16)
        for a in FROM_CSMS_16:
            self.assertIn(a, REQ_16)
        for a in FROM_CP_201:
            self.assertIn(a, CONF_201)
        for a in FROM_CSMS_201:
            self.assertIn(a, REQ_201)


class WsHelperTests(unittest.TestCase):
    def test_accept_key(self) -> None:
        # RFC 6455 example
        self.assertEqual(accept_key("dGhlIHNhbXBsZSBub25jZQ=="), "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=")

    def test_pick_protocol(self) -> None:
        self.assertEqual(pick_protocol("ocpp1.6, ocpp2.0.1", ("ocpp1.6", "ocpp2.0.1")), "ocpp1.6")
        self.assertEqual(pick_protocol("ocpp2.0.1", ("ocpp1.6", "ocpp2.0.1")), "ocpp2.0.1")
        self.assertIsNone(pick_protocol("foo", ("ocpp1.6",)))

    def test_reset_payload(self) -> None:
        self.assertEqual(reset_payload("ocpp1.6")["type"], "Soft")
        self.assertEqual(reset_payload("ocpp2.0.1")["type"], "Immediate")
        self.assertEqual(reset_payload("ocpp2.1")["type"], "Immediate")
        self.assertEqual(req_for("ocpp2.1", "SetDERControl")["controlType"], "VoltVar")
        self.assertIn("NotifyDERAlarm", from_cp("ocpp2.1"))
        self.assertIn("SetDERControl", from_csms("ocpp2.1"))


if __name__ == "__main__":
    unittest.main()
