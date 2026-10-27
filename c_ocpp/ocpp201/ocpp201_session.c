#include "ocpp201_session.h"
#include "ocpp_port.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
/* 仅 Boot 已被接受后使用。到期时发一帧 Heartbeat。 */
static void heartbeat_timer(int id, void *ctx) {
    ocpp201_session_t *s = (ocpp201_session_t *)ctx;
    (void)id;
    ocpp201_heartbeat_req_t req;
    ocpp201_heartbeat_req_example(&req);
    ocpp201_session_send_heartbeat(s, &req);
}

/* Pending/Rejected 的一次性定时器。到点重发上次的 BootNotification。 */
static void boot_retry_timer(int id, void *ctx) {
    ocpp201_session_t *s = (ocpp201_session_t *)ctx;
    (void)id;
    if (s->has_boot_req) {
        ocpp201_session_send_boot_notification(s, &s->boot_req);
    }
}
/* 把 s->frame 交给这一路 send。0 成功。 */
static int session_tx(ocpp201_session_t *s) { return ocpp_link_send(&s->link, s->frame, strlen(s->frame)); }
/**
 * ocpp201_session_bind - 换掉这一路的发送函数、心跳槽或控制权
 * @s: 已初始化的 session
 * @link: 新链路。NULL 时保持原样
 *
 * 用途与 ocpp16_session_bind 相同。
 */
void ocpp201_session_bind(ocpp201_session_t *s, const ocpp_link_t *link) { if(link) s->link=*link; }
/**
 * ocpp201_session_init - 清零并绑定一路 2.0.1 会话
 * @s: 调用方提供的对象，放 BSS，不要放任务栈
 * @h: 业务回调。NULL 表示全部用示例应答
 * @link: 这一路的发送函数。NULL 时走全局 send，心跳槽 0，accept_control 为 1
 *
 * uniqueId 从 1 开始，默认心跳间隔 300 秒。
 */
void ocpp201_session_init(ocpp201_session_t *s, const ocpp201_handlers_t *h, const ocpp_link_t *link) {
    memset(s, 0, sizeof(*s));
    s->seq = 1;
    s->heartbeat_interval_s = 300;
    s->link.accept_control = 1;
    s->link.heartbeat_timer_id = OCPP_PORT_TIMER_HEARTBEAT;
    if (h) {
        s->handlers = *h;
    }
    ocpp201_session_bind(s, link);
}
/* 记下尚未收到 CALLRESULT 的 uniqueId。表满返回 -1。 */
static int pending_add(ocpp201_session_t *s, const char *uid, const char *action) {
    for (int i = 0; i < OCPP201_PENDING_MAX; i++) {
        if (!s->pending[i].used) {
            s->pending[i].used = 1;
            ocpp_str_copy(s->pending[i].uid, sizeof(s->pending[i].uid), uid);
            ocpp_str_copy(s->pending[i].action, sizeof(s->pending[i].action), action);
            return 0;
        }
    }
    return -1;
}
/* 发送失败时撤掉刚记下的 pending，避免槽被一条没发出去的 CALL 占死。 */
static void pending_drop(ocpp201_session_t *s, const char *uid) {
    for (int i = 0; i < OCPP201_PENDING_MAX; i++) {
        if (s->pending[i].used && strcmp(s->pending[i].uid, uid) == 0) {
            s->pending[i].used = 0;
            return;
        }
    }
}
/**
 * ocpp201_session_call - 发出一条 2.0.1 CALL
 * @s: 本路会话
 * @action: OCPP action 名
 * @payload_json: 已编好的对象文本
 *
 * uniqueId 用本路递增序号。返回 OCPP_OK，或组帧、pending、发送的失败码。
 */
ocpp_err_t ocpp201_session_call(ocpp201_session_t *s, const char *action, const char *payload_json) {
    char uid[16];
    snprintf(uid, sizeof(uid), "%u", s->seq++);
    ocpp_err_t rc = ocpp_rpc_pack_call(uid, action, payload_json, s->frame, sizeof(s->frame));
    if (rc != OCPP_OK) return rc;
    if (pending_add(s, uid, action) != 0) return OCPP_ERR_OVERFLOW;
    if (session_tx(s) != 0) {
        pending_drop(s, uid);
        return OCPP_ERR_IO;
    }
    return OCPP_OK;
}
/**
 * ocpp201_session_send_authorize - 桩发出 Authorize
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp201_session_send_authorize(ocpp201_session_t *s, const ocpp201_authorize_req_t *req) { ocpp_err_t rc=ocpp201_authorize_req_encode(req,s->payload,sizeof(s->payload)); if(rc!=OCPP_OK) return rc; return ocpp201_session_call(s,"Authorize",s->payload); }
/**
 * ocpp201_session_send_boot_notification - 桩发出 BootNotification
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp201_session_send_boot_notification(ocpp201_session_t *s, const ocpp201_boot_notification_req_t *req) {
    /* 留下请求。Pending/Rejected 到点要原样重发。 */
    if (req != NULL) {
        s->boot_req = *req;
        s->has_boot_req = 1;
    }
    ocpp_err_t rc = ocpp201_boot_notification_req_encode(req, s->payload, sizeof(s->payload));
    if (rc != OCPP_OK) return rc;
    return ocpp201_session_call(s, "BootNotification", s->payload);
}
/**
 * ocpp201_session_send_cleared_charging_limit - 桩发出 ClearedChargingLimit
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp201_session_send_cleared_charging_limit(ocpp201_session_t *s, const ocpp201_cleared_charging_limit_req_t *req) { ocpp_err_t rc=ocpp201_cleared_charging_limit_req_encode(req,s->payload,sizeof(s->payload)); if(rc!=OCPP_OK) return rc; return ocpp201_session_call(s,"ClearedChargingLimit",s->payload); }
/**
 * ocpp201_session_send_data_transfer - 桩发出 DataTransfer
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp201_session_send_data_transfer(ocpp201_session_t *s, const ocpp201_data_transfer_req_t *req) { ocpp_err_t rc=ocpp201_data_transfer_req_encode(req,s->payload,sizeof(s->payload)); if(rc!=OCPP_OK) return rc; return ocpp201_session_call(s,"DataTransfer",s->payload); }
/**
 * ocpp201_session_send_firmware_status_notification - 桩发出 FirmwareStatusNotification
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp201_session_send_firmware_status_notification(ocpp201_session_t *s, const ocpp201_firmware_status_notification_req_t *req) { ocpp_err_t rc=ocpp201_firmware_status_notification_req_encode(req,s->payload,sizeof(s->payload)); if(rc!=OCPP_OK) return rc; return ocpp201_session_call(s,"FirmwareStatusNotification",s->payload); }
/**
 * ocpp201_session_send_get15118_ev_certificate - 桩发出 Get15118EVCertificate
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp201_session_send_get15118_ev_certificate(ocpp201_session_t *s, const ocpp201_get15118_ev_certificate_req_t *req) { ocpp_err_t rc=ocpp201_get15118_ev_certificate_req_encode(req,s->payload,sizeof(s->payload)); if(rc!=OCPP_OK) return rc; return ocpp201_session_call(s,"Get15118EVCertificate",s->payload); }
/**
 * ocpp201_session_send_get_certificate_status - 桩发出 GetCertificateStatus
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp201_session_send_get_certificate_status(ocpp201_session_t *s, const ocpp201_get_certificate_status_req_t *req) { ocpp_err_t rc=ocpp201_get_certificate_status_req_encode(req,s->payload,sizeof(s->payload)); if(rc!=OCPP_OK) return rc; return ocpp201_session_call(s,"GetCertificateStatus",s->payload); }
/**
 * ocpp201_session_send_heartbeat - 桩发出 Heartbeat
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp201_session_send_heartbeat(ocpp201_session_t *s, const ocpp201_heartbeat_req_t *req) { ocpp_err_t rc=ocpp201_heartbeat_req_encode(req,s->payload,sizeof(s->payload)); if(rc!=OCPP_OK) return rc; return ocpp201_session_call(s,"Heartbeat",s->payload); }
/**
 * ocpp201_session_send_log_status_notification - 桩发出 LogStatusNotification
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp201_session_send_log_status_notification(ocpp201_session_t *s, const ocpp201_log_status_notification_req_t *req) { ocpp_err_t rc=ocpp201_log_status_notification_req_encode(req,s->payload,sizeof(s->payload)); if(rc!=OCPP_OK) return rc; return ocpp201_session_call(s,"LogStatusNotification",s->payload); }
/**
 * ocpp201_session_send_meter_values - 桩发出 MeterValues
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp201_session_send_meter_values(ocpp201_session_t *s, const ocpp201_meter_values_req_t *req) { ocpp_err_t rc=ocpp201_meter_values_req_encode(req,s->payload,sizeof(s->payload)); if(rc!=OCPP_OK) return rc; return ocpp201_session_call(s,"MeterValues",s->payload); }
/**
 * ocpp201_session_send_notify_charging_limit - 桩发出 NotifyChargingLimit
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp201_session_send_notify_charging_limit(ocpp201_session_t *s, const ocpp201_notify_charging_limit_req_t *req) { ocpp_err_t rc=ocpp201_notify_charging_limit_req_encode(req,s->payload,sizeof(s->payload)); if(rc!=OCPP_OK) return rc; return ocpp201_session_call(s,"NotifyChargingLimit",s->payload); }
/**
 * ocpp201_session_send_notify_customer_information - 桩发出 NotifyCustomerInformation
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp201_session_send_notify_customer_information(ocpp201_session_t *s, const ocpp201_notify_customer_information_req_t *req) { ocpp_err_t rc=ocpp201_notify_customer_information_req_encode(req,s->payload,sizeof(s->payload)); if(rc!=OCPP_OK) return rc; return ocpp201_session_call(s,"NotifyCustomerInformation",s->payload); }
/**
 * ocpp201_session_send_notify_display_messages - 桩发出 NotifyDisplayMessages
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp201_session_send_notify_display_messages(ocpp201_session_t *s, const ocpp201_notify_display_messages_req_t *req) { ocpp_err_t rc=ocpp201_notify_display_messages_req_encode(req,s->payload,sizeof(s->payload)); if(rc!=OCPP_OK) return rc; return ocpp201_session_call(s,"NotifyDisplayMessages",s->payload); }
/**
 * ocpp201_session_send_notify_ev_charging_needs - 桩发出 NotifyEVChargingNeeds
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp201_session_send_notify_ev_charging_needs(ocpp201_session_t *s, const ocpp201_notify_ev_charging_needs_req_t *req) { ocpp_err_t rc=ocpp201_notify_ev_charging_needs_req_encode(req,s->payload,sizeof(s->payload)); if(rc!=OCPP_OK) return rc; return ocpp201_session_call(s,"NotifyEVChargingNeeds",s->payload); }
/**
 * ocpp201_session_send_notify_ev_charging_schedule - 桩发出 NotifyEVChargingSchedule
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp201_session_send_notify_ev_charging_schedule(ocpp201_session_t *s, const ocpp201_notify_ev_charging_schedule_req_t *req) { ocpp_err_t rc=ocpp201_notify_ev_charging_schedule_req_encode(req,s->payload,sizeof(s->payload)); if(rc!=OCPP_OK) return rc; return ocpp201_session_call(s,"NotifyEVChargingSchedule",s->payload); }
/**
 * ocpp201_session_send_notify_event - 桩发出 NotifyEvent
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp201_session_send_notify_event(ocpp201_session_t *s, const ocpp201_notify_event_req_t *req) { ocpp_err_t rc=ocpp201_notify_event_req_encode(req,s->payload,sizeof(s->payload)); if(rc!=OCPP_OK) return rc; return ocpp201_session_call(s,"NotifyEvent",s->payload); }
/**
 * ocpp201_session_send_notify_monitoring_report - 桩发出 NotifyMonitoringReport
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp201_session_send_notify_monitoring_report(ocpp201_session_t *s, const ocpp201_notify_monitoring_report_req_t *req) { ocpp_err_t rc=ocpp201_notify_monitoring_report_req_encode(req,s->payload,sizeof(s->payload)); if(rc!=OCPP_OK) return rc; return ocpp201_session_call(s,"NotifyMonitoringReport",s->payload); }
/**
 * ocpp201_session_send_notify_report - 桩发出 NotifyReport
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp201_session_send_notify_report(ocpp201_session_t *s, const ocpp201_notify_report_req_t *req) { ocpp_err_t rc=ocpp201_notify_report_req_encode(req,s->payload,sizeof(s->payload)); if(rc!=OCPP_OK) return rc; return ocpp201_session_call(s,"NotifyReport",s->payload); }
/**
 * ocpp201_session_send_publish_firmware_status_notification - 桩发出 PublishFirmwareStatusNotification
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp201_session_send_publish_firmware_status_notification(ocpp201_session_t *s, const ocpp201_publish_firmware_status_notification_req_t *req) { ocpp_err_t rc=ocpp201_publish_firmware_status_notification_req_encode(req,s->payload,sizeof(s->payload)); if(rc!=OCPP_OK) return rc; return ocpp201_session_call(s,"PublishFirmwareStatusNotification",s->payload); }
/**
 * ocpp201_session_send_report_charging_profiles - 桩发出 ReportChargingProfiles
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp201_session_send_report_charging_profiles(ocpp201_session_t *s, const ocpp201_report_charging_profiles_req_t *req) { ocpp_err_t rc=ocpp201_report_charging_profiles_req_encode(req,s->payload,sizeof(s->payload)); if(rc!=OCPP_OK) return rc; return ocpp201_session_call(s,"ReportChargingProfiles",s->payload); }
/**
 * ocpp201_session_send_reservation_status_update - 桩发出 ReservationStatusUpdate
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp201_session_send_reservation_status_update(ocpp201_session_t *s, const ocpp201_reservation_status_update_req_t *req) { ocpp_err_t rc=ocpp201_reservation_status_update_req_encode(req,s->payload,sizeof(s->payload)); if(rc!=OCPP_OK) return rc; return ocpp201_session_call(s,"ReservationStatusUpdate",s->payload); }
/**
 * ocpp201_session_send_security_event_notification - 桩发出 SecurityEventNotification
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp201_session_send_security_event_notification(ocpp201_session_t *s, const ocpp201_security_event_notification_req_t *req) { ocpp_err_t rc=ocpp201_security_event_notification_req_encode(req,s->payload,sizeof(s->payload)); if(rc!=OCPP_OK) return rc; return ocpp201_session_call(s,"SecurityEventNotification",s->payload); }
/**
 * ocpp201_session_send_sign_certificate - 桩发出 SignCertificate
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp201_session_send_sign_certificate(ocpp201_session_t *s, const ocpp201_sign_certificate_req_t *req) { ocpp_err_t rc=ocpp201_sign_certificate_req_encode(req,s->payload,sizeof(s->payload)); if(rc!=OCPP_OK) return rc; return ocpp201_session_call(s,"SignCertificate",s->payload); }
/**
 * ocpp201_session_send_status_notification - 桩发出 StatusNotification
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp201_session_send_status_notification(ocpp201_session_t *s, const ocpp201_status_notification_req_t *req) { ocpp_err_t rc=ocpp201_status_notification_req_encode(req,s->payload,sizeof(s->payload)); if(rc!=OCPP_OK) return rc; return ocpp201_session_call(s,"StatusNotification",s->payload); }
/**
 * ocpp201_session_send_transaction_event - 桩发出 TransactionEvent
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp201_session_send_transaction_event(ocpp201_session_t *s, const ocpp201_transaction_event_req_t *req) { ocpp_err_t rc=ocpp201_transaction_event_req_encode(req,s->payload,sizeof(s->payload)); if(rc!=OCPP_OK) return rc; return ocpp201_session_call(s,"TransactionEvent",s->payload); }
/* CSMS→桩 CALL：decode → handler（可改 conf）→ CALLRESULT；handler 非 0 则 InternalError。 */
/*
 * 处理 CSMS 发来的 CALL。控制类 action 在 accept_control 为 0 时回 SecurityError。
 * 回调返回非 0 则回 InternalError，没有回调就用示例 conf。
 */
static ocpp_err_t handle_call(ocpp201_session_t *s, ocpp_rpc_msg_t *msg) {
    /* 遥测链路：不跑 handler，避免第二家 CSMS Reset/启停。 */
    if (!s->link.accept_control && ocpp_action_is_control(msg->action)) {
        ocpp_rpc_pack_callerror(msg->unique_id,"SecurityError","this link does not accept control",s->frame,sizeof(s->frame));
        session_tx(s); return OCPP_OK;
    }
    if (strcmp(msg->action,"CancelReservation")==0) { ocpp201_cancel_reservation_req_t req; ocpp201_cancel_reservation_conf_t conf; ocpp201_cancel_reservation_req_from_json(msg->payload,&req); ocpp201_cancel_reservation_conf_example(&conf); if(s->handlers.cancel_reservation_req){ if(s->handlers.cancel_reservation_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_cancel_reservation_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"CertificateSigned")==0) { ocpp201_certificate_signed_req_t req; ocpp201_certificate_signed_conf_t conf; ocpp201_certificate_signed_req_from_json(msg->payload,&req); ocpp201_certificate_signed_conf_example(&conf); if(s->handlers.certificate_signed_req){ if(s->handlers.certificate_signed_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_certificate_signed_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"ChangeAvailability")==0) { ocpp201_change_availability_req_t req; ocpp201_change_availability_conf_t conf; ocpp201_change_availability_req_from_json(msg->payload,&req); ocpp201_change_availability_conf_example(&conf); if(s->handlers.change_availability_req){ if(s->handlers.change_availability_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_change_availability_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"ClearCache")==0) { ocpp201_clear_cache_req_t req; ocpp201_clear_cache_conf_t conf; ocpp201_clear_cache_req_from_json(msg->payload,&req); ocpp201_clear_cache_conf_example(&conf); if(s->handlers.clear_cache_req){ if(s->handlers.clear_cache_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_clear_cache_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"ClearChargingProfile")==0) { ocpp201_clear_charging_profile_req_t req; ocpp201_clear_charging_profile_conf_t conf; ocpp201_clear_charging_profile_req_from_json(msg->payload,&req); ocpp201_clear_charging_profile_conf_example(&conf); if(s->handlers.clear_charging_profile_req){ if(s->handlers.clear_charging_profile_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_clear_charging_profile_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"ClearDisplayMessage")==0) { ocpp201_clear_display_message_req_t req; ocpp201_clear_display_message_conf_t conf; ocpp201_clear_display_message_req_from_json(msg->payload,&req); ocpp201_clear_display_message_conf_example(&conf); if(s->handlers.clear_display_message_req){ if(s->handlers.clear_display_message_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_clear_display_message_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"ClearVariableMonitoring")==0) { ocpp201_clear_variable_monitoring_req_t req; ocpp201_clear_variable_monitoring_conf_t conf; ocpp201_clear_variable_monitoring_req_from_json(msg->payload,&req); ocpp201_clear_variable_monitoring_conf_example(&conf); if(s->handlers.clear_variable_monitoring_req){ if(s->handlers.clear_variable_monitoring_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_clear_variable_monitoring_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"CostUpdated")==0) { ocpp201_cost_updated_req_t req; ocpp201_cost_updated_conf_t conf; ocpp201_cost_updated_req_from_json(msg->payload,&req); ocpp201_cost_updated_conf_example(&conf); if(s->handlers.cost_updated_req){ if(s->handlers.cost_updated_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_cost_updated_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"CustomerInformation")==0) { ocpp201_customer_information_req_t req; ocpp201_customer_information_conf_t conf; ocpp201_customer_information_req_from_json(msg->payload,&req); ocpp201_customer_information_conf_example(&conf); if(s->handlers.customer_information_req){ if(s->handlers.customer_information_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_customer_information_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"DataTransfer")==0) { ocpp201_data_transfer_req_t req; ocpp201_data_transfer_conf_t conf; ocpp201_data_transfer_req_from_json(msg->payload,&req); ocpp201_data_transfer_conf_example(&conf); if(s->handlers.data_transfer_req){ if(s->handlers.data_transfer_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_data_transfer_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"DeleteCertificate")==0) { ocpp201_delete_certificate_req_t req; ocpp201_delete_certificate_conf_t conf; ocpp201_delete_certificate_req_from_json(msg->payload,&req); ocpp201_delete_certificate_conf_example(&conf); if(s->handlers.delete_certificate_req){ if(s->handlers.delete_certificate_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_delete_certificate_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"GetBaseReport")==0) { ocpp201_get_base_report_req_t req; ocpp201_get_base_report_conf_t conf; ocpp201_get_base_report_req_from_json(msg->payload,&req); ocpp201_get_base_report_conf_example(&conf); if(s->handlers.get_base_report_req){ if(s->handlers.get_base_report_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_get_base_report_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"GetChargingProfiles")==0) { ocpp201_get_charging_profiles_req_t req; ocpp201_get_charging_profiles_conf_t conf; ocpp201_get_charging_profiles_req_from_json(msg->payload,&req); ocpp201_get_charging_profiles_conf_example(&conf); if(s->handlers.get_charging_profiles_req){ if(s->handlers.get_charging_profiles_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_get_charging_profiles_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"GetCompositeSchedule")==0) { ocpp201_get_composite_schedule_req_t req; ocpp201_get_composite_schedule_conf_t conf; ocpp201_get_composite_schedule_req_from_json(msg->payload,&req); ocpp201_get_composite_schedule_conf_example(&conf); if(s->handlers.get_composite_schedule_req){ if(s->handlers.get_composite_schedule_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_get_composite_schedule_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"GetDisplayMessages")==0) { ocpp201_get_display_messages_req_t req; ocpp201_get_display_messages_conf_t conf; ocpp201_get_display_messages_req_from_json(msg->payload,&req); ocpp201_get_display_messages_conf_example(&conf); if(s->handlers.get_display_messages_req){ if(s->handlers.get_display_messages_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_get_display_messages_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"GetInstalledCertificateIds")==0) { ocpp201_get_installed_certificate_ids_req_t req; ocpp201_get_installed_certificate_ids_conf_t conf; ocpp201_get_installed_certificate_ids_req_from_json(msg->payload,&req); ocpp201_get_installed_certificate_ids_conf_example(&conf); if(s->handlers.get_installed_certificate_ids_req){ if(s->handlers.get_installed_certificate_ids_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_get_installed_certificate_ids_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"GetLocalListVersion")==0) { ocpp201_get_local_list_version_req_t req; ocpp201_get_local_list_version_conf_t conf; ocpp201_get_local_list_version_req_from_json(msg->payload,&req); ocpp201_get_local_list_version_conf_example(&conf); if(s->handlers.get_local_list_version_req){ if(s->handlers.get_local_list_version_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_get_local_list_version_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"GetLog")==0) { ocpp201_get_log_req_t req; ocpp201_get_log_conf_t conf; ocpp201_get_log_req_from_json(msg->payload,&req); ocpp201_get_log_conf_example(&conf); if(s->handlers.get_log_req){ if(s->handlers.get_log_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_get_log_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"GetMonitoringReport")==0) { ocpp201_get_monitoring_report_req_t req; ocpp201_get_monitoring_report_conf_t conf; ocpp201_get_monitoring_report_req_from_json(msg->payload,&req); ocpp201_get_monitoring_report_conf_example(&conf); if(s->handlers.get_monitoring_report_req){ if(s->handlers.get_monitoring_report_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_get_monitoring_report_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"GetReport")==0) { ocpp201_get_report_req_t req; ocpp201_get_report_conf_t conf; ocpp201_get_report_req_from_json(msg->payload,&req); ocpp201_get_report_conf_example(&conf); if(s->handlers.get_report_req){ if(s->handlers.get_report_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_get_report_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"GetTransactionStatus")==0) { ocpp201_get_transaction_status_req_t req; ocpp201_get_transaction_status_conf_t conf; ocpp201_get_transaction_status_req_from_json(msg->payload,&req); ocpp201_get_transaction_status_conf_example(&conf); if(s->handlers.get_transaction_status_req){ if(s->handlers.get_transaction_status_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_get_transaction_status_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"GetVariables")==0) { ocpp201_get_variables_req_t req; ocpp201_get_variables_conf_t conf; ocpp201_get_variables_req_from_json(msg->payload,&req); ocpp201_get_variables_conf_example(&conf); if(s->handlers.get_variables_req){ if(s->handlers.get_variables_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_get_variables_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"InstallCertificate")==0) { ocpp201_install_certificate_req_t req; ocpp201_install_certificate_conf_t conf; ocpp201_install_certificate_req_from_json(msg->payload,&req); ocpp201_install_certificate_conf_example(&conf); if(s->handlers.install_certificate_req){ if(s->handlers.install_certificate_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_install_certificate_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"PublishFirmware")==0) { ocpp201_publish_firmware_req_t req; ocpp201_publish_firmware_conf_t conf; ocpp201_publish_firmware_req_from_json(msg->payload,&req); ocpp201_publish_firmware_conf_example(&conf); if(s->handlers.publish_firmware_req){ if(s->handlers.publish_firmware_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_publish_firmware_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"RequestStartTransaction")==0) { ocpp201_request_start_transaction_req_t req; ocpp201_request_start_transaction_conf_t conf; ocpp201_request_start_transaction_req_from_json(msg->payload,&req); ocpp201_request_start_transaction_conf_example(&conf); if(s->handlers.request_start_transaction_req){ if(s->handlers.request_start_transaction_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_request_start_transaction_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"RequestStopTransaction")==0) { ocpp201_request_stop_transaction_req_t req; ocpp201_request_stop_transaction_conf_t conf; ocpp201_request_stop_transaction_req_from_json(msg->payload,&req); ocpp201_request_stop_transaction_conf_example(&conf); if(s->handlers.request_stop_transaction_req){ if(s->handlers.request_stop_transaction_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_request_stop_transaction_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"ReserveNow")==0) { ocpp201_reserve_now_req_t req; ocpp201_reserve_now_conf_t conf; ocpp201_reserve_now_req_from_json(msg->payload,&req); ocpp201_reserve_now_conf_example(&conf); if(s->handlers.reserve_now_req){ if(s->handlers.reserve_now_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_reserve_now_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"Reset")==0) { ocpp201_reset_req_t req; ocpp201_reset_conf_t conf; ocpp201_reset_req_from_json(msg->payload,&req); ocpp201_reset_conf_example(&conf); if(s->handlers.reset_req){ if(s->handlers.reset_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_reset_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"SendLocalList")==0) { ocpp201_send_local_list_req_t req; ocpp201_send_local_list_conf_t conf; ocpp201_send_local_list_req_from_json(msg->payload,&req); ocpp201_send_local_list_conf_example(&conf); if(s->handlers.send_local_list_req){ if(s->handlers.send_local_list_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_send_local_list_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"SetChargingProfile")==0) { ocpp201_set_charging_profile_req_t req; ocpp201_set_charging_profile_conf_t conf; ocpp201_set_charging_profile_req_from_json(msg->payload,&req); ocpp201_set_charging_profile_conf_example(&conf); if(s->handlers.set_charging_profile_req){ if(s->handlers.set_charging_profile_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_set_charging_profile_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"SetDisplayMessage")==0) { ocpp201_set_display_message_req_t req; ocpp201_set_display_message_conf_t conf; ocpp201_set_display_message_req_from_json(msg->payload,&req); ocpp201_set_display_message_conf_example(&conf); if(s->handlers.set_display_message_req){ if(s->handlers.set_display_message_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_set_display_message_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"SetMonitoringBase")==0) { ocpp201_set_monitoring_base_req_t req; ocpp201_set_monitoring_base_conf_t conf; ocpp201_set_monitoring_base_req_from_json(msg->payload,&req); ocpp201_set_monitoring_base_conf_example(&conf); if(s->handlers.set_monitoring_base_req){ if(s->handlers.set_monitoring_base_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_set_monitoring_base_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"SetMonitoringLevel")==0) { ocpp201_set_monitoring_level_req_t req; ocpp201_set_monitoring_level_conf_t conf; ocpp201_set_monitoring_level_req_from_json(msg->payload,&req); ocpp201_set_monitoring_level_conf_example(&conf); if(s->handlers.set_monitoring_level_req){ if(s->handlers.set_monitoring_level_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_set_monitoring_level_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"SetNetworkProfile")==0) { ocpp201_set_network_profile_req_t req; ocpp201_set_network_profile_conf_t conf; ocpp201_set_network_profile_req_from_json(msg->payload,&req); ocpp201_set_network_profile_conf_example(&conf); if(s->handlers.set_network_profile_req){ if(s->handlers.set_network_profile_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_set_network_profile_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"SetVariableMonitoring")==0) { ocpp201_set_variable_monitoring_req_t req; ocpp201_set_variable_monitoring_conf_t conf; ocpp201_set_variable_monitoring_req_from_json(msg->payload,&req); ocpp201_set_variable_monitoring_conf_example(&conf); if(s->handlers.set_variable_monitoring_req){ if(s->handlers.set_variable_monitoring_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_set_variable_monitoring_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"SetVariables")==0) { ocpp201_set_variables_req_t req; ocpp201_set_variables_conf_t conf; ocpp201_set_variables_req_from_json(msg->payload,&req); ocpp201_set_variables_conf_example(&conf); if(s->handlers.set_variables_req){ if(s->handlers.set_variables_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_set_variables_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"TriggerMessage")==0) { ocpp201_trigger_message_req_t req; ocpp201_trigger_message_conf_t conf; ocpp201_trigger_message_req_from_json(msg->payload,&req); ocpp201_trigger_message_conf_example(&conf); if(s->handlers.trigger_message_req){ if(s->handlers.trigger_message_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_trigger_message_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"UnlockConnector")==0) { ocpp201_unlock_connector_req_t req; ocpp201_unlock_connector_conf_t conf; ocpp201_unlock_connector_req_from_json(msg->payload,&req); ocpp201_unlock_connector_conf_example(&conf); if(s->handlers.unlock_connector_req){ if(s->handlers.unlock_connector_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_unlock_connector_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"UnpublishFirmware")==0) { ocpp201_unpublish_firmware_req_t req; ocpp201_unpublish_firmware_conf_t conf; ocpp201_unpublish_firmware_req_from_json(msg->payload,&req); ocpp201_unpublish_firmware_conf_example(&conf); if(s->handlers.unpublish_firmware_req){ if(s->handlers.unpublish_firmware_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_unpublish_firmware_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    else if (strcmp(msg->action,"UpdateFirmware")==0) { ocpp201_update_firmware_req_t req; ocpp201_update_firmware_conf_t conf; ocpp201_update_firmware_req_from_json(msg->payload,&req); ocpp201_update_firmware_conf_example(&conf); if(s->handlers.update_firmware_req){ if(s->handlers.update_firmware_req(&req,&conf,s->handlers.user)!=0){ ocpp_rpc_pack_callerror(msg->unique_id,"InternalError","handler",s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; } } ocpp201_update_firmware_conf_encode(&conf,s->payload,sizeof(s->payload)); ocpp_rpc_pack_callresult(msg->unique_id,s->payload,s->frame,sizeof(s->frame)); if(session_tx(s)!=0) return OCPP_ERR_IO; return OCPP_OK; }
    ocpp_rpc_pack_callerror(msg->unique_id,"NotImplemented",msg->action,s->frame,sizeof(s->frame)); session_tx(s); return OCPP_OK; }
/* 桩发出 CALL 的 CALLRESULT：按 pending.action 分发 *_conf 回调。Boot 成功则启心跳。 */
/*
 * 处理本端 CALL 的 CALLRESULT。Boot 应答会启动本路心跳，不判断 status 是不是 Accepted。
 */
static void handle_result(ocpp201_session_t *s, const char *action, const cJSON *payload) {
    if (strcmp(action,"Authorize")==0) { ocpp201_authorize_conf_t conf; ocpp201_authorize_conf_from_json(payload,&conf); if(s->handlers.authorize_conf) s->handlers.authorize_conf(&conf,s->handlers.user); return; }
    else if (strcmp(action,"BootNotification")==0) {
        ocpp201_boot_notification_conf_t conf;
        ocpp201_boot_notification_conf_from_json(payload, &conf);
        {
            int accepted = strcmp(conf.status, "Accepted") == 0;
            s->registered = accepted ? 1 : 0;
            if (conf.interval > 0) s->heartbeat_interval_s = conf.interval;
            /* Accepted 才周期心跳。未批准时 interval 是下一次 Boot 的等待时间。 */
            if (s->link.heartbeat_timer_id >= 0) {
                if (accepted) {
                    ocpp_port_timer_start(s->link.heartbeat_timer_id, (uint32_t)s->heartbeat_interval_s * 1000u, 1, heartbeat_timer, s);
                } else if (s->has_boot_req && s->heartbeat_interval_s > 0) {
                    ocpp_port_timer_start(s->link.heartbeat_timer_id, (uint32_t)s->heartbeat_interval_s * 1000u, 0, boot_retry_timer, s);
                } else {
                    ocpp_port_timer_stop(s->link.heartbeat_timer_id);
                }
            }
        }
        if (s->handlers.boot_notification_conf) s->handlers.boot_notification_conf(&conf, s->handlers.user);
        return;
    }
    else if (strcmp(action,"ClearedChargingLimit")==0) { ocpp201_cleared_charging_limit_conf_t conf; ocpp201_cleared_charging_limit_conf_from_json(payload,&conf); if(s->handlers.cleared_charging_limit_conf) s->handlers.cleared_charging_limit_conf(&conf,s->handlers.user); return; }
    else if (strcmp(action,"DataTransfer")==0) { ocpp201_data_transfer_conf_t conf; ocpp201_data_transfer_conf_from_json(payload,&conf); if(s->handlers.data_transfer_conf) s->handlers.data_transfer_conf(&conf,s->handlers.user); return; }
    else if (strcmp(action,"FirmwareStatusNotification")==0) { ocpp201_firmware_status_notification_conf_t conf; ocpp201_firmware_status_notification_conf_from_json(payload,&conf); if(s->handlers.firmware_status_notification_conf) s->handlers.firmware_status_notification_conf(&conf,s->handlers.user); return; }
    else if (strcmp(action,"Get15118EVCertificate")==0) { ocpp201_get15118_ev_certificate_conf_t conf; ocpp201_get15118_ev_certificate_conf_from_json(payload,&conf); if(s->handlers.get15118_ev_certificate_conf) s->handlers.get15118_ev_certificate_conf(&conf,s->handlers.user); return; }
    else if (strcmp(action,"GetCertificateStatus")==0) { ocpp201_get_certificate_status_conf_t conf; ocpp201_get_certificate_status_conf_from_json(payload,&conf); if(s->handlers.get_certificate_status_conf) s->handlers.get_certificate_status_conf(&conf,s->handlers.user); return; }
    else if (strcmp(action,"Heartbeat")==0) { ocpp201_heartbeat_conf_t conf; ocpp201_heartbeat_conf_from_json(payload,&conf); if(s->handlers.heartbeat_conf) s->handlers.heartbeat_conf(&conf,s->handlers.user); return; }
    else if (strcmp(action,"LogStatusNotification")==0) { ocpp201_log_status_notification_conf_t conf; ocpp201_log_status_notification_conf_from_json(payload,&conf); if(s->handlers.log_status_notification_conf) s->handlers.log_status_notification_conf(&conf,s->handlers.user); return; }
    else if (strcmp(action,"MeterValues")==0) { ocpp201_meter_values_conf_t conf; ocpp201_meter_values_conf_from_json(payload,&conf); if(s->handlers.meter_values_conf) s->handlers.meter_values_conf(&conf,s->handlers.user); return; }
    else if (strcmp(action,"NotifyChargingLimit")==0) { ocpp201_notify_charging_limit_conf_t conf; ocpp201_notify_charging_limit_conf_from_json(payload,&conf); if(s->handlers.notify_charging_limit_conf) s->handlers.notify_charging_limit_conf(&conf,s->handlers.user); return; }
    else if (strcmp(action,"NotifyCustomerInformation")==0) { ocpp201_notify_customer_information_conf_t conf; ocpp201_notify_customer_information_conf_from_json(payload,&conf); if(s->handlers.notify_customer_information_conf) s->handlers.notify_customer_information_conf(&conf,s->handlers.user); return; }
    else if (strcmp(action,"NotifyDisplayMessages")==0) { ocpp201_notify_display_messages_conf_t conf; ocpp201_notify_display_messages_conf_from_json(payload,&conf); if(s->handlers.notify_display_messages_conf) s->handlers.notify_display_messages_conf(&conf,s->handlers.user); return; }
    else if (strcmp(action,"NotifyEVChargingNeeds")==0) { ocpp201_notify_ev_charging_needs_conf_t conf; ocpp201_notify_ev_charging_needs_conf_from_json(payload,&conf); if(s->handlers.notify_ev_charging_needs_conf) s->handlers.notify_ev_charging_needs_conf(&conf,s->handlers.user); return; }
    else if (strcmp(action,"NotifyEVChargingSchedule")==0) { ocpp201_notify_ev_charging_schedule_conf_t conf; ocpp201_notify_ev_charging_schedule_conf_from_json(payload,&conf); if(s->handlers.notify_ev_charging_schedule_conf) s->handlers.notify_ev_charging_schedule_conf(&conf,s->handlers.user); return; }
    else if (strcmp(action,"NotifyEvent")==0) { ocpp201_notify_event_conf_t conf; ocpp201_notify_event_conf_from_json(payload,&conf); if(s->handlers.notify_event_conf) s->handlers.notify_event_conf(&conf,s->handlers.user); return; }
    else if (strcmp(action,"NotifyMonitoringReport")==0) { ocpp201_notify_monitoring_report_conf_t conf; ocpp201_notify_monitoring_report_conf_from_json(payload,&conf); if(s->handlers.notify_monitoring_report_conf) s->handlers.notify_monitoring_report_conf(&conf,s->handlers.user); return; }
    else if (strcmp(action,"NotifyReport")==0) { ocpp201_notify_report_conf_t conf; ocpp201_notify_report_conf_from_json(payload,&conf); if(s->handlers.notify_report_conf) s->handlers.notify_report_conf(&conf,s->handlers.user); return; }
    else if (strcmp(action,"PublishFirmwareStatusNotification")==0) { ocpp201_publish_firmware_status_notification_conf_t conf; ocpp201_publish_firmware_status_notification_conf_from_json(payload,&conf); if(s->handlers.publish_firmware_status_notification_conf) s->handlers.publish_firmware_status_notification_conf(&conf,s->handlers.user); return; }
    else if (strcmp(action,"ReportChargingProfiles")==0) { ocpp201_report_charging_profiles_conf_t conf; ocpp201_report_charging_profiles_conf_from_json(payload,&conf); if(s->handlers.report_charging_profiles_conf) s->handlers.report_charging_profiles_conf(&conf,s->handlers.user); return; }
    else if (strcmp(action,"ReservationStatusUpdate")==0) { ocpp201_reservation_status_update_conf_t conf; ocpp201_reservation_status_update_conf_from_json(payload,&conf); if(s->handlers.reservation_status_update_conf) s->handlers.reservation_status_update_conf(&conf,s->handlers.user); return; }
    else if (strcmp(action,"SecurityEventNotification")==0) { ocpp201_security_event_notification_conf_t conf; ocpp201_security_event_notification_conf_from_json(payload,&conf); if(s->handlers.security_event_notification_conf) s->handlers.security_event_notification_conf(&conf,s->handlers.user); return; }
    else if (strcmp(action,"SignCertificate")==0) { ocpp201_sign_certificate_conf_t conf; ocpp201_sign_certificate_conf_from_json(payload,&conf); if(s->handlers.sign_certificate_conf) s->handlers.sign_certificate_conf(&conf,s->handlers.user); return; }
    else if (strcmp(action,"StatusNotification")==0) { ocpp201_status_notification_conf_t conf; ocpp201_status_notification_conf_from_json(payload,&conf); if(s->handlers.status_notification_conf) s->handlers.status_notification_conf(&conf,s->handlers.user); return; }
    else if (strcmp(action,"TransactionEvent")==0) { ocpp201_transaction_event_conf_t conf; ocpp201_transaction_event_conf_from_json(payload,&conf); if(s->handlers.transaction_event_conf) s->handlers.transaction_event_conf(&conf,s->handlers.user); return; }
}
/**
 * ocpp201_session_rx - 喂入一条完整的 WebSocket 文本帧
 * @s: 本路会话
 * @frame: 一整帧 JSON，不要半包
 * @len: 字节数
 *
 * 分发规则与 ocpp16_session_rx 相同。返回前释放 unpack 摘出的 payload。
 */
ocpp_err_t ocpp201_session_rx(ocpp201_session_t *s, const char *frame, size_t len) {
    ocpp_rpc_msg_t msg;
    ocpp_err_t rc = ocpp_rpc_unpack(frame, len, &msg);
    if (rc != OCPP_OK) return rc;
    if (msg.type == OCPP_RPC_CALL) {
        rc = handle_call(s, &msg);
    } else if (msg.type == OCPP_RPC_CALLRESULT) {
        rc = OCPP_ERR_NOTFOUND;
        for (int i = 0; i < OCPP201_PENDING_MAX; i++) {
            if (s->pending[i].used && strcmp(s->pending[i].uid, msg.unique_id) == 0) {
                s->pending[i].used = 0;
                handle_result(s, s->pending[i].action, msg.payload);
                rc = OCPP_OK;
                break;
            }
        }
    }
    /* unpack 摘下的 payload 还在堆上，用完交给 free。 */
    cJSON_Delete(msg.payload);
    return rc;
}
