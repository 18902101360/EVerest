# Modular OCPP CSMS (1.6 + 2.0.1 + 2.1)

Python charge-point management system used to exercise `c_ocpp` over a real WebSocket.
No extra pip packages: `asyncio` + RFC6455 in `ocpp_csms/ws.py`.

```
csms/
  ocpp_csms/
    ws.py           WebSocket handshake / text frames
    rpc.py          CALL / CALLRESULT / CALLERROR
    session.py      one connected charge point
    dispatch.py     action → handler
    store.py        in-memory CP records / transaction ids
    outbound.py     CSMS→CP payloads (Reset after Boot)
    handlers/v16/   Boot, Authorize, Heartbeat, Start/StopTransaction, …
    handlers/v201/  Boot, Authorize, TransactionEvent, StatusNotification, …
  tests/            unittest for rpc + handlers
  run_live_test.py  start CSMS, run test_ocpp_csms
```

## Run the server

```bash
cd c_ocpp/csms
python3 -m ocpp_csms --host 0.0.0.0 --port 9000 --interval 300
```

Charge point URL: `ws://HOST:9000/<chargePointId>`

Subprotocols (must match the C library you link):

- `ocpp1.6`
- `ocpp2.0.1`
- `ocpp2.1`

After `BootNotification`, the live test sends `DataTransfer` `vendorId=PROBE_CSMS`. The CSMS then issues **every CSMS-originated CALL** (example payloads from `catalog.py`) and finishes with `vendorId=ALL_CSMS_DONE`. Disable with `--no-probe`. Optional `--reset-after-boot` still sends a single Reset after Boot.

## Live test with c_ocpp

Host `ctest` target `test_ocpp_csms` starts this server and a C client that uses `ocpp16_session` / `ocpp201_session` plus a small WS helper (`tests/ws_client.c`). The protocol library still has no networking.
