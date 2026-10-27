#include "ocpp21_messages.h"
static const ocpp21_message_info_t k_table[OCPP21_MESSAGE_COUNT] = {
    { "AFRRSignal", 0, 1 },
    { "AdjustPeriodicEventStream", 0, 1 },
    { "BatterySwap", 1, 0 },
    { "ChangeTransactionTariff", 0, 1 },
    { "ClearDERControl", 0, 1 },
    { "ClearTariffs", 0, 1 },
    { "ClosePeriodicEventStream", 0, 1 },
    { "GetCertificateChainStatus", 1, 0 },
    { "GetDERControl", 0, 1 },
    { "GetPeriodicEventStream", 0, 1 },
    { "GetTariffs", 0, 1 },
    { "NotifyAllowedEnergyTransfer", 1, 0 },
    { "NotifyDERAlarm", 1, 0 },
    { "NotifyDERStartStop", 1, 0 },
    { "NotifyPeriodicEventStream", 1, 0 },
    { "NotifyPriorityCharging", 1, 0 },
    { "NotifySettlement", 1, 0 },
    { "NotifyWebPaymentStarted", 1, 0 },
    { "OpenPeriodicEventStream", 0, 1 },
    { "PullDynamicScheduleUpdate", 1, 0 },
    { "ReportDERControl", 1, 0 },
    { "RequestBatterySwap", 0, 1 },
    { "SetDERControl", 0, 1 },
    { "SetDefaultTariff", 0, 1 },
    { "UpdateDynamicSchedule", 0, 1 },
    { "UsePriorityCharging", 0, 1 },
    { "VatNumberValidation", 1, 0 }
};
/**
 * ocpp21_message_table - 2.1 才有的 action，以及 CALL 由桩还是平台发起
 *
 * 返回静态表，不要释放。不含 2.0.1 已经有的报文。
 */
const ocpp21_message_info_t *ocpp21_message_table(void) { return k_table; }
