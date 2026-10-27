#!/usr/bin/env python3
"""Start the CSMS, then run the c_ocpp WebSocket client binary."""

from __future__ import annotations

import os
import socket
import subprocess
import sys
import time
from pathlib import Path

CSMS_DIR = Path(__file__).resolve().parent


def wait_port(host: str, port: int, timeout: float = 5.0) -> None:
    deadline = time.time() + timeout
    while time.time() < deadline:
        try:
            with socket.create_connection((host, port), timeout=0.2):
                return
        except OSError:
            time.sleep(0.05)
    raise RuntimeError(f"CSMS did not listen on {host}:{port}")


def main() -> int:
    if len(sys.argv) < 2:
        print("usage: run_live_test.py <test_ocpp_csms_binary>", file=sys.stderr)
        return 2
    client = sys.argv[1]
    sock = socket.socket()
    sock.bind(("127.0.0.1", 0))
    port = sock.getsockname()[1]
    sock.close()

    env = os.environ.copy()
    env["PYTHONPATH"] = str(CSMS_DIR) + os.pathsep + env.get("PYTHONPATH", "")
    server = subprocess.Popen(
        [
            sys.executable,
            "-m",
            "ocpp_csms",
            "--host",
            "127.0.0.1",
            "--port",
            str(port),
            "--interval",
            "3600",
            "--quiet",
        ],
        cwd=str(CSMS_DIR),
        env=env,
    )
    try:
        wait_port("127.0.0.1", port)
        proc = subprocess.run([client, "127.0.0.1", str(port)], timeout=90)
        return proc.returncode
    finally:
        server.terminate()
        try:
            server.wait(timeout=3)
        except subprocess.TimeoutExpired:
            server.kill()


if __name__ == "__main__":
    raise SystemExit(main())
