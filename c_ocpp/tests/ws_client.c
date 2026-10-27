/* Minimal RFC6455 client (text frames) for host tests. Not part of the MCU library. */
#include "ws_client.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#define WS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

/* public-domain SHA-1 */
typedef struct {
    uint32_t state[5];
    uint32_t count[2];
    uint8_t buffer[64];
} sha1_ctx;

/* SHA-1 用的 32 位循环左移。 */
static uint32_t rol(uint32_t v, int n) { return (v << n) | (v >> (32 - n)); }

/* 装入 SHA-1 初始状态。只给握手的 Sec-WebSocket-Accept 用。 */
static void sha1_init(sha1_ctx *c) {
    c->state[0] = 0x67452301u;
    c->state[1] = 0xEFCDAB89u;
    c->state[2] = 0x98BADCFEu;
    c->state[3] = 0x10325476u;
    c->state[4] = 0xC3D2E1F0u;
    c->count[0] = c->count[1] = 0;
}

/* 压缩一个 64 字节块。 */
static void sha1_block(sha1_ctx *c, const uint8_t *buf) {
    uint32_t w[80];
    for (int i = 0; i < 16; i++) {
        w[i] = ((uint32_t)buf[i * 4] << 24) | ((uint32_t)buf[i * 4 + 1] << 16) |
               ((uint32_t)buf[i * 4 + 2] << 8) | buf[i * 4 + 3];
    }
    for (int i = 16; i < 80; i++) {
        w[i] = rol(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
    }
    uint32_t a = c->state[0], b = c->state[1], d = c->state[3], e = c->state[4];
    uint32_t cc = c->state[2];
    for (int i = 0; i < 80; i++) {
        uint32_t f, k;
        if (i < 20) {
            f = (b & cc) | ((~b) & d);
            k = 0x5A827999u;
        } else if (i < 40) {
            f = b ^ cc ^ d;
            k = 0x6ED9EBA1u;
        } else if (i < 60) {
            f = (b & cc) | (b & d) | (cc & d);
            k = 0x8F1BBCDCu;
        } else {
            f = b ^ cc ^ d;
            k = 0xCA62C1D6u;
        }
        uint32_t t = rol(a, 5) + f + e + k + w[i];
        e = d;
        d = cc;
        cc = rol(b, 30);
        b = a;
        a = t;
    }
    c->state[0] += a;
    c->state[1] += b;
    c->state[2] += cc;
    c->state[3] += d;
    c->state[4] += e;
}

/* 把任意长度的数据送进 SHA-1。 */
static void sha1_update(sha1_ctx *c, const uint8_t *data, size_t len) {
    uint32_t i = c->count[0];
    uint32_t j = (i >> 3) & 63;
    c->count[0] += (uint32_t)(len << 3);
    if (c->count[0] < (uint32_t)(len << 3)) {
        c->count[1]++;
    }
    c->count[1] += (uint32_t)(len >> 29);
    size_t off = 0;
    if (j) {
        size_t n = 64 - j;
        if (n > len) {
            n = len;
        }
        memcpy(c->buffer + j, data, n);
        if (j + n < 64) {
            return;
        }
        sha1_block(c, c->buffer);
        off = n;
    }
    for (; off + 64 <= len; off += 64) {
        sha1_block(c, data + off);
    }
    if (off < len) {
        memcpy(c->buffer, data + off, len - off);
    }
}

/* 结束 SHA-1，写出 20 字节摘要。 */
static void sha1_final(sha1_ctx *c, uint8_t out[20]) {
    uint8_t finalcount[8];
    for (int i = 0; i < 8; i++) {
        finalcount[i] = (uint8_t)((c->count[(i >= 4) ? 0 : 1] >> ((3 - (i & 3)) * 8)) & 255);
    }
    uint8_t pad = 0x80;
    sha1_update(c, &pad, 1);
    pad = 0;
    while ((c->count[0] & 504) != 448) {
        sha1_update(c, &pad, 1);
    }
    sha1_update(c, finalcount, 8);
    for (int i = 0; i < 20; i++) {
        out[i] = (uint8_t)((c->state[i >> 2] >> ((3 - (i & 3)) * 8)) & 255);
    }
}

/* 把 20 字节摘要编成握手需要的 Base64。 */
static void b64_20(const uint8_t in[20], char out[29]) {
    static const char t[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int o = 0;
    for (int i = 0; i < 18; i += 3) {
        uint32_t v = ((uint32_t)in[i] << 16) | ((uint32_t)in[i + 1] << 8) | in[i + 2];
        out[o++] = t[(v >> 18) & 63];
        out[o++] = t[(v >> 12) & 63];
        out[o++] = t[(v >> 6) & 63];
        out[o++] = t[v & 63];
    }
    uint32_t v = ((uint32_t)in[18] << 16) | ((uint32_t)in[19] << 8);
    out[o++] = t[(v >> 18) & 63];
    out[o++] = t[(v >> 12) & 63];
    out[o++] = t[(v >> 6) & 63];
    out[o++] = '=';
    out[o] = 0;
}

/* 带超时的 recv。超时返回 0，对端关闭或出错返回 -1。 */
static int timed_recv(int fd, void *buf, size_t n, int timeout_ms) {
    size_t got = 0;
    while (got < n) {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        struct timeval tv;
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        int rc = select(fd + 1, &rfds, NULL, NULL, timeout_ms < 0 ? NULL : &tv);
        if (rc == 0) {
            if (got == 0) {
                return 0;
            }
            timeout_ms = 2000;
            continue;
        }
        if (rc < 0) {
            return -1;
        }
        ssize_t r = recv(fd, (char *)buf + got, n - got, 0);
        if (r <= 0) {
            return -1;
        }
        got += (size_t)r;
        timeout_ms = 1000;
    }
    return (int)got;
}

/* 把缓冲区写完。短写会继续，失败返回 -1。 */
static int send_all(int fd, const void *buf, size_t n) {
    const char *p = buf;
    size_t off = 0;
    while (off < n) {
        ssize_t w = send(fd, p + off, n - off, 0);
        if (w <= 0) {
            return -1;
        }
        off += (size_t)w;
    }
    return 0;
}

/**
 * ws_connect - 作为充电桩连上 CSMS 的 WebSocket
 * @c: 连接对象。成功后 fd 有效
 * @host: 主机名或地址
 * @port: TCP 端口
 * @path: URL 路径，OCPP 里就是桩号，例如 /CP001
 * @subprotocol: ocpp1.6 或 ocpp2.0.1。写进 Sec-WebSocket-Protocol
 *
 * 只完成 RFC6455 握手。返回 0 成功，-1 失败。这个文件不进单片机固件。
 */
int ws_connect(ws_conn_t *c, const char *host, int port, const char *path, const char *subprotocol) {
    memset(c, 0, sizeof(*c));
    c->fd = -1;
    char portstr[16];
    snprintf(portstr, sizeof portstr, "%d", port);
    struct addrinfo hints, *res = NULL;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(host, portstr, &hints, &res) != 0) {
        return -1;
    }
    int fd = -1;
    for (struct addrinfo *ai = res; ai; ai = ai->ai_next) {
        fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (fd < 0) {
            continue;
        }
        if (connect(fd, ai->ai_addr, ai->ai_addrlen) == 0) {
            break;
        }
        close(fd);
        fd = -1;
    }
    freeaddrinfo(res);
    if (fd < 0) {
        return -1;
    }
    uint8_t nonce[16];
    for (int i = 0; i < 16; i++) {
        nonce[i] = (uint8_t)(rand() & 255);
    }
    char key_b64[25];
    static const char t[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int o = 0;
    for (int i = 0; i < 15; i += 3) {
        uint32_t v = ((uint32_t)nonce[i] << 16) | ((uint32_t)nonce[i + 1] << 8) | nonce[i + 2];
        key_b64[o++] = t[(v >> 18) & 63];
        key_b64[o++] = t[(v >> 12) & 63];
        key_b64[o++] = t[(v >> 6) & 63];
        key_b64[o++] = t[v & 63];
    }
    uint32_t v = ((uint32_t)nonce[15] << 16);
    key_b64[o++] = t[(v >> 18) & 63];
    key_b64[o++] = t[(v >> 12) & 63];
    key_b64[o++] = '=';
    key_b64[o++] = '=';
    key_b64[o] = 0;

    char req[1024];
    int n = snprintf(req, sizeof req,
                     "GET %s HTTP/1.1\r\n"
                     "Host: %s:%d\r\n"
                     "Upgrade: websocket\r\n"
                     "Connection: Upgrade\r\n"
                     "Sec-WebSocket-Key: %s\r\n"
                     "Sec-WebSocket-Version: 13\r\n"
                     "Sec-WebSocket-Protocol: %s\r\n"
                     "\r\n",
                     path, host, port, key_b64, subprotocol);
    if (n < 0 || send_all(fd, req, (size_t)n) != 0) {
        close(fd);
        return -1;
    }
    char hdr[4096];
    size_t hgot = 0;
    while (hgot < sizeof hdr - 1) {
        int r = timed_recv(fd, hdr + hgot, 1, 3000);
        if (r <= 0) {
            close(fd);
            return -1;
        }
        hgot++;
        hdr[hgot] = 0;
        if (hgot >= 4 && strcmp(hdr + hgot - 4, "\r\n\r\n") == 0) {
            break;
        }
    }
    if (strstr(hdr, "101") == NULL) {
        close(fd);
        return -1;
    }
    sha1_ctx sh;
    sha1_init(&sh);
    sha1_update(&sh, (const uint8_t *)key_b64, strlen(key_b64));
    sha1_update(&sh, (const uint8_t *)WS_GUID, strlen(WS_GUID));
    uint8_t digest[20];
    sha1_final(&sh, digest);
    char expect[29];
    b64_20(digest, expect);
    if (strstr(hdr, expect) == NULL) {
        close(fd);
        return -1;
    }
    c->fd = fd;
    return 0;
}

/**
 * ws_send_text - 发一条 WebSocket 文本帧
 * @c: 已连接的对象
 * @data: UTF-8 文本，不要求以 NUL 结尾
 * @len: 字节数
 *
 * 客户端帧必须掩码。返回 0 成功，-1 表示发送失败。
 */
int ws_send_text(ws_conn_t *c, const void *data, size_t len) {
    if (c == NULL || c->fd < 0) {
        return -1;
    }
    uint8_t hdr[14];
    size_t hlen = 2;
    hdr[0] = 0x81;
    if (len < 126) {
        hdr[1] = (uint8_t)(0x80 | len);
    } else if (len < 65536) {
        hdr[1] = 0x80 | 126;
        hdr[2] = (uint8_t)(len >> 8);
        hdr[3] = (uint8_t)len;
        hlen = 4;
    } else {
        return -1;
    }
    uint8_t mask[4];
    for (int i = 0; i < 4; i++) {
        mask[i] = (uint8_t)(rand() & 255);
    }
    memcpy(hdr + hlen, mask, 4);
    hlen += 4;
    if (send_all(c->fd, hdr, hlen) != 0) {
        return -1;
    }
    uint8_t tmp[512];
    const uint8_t *p = data;
    size_t off = 0;
    while (off < len) {
        size_t n = len - off;
        if (n > sizeof tmp) {
            n = sizeof tmp;
        }
        for (size_t i = 0; i < n; i++) {
            tmp[i] = p[off + i] ^ mask[(off + i) % 4];
        }
        if (send_all(c->fd, tmp, n) != 0) {
            return -1;
        }
        off += n;
    }
    return 0;
}

/**
 * ws_recv_text - 读一条 WebSocket 文本帧
 * @c: 已连接的对象
 * @buf: 输出缓冲
 * @buflen: buf 容量，含结尾 NUL 的位置
 * @timeout_ms: 等待毫秒。0 表示一直等
 *
 * 返回写入 buf 的字节数。超时返回 0，关闭或错误返回 -1。
 * Ping 会自动回 Pong，不返回给调用方。
 */
int ws_recv_text(ws_conn_t *c, char *buf, size_t buflen, int timeout_ms) {
    if (c == NULL || c->fd < 0) {
        return -1;
    }
    for (;;) {
        uint8_t h[2];
        int r = timed_recv(c->fd, h, 2, timeout_ms);
        if (r == 0) {
            return 0;
        }
        if (r < 0) {
            return -1;
        }
        int opcode = h[0] & 0x0F;
        int masked = (h[1] & 0x80) != 0;
        uint64_t length = h[1] & 0x7F;
        if (length == 126) {
            uint8_t ext[2];
            if (timed_recv(c->fd, ext, 2, 3000) <= 0) {
                return -1;
            }
            length = ((uint64_t)ext[0] << 8) | ext[1];
        } else if (length == 127) {
            return -1;
        }
        uint8_t mask[4] = {0};
        if (masked) {
            if (timed_recv(c->fd, mask, 4, 3000) <= 0) {
                return -1;
            }
        }
        if (length >= buflen) {
            return -1;
        }
        if (length > 0 && timed_recv(c->fd, buf, (size_t)length, 3000) <= 0) {
            return -1;
        }
        if (masked) {
            for (uint64_t i = 0; i < length; i++) {
                buf[i] = (char)((uint8_t)buf[i] ^ mask[i % 4]);
            }
        }
        buf[length] = 0;
        if (opcode == 0x8) {
            return -1;
        }
        if (opcode == 0x9) {
            uint8_t ph[2] = {0x8A, (uint8_t)length};
            send_all(c->fd, ph, 2);
            if (length) {
                send_all(c->fd, buf, (size_t)length);
            }
            continue;
        }
        if (opcode == 0xA) {
            continue;
        }
        if (opcode == 0x1) {
            return (int)length;
        }
    }
}

/**
 * ws_close - 关闭套接字
 * @c: 连接。NULL 或已经关闭时什么都不做
 */
void ws_close(ws_conn_t *c) {
    if (c && c->fd >= 0) {
        close(c->fd);
        c->fd = -1;
    }
}
