"""RFC6455 WebSocket (text frames) with OCPP subprotocol negotiation. Stdlib only."""

from __future__ import annotations

import asyncio
import base64
import hashlib
import os
from dataclasses import dataclass

GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
OP_CONT = 0x0
OP_TEXT = 0x1
OP_CLOSE = 0x8
OP_PING = 0x9
OP_PONG = 0xA


def accept_key(client_key: str) -> str:
    digest = hashlib.sha1((client_key.strip() + GUID).encode("ascii")).digest()
    return base64.b64encode(digest).decode("ascii")


def pick_protocol(header: str | None, supported: tuple[str, ...]) -> str | None:
    if not header:
        return None
    offered = [p.strip() for p in header.split(",") if p.strip()]
    for proto in offered:
        if proto in supported:
            return proto
    return None


@dataclass
class WsConnection:
    reader: asyncio.StreamReader
    writer: asyncio.StreamWriter
    protocol: str
    path: str
    cp_id: str

    async def send_text(self, text: str) -> None:
        data = text.encode("utf-8")
        header = bytearray()
        header.append(0x80 | OP_TEXT)
        n = len(data)
        if n < 126:
            header.append(n)
        elif n < 65536:
            header.append(126)
            header.extend(n.to_bytes(2, "big"))
        else:
            header.append(127)
            header.extend(n.to_bytes(8, "big"))
        self.writer.write(header + data)
        await self.writer.drain()

    async def recv_text(self) -> str | None:
        while True:
            opcode, payload = await _read_frame(self.reader)
            if opcode is None:
                return None
            if opcode == OP_CLOSE:
                return None
            if opcode == OP_PING:
                await _send_control(self.writer, OP_PONG, payload)
                continue
            if opcode == OP_PONG:
                continue
            if opcode == OP_TEXT:
                return payload.decode("utf-8")
            if opcode == OP_CONT:
                continue

    def close(self) -> None:
        try:
            self.writer.close()
        except Exception:
            pass


async def _send_control(writer: asyncio.StreamWriter, opcode: int, payload: bytes) -> None:
    header = bytes([0x80 | opcode, len(payload) & 0x7F])
    writer.write(header + payload[:125])
    await writer.drain()


async def _read_frame(reader: asyncio.StreamReader) -> tuple[int | None, bytes]:
    hdr = await reader.readexactly(2)
    opcode = hdr[0] & 0x0F
    masked = (hdr[1] & 0x80) != 0
    length = hdr[1] & 0x7F
    if length == 126:
        length = int.from_bytes(await reader.readexactly(2), "big")
    elif length == 127:
        length = int.from_bytes(await reader.readexactly(8), "big")
    mask = await reader.readexactly(4) if masked else b""
    data = await reader.readexactly(length) if length else b""
    if masked:
        data = bytes(b ^ mask[i % 4] for i, b in enumerate(data))
    return opcode, data


async def handshake(
    reader: asyncio.StreamReader,
    writer: asyncio.StreamWriter,
    supported: tuple[str, ...],
) -> WsConnection:
    raw = await _read_headers(reader)
    lines = raw.split("\r\n")
    request = lines[0]
    parts = request.split()
    if len(parts) < 2 or parts[0] != "GET":
        raise ConnectionError("not a websocket GET")
    path = parts[1]
    headers = {}
    for line in lines[1:]:
        if not line or ":" not in line:
            continue
        k, v = line.split(":", 1)
        headers[k.strip().lower()] = v.strip()
    key = headers.get("sec-websocket-key")
    if not key or headers.get("upgrade", "").lower() != "websocket":
        raise ConnectionError("missing websocket headers")
    proto = pick_protocol(headers.get("sec-websocket-protocol"), supported)
    if proto is None:
        raise ConnectionError("unsupported OCPP subprotocol")
    accept = accept_key(key)
    extra = f"Sec-WebSocket-Protocol: {proto}\r\n"
    resp = (
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        f"Sec-WebSocket-Accept: {accept}\r\n"
        f"{extra}"
        "\r\n"
    )
    writer.write(resp.encode("ascii"))
    await writer.drain()
    cp_id = path.strip("/").split("/")[-1] or "unknown"
    return WsConnection(reader=reader, writer=writer, protocol=proto, path=path, cp_id=cp_id)


async def _read_headers(reader: asyncio.StreamReader) -> str:
    buf = b""
    while b"\r\n\r\n" not in buf:
        chunk = await reader.read(1024)
        if not chunk:
            raise ConnectionError("client closed during handshake")
        buf += chunk
        if len(buf) > 16384:
            raise ConnectionError("handshake too large")
    return buf.split(b"\r\n\r\n", 1)[0].decode("iso-8859-1")


def random_mask_key() -> bytes:
    return os.urandom(4)
