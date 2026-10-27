#ifndef OCPP21_SESSION_H
#define OCPP21_SESSION_H
#include "messages/ocpp21_messages.h"
#include "ocpp201.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct ocpp21_handlers { void *user;
    int (*afrr_signal_req)(const ocpp21_afrr_signal_req_t *req, ocpp21_afrr_signal_conf_t *conf, void *user);
    int (*adjust_periodic_event_stream_req)(const ocpp21_adjust_periodic_event_stream_req_t *req, ocpp21_adjust_periodic_event_stream_conf_t *conf, void *user);
    int (*change_transaction_tariff_req)(const ocpp21_change_transaction_tariff_req_t *req, ocpp21_change_transaction_tariff_conf_t *conf, void *user);
    int (*clear_der_control_req)(const ocpp21_clear_der_control_req_t *req, ocpp21_clear_der_control_conf_t *conf, void *user);
    int (*clear_tariffs_req)(const ocpp21_clear_tariffs_req_t *req, ocpp21_clear_tariffs_conf_t *conf, void *user);
    int (*close_periodic_event_stream_req)(const ocpp21_close_periodic_event_stream_req_t *req, ocpp21_close_periodic_event_stream_conf_t *conf, void *user);
    int (*get_der_control_req)(const ocpp21_get_der_control_req_t *req, ocpp21_get_der_control_conf_t *conf, void *user);
    int (*get_periodic_event_stream_req)(const ocpp21_get_periodic_event_stream_req_t *req, ocpp21_get_periodic_event_stream_conf_t *conf, void *user);
    int (*get_tariffs_req)(const ocpp21_get_tariffs_req_t *req, ocpp21_get_tariffs_conf_t *conf, void *user);
    int (*open_periodic_event_stream_req)(const ocpp21_open_periodic_event_stream_req_t *req, ocpp21_open_periodic_event_stream_conf_t *conf, void *user);
    int (*request_battery_swap_req)(const ocpp21_request_battery_swap_req_t *req, ocpp21_request_battery_swap_conf_t *conf, void *user);
    int (*set_der_control_req)(const ocpp21_set_der_control_req_t *req, ocpp21_set_der_control_conf_t *conf, void *user);
    int (*set_default_tariff_req)(const ocpp21_set_default_tariff_req_t *req, ocpp21_set_default_tariff_conf_t *conf, void *user);
    int (*update_dynamic_schedule_req)(const ocpp21_update_dynamic_schedule_req_t *req, ocpp21_update_dynamic_schedule_conf_t *conf, void *user);
    int (*use_priority_charging_req)(const ocpp21_use_priority_charging_req_t *req, ocpp21_use_priority_charging_conf_t *conf, void *user);
    void (*battery_swap_conf)(const ocpp21_battery_swap_conf_t *conf, void *user);
    void (*get_certificate_chain_status_conf)(const ocpp21_get_certificate_chain_status_conf_t *conf, void *user);
    void (*notify_allowed_energy_transfer_conf)(const ocpp21_notify_allowed_energy_transfer_conf_t *conf, void *user);
    void (*notify_der_alarm_conf)(const ocpp21_notify_der_alarm_conf_t *conf, void *user);
    void (*notify_der_start_stop_conf)(const ocpp21_notify_der_start_stop_conf_t *conf, void *user);
    void (*notify_periodic_event_stream_conf)(const ocpp21_notify_periodic_event_stream_conf_t *conf, void *user);
    void (*notify_priority_charging_conf)(const ocpp21_notify_priority_charging_conf_t *conf, void *user);
    void (*notify_settlement_conf)(const ocpp21_notify_settlement_conf_t *conf, void *user);
    void (*notify_web_payment_started_conf)(const ocpp21_notify_web_payment_started_conf_t *conf, void *user);
    void (*pull_dynamic_schedule_update_conf)(const ocpp21_pull_dynamic_schedule_update_conf_t *conf, void *user);
    void (*report_der_control_conf)(const ocpp21_report_der_control_conf_t *conf, void *user);
    void (*vat_number_validation_conf)(const ocpp21_vat_number_validation_conf_t *conf, void *user);
} ocpp21_handlers_t;
typedef struct ocpp21_session {
    ocpp201_session_t base; /* 2.0.1 报文、pending、心跳都在这里 */
    ocpp21_handlers_t handlers;
} ocpp21_session_t;
/**
 * ocpp21_session_init - 清零并绑定一路 2.1 会话
 * @s: 调用方提供的对象，放 BSS，不要放任务栈
 * @h201: 2.0.1 业务回调。NULL 表示那些 action 用示例应答
 * @h21: 2.1 新增 action 的回调。NULL 表示用示例应答
 * @link: 这一路的发送函数
 *
 * 子协议应为 ocpp2.1。Boot、心跳、交易仍调用 ocpp201_session_send_*，传入 &s->base。
 */
void ocpp21_session_init(ocpp21_session_t *s, const ocpp201_handlers_t *h201, const ocpp21_handlers_t *h21, const ocpp_link_t *link);
/**
 * ocpp21_session_rx - 喂入一条完整的 WebSocket 文本帧
 * @s: 本路会话
 * @frame: 一整帧 JSON，不要半包
 * @len: 字节数
 *
 * 2.1 才有的 action 在这里分发。其余交给内嵌的 2.0.1 session。
 */
ocpp_err_t ocpp21_session_rx(ocpp21_session_t *s, const char *frame, size_t len);
ocpp_err_t ocpp21_session_send_battery_swap(ocpp21_session_t *s, const ocpp21_battery_swap_req_t *req);
ocpp_err_t ocpp21_session_send_get_certificate_chain_status(ocpp21_session_t *s, const ocpp21_get_certificate_chain_status_req_t *req);
ocpp_err_t ocpp21_session_send_notify_allowed_energy_transfer(ocpp21_session_t *s, const ocpp21_notify_allowed_energy_transfer_req_t *req);
ocpp_err_t ocpp21_session_send_notify_der_alarm(ocpp21_session_t *s, const ocpp21_notify_der_alarm_req_t *req);
ocpp_err_t ocpp21_session_send_notify_der_start_stop(ocpp21_session_t *s, const ocpp21_notify_der_start_stop_req_t *req);
ocpp_err_t ocpp21_session_send_notify_periodic_event_stream(ocpp21_session_t *s, const ocpp21_notify_periodic_event_stream_req_t *req);
ocpp_err_t ocpp21_session_send_notify_priority_charging(ocpp21_session_t *s, const ocpp21_notify_priority_charging_req_t *req);
ocpp_err_t ocpp21_session_send_notify_settlement(ocpp21_session_t *s, const ocpp21_notify_settlement_req_t *req);
ocpp_err_t ocpp21_session_send_notify_web_payment_started(ocpp21_session_t *s, const ocpp21_notify_web_payment_started_req_t *req);
ocpp_err_t ocpp21_session_send_pull_dynamic_schedule_update(ocpp21_session_t *s, const ocpp21_pull_dynamic_schedule_update_req_t *req);
ocpp_err_t ocpp21_session_send_report_der_control(ocpp21_session_t *s, const ocpp21_report_der_control_req_t *req);
ocpp_err_t ocpp21_session_send_vat_number_validation(ocpp21_session_t *s, const ocpp21_vat_number_validation_req_t *req);
#ifdef __cplusplus
}
#endif
#endif
