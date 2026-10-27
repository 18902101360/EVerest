/* 2.1 联调辅助。未知 action 返回 1，调用方再试 2.0.1。 */
static int decode_conf21(const char *action, const char *json) {
    if (strcmp(action, "BatterySwap") == 0) { ocpp21_battery_swap_conf_t c; return ocpp21_battery_swap_conf_decode(json, &c) == OCPP_OK ? 0 : -1; }
    if (strcmp(action, "GetCertificateChainStatus") == 0) { ocpp21_get_certificate_chain_status_conf_t c; return ocpp21_get_certificate_chain_status_conf_decode(json, &c) == OCPP_OK ? 0 : -1; }
    if (strcmp(action, "NotifyAllowedEnergyTransfer") == 0) { ocpp21_notify_allowed_energy_transfer_conf_t c; return ocpp21_notify_allowed_energy_transfer_conf_decode(json, &c) == OCPP_OK ? 0 : -1; }
    if (strcmp(action, "NotifyDERAlarm") == 0) { ocpp21_notify_der_alarm_conf_t c; return ocpp21_notify_der_alarm_conf_decode(json, &c) == OCPP_OK ? 0 : -1; }
    if (strcmp(action, "NotifyDERStartStop") == 0) { ocpp21_notify_der_start_stop_conf_t c; return ocpp21_notify_der_start_stop_conf_decode(json, &c) == OCPP_OK ? 0 : -1; }
    if (strcmp(action, "NotifyPeriodicEventStream") == 0) { ocpp21_notify_periodic_event_stream_conf_t c; return ocpp21_notify_periodic_event_stream_conf_decode(json, &c) == OCPP_OK ? 0 : -1; }
    if (strcmp(action, "NotifyPriorityCharging") == 0) { ocpp21_notify_priority_charging_conf_t c; return ocpp21_notify_priority_charging_conf_decode(json, &c) == OCPP_OK ? 0 : -1; }
    if (strcmp(action, "NotifySettlement") == 0) { ocpp21_notify_settlement_conf_t c; return ocpp21_notify_settlement_conf_decode(json, &c) == OCPP_OK ? 0 : -1; }
    if (strcmp(action, "NotifyWebPaymentStarted") == 0) { ocpp21_notify_web_payment_started_conf_t c; return ocpp21_notify_web_payment_started_conf_decode(json, &c) == OCPP_OK ? 0 : -1; }
    if (strcmp(action, "PullDynamicScheduleUpdate") == 0) { ocpp21_pull_dynamic_schedule_update_conf_t c; return ocpp21_pull_dynamic_schedule_update_conf_decode(json, &c) == OCPP_OK ? 0 : -1; }
    if (strcmp(action, "ReportDERControl") == 0) { ocpp21_report_der_control_conf_t c; return ocpp21_report_der_control_conf_decode(json, &c) == OCPP_OK ? 0 : -1; }
    if (strcmp(action, "VatNumberValidation") == 0) { ocpp21_vat_number_validation_conf_t c; return ocpp21_vat_number_validation_conf_decode(json, &c) == OCPP_OK ? 0 : -1; }
    return 1;
}
static int decode_req21(const char *action, const char *json) {
    if (strcmp(action, "AFRRSignal") == 0) { ocpp21_afrr_signal_req_t r; return ocpp21_afrr_signal_req_decode(json, &r) == OCPP_OK ? 0 : -1; }
    if (strcmp(action, "AdjustPeriodicEventStream") == 0) { ocpp21_adjust_periodic_event_stream_req_t r; return ocpp21_adjust_periodic_event_stream_req_decode(json, &r) == OCPP_OK ? 0 : -1; }
    if (strcmp(action, "ChangeTransactionTariff") == 0) { ocpp21_change_transaction_tariff_req_t r; return ocpp21_change_transaction_tariff_req_decode(json, &r) == OCPP_OK ? 0 : -1; }
    if (strcmp(action, "ClearDERControl") == 0) { ocpp21_clear_der_control_req_t r; return ocpp21_clear_der_control_req_decode(json, &r) == OCPP_OK ? 0 : -1; }
    if (strcmp(action, "ClearTariffs") == 0) { ocpp21_clear_tariffs_req_t r; return ocpp21_clear_tariffs_req_decode(json, &r) == OCPP_OK ? 0 : -1; }
    if (strcmp(action, "ClosePeriodicEventStream") == 0) { ocpp21_close_periodic_event_stream_req_t r; return ocpp21_close_periodic_event_stream_req_decode(json, &r) == OCPP_OK ? 0 : -1; }
    if (strcmp(action, "GetDERControl") == 0) { ocpp21_get_der_control_req_t r; return ocpp21_get_der_control_req_decode(json, &r) == OCPP_OK ? 0 : -1; }
    if (strcmp(action, "GetPeriodicEventStream") == 0) { ocpp21_get_periodic_event_stream_req_t r; return ocpp21_get_periodic_event_stream_req_decode(json, &r) == OCPP_OK ? 0 : -1; }
    if (strcmp(action, "GetTariffs") == 0) { ocpp21_get_tariffs_req_t r; return ocpp21_get_tariffs_req_decode(json, &r) == OCPP_OK ? 0 : -1; }
    if (strcmp(action, "OpenPeriodicEventStream") == 0) { ocpp21_open_periodic_event_stream_req_t r; return ocpp21_open_periodic_event_stream_req_decode(json, &r) == OCPP_OK ? 0 : -1; }
    if (strcmp(action, "RequestBatterySwap") == 0) { ocpp21_request_battery_swap_req_t r; return ocpp21_request_battery_swap_req_decode(json, &r) == OCPP_OK ? 0 : -1; }
    if (strcmp(action, "SetDERControl") == 0) { ocpp21_set_der_control_req_t r; return ocpp21_set_der_control_req_decode(json, &r) == OCPP_OK ? 0 : -1; }
    if (strcmp(action, "SetDefaultTariff") == 0) { ocpp21_set_default_tariff_req_t r; return ocpp21_set_default_tariff_req_decode(json, &r) == OCPP_OK ? 0 : -1; }
    if (strcmp(action, "UpdateDynamicSchedule") == 0) { ocpp21_update_dynamic_schedule_req_t r; return ocpp21_update_dynamic_schedule_req_decode(json, &r) == OCPP_OK ? 0 : -1; }
    if (strcmp(action, "UsePriorityCharging") == 0) { ocpp21_use_priority_charging_req_t r; return ocpp21_use_priority_charging_req_decode(json, &r) == OCPP_OK ? 0 : -1; }
    return 1;
}
#define SEND21(name) do { \
    ocpp21_##name##_req_t req; \
    ocpp21_##name##_req_example(&req); \
    if (ocpp21_session_send_##name(g_s21, &req) != OCPP_OK) fail("send21 " #name); \
    if (pump21(g_s21, ws, idle201, 8000) != 0) fail("wait21 " #name); \
} while (0)
static void send_all_21(ws_conn_t *ws) {
    SEND21(battery_swap);
    SEND21(get_certificate_chain_status);
    SEND21(notify_allowed_energy_transfer);
    SEND21(notify_der_alarm);
    SEND21(notify_der_start_stop);
    SEND21(notify_periodic_event_stream);
    SEND21(notify_priority_charging);
    SEND21(notify_settlement);
    SEND21(notify_web_payment_started);
    SEND21(pull_dynamic_schedule_update);
    SEND21(report_der_control);
    SEND21(vat_number_validation);
}
