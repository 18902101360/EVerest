/* SPDX-License-Identifier: Apache-2.0
 *
 * 第 2 层：一条 OCPP 1.6 连接的上下文（pending uniqueId、心跳、控制权）。
 * N 个运营商 = N 个本结构 + N 条 WebSocket；rx 喂完整 JSON 文本帧。
 */
#ifndef OCPP16_SESSION_H
#define OCPP16_SESSION_H

#include "messages/ocpp16_messages.h"
#include "ocpp_port.h"
#include "ocpp_rpc.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef OCPP16_PENDING_MAX
#define OCPP16_PENDING_MAX 8 /* 本路未完成 CALL 条数上限 */
#endif

/**
 * 固件回调。*_req：CSMS→桩，填好 conf 后返回 0；非 0 则回 CallError InternalError。
 * *_conf：桩→CSMS 的 CALLRESULT。未实现的指针可为 NULL（req 侧用 example conf）。
 */
typedef struct ocpp16_handlers {
    void *user;
    int (*data_transfer_req)(const ocpp16_data_transfer_req_t *req, ocpp16_data_transfer_conf_t *conf, void *user);
    int (*cancel_reservation_req)(const ocpp16_cancel_reservation_req_t *req, ocpp16_cancel_reservation_conf_t *conf, void *user);
    int (*change_availability_req)(const ocpp16_change_availability_req_t *req, ocpp16_change_availability_conf_t *conf, void *user);
    int (*change_configuration_req)(const ocpp16_change_configuration_req_t *req, ocpp16_change_configuration_conf_t *conf, void *user);
    int (*clear_cache_req)(const ocpp16_clear_cache_req_t *req, ocpp16_clear_cache_conf_t *conf, void *user);
    int (*clear_charging_profile_req)(const ocpp16_clear_charging_profile_req_t *req, ocpp16_clear_charging_profile_conf_t *conf, void *user);
    int (*get_composite_schedule_req)(const ocpp16_get_composite_schedule_req_t *req, ocpp16_get_composite_schedule_conf_t *conf, void *user);
    int (*get_configuration_req)(const ocpp16_get_configuration_req_t *req, ocpp16_get_configuration_conf_t *conf, void *user);
    int (*get_diagnostics_req)(const ocpp16_get_diagnostics_req_t *req, ocpp16_get_diagnostics_conf_t *conf, void *user);
    int (*get_local_list_version_req)(const ocpp16_get_local_list_version_req_t *req, ocpp16_get_local_list_version_conf_t *conf, void *user);
    int (*remote_start_transaction_req)(const ocpp16_remote_start_transaction_req_t *req, ocpp16_remote_start_transaction_conf_t *conf, void *user);
    int (*remote_stop_transaction_req)(const ocpp16_remote_stop_transaction_req_t *req, ocpp16_remote_stop_transaction_conf_t *conf, void *user);
    int (*reserve_now_req)(const ocpp16_reserve_now_req_t *req, ocpp16_reserve_now_conf_t *conf, void *user);
    int (*reset_req)(const ocpp16_reset_req_t *req, ocpp16_reset_conf_t *conf, void *user);
    int (*send_local_list_req)(const ocpp16_send_local_list_req_t *req, ocpp16_send_local_list_conf_t *conf, void *user);
    int (*set_charging_profile_req)(const ocpp16_set_charging_profile_req_t *req, ocpp16_set_charging_profile_conf_t *conf, void *user);
    int (*trigger_message_req)(const ocpp16_trigger_message_req_t *req, ocpp16_trigger_message_conf_t *conf, void *user);
    int (*unlock_connector_req)(const ocpp16_unlock_connector_req_t *req, ocpp16_unlock_connector_conf_t *conf, void *user);
    int (*update_firmware_req)(const ocpp16_update_firmware_req_t *req, ocpp16_update_firmware_conf_t *conf, void *user);
    int (*certificate_signed_req)(const ocpp16_certificate_signed_req_t *req, ocpp16_certificate_signed_conf_t *conf, void *user);
    int (*delete_certificate_req)(const ocpp16_delete_certificate_req_t *req, ocpp16_delete_certificate_conf_t *conf, void *user);
    int (*extended_trigger_message_req)(const ocpp16_extended_trigger_message_req_t *req, ocpp16_extended_trigger_message_conf_t *conf, void *user);
    int (*get_installed_certificate_ids_req)(const ocpp16_get_installed_certificate_ids_req_t *req, ocpp16_get_installed_certificate_ids_conf_t *conf, void *user);
    int (*get_log_req)(const ocpp16_get_log_req_t *req, ocpp16_get_log_conf_t *conf, void *user);
    int (*install_certificate_req)(const ocpp16_install_certificate_req_t *req, ocpp16_install_certificate_conf_t *conf, void *user);
    int (*signed_update_firmware_req)(const ocpp16_signed_update_firmware_req_t *req, ocpp16_signed_update_firmware_conf_t *conf, void *user);
    void (*authorize_conf)(const ocpp16_authorize_conf_t *conf, void *user);
    void (*boot_notification_conf)(const ocpp16_boot_notification_conf_t *conf, void *user);
    void (*data_transfer_conf)(const ocpp16_data_transfer_conf_t *conf, void *user);
    void (*diagnostics_status_notification_conf)(const ocpp16_diagnostics_status_notification_conf_t *conf, void *user);
    void (*firmware_status_notification_conf)(const ocpp16_firmware_status_notification_conf_t *conf, void *user);
    void (*heartbeat_conf)(const ocpp16_heartbeat_conf_t *conf, void *user);
    void (*meter_values_conf)(const ocpp16_meter_values_conf_t *conf, void *user);
    void (*start_transaction_conf)(const ocpp16_start_transaction_conf_t *conf, void *user);
    void (*status_notification_conf)(const ocpp16_status_notification_conf_t *conf, void *user);
    void (*stop_transaction_conf)(const ocpp16_stop_transaction_conf_t *conf, void *user);
    void (*log_status_notification_conf)(const ocpp16_log_status_notification_conf_t *conf, void *user);
    void (*security_event_notification_conf)(const ocpp16_security_event_notification_conf_t *conf, void *user);
    void (*sign_certificate_conf)(const ocpp16_sign_certificate_conf_t *conf, void *user);
    void (*signed_firmware_status_notification_conf)(const ocpp16_signed_firmware_status_notification_conf_t *conf, void *user);
} ocpp16_handlers_t;

#ifndef OCPP_PAYLOAD_MAX
#define OCPP_PAYLOAD_MAX 2048 /* 业务 JSON，不含 RPC 外壳 */
#endif
#ifndef OCPP_FRAME_MAX
#define OCPP_FRAME_MAX 4096 /* 整帧 [2,id,action,payload] */
#endif

typedef struct ocpp16_session {
    ocpp_link_t link;                 /* 本路 send / 心跳槽 / accept_control */
    ocpp16_handlers_t handlers;
    unsigned seq;                     /* 发出 CALL 的 uniqueId 计数，从 1 起 */
    int registered;                   /* 仅 Boot status 为 Accepted 时置 1 */
    int heartbeat_interval_s;         /* Accepted 时是心跳秒数；Pending/Rejected 时是重试秒数 */
    int has_boot_req;                 /* 保存过 Boot 请求，Pending/Rejected 到点后重发 */
    ocpp16_boot_notification_req_t boot_req;
    struct { int used; char uid[37]; char action[48]; } pending[OCPP16_PENDING_MAX];
    char payload[OCPP_PAYLOAD_MAX];
    char frame[OCPP_FRAME_MAX];
} ocpp16_session_t;

/** link 可为 NULL：send 走全局 ocpp_port_send，心跳槽 0，accept_control=1。 */
void ocpp16_session_init(ocpp16_session_t *s, const ocpp16_handlers_t *h, const ocpp_link_t *link);
/** 运行时换链路或改 accept_control（主备切换）。 */
void ocpp16_session_bind(ocpp16_session_t *s, const ocpp_link_t *link);
/** 喂入完整一帧。返回前释放 unpack 摘出的 payload。不要传半包。 */
ocpp_err_t ocpp16_session_rx(ocpp16_session_t *s, const char *frame, size_t len);
/** 发任意 action 的 CALL；uniqueId 由 seq 生成并记入 pending。 */
ocpp_err_t ocpp16_session_call(ocpp16_session_t *s, const char *action, const char *payload_json);
/**
 * ocpp16_session_send_authorize - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp16_session_send_authorize(ocpp16_session_t *s, const ocpp16_authorize_req_t *req);
/**
 * ocpp16_session_send_boot_notification - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp16_session_send_boot_notification(ocpp16_session_t *s, const ocpp16_boot_notification_req_t *req);
/**
 * ocpp16_session_send_data_transfer - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp16_session_send_data_transfer(ocpp16_session_t *s, const ocpp16_data_transfer_req_t *req);
/**
 * ocpp16_session_send_diagnostics_status_notification - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp16_session_send_diagnostics_status_notification(ocpp16_session_t *s, const ocpp16_diagnostics_status_notification_req_t *req);
/**
 * ocpp16_session_send_firmware_status_notification - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp16_session_send_firmware_status_notification(ocpp16_session_t *s, const ocpp16_firmware_status_notification_req_t *req);
/**
 * ocpp16_session_send_heartbeat - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp16_session_send_heartbeat(ocpp16_session_t *s, const ocpp16_heartbeat_req_t *req);
/**
 * ocpp16_session_send_meter_values - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp16_session_send_meter_values(ocpp16_session_t *s, const ocpp16_meter_values_req_t *req);
/**
 * ocpp16_session_send_start_transaction - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp16_session_send_start_transaction(ocpp16_session_t *s, const ocpp16_start_transaction_req_t *req);
/**
 * ocpp16_session_send_status_notification - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp16_session_send_status_notification(ocpp16_session_t *s, const ocpp16_status_notification_req_t *req);
/**
 * ocpp16_session_send_stop_transaction - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp16_session_send_stop_transaction(ocpp16_session_t *s, const ocpp16_stop_transaction_req_t *req);
/**
 * ocpp16_session_send_log_status_notification - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp16_session_send_log_status_notification(ocpp16_session_t *s, const ocpp16_log_status_notification_req_t *req);
/**
 * ocpp16_session_send_security_event_notification - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp16_session_send_security_event_notification(ocpp16_session_t *s, const ocpp16_security_event_notification_req_t *req);
/**
 * ocpp16_session_send_sign_certificate - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp16_session_send_sign_certificate(ocpp16_session_t *s, const ocpp16_sign_certificate_req_t *req);
/**
 * ocpp16_session_send_signed_firmware_status_notification - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp16_session_send_signed_firmware_status_notification(ocpp16_session_t *s, const ocpp16_signed_firmware_status_notification_req_t *req);

#ifdef __cplusplus
}
#endif

#endif
