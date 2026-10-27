#ifndef WS_CLIENT_H
#define WS_CLIENT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int fd;
} ws_conn_t;

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
int ws_connect(ws_conn_t *c, const char *host, int port, const char *path, const char *subprotocol);
/**
 * ws_send_text - 发一条 WebSocket 文本帧
 * @c: 已连接的对象
 * @data: UTF-8 文本，不要求以 NUL 结尾
 * @len: 字节数
 *
 * 客户端帧必须掩码。返回 0 成功，-1 表示发送失败。
 */
int ws_send_text(ws_conn_t *c, const void *data, size_t len);
/* timeout_ms: 0 = wait forever. Returns bytes, 0 on timeout, -1 on error/close. */
int ws_recv_text(ws_conn_t *c, char *buf, size_t buflen, int timeout_ms);
/**
 * ws_close - 关闭套接字
 * @c: 连接。NULL 或已经关闭时什么都不做
 */
void ws_close(ws_conn_t *c);

#ifdef __cplusplus
}
#endif

#endif
