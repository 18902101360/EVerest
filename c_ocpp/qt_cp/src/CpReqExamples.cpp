/*
 * 用各版本 *_example / *_encode 生成文本框初值。
 * 宏展开后每个函数对应一条桩侧 CALL，注释写在宏里，展开后落在函数上方。
 */
#include "CpReqExamples.h"

#include "ocpp16.h"
#include "ocpp201.h"
#include "ocpp21.h"

#include <cstddef>

/*
 * EX - 生成“把示例请求编成 JSON”的函数
 * prefix 是 ocpp16 / ocpp201 / ocpp21，snake 是报文文件名里的那一段。
 * 编出来的是 CALL 的 payload，不含 [2, uniqueId, action] 外壳。
 */
#define EX(prefix, snake)                                                                                          \
    /* prefix snake：桩侧 CALL 的示例 JSON，字段来自库内 example。 */                                    \
    static void ex_##prefix##_##snake(char *buf, size_t n) {                                                       \
        prefix##_##snake##_req_t req;                                                                              \
        prefix##_##snake##_req_example(&req);                                                                      \
        if (n == 0) {                                                                                              \
            return;                                                                                                \
        }                                                                                                          \
        if (prefix##_##snake##_req_encode(&req, buf, n) != OCPP_OK) {                                              \
            buf[0] = '\0';                                                                                         \
        }                                                                                                          \
    }

EX(ocpp16, authorize)
EX(ocpp16, boot_notification)
EX(ocpp16, data_transfer)
EX(ocpp16, diagnostics_status_notification)
EX(ocpp16, firmware_status_notification)
EX(ocpp16, heartbeat)
EX(ocpp16, meter_values)
EX(ocpp16, start_transaction)
EX(ocpp16, status_notification)
EX(ocpp16, stop_transaction)
EX(ocpp16, log_status_notification)
EX(ocpp16, security_event_notification)
EX(ocpp16, sign_certificate)
EX(ocpp16, signed_firmware_status_notification)

EX(ocpp201, authorize)
EX(ocpp201, boot_notification)
EX(ocpp201, cleared_charging_limit)
EX(ocpp201, data_transfer)
EX(ocpp201, firmware_status_notification)
EX(ocpp201, get15118_ev_certificate)
EX(ocpp201, get_certificate_status)
EX(ocpp201, heartbeat)
EX(ocpp201, log_status_notification)
EX(ocpp201, meter_values)
EX(ocpp201, notify_charging_limit)
EX(ocpp201, notify_customer_information)
EX(ocpp201, notify_display_messages)
EX(ocpp201, notify_ev_charging_needs)
EX(ocpp201, notify_ev_charging_schedule)
EX(ocpp201, notify_event)
EX(ocpp201, notify_monitoring_report)
EX(ocpp201, notify_report)
EX(ocpp201, publish_firmware_status_notification)
EX(ocpp201, report_charging_profiles)
EX(ocpp201, reservation_status_update)
EX(ocpp201, security_event_notification)
EX(ocpp201, sign_certificate)
EX(ocpp201, status_notification)
EX(ocpp201, transaction_event)

EX(ocpp21, battery_swap)
EX(ocpp21, get_certificate_chain_status)
EX(ocpp21, notify_allowed_energy_transfer)
EX(ocpp21, notify_der_alarm)
EX(ocpp21, notify_der_start_stop)
EX(ocpp21, notify_periodic_event_stream)
EX(ocpp21, notify_priority_charging)
EX(ocpp21, notify_settlement)
EX(ocpp21, notify_web_payment_started)
EX(ocpp21, pull_dynamic_schedule_update)
EX(ocpp21, report_der_control)
EX(ocpp21, vat_number_validation)

#undef EX

/* 1.6 桩主动发出的 CALL。顺序与协议表里 from_cp 的条目一致。 */
static const CpReqExample k16[] = {
    {"Authorize", ex_ocpp16_authorize},
    {"BootNotification", ex_ocpp16_boot_notification},
    {"DataTransfer", ex_ocpp16_data_transfer},
    {"DiagnosticsStatusNotification", ex_ocpp16_diagnostics_status_notification},
    {"FirmwareStatusNotification", ex_ocpp16_firmware_status_notification},
    {"Heartbeat", ex_ocpp16_heartbeat},
    {"MeterValues", ex_ocpp16_meter_values},
    {"StartTransaction", ex_ocpp16_start_transaction},
    {"StatusNotification", ex_ocpp16_status_notification},
    {"StopTransaction", ex_ocpp16_stop_transaction},
    {"LogStatusNotification", ex_ocpp16_log_status_notification},
    {"SecurityEventNotification", ex_ocpp16_security_event_notification},
    {"SignCertificate", ex_ocpp16_sign_certificate},
    {"SignedFirmwareStatusNotification", ex_ocpp16_signed_firmware_status_notification},
};

/* 2.0.1 桩主动发出的 CALL。2.1 会话里这些名字不变。 */
static const CpReqExample k201[] = {
    {"Authorize", ex_ocpp201_authorize},
    {"BootNotification", ex_ocpp201_boot_notification},
    {"ClearedChargingLimit", ex_ocpp201_cleared_charging_limit},
    {"DataTransfer", ex_ocpp201_data_transfer},
    {"FirmwareStatusNotification", ex_ocpp201_firmware_status_notification},
    {"Get15118EVCertificate", ex_ocpp201_get15118_ev_certificate},
    {"GetCertificateStatus", ex_ocpp201_get_certificate_status},
    {"Heartbeat", ex_ocpp201_heartbeat},
    {"LogStatusNotification", ex_ocpp201_log_status_notification},
    {"MeterValues", ex_ocpp201_meter_values},
    {"NotifyChargingLimit", ex_ocpp201_notify_charging_limit},
    {"NotifyCustomerInformation", ex_ocpp201_notify_customer_information},
    {"NotifyDisplayMessages", ex_ocpp201_notify_display_messages},
    {"NotifyEVChargingNeeds", ex_ocpp201_notify_ev_charging_needs},
    {"NotifyEVChargingSchedule", ex_ocpp201_notify_ev_charging_schedule},
    {"NotifyEvent", ex_ocpp201_notify_event},
    {"NotifyMonitoringReport", ex_ocpp201_notify_monitoring_report},
    {"NotifyReport", ex_ocpp201_notify_report},
    {"PublishFirmwareStatusNotification", ex_ocpp201_publish_firmware_status_notification},
    {"ReportChargingProfiles", ex_ocpp201_report_charging_profiles},
    {"ReservationStatusUpdate", ex_ocpp201_reservation_status_update},
    {"SecurityEventNotification", ex_ocpp201_security_event_notification},
    {"SignCertificate", ex_ocpp201_sign_certificate},
    {"StatusNotification", ex_ocpp201_status_notification},
    {"TransactionEvent", ex_ocpp201_transaction_event},
};

/* 2.1 才有的桩侧 CALL。平台下发的 SetDERControl 等不在这张表里。 */
static const CpReqExample k21[] = {
    {"BatterySwap", ex_ocpp21_battery_swap},
    {"GetCertificateChainStatus", ex_ocpp21_get_certificate_chain_status},
    {"NotifyAllowedEnergyTransfer", ex_ocpp21_notify_allowed_energy_transfer},
    {"NotifyDERAlarm", ex_ocpp21_notify_der_alarm},
    {"NotifyDERStartStop", ex_ocpp21_notify_der_start_stop},
    {"NotifyPeriodicEventStream", ex_ocpp21_notify_periodic_event_stream},
    {"NotifyPriorityCharging", ex_ocpp21_notify_priority_charging},
    {"NotifySettlement", ex_ocpp21_notify_settlement},
    {"NotifyWebPaymentStarted", ex_ocpp21_notify_web_payment_started},
    {"PullDynamicScheduleUpdate", ex_ocpp21_pull_dynamic_schedule_update},
    {"ReportDERControl", ex_ocpp21_report_der_control},
    {"VatNumberValidation", ex_ocpp21_vat_number_validation},
};

/**
 * cpReqExamples16 - 返回 1.6 桩侧请求表
 * @count: 写出条数，不能为 NULL
 */
const CpReqExample *cpReqExamples16(int *count) {
    *count = static_cast<int>(sizeof(k16) / sizeof(k16[0]));
    return k16;
}

/**
 * cpReqExamples201 - 返回 2.0.1 桩侧请求表
 * @count: 写出条数，不能为 NULL
 */
const CpReqExample *cpReqExamples201(int *count) {
    *count = static_cast<int>(sizeof(k201) / sizeof(k201[0]));
    return k201;
}

/**
 * cpReqExamples21 - 返回 2.1 新增的桩侧请求表
 * @count: 写出条数，不能为 NULL
 */
const CpReqExample *cpReqExamples21(int *count) {
    *count = static_cast<int>(sizeof(k21) / sizeof(k21[0]));
    return k21;
}
