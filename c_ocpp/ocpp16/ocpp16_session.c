#include "ocpp16_session.h"
#include "ocpp_port.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* 仅 Boot 已被接受后使用。到期时发一帧 Heartbeat。 */
static void heartbeat_timer(int id, void *ctx) {
    ocpp16_session_t *s = (ocpp16_session_t *)ctx;
    (void)id;
    ocpp16_heartbeat_req_t req;
    ocpp16_heartbeat_req_example(&req);
    ocpp16_session_send_heartbeat(s, &req);
}

/* Pending/Rejected 的一次性定时器。到点重发上次的 BootNotification。 */
static void boot_retry_timer(int id, void *ctx) {
    ocpp16_session_t *s = (ocpp16_session_t *)ctx;
    (void)id;
    if (s->has_boot_req) {
        ocpp16_session_send_boot_notification(s, &s->boot_req);
    }
}

/**
 * ocpp16_session_bind - 换掉这一路的发送函数、心跳槽或控制权
 * @s: 已初始化的 session
 * @link: 新链路。NULL 时保持原样
 *
 * 主备切换时用来改 accept_control，不必把 pending 清掉。
 */
void ocpp16_session_bind(ocpp16_session_t *s, const ocpp_link_t *link) {
    if (link) {
        s->link = *link;
    }
}

/**
 * ocpp16_session_init - 清零并绑定一路 1.6 会话
 * @s: 调用方提供的对象，应放在 BSS，不要放任务栈
 * @h: 业务回调。NULL 表示全部用库内示例应答
 * @link: 这一路的发送函数。NULL 时走全局 ocpp_port_send，心跳槽用 0，accept_control 为 1
 *
 * uniqueId 从 1 开始。默认心跳间隔 300 秒，Boot 应答里的 interval 会覆盖它。
 */
void ocpp16_session_init(ocpp16_session_t *s, const ocpp16_handlers_t *h, const ocpp_link_t *link) {
    memset(s, 0, sizeof(*s));
    s->seq = 1;
    s->heartbeat_interval_s = 300;
    s->link.accept_control = 1;
    s->link.heartbeat_timer_id = OCPP_PORT_TIMER_HEARTBEAT;
    if (h) s->handlers = *h;
    ocpp16_session_bind(s, link);
}

/* 把 s->frame 里已经组好的文本交给这一路的 send。0 成功。 */
static int session_tx(ocpp16_session_t *s) {
    return ocpp_link_send(&s->link, s->frame, strlen(s->frame));
}

/* 记下尚未收到 CALLRESULT 的 uniqueId。表满返回 -1。 */
static int pending_add(ocpp16_session_t *s, const char *uid, const char *action) {
    for (int i = 0; i < OCPP16_PENDING_MAX; i++) if (!s->pending[i].used) {
        s->pending[i].used = 1;
        ocpp_str_copy(s->pending[i].uid, sizeof(s->pending[i].uid), uid);
        ocpp_str_copy(s->pending[i].action, sizeof(s->pending[i].action), action);
        return 0;
    }
    return -1;
}

/* 发送失败时撤掉刚记下的 pending，避免槽被一条没发出去的 CALL 占死。 */
static void pending_drop(ocpp16_session_t *s, const char *uid) {
    for (int i = 0; i < OCPP16_PENDING_MAX; i++) {
        if (s->pending[i].used && strcmp(s->pending[i].uid, uid) == 0) {
            s->pending[i].used = 0;
            return;
        }
    }
}

/**
 * ocpp16_session_call - 发出一条 CALL
 * @s: 本路会话
 * @action: OCPP action 名
 * @payload_json: 已编好的对象文本，一般在 s->payload 里
 *
 * uniqueId 用本路递增序号，并记入 pending。对端的 CALLRESULT 靠这个序号找回 action。
 * 返回 OCPP_OK。组帧失败、pending 满或 send 失败时返回对应的负错误码。
 */
ocpp_err_t ocpp16_session_call(ocpp16_session_t *s, const char *action, const char *payload_json) {
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
 * ocpp16_session_send_authorize - 桩发出 Authorize
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp16_session_send_authorize(ocpp16_session_t *s, const ocpp16_authorize_req_t *req) {
    ocpp_err_t rc = ocpp16_authorize_req_encode(req, s->payload, sizeof(s->payload));
    if (rc != OCPP_OK) return rc;
    return ocpp16_session_call(s, "Authorize", s->payload);
}

/**
 * ocpp16_session_send_boot_notification - 桩发出 BootNotification
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp16_session_send_boot_notification(ocpp16_session_t *s, const ocpp16_boot_notification_req_t *req) {
    /* 留下请求。Pending/Rejected 到点要原样重发，不能改用 example。 */
    if (req != NULL) {
        s->boot_req = *req;
        s->has_boot_req = 1;
    }
    ocpp_err_t rc = ocpp16_boot_notification_req_encode(req, s->payload, sizeof(s->payload));
    if (rc != OCPP_OK) return rc;
    return ocpp16_session_call(s, "BootNotification", s->payload);
}

/**
 * ocpp16_session_send_data_transfer - 桩发出 DataTransfer
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp16_session_send_data_transfer(ocpp16_session_t *s, const ocpp16_data_transfer_req_t *req) {
    ocpp_err_t rc = ocpp16_data_transfer_req_encode(req, s->payload, sizeof(s->payload));
    if (rc != OCPP_OK) return rc;
    return ocpp16_session_call(s, "DataTransfer", s->payload);
}

/**
 * ocpp16_session_send_diagnostics_status_notification - 桩发出 DiagnosticsStatusNotification
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp16_session_send_diagnostics_status_notification(ocpp16_session_t *s, const ocpp16_diagnostics_status_notification_req_t *req) {
    ocpp_err_t rc = ocpp16_diagnostics_status_notification_req_encode(req, s->payload, sizeof(s->payload));
    if (rc != OCPP_OK) return rc;
    return ocpp16_session_call(s, "DiagnosticsStatusNotification", s->payload);
}

/**
 * ocpp16_session_send_firmware_status_notification - 桩发出 FirmwareStatusNotification
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp16_session_send_firmware_status_notification(ocpp16_session_t *s, const ocpp16_firmware_status_notification_req_t *req) {
    ocpp_err_t rc = ocpp16_firmware_status_notification_req_encode(req, s->payload, sizeof(s->payload));
    if (rc != OCPP_OK) return rc;
    return ocpp16_session_call(s, "FirmwareStatusNotification", s->payload);
}

/**
 * ocpp16_session_send_heartbeat - 桩发出 Heartbeat
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp16_session_send_heartbeat(ocpp16_session_t *s, const ocpp16_heartbeat_req_t *req) {
    ocpp_err_t rc = ocpp16_heartbeat_req_encode(req, s->payload, sizeof(s->payload));
    if (rc != OCPP_OK) return rc;
    return ocpp16_session_call(s, "Heartbeat", s->payload);
}

/**
 * ocpp16_session_send_meter_values - 桩发出 MeterValues
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp16_session_send_meter_values(ocpp16_session_t *s, const ocpp16_meter_values_req_t *req) {
    ocpp_err_t rc = ocpp16_meter_values_req_encode(req, s->payload, sizeof(s->payload));
    if (rc != OCPP_OK) return rc;
    return ocpp16_session_call(s, "MeterValues", s->payload);
}

/**
 * ocpp16_session_send_start_transaction - 桩发出 StartTransaction
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp16_session_send_start_transaction(ocpp16_session_t *s, const ocpp16_start_transaction_req_t *req) {
    ocpp_err_t rc = ocpp16_start_transaction_req_encode(req, s->payload, sizeof(s->payload));
    if (rc != OCPP_OK) return rc;
    return ocpp16_session_call(s, "StartTransaction", s->payload);
}

/**
 * ocpp16_session_send_status_notification - 桩发出 StatusNotification
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp16_session_send_status_notification(ocpp16_session_t *s, const ocpp16_status_notification_req_t *req) {
    ocpp_err_t rc = ocpp16_status_notification_req_encode(req, s->payload, sizeof(s->payload));
    if (rc != OCPP_OK) return rc;
    return ocpp16_session_call(s, "StatusNotification", s->payload);
}

/**
 * ocpp16_session_send_stop_transaction - 桩发出 StopTransaction
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp16_session_send_stop_transaction(ocpp16_session_t *s, const ocpp16_stop_transaction_req_t *req) {
    ocpp_err_t rc = ocpp16_stop_transaction_req_encode(req, s->payload, sizeof(s->payload));
    if (rc != OCPP_OK) return rc;
    return ocpp16_session_call(s, "StopTransaction", s->payload);
}

/**
 * ocpp16_session_send_log_status_notification - 桩发出 LogStatusNotification
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp16_session_send_log_status_notification(ocpp16_session_t *s, const ocpp16_log_status_notification_req_t *req) {
    ocpp_err_t rc = ocpp16_log_status_notification_req_encode(req, s->payload, sizeof(s->payload));
    if (rc != OCPP_OK) return rc;
    return ocpp16_session_call(s, "LogStatusNotification", s->payload);
}

/**
 * ocpp16_session_send_security_event_notification - 桩发出 SecurityEventNotification
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp16_session_send_security_event_notification(ocpp16_session_t *s, const ocpp16_security_event_notification_req_t *req) {
    ocpp_err_t rc = ocpp16_security_event_notification_req_encode(req, s->payload, sizeof(s->payload));
    if (rc != OCPP_OK) return rc;
    return ocpp16_session_call(s, "SecurityEventNotification", s->payload);
}

/**
 * ocpp16_session_send_sign_certificate - 桩发出 SignCertificate
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp16_session_send_sign_certificate(ocpp16_session_t *s, const ocpp16_sign_certificate_req_t *req) {
    ocpp_err_t rc = ocpp16_sign_certificate_req_encode(req, s->payload, sizeof(s->payload));
    if (rc != OCPP_OK) return rc;
    return ocpp16_session_call(s, "SignCertificate", s->payload);
}

/**
 * ocpp16_session_send_signed_firmware_status_notification - 桩发出 SignedFirmwareStatusNotification
 * @s: 本路会话
 * @req: 已填好的请求。encode 写入 s->payload
 *
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp16_session_send_signed_firmware_status_notification(ocpp16_session_t *s, const ocpp16_signed_firmware_status_notification_req_t *req) {
    ocpp_err_t rc = ocpp16_signed_firmware_status_notification_req_encode(req, s->payload, sizeof(s->payload));
    if (rc != OCPP_OK) return rc;
    return ocpp16_session_call(s, "SignedFirmwareStatusNotification", s->payload);
}

/*
 * 处理 CSMS 发来的 CALL。
 * accept_control 为 0 且 action 会改桩状态时，直接回 SecurityError。
 * 有回调就让回调填 conf；回调返回非 0 则回 InternalError。没有回调就用示例 conf。
 */
static ocpp_err_t handle_call(ocpp16_session_t *s, ocpp_rpc_msg_t *msg) {
    /* 遥测链路：不跑 handler，避免第二家 CSMS Reset/启停。 */
    if (!s->link.accept_control && ocpp_action_is_control(msg->action)) {
        ocpp_rpc_pack_callerror(msg->unique_id, "SecurityError", "this link does not accept control", s->frame, sizeof(s->frame));
        session_tx(s);
        return OCPP_OK;
    }
    if (strcmp(msg->action, "DataTransfer") == 0) {
        ocpp16_data_transfer_req_t req; ocpp16_data_transfer_conf_t conf;
        ocpp16_data_transfer_req_from_json(msg->payload, &req);
        ocpp16_data_transfer_conf_example(&conf);
        if (s->handlers.data_transfer_req) {
            if (s->handlers.data_transfer_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_data_transfer_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    else if (strcmp(msg->action, "CancelReservation") == 0) {
        ocpp16_cancel_reservation_req_t req; ocpp16_cancel_reservation_conf_t conf;
        ocpp16_cancel_reservation_req_from_json(msg->payload, &req);
        ocpp16_cancel_reservation_conf_example(&conf);
        if (s->handlers.cancel_reservation_req) {
            if (s->handlers.cancel_reservation_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_cancel_reservation_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    else if (strcmp(msg->action, "ChangeAvailability") == 0) {
        ocpp16_change_availability_req_t req; ocpp16_change_availability_conf_t conf;
        ocpp16_change_availability_req_from_json(msg->payload, &req);
        ocpp16_change_availability_conf_example(&conf);
        if (s->handlers.change_availability_req) {
            if (s->handlers.change_availability_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_change_availability_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    else if (strcmp(msg->action, "ChangeConfiguration") == 0) {
        ocpp16_change_configuration_req_t req; ocpp16_change_configuration_conf_t conf;
        ocpp16_change_configuration_req_from_json(msg->payload, &req);
        ocpp16_change_configuration_conf_example(&conf);
        if (s->handlers.change_configuration_req) {
            if (s->handlers.change_configuration_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_change_configuration_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    else if (strcmp(msg->action, "ClearCache") == 0) {
        ocpp16_clear_cache_req_t req; ocpp16_clear_cache_conf_t conf;
        ocpp16_clear_cache_req_from_json(msg->payload, &req);
        ocpp16_clear_cache_conf_example(&conf);
        if (s->handlers.clear_cache_req) {
            if (s->handlers.clear_cache_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_clear_cache_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    else if (strcmp(msg->action, "ClearChargingProfile") == 0) {
        ocpp16_clear_charging_profile_req_t req; ocpp16_clear_charging_profile_conf_t conf;
        ocpp16_clear_charging_profile_req_from_json(msg->payload, &req);
        ocpp16_clear_charging_profile_conf_example(&conf);
        if (s->handlers.clear_charging_profile_req) {
            if (s->handlers.clear_charging_profile_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_clear_charging_profile_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    else if (strcmp(msg->action, "GetCompositeSchedule") == 0) {
        ocpp16_get_composite_schedule_req_t req; ocpp16_get_composite_schedule_conf_t conf;
        ocpp16_get_composite_schedule_req_from_json(msg->payload, &req);
        ocpp16_get_composite_schedule_conf_example(&conf);
        if (s->handlers.get_composite_schedule_req) {
            if (s->handlers.get_composite_schedule_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_get_composite_schedule_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    else if (strcmp(msg->action, "GetConfiguration") == 0) {
        ocpp16_get_configuration_req_t req; ocpp16_get_configuration_conf_t conf;
        ocpp16_get_configuration_req_from_json(msg->payload, &req);
        ocpp16_get_configuration_conf_example(&conf);
        if (s->handlers.get_configuration_req) {
            if (s->handlers.get_configuration_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_get_configuration_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    else if (strcmp(msg->action, "GetDiagnostics") == 0) {
        ocpp16_get_diagnostics_req_t req; ocpp16_get_diagnostics_conf_t conf;
        ocpp16_get_diagnostics_req_from_json(msg->payload, &req);
        ocpp16_get_diagnostics_conf_example(&conf);
        if (s->handlers.get_diagnostics_req) {
            if (s->handlers.get_diagnostics_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_get_diagnostics_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    else if (strcmp(msg->action, "GetLocalListVersion") == 0) {
        ocpp16_get_local_list_version_req_t req; ocpp16_get_local_list_version_conf_t conf;
        ocpp16_get_local_list_version_req_from_json(msg->payload, &req);
        ocpp16_get_local_list_version_conf_example(&conf);
        if (s->handlers.get_local_list_version_req) {
            if (s->handlers.get_local_list_version_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_get_local_list_version_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    else if (strcmp(msg->action, "RemoteStartTransaction") == 0) {
        ocpp16_remote_start_transaction_req_t req; ocpp16_remote_start_transaction_conf_t conf;
        ocpp16_remote_start_transaction_req_from_json(msg->payload, &req);
        ocpp16_remote_start_transaction_conf_example(&conf);
        if (s->handlers.remote_start_transaction_req) {
            if (s->handlers.remote_start_transaction_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_remote_start_transaction_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    else if (strcmp(msg->action, "RemoteStopTransaction") == 0) {
        ocpp16_remote_stop_transaction_req_t req; ocpp16_remote_stop_transaction_conf_t conf;
        ocpp16_remote_stop_transaction_req_from_json(msg->payload, &req);
        ocpp16_remote_stop_transaction_conf_example(&conf);
        if (s->handlers.remote_stop_transaction_req) {
            if (s->handlers.remote_stop_transaction_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_remote_stop_transaction_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    else if (strcmp(msg->action, "ReserveNow") == 0) {
        ocpp16_reserve_now_req_t req; ocpp16_reserve_now_conf_t conf;
        ocpp16_reserve_now_req_from_json(msg->payload, &req);
        ocpp16_reserve_now_conf_example(&conf);
        if (s->handlers.reserve_now_req) {
            if (s->handlers.reserve_now_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_reserve_now_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    else if (strcmp(msg->action, "Reset") == 0) {
        ocpp16_reset_req_t req; ocpp16_reset_conf_t conf;
        ocpp16_reset_req_from_json(msg->payload, &req);
        ocpp16_reset_conf_example(&conf);
        if (s->handlers.reset_req) {
            if (s->handlers.reset_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_reset_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    else if (strcmp(msg->action, "SendLocalList") == 0) {
        ocpp16_send_local_list_req_t req; ocpp16_send_local_list_conf_t conf;
        ocpp16_send_local_list_req_from_json(msg->payload, &req);
        ocpp16_send_local_list_conf_example(&conf);
        if (s->handlers.send_local_list_req) {
            if (s->handlers.send_local_list_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_send_local_list_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    else if (strcmp(msg->action, "SetChargingProfile") == 0) {
        ocpp16_set_charging_profile_req_t req; ocpp16_set_charging_profile_conf_t conf;
        ocpp16_set_charging_profile_req_from_json(msg->payload, &req);
        ocpp16_set_charging_profile_conf_example(&conf);
        if (s->handlers.set_charging_profile_req) {
            if (s->handlers.set_charging_profile_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_set_charging_profile_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    else if (strcmp(msg->action, "TriggerMessage") == 0) {
        ocpp16_trigger_message_req_t req; ocpp16_trigger_message_conf_t conf;
        ocpp16_trigger_message_req_from_json(msg->payload, &req);
        ocpp16_trigger_message_conf_example(&conf);
        if (s->handlers.trigger_message_req) {
            if (s->handlers.trigger_message_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_trigger_message_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    else if (strcmp(msg->action, "UnlockConnector") == 0) {
        ocpp16_unlock_connector_req_t req; ocpp16_unlock_connector_conf_t conf;
        ocpp16_unlock_connector_req_from_json(msg->payload, &req);
        ocpp16_unlock_connector_conf_example(&conf);
        if (s->handlers.unlock_connector_req) {
            if (s->handlers.unlock_connector_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_unlock_connector_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    else if (strcmp(msg->action, "UpdateFirmware") == 0) {
        ocpp16_update_firmware_req_t req; ocpp16_update_firmware_conf_t conf;
        ocpp16_update_firmware_req_from_json(msg->payload, &req);
        ocpp16_update_firmware_conf_example(&conf);
        if (s->handlers.update_firmware_req) {
            if (s->handlers.update_firmware_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_update_firmware_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    else if (strcmp(msg->action, "CertificateSigned") == 0) {
        ocpp16_certificate_signed_req_t req; ocpp16_certificate_signed_conf_t conf;
        ocpp16_certificate_signed_req_from_json(msg->payload, &req);
        ocpp16_certificate_signed_conf_example(&conf);
        if (s->handlers.certificate_signed_req) {
            if (s->handlers.certificate_signed_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_certificate_signed_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    else if (strcmp(msg->action, "DeleteCertificate") == 0) {
        ocpp16_delete_certificate_req_t req; ocpp16_delete_certificate_conf_t conf;
        ocpp16_delete_certificate_req_from_json(msg->payload, &req);
        ocpp16_delete_certificate_conf_example(&conf);
        if (s->handlers.delete_certificate_req) {
            if (s->handlers.delete_certificate_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_delete_certificate_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    else if (strcmp(msg->action, "ExtendedTriggerMessage") == 0) {
        ocpp16_extended_trigger_message_req_t req; ocpp16_extended_trigger_message_conf_t conf;
        ocpp16_extended_trigger_message_req_from_json(msg->payload, &req);
        ocpp16_extended_trigger_message_conf_example(&conf);
        if (s->handlers.extended_trigger_message_req) {
            if (s->handlers.extended_trigger_message_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_extended_trigger_message_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    else if (strcmp(msg->action, "GetInstalledCertificateIds") == 0) {
        ocpp16_get_installed_certificate_ids_req_t req; ocpp16_get_installed_certificate_ids_conf_t conf;
        ocpp16_get_installed_certificate_ids_req_from_json(msg->payload, &req);
        ocpp16_get_installed_certificate_ids_conf_example(&conf);
        if (s->handlers.get_installed_certificate_ids_req) {
            if (s->handlers.get_installed_certificate_ids_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_get_installed_certificate_ids_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    else if (strcmp(msg->action, "GetLog") == 0) {
        ocpp16_get_log_req_t req; ocpp16_get_log_conf_t conf;
        ocpp16_get_log_req_from_json(msg->payload, &req);
        ocpp16_get_log_conf_example(&conf);
        if (s->handlers.get_log_req) {
            if (s->handlers.get_log_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_get_log_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    else if (strcmp(msg->action, "InstallCertificate") == 0) {
        ocpp16_install_certificate_req_t req; ocpp16_install_certificate_conf_t conf;
        ocpp16_install_certificate_req_from_json(msg->payload, &req);
        ocpp16_install_certificate_conf_example(&conf);
        if (s->handlers.install_certificate_req) {
            if (s->handlers.install_certificate_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_install_certificate_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    else if (strcmp(msg->action, "SignedUpdateFirmware") == 0) {
        ocpp16_signed_update_firmware_req_t req; ocpp16_signed_update_firmware_conf_t conf;
        ocpp16_signed_update_firmware_req_from_json(msg->payload, &req);
        ocpp16_signed_update_firmware_conf_example(&conf);
        if (s->handlers.signed_update_firmware_req) {
            if (s->handlers.signed_update_firmware_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->frame, sizeof(s->frame));
                session_tx(s); return OCPP_OK;
            }
        }
        ocpp16_signed_update_firmware_conf_encode(&conf, s->payload, sizeof(s->payload));
        ocpp_rpc_pack_callresult(msg->unique_id, s->payload, s->frame, sizeof(s->frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    ocpp_rpc_pack_callerror(msg->unique_id, "NotImplemented", msg->action, s->frame, sizeof(s->frame));
    session_tx(s);
    return OCPP_OK;
}

/* Accepted 才周期心跳。Pending/Rejected 用同一槽做一次性 Boot 重试。 */
static void arm_boot_timer(ocpp16_session_t *s, const ocpp16_boot_notification_conf_t *conf) {
    int accepted = strcmp(conf->status, "Accepted") == 0;
    s->registered = accepted ? 1 : 0;
    if (conf->interval > 0) {
        s->heartbeat_interval_s = conf->interval;
    }
    if (s->link.heartbeat_timer_id < 0) {
        return;
    }
    if (accepted) {
        ocpp_port_timer_start(s->link.heartbeat_timer_id, (uint32_t)s->heartbeat_interval_s * 1000u, 1,
                              heartbeat_timer, s);
        return;
    }
    /* 未批准时不要心跳。interval 是下一次 Boot 的等待时间。 */
    if (s->has_boot_req && s->heartbeat_interval_s > 0) {
        ocpp_port_timer_start(s->link.heartbeat_timer_id, (uint32_t)s->heartbeat_interval_s * 1000u, 0,
                              boot_retry_timer, s);
        return;
    }
    ocpp_port_timer_stop(s->link.heartbeat_timer_id);
}

/*
 * 处理本端 CALL 的 CALLRESULT。action 来自 pending，不是报文本身。
 */
static void handle_result(ocpp16_session_t *s, const char *action, const cJSON *payload) {
    if (strcmp(action, "Authorize") == 0) {
        ocpp16_authorize_conf_t conf; ocpp16_authorize_conf_from_json(payload, &conf);
        if (s->handlers.authorize_conf) s->handlers.authorize_conf(&conf, s->handlers.user);
        return;
    }
    else if (strcmp(action, "BootNotification") == 0) {
        ocpp16_boot_notification_conf_t conf;
        ocpp16_boot_notification_conf_from_json(payload, &conf);
        arm_boot_timer(s, &conf);
        if (s->handlers.boot_notification_conf) s->handlers.boot_notification_conf(&conf, s->handlers.user);
        return;
    }
    else if (strcmp(action, "DataTransfer") == 0) {
        ocpp16_data_transfer_conf_t conf; ocpp16_data_transfer_conf_from_json(payload, &conf);
        if (s->handlers.data_transfer_conf) s->handlers.data_transfer_conf(&conf, s->handlers.user);
        return;
    }
    else if (strcmp(action, "DiagnosticsStatusNotification") == 0) {
        ocpp16_diagnostics_status_notification_conf_t conf; ocpp16_diagnostics_status_notification_conf_from_json(payload, &conf);
        if (s->handlers.diagnostics_status_notification_conf) s->handlers.diagnostics_status_notification_conf(&conf, s->handlers.user);
        return;
    }
    else if (strcmp(action, "FirmwareStatusNotification") == 0) {
        ocpp16_firmware_status_notification_conf_t conf; ocpp16_firmware_status_notification_conf_from_json(payload, &conf);
        if (s->handlers.firmware_status_notification_conf) s->handlers.firmware_status_notification_conf(&conf, s->handlers.user);
        return;
    }
    else if (strcmp(action, "Heartbeat") == 0) {
        ocpp16_heartbeat_conf_t conf; ocpp16_heartbeat_conf_from_json(payload, &conf);
        if (s->handlers.heartbeat_conf) s->handlers.heartbeat_conf(&conf, s->handlers.user);
        return;
    }
    else if (strcmp(action, "MeterValues") == 0) {
        ocpp16_meter_values_conf_t conf; ocpp16_meter_values_conf_from_json(payload, &conf);
        if (s->handlers.meter_values_conf) s->handlers.meter_values_conf(&conf, s->handlers.user);
        return;
    }
    else if (strcmp(action, "StartTransaction") == 0) {
        ocpp16_start_transaction_conf_t conf; ocpp16_start_transaction_conf_from_json(payload, &conf);
        if (s->handlers.start_transaction_conf) s->handlers.start_transaction_conf(&conf, s->handlers.user);
        return;
    }
    else if (strcmp(action, "StatusNotification") == 0) {
        ocpp16_status_notification_conf_t conf; ocpp16_status_notification_conf_from_json(payload, &conf);
        if (s->handlers.status_notification_conf) s->handlers.status_notification_conf(&conf, s->handlers.user);
        return;
    }
    else if (strcmp(action, "StopTransaction") == 0) {
        ocpp16_stop_transaction_conf_t conf; ocpp16_stop_transaction_conf_from_json(payload, &conf);
        if (s->handlers.stop_transaction_conf) s->handlers.stop_transaction_conf(&conf, s->handlers.user);
        return;
    }
    else if (strcmp(action, "LogStatusNotification") == 0) {
        ocpp16_log_status_notification_conf_t conf; ocpp16_log_status_notification_conf_from_json(payload, &conf);
        if (s->handlers.log_status_notification_conf) s->handlers.log_status_notification_conf(&conf, s->handlers.user);
        return;
    }
    else if (strcmp(action, "SecurityEventNotification") == 0) {
        ocpp16_security_event_notification_conf_t conf; ocpp16_security_event_notification_conf_from_json(payload, &conf);
        if (s->handlers.security_event_notification_conf) s->handlers.security_event_notification_conf(&conf, s->handlers.user);
        return;
    }
    else if (strcmp(action, "SignCertificate") == 0) {
        ocpp16_sign_certificate_conf_t conf; ocpp16_sign_certificate_conf_from_json(payload, &conf);
        if (s->handlers.sign_certificate_conf) s->handlers.sign_certificate_conf(&conf, s->handlers.user);
        return;
    }
    else if (strcmp(action, "SignedFirmwareStatusNotification") == 0) {
        ocpp16_signed_firmware_status_notification_conf_t conf; ocpp16_signed_firmware_status_notification_conf_from_json(payload, &conf);
        if (s->handlers.signed_firmware_status_notification_conf) s->handlers.signed_firmware_status_notification_conf(&conf, s->handlers.user);
        return;
    }
}

/**
 * ocpp16_session_rx - 喂入一条完整的 WebSocket 文本帧
 * @s: 本路会话
 * @frame: 一整帧 JSON，不要半包
 * @len: 字节数
 *
 * CALL 走 handle_call，CALLRESULT 用 uniqueId 找回 pending。
 * 返回前释放 unpack 摘出来的 payload。未知类型忽略并返回 OCPP_OK。
 * 对不上 pending 的 CALLRESULT 返回 OCPP_ERR_NOTFOUND。
 */
ocpp_err_t ocpp16_session_rx(ocpp16_session_t *s, const char *frame, size_t len) {
    ocpp_rpc_msg_t msg;
    ocpp_err_t rc = ocpp_rpc_unpack(frame, len, &msg);
    if (rc != OCPP_OK) {
        return rc;
    }
    if (msg.type == OCPP_RPC_CALL) {
        rc = handle_call(s, &msg);
    } else if (msg.type == OCPP_RPC_CALLRESULT) {
        rc = OCPP_ERR_NOTFOUND;
        for (int i = 0; i < OCPP16_PENDING_MAX; i++) {
            if (s->pending[i].used && strcmp(s->pending[i].uid, msg.unique_id) == 0) {
                s->pending[i].used = 0;
                handle_result(s, s->pending[i].action, msg.payload);
                rc = OCPP_OK;
                break;
            }
        }
    }
    /* unpack 把 payload 从根上摘下来了，根已经 Delete。payload 还在堆上。 */
    cJSON_Delete(msg.payload);
    return rc;
}
