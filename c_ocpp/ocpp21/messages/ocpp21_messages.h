#ifndef OCPP21_MESSAGES_H
#define OCPP21_MESSAGES_H
/* OCPP 2.1 新增报文。2.0.1 已有的 action 仍走 ocpp201。 */
#include "ocpp21/messages/ocpp21_afrr_signal.h"
#include "ocpp21/messages/ocpp21_adjust_periodic_event_stream.h"
#include "ocpp21/messages/ocpp21_battery_swap.h"
#include "ocpp21/messages/ocpp21_change_transaction_tariff.h"
#include "ocpp21/messages/ocpp21_clear_der_control.h"
#include "ocpp21/messages/ocpp21_clear_tariffs.h"
#include "ocpp21/messages/ocpp21_close_periodic_event_stream.h"
#include "ocpp21/messages/ocpp21_get_certificate_chain_status.h"
#include "ocpp21/messages/ocpp21_get_der_control.h"
#include "ocpp21/messages/ocpp21_get_periodic_event_stream.h"
#include "ocpp21/messages/ocpp21_get_tariffs.h"
#include "ocpp21/messages/ocpp21_notify_allowed_energy_transfer.h"
#include "ocpp21/messages/ocpp21_notify_der_alarm.h"
#include "ocpp21/messages/ocpp21_notify_der_start_stop.h"
#include "ocpp21/messages/ocpp21_notify_periodic_event_stream.h"
#include "ocpp21/messages/ocpp21_notify_priority_charging.h"
#include "ocpp21/messages/ocpp21_notify_settlement.h"
#include "ocpp21/messages/ocpp21_notify_web_payment_started.h"
#include "ocpp21/messages/ocpp21_open_periodic_event_stream.h"
#include "ocpp21/messages/ocpp21_pull_dynamic_schedule_update.h"
#include "ocpp21/messages/ocpp21_report_der_control.h"
#include "ocpp21/messages/ocpp21_request_battery_swap.h"
#include "ocpp21/messages/ocpp21_set_der_control.h"
#include "ocpp21/messages/ocpp21_set_default_tariff.h"
#include "ocpp21/messages/ocpp21_update_dynamic_schedule.h"
#include "ocpp21/messages/ocpp21_use_priority_charging.h"
#include "ocpp21/messages/ocpp21_vat_number_validation.h"
#define OCPP21_MESSAGE_COUNT 27
typedef struct ocpp21_message_info { const char *action; int from_cp; int from_csms; } ocpp21_message_info_t;
/**
 * ocpp21_message_table - 2.1 才有的 action，以及 CALL 由桩还是平台发起
 *
 * 返回静态表，不要释放。不含 2.0.1 已经有的报文。
 */
const ocpp21_message_info_t *ocpp21_message_table(void);
#endif
