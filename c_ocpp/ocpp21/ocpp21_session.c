#include "ocpp21_session.h"
#include <string.h>
static int session_tx(ocpp21_session_t *s) {
    return ocpp_link_send(&s->base.link, s->base.frame, strlen(s->base.frame));
}
static int is_cp_action(const char *action) {
    return strcmp(action, "BatterySwap") == 0 || strcmp(action, "GetCertificateChainStatus") == 0 || strcmp(action, "NotifyAllowedEnergyTransfer") == 0 || strcmp(action, "NotifyDERAlarm") == 0 || strcmp(action, "NotifyDERStartStop") == 0 || strcmp(action, "NotifyPeriodicEventStream") == 0 || strcmp(action, "NotifyPriorityCharging") == 0 || strcmp(action, "NotifySettlement") == 0 || strcmp(action, "NotifyWebPaymentStarted") == 0 || strcmp(action, "PullDynamicScheduleUpdate") == 0 || strcmp(action, "ReportDERControl") == 0 || strcmp(action, "VatNumberValidation") == 0;
}
static int is_csms_action(const char *action) {
    return strcmp(action, "AFRRSignal") == 0 || strcmp(action, "AdjustPeriodicEventStream") == 0 || strcmp(action, "ChangeTransactionTariff") == 0 || strcmp(action, "ClearDERControl") == 0 || strcmp(action, "ClearTariffs") == 0 || strcmp(action, "ClosePeriodicEventStream") == 0 || strcmp(action, "GetDERControl") == 0 || strcmp(action, "GetPeriodicEventStream") == 0 || strcmp(action, "GetTariffs") == 0 || strcmp(action, "OpenPeriodicEventStream") == 0 || strcmp(action, "RequestBatterySwap") == 0 || strcmp(action, "SetDERControl") == 0 || strcmp(action, "SetDefaultTariff") == 0 || strcmp(action, "UpdateDynamicSchedule") == 0 || strcmp(action, "UsePriorityCharging") == 0;
}
/**
 * ocpp21_session_init - 清零并绑定一路 2.1 会话
 * @s: 调用方提供的对象，放 BSS，不要放任务栈
 * @h201: 2.0.1 业务回调。NULL 表示那些 action 用示例应答
 * @h21: 2.1 新增 action 的回调。NULL 表示用示例应答
 * @link: 这一路的发送函数
 *
 * 子协议应为 ocpp2.1。Boot、心跳、交易仍调用 ocpp201_session_send_*，传入 &s->base。
 */
void ocpp21_session_init(ocpp21_session_t *s, const ocpp201_handlers_t *h201, const ocpp21_handlers_t *h21, const ocpp_link_t *link) {
    memset(s, 0, sizeof(*s));
    ocpp201_session_init(&s->base, h201, link);
    if (h21) s->handlers = *h21;
}
/**
 * ocpp21_session_send_battery_swap - 桩发出 BatterySwap
 * @s: 本路 2.1 会话
 * @req: 已填好的请求。encode 写入内嵌 2.0.1 session 的 payload
 * 
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp21_session_send_battery_swap(ocpp21_session_t *s, const ocpp21_battery_swap_req_t *req) {
    ocpp_err_t rc = ocpp21_battery_swap_req_encode(req, s->base.payload, sizeof(s->base.payload));
    if (rc != OCPP_OK) return rc;
    return ocpp201_session_call(&s->base, "BatterySwap", s->base.payload);
}
/**
 * ocpp21_session_send_get_certificate_chain_status - 桩发出 GetCertificateChainStatus
 * @s: 本路 2.1 会话
 * @req: 已填好的请求。encode 写入内嵌 2.0.1 session 的 payload
 * 
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp21_session_send_get_certificate_chain_status(ocpp21_session_t *s, const ocpp21_get_certificate_chain_status_req_t *req) {
    ocpp_err_t rc = ocpp21_get_certificate_chain_status_req_encode(req, s->base.payload, sizeof(s->base.payload));
    if (rc != OCPP_OK) return rc;
    return ocpp201_session_call(&s->base, "GetCertificateChainStatus", s->base.payload);
}
/**
 * ocpp21_session_send_notify_allowed_energy_transfer - 桩发出 NotifyAllowedEnergyTransfer
 * @s: 本路 2.1 会话
 * @req: 已填好的请求。encode 写入内嵌 2.0.1 session 的 payload
 * 
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp21_session_send_notify_allowed_energy_transfer(ocpp21_session_t *s, const ocpp21_notify_allowed_energy_transfer_req_t *req) {
    ocpp_err_t rc = ocpp21_notify_allowed_energy_transfer_req_encode(req, s->base.payload, sizeof(s->base.payload));
    if (rc != OCPP_OK) return rc;
    return ocpp201_session_call(&s->base, "NotifyAllowedEnergyTransfer", s->base.payload);
}
/**
 * ocpp21_session_send_notify_der_alarm - 桩发出 NotifyDERAlarm
 * @s: 本路 2.1 会话
 * @req: 已填好的请求。encode 写入内嵌 2.0.1 session 的 payload
 * 
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp21_session_send_notify_der_alarm(ocpp21_session_t *s, const ocpp21_notify_der_alarm_req_t *req) {
    ocpp_err_t rc = ocpp21_notify_der_alarm_req_encode(req, s->base.payload, sizeof(s->base.payload));
    if (rc != OCPP_OK) return rc;
    return ocpp201_session_call(&s->base, "NotifyDERAlarm", s->base.payload);
}
/**
 * ocpp21_session_send_notify_der_start_stop - 桩发出 NotifyDERStartStop
 * @s: 本路 2.1 会话
 * @req: 已填好的请求。encode 写入内嵌 2.0.1 session 的 payload
 * 
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp21_session_send_notify_der_start_stop(ocpp21_session_t *s, const ocpp21_notify_der_start_stop_req_t *req) {
    ocpp_err_t rc = ocpp21_notify_der_start_stop_req_encode(req, s->base.payload, sizeof(s->base.payload));
    if (rc != OCPP_OK) return rc;
    return ocpp201_session_call(&s->base, "NotifyDERStartStop", s->base.payload);
}
/**
 * ocpp21_session_send_notify_periodic_event_stream - 桩发出 NotifyPeriodicEventStream
 * @s: 本路 2.1 会话
 * @req: 已填好的请求。encode 写入内嵌 2.0.1 session 的 payload
 * 
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp21_session_send_notify_periodic_event_stream(ocpp21_session_t *s, const ocpp21_notify_periodic_event_stream_req_t *req) {
    ocpp_err_t rc = ocpp21_notify_periodic_event_stream_req_encode(req, s->base.payload, sizeof(s->base.payload));
    if (rc != OCPP_OK) return rc;
    return ocpp201_session_call(&s->base, "NotifyPeriodicEventStream", s->base.payload);
}
/**
 * ocpp21_session_send_notify_priority_charging - 桩发出 NotifyPriorityCharging
 * @s: 本路 2.1 会话
 * @req: 已填好的请求。encode 写入内嵌 2.0.1 session 的 payload
 * 
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp21_session_send_notify_priority_charging(ocpp21_session_t *s, const ocpp21_notify_priority_charging_req_t *req) {
    ocpp_err_t rc = ocpp21_notify_priority_charging_req_encode(req, s->base.payload, sizeof(s->base.payload));
    if (rc != OCPP_OK) return rc;
    return ocpp201_session_call(&s->base, "NotifyPriorityCharging", s->base.payload);
}
/**
 * ocpp21_session_send_notify_settlement - 桩发出 NotifySettlement
 * @s: 本路 2.1 会话
 * @req: 已填好的请求。encode 写入内嵌 2.0.1 session 的 payload
 * 
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp21_session_send_notify_settlement(ocpp21_session_t *s, const ocpp21_notify_settlement_req_t *req) {
    ocpp_err_t rc = ocpp21_notify_settlement_req_encode(req, s->base.payload, sizeof(s->base.payload));
    if (rc != OCPP_OK) return rc;
    return ocpp201_session_call(&s->base, "NotifySettlement", s->base.payload);
}
/**
 * ocpp21_session_send_notify_web_payment_started - 桩发出 NotifyWebPaymentStarted
 * @s: 本路 2.1 会话
 * @req: 已填好的请求。encode 写入内嵌 2.0.1 session 的 payload
 * 
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp21_session_send_notify_web_payment_started(ocpp21_session_t *s, const ocpp21_notify_web_payment_started_req_t *req) {
    ocpp_err_t rc = ocpp21_notify_web_payment_started_req_encode(req, s->base.payload, sizeof(s->base.payload));
    if (rc != OCPP_OK) return rc;
    return ocpp201_session_call(&s->base, "NotifyWebPaymentStarted", s->base.payload);
}
/**
 * ocpp21_session_send_pull_dynamic_schedule_update - 桩发出 PullDynamicScheduleUpdate
 * @s: 本路 2.1 会话
 * @req: 已填好的请求。encode 写入内嵌 2.0.1 session 的 payload
 * 
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp21_session_send_pull_dynamic_schedule_update(ocpp21_session_t *s, const ocpp21_pull_dynamic_schedule_update_req_t *req) {
    ocpp_err_t rc = ocpp21_pull_dynamic_schedule_update_req_encode(req, s->base.payload, sizeof(s->base.payload));
    if (rc != OCPP_OK) return rc;
    return ocpp201_session_call(&s->base, "PullDynamicScheduleUpdate", s->base.payload);
}
/**
 * ocpp21_session_send_report_der_control - 桩发出 ReportDERControl
 * @s: 本路 2.1 会话
 * @req: 已填好的请求。encode 写入内嵌 2.0.1 session 的 payload
 * 
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp21_session_send_report_der_control(ocpp21_session_t *s, const ocpp21_report_der_control_req_t *req) {
    ocpp_err_t rc = ocpp21_report_der_control_req_encode(req, s->base.payload, sizeof(s->base.payload));
    if (rc != OCPP_OK) return rc;
    return ocpp201_session_call(&s->base, "ReportDERControl", s->base.payload);
}
/**
 * ocpp21_session_send_vat_number_validation - 桩发出 VatNumberValidation
 * @s: 本路 2.1 会话
 * @req: 已填好的请求。encode 写入内嵌 2.0.1 session 的 payload
 * 
 * 成功返回 OCPP_OK。encode 失败、pending 满或 send 失败时返回负的 ocpp_err_t。
 * 应答由 session_rx 按 uniqueId 找回，再调用对应的 *_conf。
 */
ocpp_err_t ocpp21_session_send_vat_number_validation(ocpp21_session_t *s, const ocpp21_vat_number_validation_req_t *req) {
    ocpp_err_t rc = ocpp21_vat_number_validation_req_encode(req, s->base.payload, sizeof(s->base.payload));
    if (rc != OCPP_OK) return rc;
    return ocpp201_session_call(&s->base, "VatNumberValidation", s->base.payload);
}
/* CSMS 发来的 2.1 CALL。解析失败回 FormationViolation，回调非 0 回 InternalError。 */
static ocpp_err_t handle_call(ocpp21_session_t *s, ocpp_rpc_msg_t *msg) {
    if (!s->base.link.accept_control && ocpp_action_is_control(msg->action)) {
        ocpp_rpc_pack_callerror(msg->unique_id, "SecurityError", "this link does not accept control", s->base.frame, sizeof(s->base.frame));
        session_tx(s);
        return OCPP_OK;
    }
    if (strcmp(msg->action, "AFRRSignal") == 0) {
        ocpp21_afrr_signal_req_t req; ocpp21_afrr_signal_conf_t conf;
        if (ocpp21_afrr_signal_req_from_json(msg->payload, &req) != OCPP_OK) {
            ocpp_rpc_pack_callerror(msg->unique_id, "FormationViolation", "AFRRSignal", s->base.frame, sizeof(s->base.frame));
            session_tx(s); return OCPP_OK;
        }
        ocpp21_afrr_signal_conf_example(&conf);
        if (s->handlers.afrr_signal_req) {
            if (s->handlers.afrr_signal_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->base.frame, sizeof(s->base.frame));
                session_tx(s); return OCPP_OK;
            }
        }
        if (ocpp21_afrr_signal_conf_encode(&conf, s->base.payload, sizeof(s->base.payload)) != OCPP_OK) return OCPP_ERR_NOMEM;
        ocpp_rpc_pack_callresult(msg->unique_id, s->base.payload, s->base.frame, sizeof(s->base.frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    if (strcmp(msg->action, "AdjustPeriodicEventStream") == 0) {
        ocpp21_adjust_periodic_event_stream_req_t req; ocpp21_adjust_periodic_event_stream_conf_t conf;
        if (ocpp21_adjust_periodic_event_stream_req_from_json(msg->payload, &req) != OCPP_OK) {
            ocpp_rpc_pack_callerror(msg->unique_id, "FormationViolation", "AdjustPeriodicEventStream", s->base.frame, sizeof(s->base.frame));
            session_tx(s); return OCPP_OK;
        }
        ocpp21_adjust_periodic_event_stream_conf_example(&conf);
        if (s->handlers.adjust_periodic_event_stream_req) {
            if (s->handlers.adjust_periodic_event_stream_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->base.frame, sizeof(s->base.frame));
                session_tx(s); return OCPP_OK;
            }
        }
        if (ocpp21_adjust_periodic_event_stream_conf_encode(&conf, s->base.payload, sizeof(s->base.payload)) != OCPP_OK) return OCPP_ERR_NOMEM;
        ocpp_rpc_pack_callresult(msg->unique_id, s->base.payload, s->base.frame, sizeof(s->base.frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    if (strcmp(msg->action, "ChangeTransactionTariff") == 0) {
        ocpp21_change_transaction_tariff_req_t req; ocpp21_change_transaction_tariff_conf_t conf;
        if (ocpp21_change_transaction_tariff_req_from_json(msg->payload, &req) != OCPP_OK) {
            ocpp_rpc_pack_callerror(msg->unique_id, "FormationViolation", "ChangeTransactionTariff", s->base.frame, sizeof(s->base.frame));
            session_tx(s); return OCPP_OK;
        }
        ocpp21_change_transaction_tariff_conf_example(&conf);
        if (s->handlers.change_transaction_tariff_req) {
            if (s->handlers.change_transaction_tariff_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->base.frame, sizeof(s->base.frame));
                session_tx(s); return OCPP_OK;
            }
        }
        if (ocpp21_change_transaction_tariff_conf_encode(&conf, s->base.payload, sizeof(s->base.payload)) != OCPP_OK) return OCPP_ERR_NOMEM;
        ocpp_rpc_pack_callresult(msg->unique_id, s->base.payload, s->base.frame, sizeof(s->base.frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    if (strcmp(msg->action, "ClearDERControl") == 0) {
        ocpp21_clear_der_control_req_t req; ocpp21_clear_der_control_conf_t conf;
        if (ocpp21_clear_der_control_req_from_json(msg->payload, &req) != OCPP_OK) {
            ocpp_rpc_pack_callerror(msg->unique_id, "FormationViolation", "ClearDERControl", s->base.frame, sizeof(s->base.frame));
            session_tx(s); return OCPP_OK;
        }
        ocpp21_clear_der_control_conf_example(&conf);
        if (s->handlers.clear_der_control_req) {
            if (s->handlers.clear_der_control_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->base.frame, sizeof(s->base.frame));
                session_tx(s); return OCPP_OK;
            }
        }
        if (ocpp21_clear_der_control_conf_encode(&conf, s->base.payload, sizeof(s->base.payload)) != OCPP_OK) return OCPP_ERR_NOMEM;
        ocpp_rpc_pack_callresult(msg->unique_id, s->base.payload, s->base.frame, sizeof(s->base.frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    if (strcmp(msg->action, "ClearTariffs") == 0) {
        ocpp21_clear_tariffs_req_t req; ocpp21_clear_tariffs_conf_t conf;
        if (ocpp21_clear_tariffs_req_from_json(msg->payload, &req) != OCPP_OK) {
            ocpp_rpc_pack_callerror(msg->unique_id, "FormationViolation", "ClearTariffs", s->base.frame, sizeof(s->base.frame));
            session_tx(s); return OCPP_OK;
        }
        ocpp21_clear_tariffs_conf_example(&conf);
        if (s->handlers.clear_tariffs_req) {
            if (s->handlers.clear_tariffs_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->base.frame, sizeof(s->base.frame));
                session_tx(s); return OCPP_OK;
            }
        }
        if (ocpp21_clear_tariffs_conf_encode(&conf, s->base.payload, sizeof(s->base.payload)) != OCPP_OK) return OCPP_ERR_NOMEM;
        ocpp_rpc_pack_callresult(msg->unique_id, s->base.payload, s->base.frame, sizeof(s->base.frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    if (strcmp(msg->action, "ClosePeriodicEventStream") == 0) {
        ocpp21_close_periodic_event_stream_req_t req; ocpp21_close_periodic_event_stream_conf_t conf;
        if (ocpp21_close_periodic_event_stream_req_from_json(msg->payload, &req) != OCPP_OK) {
            ocpp_rpc_pack_callerror(msg->unique_id, "FormationViolation", "ClosePeriodicEventStream", s->base.frame, sizeof(s->base.frame));
            session_tx(s); return OCPP_OK;
        }
        ocpp21_close_periodic_event_stream_conf_example(&conf);
        if (s->handlers.close_periodic_event_stream_req) {
            if (s->handlers.close_periodic_event_stream_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->base.frame, sizeof(s->base.frame));
                session_tx(s); return OCPP_OK;
            }
        }
        if (ocpp21_close_periodic_event_stream_conf_encode(&conf, s->base.payload, sizeof(s->base.payload)) != OCPP_OK) return OCPP_ERR_NOMEM;
        ocpp_rpc_pack_callresult(msg->unique_id, s->base.payload, s->base.frame, sizeof(s->base.frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    if (strcmp(msg->action, "GetDERControl") == 0) {
        ocpp21_get_der_control_req_t req; ocpp21_get_der_control_conf_t conf;
        if (ocpp21_get_der_control_req_from_json(msg->payload, &req) != OCPP_OK) {
            ocpp_rpc_pack_callerror(msg->unique_id, "FormationViolation", "GetDERControl", s->base.frame, sizeof(s->base.frame));
            session_tx(s); return OCPP_OK;
        }
        ocpp21_get_der_control_conf_example(&conf);
        if (s->handlers.get_der_control_req) {
            if (s->handlers.get_der_control_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->base.frame, sizeof(s->base.frame));
                session_tx(s); return OCPP_OK;
            }
        }
        if (ocpp21_get_der_control_conf_encode(&conf, s->base.payload, sizeof(s->base.payload)) != OCPP_OK) return OCPP_ERR_NOMEM;
        ocpp_rpc_pack_callresult(msg->unique_id, s->base.payload, s->base.frame, sizeof(s->base.frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    if (strcmp(msg->action, "GetPeriodicEventStream") == 0) {
        ocpp21_get_periodic_event_stream_req_t req; ocpp21_get_periodic_event_stream_conf_t conf;
        if (ocpp21_get_periodic_event_stream_req_from_json(msg->payload, &req) != OCPP_OK) {
            ocpp_rpc_pack_callerror(msg->unique_id, "FormationViolation", "GetPeriodicEventStream", s->base.frame, sizeof(s->base.frame));
            session_tx(s); return OCPP_OK;
        }
        ocpp21_get_periodic_event_stream_conf_example(&conf);
        if (s->handlers.get_periodic_event_stream_req) {
            if (s->handlers.get_periodic_event_stream_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->base.frame, sizeof(s->base.frame));
                session_tx(s); return OCPP_OK;
            }
        }
        if (ocpp21_get_periodic_event_stream_conf_encode(&conf, s->base.payload, sizeof(s->base.payload)) != OCPP_OK) return OCPP_ERR_NOMEM;
        ocpp_rpc_pack_callresult(msg->unique_id, s->base.payload, s->base.frame, sizeof(s->base.frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    if (strcmp(msg->action, "GetTariffs") == 0) {
        ocpp21_get_tariffs_req_t req; ocpp21_get_tariffs_conf_t conf;
        if (ocpp21_get_tariffs_req_from_json(msg->payload, &req) != OCPP_OK) {
            ocpp_rpc_pack_callerror(msg->unique_id, "FormationViolation", "GetTariffs", s->base.frame, sizeof(s->base.frame));
            session_tx(s); return OCPP_OK;
        }
        ocpp21_get_tariffs_conf_example(&conf);
        if (s->handlers.get_tariffs_req) {
            if (s->handlers.get_tariffs_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->base.frame, sizeof(s->base.frame));
                session_tx(s); return OCPP_OK;
            }
        }
        if (ocpp21_get_tariffs_conf_encode(&conf, s->base.payload, sizeof(s->base.payload)) != OCPP_OK) return OCPP_ERR_NOMEM;
        ocpp_rpc_pack_callresult(msg->unique_id, s->base.payload, s->base.frame, sizeof(s->base.frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    if (strcmp(msg->action, "OpenPeriodicEventStream") == 0) {
        ocpp21_open_periodic_event_stream_req_t req; ocpp21_open_periodic_event_stream_conf_t conf;
        if (ocpp21_open_periodic_event_stream_req_from_json(msg->payload, &req) != OCPP_OK) {
            ocpp_rpc_pack_callerror(msg->unique_id, "FormationViolation", "OpenPeriodicEventStream", s->base.frame, sizeof(s->base.frame));
            session_tx(s); return OCPP_OK;
        }
        ocpp21_open_periodic_event_stream_conf_example(&conf);
        if (s->handlers.open_periodic_event_stream_req) {
            if (s->handlers.open_periodic_event_stream_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->base.frame, sizeof(s->base.frame));
                session_tx(s); return OCPP_OK;
            }
        }
        if (ocpp21_open_periodic_event_stream_conf_encode(&conf, s->base.payload, sizeof(s->base.payload)) != OCPP_OK) return OCPP_ERR_NOMEM;
        ocpp_rpc_pack_callresult(msg->unique_id, s->base.payload, s->base.frame, sizeof(s->base.frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    if (strcmp(msg->action, "RequestBatterySwap") == 0) {
        ocpp21_request_battery_swap_req_t req; ocpp21_request_battery_swap_conf_t conf;
        if (ocpp21_request_battery_swap_req_from_json(msg->payload, &req) != OCPP_OK) {
            ocpp_rpc_pack_callerror(msg->unique_id, "FormationViolation", "RequestBatterySwap", s->base.frame, sizeof(s->base.frame));
            session_tx(s); return OCPP_OK;
        }
        ocpp21_request_battery_swap_conf_example(&conf);
        if (s->handlers.request_battery_swap_req) {
            if (s->handlers.request_battery_swap_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->base.frame, sizeof(s->base.frame));
                session_tx(s); return OCPP_OK;
            }
        }
        if (ocpp21_request_battery_swap_conf_encode(&conf, s->base.payload, sizeof(s->base.payload)) != OCPP_OK) return OCPP_ERR_NOMEM;
        ocpp_rpc_pack_callresult(msg->unique_id, s->base.payload, s->base.frame, sizeof(s->base.frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    if (strcmp(msg->action, "SetDERControl") == 0) {
        ocpp21_set_der_control_req_t req; ocpp21_set_der_control_conf_t conf;
        if (ocpp21_set_der_control_req_from_json(msg->payload, &req) != OCPP_OK) {
            ocpp_rpc_pack_callerror(msg->unique_id, "FormationViolation", "SetDERControl", s->base.frame, sizeof(s->base.frame));
            session_tx(s); return OCPP_OK;
        }
        ocpp21_set_der_control_conf_example(&conf);
        if (s->handlers.set_der_control_req) {
            if (s->handlers.set_der_control_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->base.frame, sizeof(s->base.frame));
                session_tx(s); return OCPP_OK;
            }
        }
        if (ocpp21_set_der_control_conf_encode(&conf, s->base.payload, sizeof(s->base.payload)) != OCPP_OK) return OCPP_ERR_NOMEM;
        ocpp_rpc_pack_callresult(msg->unique_id, s->base.payload, s->base.frame, sizeof(s->base.frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    if (strcmp(msg->action, "SetDefaultTariff") == 0) {
        ocpp21_set_default_tariff_req_t req; ocpp21_set_default_tariff_conf_t conf;
        if (ocpp21_set_default_tariff_req_from_json(msg->payload, &req) != OCPP_OK) {
            ocpp_rpc_pack_callerror(msg->unique_id, "FormationViolation", "SetDefaultTariff", s->base.frame, sizeof(s->base.frame));
            session_tx(s); return OCPP_OK;
        }
        ocpp21_set_default_tariff_conf_example(&conf);
        if (s->handlers.set_default_tariff_req) {
            if (s->handlers.set_default_tariff_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->base.frame, sizeof(s->base.frame));
                session_tx(s); return OCPP_OK;
            }
        }
        if (ocpp21_set_default_tariff_conf_encode(&conf, s->base.payload, sizeof(s->base.payload)) != OCPP_OK) return OCPP_ERR_NOMEM;
        ocpp_rpc_pack_callresult(msg->unique_id, s->base.payload, s->base.frame, sizeof(s->base.frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    if (strcmp(msg->action, "UpdateDynamicSchedule") == 0) {
        ocpp21_update_dynamic_schedule_req_t req; ocpp21_update_dynamic_schedule_conf_t conf;
        if (ocpp21_update_dynamic_schedule_req_from_json(msg->payload, &req) != OCPP_OK) {
            ocpp_rpc_pack_callerror(msg->unique_id, "FormationViolation", "UpdateDynamicSchedule", s->base.frame, sizeof(s->base.frame));
            session_tx(s); return OCPP_OK;
        }
        ocpp21_update_dynamic_schedule_conf_example(&conf);
        if (s->handlers.update_dynamic_schedule_req) {
            if (s->handlers.update_dynamic_schedule_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->base.frame, sizeof(s->base.frame));
                session_tx(s); return OCPP_OK;
            }
        }
        if (ocpp21_update_dynamic_schedule_conf_encode(&conf, s->base.payload, sizeof(s->base.payload)) != OCPP_OK) return OCPP_ERR_NOMEM;
        ocpp_rpc_pack_callresult(msg->unique_id, s->base.payload, s->base.frame, sizeof(s->base.frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    if (strcmp(msg->action, "UsePriorityCharging") == 0) {
        ocpp21_use_priority_charging_req_t req; ocpp21_use_priority_charging_conf_t conf;
        if (ocpp21_use_priority_charging_req_from_json(msg->payload, &req) != OCPP_OK) {
            ocpp_rpc_pack_callerror(msg->unique_id, "FormationViolation", "UsePriorityCharging", s->base.frame, sizeof(s->base.frame));
            session_tx(s); return OCPP_OK;
        }
        ocpp21_use_priority_charging_conf_example(&conf);
        if (s->handlers.use_priority_charging_req) {
            if (s->handlers.use_priority_charging_req(&req, &conf, s->handlers.user) != 0) {
                ocpp_rpc_pack_callerror(msg->unique_id, "InternalError", "handler", s->base.frame, sizeof(s->base.frame));
                session_tx(s); return OCPP_OK;
            }
        }
        if (ocpp21_use_priority_charging_conf_encode(&conf, s->base.payload, sizeof(s->base.payload)) != OCPP_OK) return OCPP_ERR_NOMEM;
        ocpp_rpc_pack_callresult(msg->unique_id, s->base.payload, s->base.frame, sizeof(s->base.frame));
        if (session_tx(s) != 0) return OCPP_ERR_IO;
        return OCPP_OK;
    }
    ocpp_rpc_pack_callerror(msg->unique_id, "NotImplemented", msg->action, s->base.frame, sizeof(s->base.frame));
    session_tx(s);
    return OCPP_OK;
}
static void clear_pending(ocpp201_session_t *base, const char *uid) {
    for (int i = 0; i < OCPP201_PENDING_MAX; i++) {
        if (base->pending[i].used && strcmp(base->pending[i].uid, uid) == 0) {
            base->pending[i].used = 0;
            return;
        }
    }
}
static void handle_result(ocpp21_session_t *s, const char *action, const cJSON *payload) {
    if (strcmp(action, "BatterySwap") == 0) {
        ocpp21_battery_swap_conf_t conf;
        if (ocpp21_battery_swap_conf_from_json(payload, &conf) != OCPP_OK) return;
        if (s->handlers.battery_swap_conf) s->handlers.battery_swap_conf(&conf, s->handlers.user);
        return;
    }
    if (strcmp(action, "GetCertificateChainStatus") == 0) {
        ocpp21_get_certificate_chain_status_conf_t conf;
        if (ocpp21_get_certificate_chain_status_conf_from_json(payload, &conf) != OCPP_OK) return;
        if (s->handlers.get_certificate_chain_status_conf) s->handlers.get_certificate_chain_status_conf(&conf, s->handlers.user);
        return;
    }
    if (strcmp(action, "NotifyAllowedEnergyTransfer") == 0) {
        ocpp21_notify_allowed_energy_transfer_conf_t conf;
        if (ocpp21_notify_allowed_energy_transfer_conf_from_json(payload, &conf) != OCPP_OK) return;
        if (s->handlers.notify_allowed_energy_transfer_conf) s->handlers.notify_allowed_energy_transfer_conf(&conf, s->handlers.user);
        return;
    }
    if (strcmp(action, "NotifyDERAlarm") == 0) {
        ocpp21_notify_der_alarm_conf_t conf;
        if (ocpp21_notify_der_alarm_conf_from_json(payload, &conf) != OCPP_OK) return;
        if (s->handlers.notify_der_alarm_conf) s->handlers.notify_der_alarm_conf(&conf, s->handlers.user);
        return;
    }
    if (strcmp(action, "NotifyDERStartStop") == 0) {
        ocpp21_notify_der_start_stop_conf_t conf;
        if (ocpp21_notify_der_start_stop_conf_from_json(payload, &conf) != OCPP_OK) return;
        if (s->handlers.notify_der_start_stop_conf) s->handlers.notify_der_start_stop_conf(&conf, s->handlers.user);
        return;
    }
    if (strcmp(action, "NotifyPeriodicEventStream") == 0) {
        ocpp21_notify_periodic_event_stream_conf_t conf;
        if (ocpp21_notify_periodic_event_stream_conf_from_json(payload, &conf) != OCPP_OK) return;
        if (s->handlers.notify_periodic_event_stream_conf) s->handlers.notify_periodic_event_stream_conf(&conf, s->handlers.user);
        return;
    }
    if (strcmp(action, "NotifyPriorityCharging") == 0) {
        ocpp21_notify_priority_charging_conf_t conf;
        if (ocpp21_notify_priority_charging_conf_from_json(payload, &conf) != OCPP_OK) return;
        if (s->handlers.notify_priority_charging_conf) s->handlers.notify_priority_charging_conf(&conf, s->handlers.user);
        return;
    }
    if (strcmp(action, "NotifySettlement") == 0) {
        ocpp21_notify_settlement_conf_t conf;
        if (ocpp21_notify_settlement_conf_from_json(payload, &conf) != OCPP_OK) return;
        if (s->handlers.notify_settlement_conf) s->handlers.notify_settlement_conf(&conf, s->handlers.user);
        return;
    }
    if (strcmp(action, "NotifyWebPaymentStarted") == 0) {
        ocpp21_notify_web_payment_started_conf_t conf;
        if (ocpp21_notify_web_payment_started_conf_from_json(payload, &conf) != OCPP_OK) return;
        if (s->handlers.notify_web_payment_started_conf) s->handlers.notify_web_payment_started_conf(&conf, s->handlers.user);
        return;
    }
    if (strcmp(action, "PullDynamicScheduleUpdate") == 0) {
        ocpp21_pull_dynamic_schedule_update_conf_t conf;
        if (ocpp21_pull_dynamic_schedule_update_conf_from_json(payload, &conf) != OCPP_OK) return;
        if (s->handlers.pull_dynamic_schedule_update_conf) s->handlers.pull_dynamic_schedule_update_conf(&conf, s->handlers.user);
        return;
    }
    if (strcmp(action, "ReportDERControl") == 0) {
        ocpp21_report_der_control_conf_t conf;
        if (ocpp21_report_der_control_conf_from_json(payload, &conf) != OCPP_OK) return;
        if (s->handlers.report_der_control_conf) s->handlers.report_der_control_conf(&conf, s->handlers.user);
        return;
    }
    if (strcmp(action, "VatNumberValidation") == 0) {
        ocpp21_vat_number_validation_conf_t conf;
        if (ocpp21_vat_number_validation_conf_from_json(payload, &conf) != OCPP_OK) return;
        if (s->handlers.vat_number_validation_conf) s->handlers.vat_number_validation_conf(&conf, s->handlers.user);
        return;
    }
    (void)s;
    (void)payload;
}
/**
 * ocpp21_session_rx - 喂入一条完整的 WebSocket 文本帧
 * @s: 本路会话
 * @frame: 一整帧 JSON，不要半包
 * @len: 字节数
 *
 * 2.1 才有的 action 在这里分发。其余交给内嵌的 2.0.1 session。
 */
ocpp_err_t ocpp21_session_rx(ocpp21_session_t *s, const char *frame, size_t len) {
    ocpp_rpc_msg_t msg;
    ocpp_err_t rc = ocpp_rpc_unpack(frame, len, &msg);
    if (rc != OCPP_OK) return rc;
    if (msg.type == OCPP_RPC_CALL && is_csms_action(msg.action)) {
        rc = handle_call(s, &msg);
        cJSON_Delete(msg.payload);
        return rc;
    }
    if (msg.type == OCPP_RPC_CALLRESULT) {
        for (int i = 0; i < OCPP201_PENDING_MAX; i++) {
            if (s->base.pending[i].used && strcmp(s->base.pending[i].uid, msg.unique_id) == 0 && is_cp_action(s->base.pending[i].action)) {
                char action[48];
                ocpp_str_copy(action, sizeof(action), s->base.pending[i].action);
                clear_pending(&s->base, msg.unique_id);
                handle_result(s, action, msg.payload);
                cJSON_Delete(msg.payload);
                return OCPP_OK;
            }
        }
    }
    cJSON_Delete(msg.payload);
    return ocpp201_session_rx(&s->base, frame, len);
}
