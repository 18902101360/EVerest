/* Dump example req/conf JSON for the CSMS catalog. Host-only. */
#include "ocpp16.h"
#include "ocpp201.h"
#include "ocpp_port.h"
#include <stdio.h>

#define D16(name, action)                                                                              \
    do {                                                                                               \
        ocpp16_##name##_req_t req;                                                                     \
        ocpp16_##name##_conf_t conf;                                                                   \
        char buf[OCPP_PAYLOAD_MAX];                                                                    \
        ocpp16_##name##_req_example(&req);                                                             \
        if (ocpp16_##name##_req_encode(&req, buf, sizeof buf) != OCPP_OK) {                            \
            fprintf(stderr, "fail req %s\n", action);                                                  \
            return 1;                                                                                  \
        }                                                                                              \
        printf("16 REQ %s %s\n", action, buf);                                                         \
        ocpp16_##name##_conf_example(&conf);                                                           \
        if (ocpp16_##name##_conf_encode(&conf, buf, sizeof buf) != OCPP_OK) {                           \
            fprintf(stderr, "fail conf %s\n", action);                                                 \
            return 1;                                                                                  \
        }                                                                                              \
        printf("16 CONF %s %s\n", action, buf);                                                        \
    } while (0)

#define D201(name, action)                                                                             \
    do {                                                                                               \
        ocpp201_##name##_req_t req;                                                                    \
        ocpp201_##name##_conf_t conf;                                                                  \
        char buf[OCPP_PAYLOAD_MAX];                                                                    \
        ocpp201_##name##_req_example(&req);                                                            \
        if (ocpp201_##name##_req_encode(&req, buf, sizeof buf) != OCPP_OK) {                           \
            fprintf(stderr, "fail req %s\n", action);                                                  \
            return 1;                                                                                  \
        }                                                                                              \
        printf("201 REQ %s %s\n", action, buf);                                                        \
        ocpp201_##name##_conf_example(&conf);                                                          \
        if (ocpp201_##name##_conf_encode(&conf, buf, sizeof buf) != OCPP_OK) {                          \
            fprintf(stderr, "fail conf %s\n", action);                                                 \
            return 1;                                                                                  \
        }                                                                                              \
        printf("201 CONF %s %s\n", action, buf);                                                       \
    } while (0)

/**
 * main - 本文件的测试入口
 *
 * 返回 0 表示全部检查通过。失败时向 stderr 打 FAIL，并返回非 0。
 */
int main(void) {
    ocpp_port_init();
    D16(authorize, "Authorize");
    D16(boot_notification, "BootNotification");
    D16(data_transfer, "DataTransfer");
    D16(diagnostics_status_notification, "DiagnosticsStatusNotification");
    D16(firmware_status_notification, "FirmwareStatusNotification");
    D16(heartbeat, "Heartbeat");
    D16(meter_values, "MeterValues");
    D16(start_transaction, "StartTransaction");
    D16(status_notification, "StatusNotification");
    D16(stop_transaction, "StopTransaction");
    D16(cancel_reservation, "CancelReservation");
    D16(change_availability, "ChangeAvailability");
    D16(change_configuration, "ChangeConfiguration");
    D16(clear_cache, "ClearCache");
    D16(clear_charging_profile, "ClearChargingProfile");
    D16(get_composite_schedule, "GetCompositeSchedule");
    D16(get_configuration, "GetConfiguration");
    D16(get_diagnostics, "GetDiagnostics");
    D16(get_local_list_version, "GetLocalListVersion");
    D16(remote_start_transaction, "RemoteStartTransaction");
    D16(remote_stop_transaction, "RemoteStopTransaction");
    D16(reserve_now, "ReserveNow");
    D16(reset, "Reset");
    D16(send_local_list, "SendLocalList");
    D16(set_charging_profile, "SetChargingProfile");
    D16(trigger_message, "TriggerMessage");
    D16(unlock_connector, "UnlockConnector");
    D16(update_firmware, "UpdateFirmware");
    D16(certificate_signed, "CertificateSigned");
    D16(delete_certificate, "DeleteCertificate");
    D16(extended_trigger_message, "ExtendedTriggerMessage");
    D16(get_installed_certificate_ids, "GetInstalledCertificateIds");
    D16(get_log, "GetLog");
    D16(install_certificate, "InstallCertificate");
    D16(log_status_notification, "LogStatusNotification");
    D16(security_event_notification, "SecurityEventNotification");
    D16(sign_certificate, "SignCertificate");
    D16(signed_firmware_status_notification, "SignedFirmwareStatusNotification");
    D16(signed_update_firmware, "SignedUpdateFirmware");

    D201(authorize, "Authorize");
    D201(boot_notification, "BootNotification");
    D201(cancel_reservation, "CancelReservation");
    D201(certificate_signed, "CertificateSigned");
    D201(change_availability, "ChangeAvailability");
    D201(clear_cache, "ClearCache");
    D201(clear_charging_profile, "ClearChargingProfile");
    D201(clear_display_message, "ClearDisplayMessage");
    D201(cleared_charging_limit, "ClearedChargingLimit");
    D201(clear_variable_monitoring, "ClearVariableMonitoring");
    D201(cost_updated, "CostUpdated");
    D201(customer_information, "CustomerInformation");
    D201(data_transfer, "DataTransfer");
    D201(delete_certificate, "DeleteCertificate");
    D201(firmware_status_notification, "FirmwareStatusNotification");
    D201(get15118_ev_certificate, "Get15118EVCertificate");
    D201(get_base_report, "GetBaseReport");
    D201(get_certificate_status, "GetCertificateStatus");
    D201(get_charging_profiles, "GetChargingProfiles");
    D201(get_composite_schedule, "GetCompositeSchedule");
    D201(get_display_messages, "GetDisplayMessages");
    D201(get_installed_certificate_ids, "GetInstalledCertificateIds");
    D201(get_local_list_version, "GetLocalListVersion");
    D201(get_log, "GetLog");
    D201(get_monitoring_report, "GetMonitoringReport");
    D201(get_report, "GetReport");
    D201(get_transaction_status, "GetTransactionStatus");
    D201(get_variables, "GetVariables");
    D201(heartbeat, "Heartbeat");
    D201(install_certificate, "InstallCertificate");
    D201(log_status_notification, "LogStatusNotification");
    D201(meter_values, "MeterValues");
    D201(notify_charging_limit, "NotifyChargingLimit");
    D201(notify_customer_information, "NotifyCustomerInformation");
    D201(notify_display_messages, "NotifyDisplayMessages");
    D201(notify_ev_charging_needs, "NotifyEVChargingNeeds");
    D201(notify_ev_charging_schedule, "NotifyEVChargingSchedule");
    D201(notify_event, "NotifyEvent");
    D201(notify_monitoring_report, "NotifyMonitoringReport");
    D201(notify_report, "NotifyReport");
    D201(publish_firmware, "PublishFirmware");
    D201(publish_firmware_status_notification, "PublishFirmwareStatusNotification");
    D201(report_charging_profiles, "ReportChargingProfiles");
    D201(request_start_transaction, "RequestStartTransaction");
    D201(request_stop_transaction, "RequestStopTransaction");
    D201(reservation_status_update, "ReservationStatusUpdate");
    D201(reserve_now, "ReserveNow");
    D201(reset, "Reset");
    D201(security_event_notification, "SecurityEventNotification");
    D201(send_local_list, "SendLocalList");
    D201(set_charging_profile, "SetChargingProfile");
    D201(set_display_message, "SetDisplayMessage");
    D201(set_monitoring_base, "SetMonitoringBase");
    D201(set_monitoring_level, "SetMonitoringLevel");
    D201(set_network_profile, "SetNetworkProfile");
    D201(set_variable_monitoring, "SetVariableMonitoring");
    D201(set_variables, "SetVariables");
    D201(sign_certificate, "SignCertificate");
    D201(status_notification, "StatusNotification");
    D201(transaction_event, "TransactionEvent");
    D201(trigger_message, "TriggerMessage");
    D201(unlock_connector, "UnlockConnector");
    D201(unpublish_firmware, "UnpublishFirmware");
    D201(update_firmware, "UpdateFirmware");
    return 0;
}
