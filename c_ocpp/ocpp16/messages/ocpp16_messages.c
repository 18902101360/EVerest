#include "ocpp16_messages.h"

static const ocpp16_message_info_t k_table[OCPP16_MESSAGE_COUNT] = {
    { "Authorize", 1, 0 },
    { "BootNotification", 1, 0 },
    { "DataTransfer", 1, 1 },
    { "DiagnosticsStatusNotification", 1, 0 },
    { "FirmwareStatusNotification", 1, 0 },
    { "Heartbeat", 1, 0 },
    { "MeterValues", 1, 0 },
    { "StartTransaction", 1, 0 },
    { "StatusNotification", 1, 0 },
    { "StopTransaction", 1, 0 },
    { "CancelReservation", 0, 1 },
    { "ChangeAvailability", 0, 1 },
    { "ChangeConfiguration", 0, 1 },
    { "ClearCache", 0, 1 },
    { "ClearChargingProfile", 0, 1 },
    { "GetCompositeSchedule", 0, 1 },
    { "GetConfiguration", 0, 1 },
    { "GetDiagnostics", 0, 1 },
    { "GetLocalListVersion", 0, 1 },
    { "RemoteStartTransaction", 0, 1 },
    { "RemoteStopTransaction", 0, 1 },
    { "ReserveNow", 0, 1 },
    { "Reset", 0, 1 },
    { "SendLocalList", 0, 1 },
    { "SetChargingProfile", 0, 1 },
    { "TriggerMessage", 0, 1 },
    { "UnlockConnector", 0, 1 },
    { "UpdateFirmware", 0, 1 },
    { "CertificateSigned", 0, 1 },
    { "DeleteCertificate", 0, 1 },
    { "ExtendedTriggerMessage", 0, 1 },
    { "GetInstalledCertificateIds", 0, 1 },
    { "GetLog", 0, 1 },
    { "InstallCertificate", 0, 1 },
    { "LogStatusNotification", 1, 0 },
    { "SecurityEventNotification", 1, 0 },
    { "SignCertificate", 1, 0 },
    { "SignedFirmwareStatusNotification", 1, 0 },
    { "SignedUpdateFirmware", 0, 1 },
};
/**
 * ocpp16_message_table - 1.6 全部 action 以及谁发起 CALL
 *
 * from_cp 为 1 表示桩发 CALL，from_csms 为 1 表示平台发 CALL。
 * DataTransfer 两边都能发起。返回的表是静态的，不要释放。
 */
const ocpp16_message_info_t *ocpp16_message_table(void) { return k_table; }
