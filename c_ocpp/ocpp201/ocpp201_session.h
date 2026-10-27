/* SPDX-License-Identifier: Apache-2.0
 *
 * 第 2 层：一条 OCPP 2.0.1 连接。API 形态与 ocpp16_session 相同，消息类型不同
 *（交易用 TransactionEvent，配置用 Get/SetVariables）。
 */
#ifndef OCPP201_SESSION_H
#define OCPP201_SESSION_H
#include "messages/ocpp201_messages.h"
#include "ocpp_port.h"
#include "ocpp_rpc.h"
#ifdef __cplusplus
extern "C" {
#endif
#ifndef OCPP201_PENDING_MAX
#define OCPP201_PENDING_MAX 8 /* 本路未完成 CALL 条数上限 */
#endif
#ifndef OCPP_PAYLOAD_MAX
#define OCPP_PAYLOAD_MAX 2048
#endif
#ifndef OCPP_FRAME_MAX
#define OCPP_FRAME_MAX 4096
#endif
typedef struct ocpp201_handlers { void *user;
    int (*cancel_reservation_req)(const ocpp201_cancel_reservation_req_t *req, ocpp201_cancel_reservation_conf_t *conf, void *user);
    int (*certificate_signed_req)(const ocpp201_certificate_signed_req_t *req, ocpp201_certificate_signed_conf_t *conf, void *user);
    int (*change_availability_req)(const ocpp201_change_availability_req_t *req, ocpp201_change_availability_conf_t *conf, void *user);
    int (*clear_cache_req)(const ocpp201_clear_cache_req_t *req, ocpp201_clear_cache_conf_t *conf, void *user);
    int (*clear_charging_profile_req)(const ocpp201_clear_charging_profile_req_t *req, ocpp201_clear_charging_profile_conf_t *conf, void *user);
    int (*clear_display_message_req)(const ocpp201_clear_display_message_req_t *req, ocpp201_clear_display_message_conf_t *conf, void *user);
    int (*clear_variable_monitoring_req)(const ocpp201_clear_variable_monitoring_req_t *req, ocpp201_clear_variable_monitoring_conf_t *conf, void *user);
    int (*cost_updated_req)(const ocpp201_cost_updated_req_t *req, ocpp201_cost_updated_conf_t *conf, void *user);
    int (*customer_information_req)(const ocpp201_customer_information_req_t *req, ocpp201_customer_information_conf_t *conf, void *user);
    int (*data_transfer_req)(const ocpp201_data_transfer_req_t *req, ocpp201_data_transfer_conf_t *conf, void *user);
    int (*delete_certificate_req)(const ocpp201_delete_certificate_req_t *req, ocpp201_delete_certificate_conf_t *conf, void *user);
    int (*get_base_report_req)(const ocpp201_get_base_report_req_t *req, ocpp201_get_base_report_conf_t *conf, void *user);
    int (*get_charging_profiles_req)(const ocpp201_get_charging_profiles_req_t *req, ocpp201_get_charging_profiles_conf_t *conf, void *user);
    int (*get_composite_schedule_req)(const ocpp201_get_composite_schedule_req_t *req, ocpp201_get_composite_schedule_conf_t *conf, void *user);
    int (*get_display_messages_req)(const ocpp201_get_display_messages_req_t *req, ocpp201_get_display_messages_conf_t *conf, void *user);
    int (*get_installed_certificate_ids_req)(const ocpp201_get_installed_certificate_ids_req_t *req, ocpp201_get_installed_certificate_ids_conf_t *conf, void *user);
    int (*get_local_list_version_req)(const ocpp201_get_local_list_version_req_t *req, ocpp201_get_local_list_version_conf_t *conf, void *user);
    int (*get_log_req)(const ocpp201_get_log_req_t *req, ocpp201_get_log_conf_t *conf, void *user);
    int (*get_monitoring_report_req)(const ocpp201_get_monitoring_report_req_t *req, ocpp201_get_monitoring_report_conf_t *conf, void *user);
    int (*get_report_req)(const ocpp201_get_report_req_t *req, ocpp201_get_report_conf_t *conf, void *user);
    int (*get_transaction_status_req)(const ocpp201_get_transaction_status_req_t *req, ocpp201_get_transaction_status_conf_t *conf, void *user);
    int (*get_variables_req)(const ocpp201_get_variables_req_t *req, ocpp201_get_variables_conf_t *conf, void *user);
    int (*install_certificate_req)(const ocpp201_install_certificate_req_t *req, ocpp201_install_certificate_conf_t *conf, void *user);
    int (*publish_firmware_req)(const ocpp201_publish_firmware_req_t *req, ocpp201_publish_firmware_conf_t *conf, void *user);
    int (*request_start_transaction_req)(const ocpp201_request_start_transaction_req_t *req, ocpp201_request_start_transaction_conf_t *conf, void *user);
    int (*request_stop_transaction_req)(const ocpp201_request_stop_transaction_req_t *req, ocpp201_request_stop_transaction_conf_t *conf, void *user);
    int (*reserve_now_req)(const ocpp201_reserve_now_req_t *req, ocpp201_reserve_now_conf_t *conf, void *user);
    int (*reset_req)(const ocpp201_reset_req_t *req, ocpp201_reset_conf_t *conf, void *user);
    int (*send_local_list_req)(const ocpp201_send_local_list_req_t *req, ocpp201_send_local_list_conf_t *conf, void *user);
    int (*set_charging_profile_req)(const ocpp201_set_charging_profile_req_t *req, ocpp201_set_charging_profile_conf_t *conf, void *user);
    int (*set_display_message_req)(const ocpp201_set_display_message_req_t *req, ocpp201_set_display_message_conf_t *conf, void *user);
    int (*set_monitoring_base_req)(const ocpp201_set_monitoring_base_req_t *req, ocpp201_set_monitoring_base_conf_t *conf, void *user);
    int (*set_monitoring_level_req)(const ocpp201_set_monitoring_level_req_t *req, ocpp201_set_monitoring_level_conf_t *conf, void *user);
    int (*set_network_profile_req)(const ocpp201_set_network_profile_req_t *req, ocpp201_set_network_profile_conf_t *conf, void *user);
    int (*set_variable_monitoring_req)(const ocpp201_set_variable_monitoring_req_t *req, ocpp201_set_variable_monitoring_conf_t *conf, void *user);
    int (*set_variables_req)(const ocpp201_set_variables_req_t *req, ocpp201_set_variables_conf_t *conf, void *user);
    int (*trigger_message_req)(const ocpp201_trigger_message_req_t *req, ocpp201_trigger_message_conf_t *conf, void *user);
    int (*unlock_connector_req)(const ocpp201_unlock_connector_req_t *req, ocpp201_unlock_connector_conf_t *conf, void *user);
    int (*unpublish_firmware_req)(const ocpp201_unpublish_firmware_req_t *req, ocpp201_unpublish_firmware_conf_t *conf, void *user);
    int (*update_firmware_req)(const ocpp201_update_firmware_req_t *req, ocpp201_update_firmware_conf_t *conf, void *user);
    void (*authorize_conf)(const ocpp201_authorize_conf_t *conf, void *user);
    void (*boot_notification_conf)(const ocpp201_boot_notification_conf_t *conf, void *user);
    void (*cleared_charging_limit_conf)(const ocpp201_cleared_charging_limit_conf_t *conf, void *user);
    void (*data_transfer_conf)(const ocpp201_data_transfer_conf_t *conf, void *user);
    void (*firmware_status_notification_conf)(const ocpp201_firmware_status_notification_conf_t *conf, void *user);
    void (*get15118_ev_certificate_conf)(const ocpp201_get15118_ev_certificate_conf_t *conf, void *user);
    void (*get_certificate_status_conf)(const ocpp201_get_certificate_status_conf_t *conf, void *user);
    void (*heartbeat_conf)(const ocpp201_heartbeat_conf_t *conf, void *user);
    void (*log_status_notification_conf)(const ocpp201_log_status_notification_conf_t *conf, void *user);
    void (*meter_values_conf)(const ocpp201_meter_values_conf_t *conf, void *user);
    void (*notify_charging_limit_conf)(const ocpp201_notify_charging_limit_conf_t *conf, void *user);
    void (*notify_customer_information_conf)(const ocpp201_notify_customer_information_conf_t *conf, void *user);
    void (*notify_display_messages_conf)(const ocpp201_notify_display_messages_conf_t *conf, void *user);
    void (*notify_ev_charging_needs_conf)(const ocpp201_notify_ev_charging_needs_conf_t *conf, void *user);
    void (*notify_ev_charging_schedule_conf)(const ocpp201_notify_ev_charging_schedule_conf_t *conf, void *user);
    void (*notify_event_conf)(const ocpp201_notify_event_conf_t *conf, void *user);
    void (*notify_monitoring_report_conf)(const ocpp201_notify_monitoring_report_conf_t *conf, void *user);
    void (*notify_report_conf)(const ocpp201_notify_report_conf_t *conf, void *user);
    void (*publish_firmware_status_notification_conf)(const ocpp201_publish_firmware_status_notification_conf_t *conf, void *user);
    void (*report_charging_profiles_conf)(const ocpp201_report_charging_profiles_conf_t *conf, void *user);
    void (*reservation_status_update_conf)(const ocpp201_reservation_status_update_conf_t *conf, void *user);
    void (*security_event_notification_conf)(const ocpp201_security_event_notification_conf_t *conf, void *user);
    void (*sign_certificate_conf)(const ocpp201_sign_certificate_conf_t *conf, void *user);
    void (*status_notification_conf)(const ocpp201_status_notification_conf_t *conf, void *user);
    void (*transaction_event_conf)(const ocpp201_transaction_event_conf_t *conf, void *user);
} ocpp201_handlers_t; /* *_req 返回 0 成功；*_conf 为桩发出 CALL 的应答 */
typedef struct ocpp201_session {
    ocpp_link_t link;                 /* 本路 send / 心跳槽 / accept_control */
    ocpp201_handlers_t handlers;
    unsigned seq;                     /* 发出 CALL 的 uniqueId 计数，从 1 起 */
    int registered;                   /* 仅 Boot status 为 Accepted 时置 1 */
    int heartbeat_interval_s;         /* Accepted 时是心跳秒数；Pending/Rejected 时是重试秒数 */
    int has_boot_req;                 /* 保存过 Boot 请求，Pending/Rejected 到点后重发 */
    ocpp201_boot_notification_req_t boot_req;
    struct { int used; char uid[37]; char action[48]; } pending[OCPP201_PENDING_MAX];
    char payload[OCPP_PAYLOAD_MAX];
    char frame[OCPP_FRAME_MAX];
} ocpp201_session_t; /* 放 BSS；payload+frame 约数 KiB */
/** link 可为 NULL：send 走全局 ocpp_port_send，心跳槽 0，accept_control=1。 */
void ocpp201_session_init(ocpp201_session_t *s, const ocpp201_handlers_t *h, const ocpp_link_t *link);
/** 运行时换链路或改 accept_control。link 为 NULL 时保持原样。 */
void ocpp201_session_bind(ocpp201_session_t *s, const ocpp_link_t *link);
/** 喂入完整一帧。返回前释放 unpack 摘出的 payload。不要传半包。 */
ocpp_err_t ocpp201_session_rx(ocpp201_session_t *s, const char *frame, size_t len);
/** 发任意 action 的 CALL。uniqueId 由本路 seq 生成并记入 pending。 */
ocpp_err_t ocpp201_session_call(ocpp201_session_t *s, const char *action, const char *payload_json);
/**
 * ocpp201_session_send_authorize - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp201_session_send_authorize(ocpp201_session_t *s, const ocpp201_authorize_req_t *req);
/**
 * ocpp201_session_send_boot_notification - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp201_session_send_boot_notification(ocpp201_session_t *s, const ocpp201_boot_notification_req_t *req);
/**
 * ocpp201_session_send_cleared_charging_limit - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp201_session_send_cleared_charging_limit(ocpp201_session_t *s, const ocpp201_cleared_charging_limit_req_t *req);
/**
 * ocpp201_session_send_data_transfer - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp201_session_send_data_transfer(ocpp201_session_t *s, const ocpp201_data_transfer_req_t *req);
/**
 * ocpp201_session_send_firmware_status_notification - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp201_session_send_firmware_status_notification(ocpp201_session_t *s, const ocpp201_firmware_status_notification_req_t *req);
/**
 * ocpp201_session_send_get15118_ev_certificate - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp201_session_send_get15118_ev_certificate(ocpp201_session_t *s, const ocpp201_get15118_ev_certificate_req_t *req);
/**
 * ocpp201_session_send_get_certificate_status - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp201_session_send_get_certificate_status(ocpp201_session_t *s, const ocpp201_get_certificate_status_req_t *req);
/**
 * ocpp201_session_send_heartbeat - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp201_session_send_heartbeat(ocpp201_session_t *s, const ocpp201_heartbeat_req_t *req);
/**
 * ocpp201_session_send_log_status_notification - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp201_session_send_log_status_notification(ocpp201_session_t *s, const ocpp201_log_status_notification_req_t *req);
/**
 * ocpp201_session_send_meter_values - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp201_session_send_meter_values(ocpp201_session_t *s, const ocpp201_meter_values_req_t *req);
/**
 * ocpp201_session_send_notify_charging_limit - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp201_session_send_notify_charging_limit(ocpp201_session_t *s, const ocpp201_notify_charging_limit_req_t *req);
/**
 * ocpp201_session_send_notify_customer_information - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp201_session_send_notify_customer_information(ocpp201_session_t *s, const ocpp201_notify_customer_information_req_t *req);
/**
 * ocpp201_session_send_notify_display_messages - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp201_session_send_notify_display_messages(ocpp201_session_t *s, const ocpp201_notify_display_messages_req_t *req);
/**
 * ocpp201_session_send_notify_ev_charging_needs - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp201_session_send_notify_ev_charging_needs(ocpp201_session_t *s, const ocpp201_notify_ev_charging_needs_req_t *req);
/**
 * ocpp201_session_send_notify_ev_charging_schedule - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp201_session_send_notify_ev_charging_schedule(ocpp201_session_t *s, const ocpp201_notify_ev_charging_schedule_req_t *req);
/**
 * ocpp201_session_send_notify_event - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp201_session_send_notify_event(ocpp201_session_t *s, const ocpp201_notify_event_req_t *req);
/**
 * ocpp201_session_send_notify_monitoring_report - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp201_session_send_notify_monitoring_report(ocpp201_session_t *s, const ocpp201_notify_monitoring_report_req_t *req);
/**
 * ocpp201_session_send_notify_report - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp201_session_send_notify_report(ocpp201_session_t *s, const ocpp201_notify_report_req_t *req);
/**
 * ocpp201_session_send_publish_firmware_status_notification - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp201_session_send_publish_firmware_status_notification(ocpp201_session_t *s, const ocpp201_publish_firmware_status_notification_req_t *req);
/**
 * ocpp201_session_send_report_charging_profiles - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp201_session_send_report_charging_profiles(ocpp201_session_t *s, const ocpp201_report_charging_profiles_req_t *req);
/**
 * ocpp201_session_send_reservation_status_update - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp201_session_send_reservation_status_update(ocpp201_session_t *s, const ocpp201_reservation_status_update_req_t *req);
/**
 * ocpp201_session_send_security_event_notification - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp201_session_send_security_event_notification(ocpp201_session_t *s, const ocpp201_security_event_notification_req_t *req);
/**
 * ocpp201_session_send_sign_certificate - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp201_session_send_sign_certificate(ocpp201_session_t *s, const ocpp201_sign_certificate_req_t *req);
/**
 * ocpp201_session_send_status_notification - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp201_session_send_status_notification(ocpp201_session_t *s, const ocpp201_status_notification_req_t *req);
/**
 * ocpp201_session_send_transaction_event - 桩发出对应的 OCPP CALL
 * @s: 本路会话，里面的 link.send 必须已经能写 WebSocket
 * @req: 已填好的请求。函数会把它编进 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 表满或 send 失败时返回负的 ocpp_err_t。
 * 对端 CALLRESULT 由 session_rx 按 uniqueId 分发到 *_conf 回调。
 */
ocpp_err_t ocpp201_session_send_transaction_event(ocpp201_session_t *s, const ocpp201_transaction_event_req_t *req);
#ifdef __cplusplus
}
#endif
#endif
