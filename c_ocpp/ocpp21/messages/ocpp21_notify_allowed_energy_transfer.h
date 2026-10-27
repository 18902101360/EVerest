#ifndef OCPP21_NOTIFYALLOWEDENERGYTRANSFER_H
#define OCPP21_NOTIFYALLOWEDENERGYTRANSFER_H
#include "ocpp_err.h"
#include "ocpp_port.h"
#include "ocpp_json.h"
#ifdef __cplusplus
extern "C" {
#endif
#define OCPP21_NOTIFYALLOWEDENERGYTRANSFER_ACTION "NotifyAllowedEnergyTransfer"

typedef struct ocpp21_notify_allowed_energy_transfer_req {
    char transactionId[37];
    int allowedEnergyTransfer_count;
    char allowedEnergyTransfer[4][33];
} ocpp21_notify_allowed_energy_transfer_req_t;

typedef struct ocpp21_notify_allowed_energy_transfer_conf {
    char status[21];
} ocpp21_notify_allowed_energy_transfer_conf_t;


cJSON *ocpp21_notify_allowed_energy_transfer_req_to_json(const ocpp21_notify_allowed_energy_transfer_req_t *in);
ocpp_err_t ocpp21_notify_allowed_energy_transfer_req_from_json(const cJSON *obj, ocpp21_notify_allowed_energy_transfer_req_t *out);
ocpp_err_t ocpp21_notify_allowed_energy_transfer_req_encode(const ocpp21_notify_allowed_energy_transfer_req_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_notify_allowed_energy_transfer_req_decode(const char *json, ocpp21_notify_allowed_energy_transfer_req_t *out);
void ocpp21_notify_allowed_energy_transfer_req_example(ocpp21_notify_allowed_energy_transfer_req_t *out);
cJSON *ocpp21_notify_allowed_energy_transfer_conf_to_json(const ocpp21_notify_allowed_energy_transfer_conf_t *in);
ocpp_err_t ocpp21_notify_allowed_energy_transfer_conf_from_json(const cJSON *obj, ocpp21_notify_allowed_energy_transfer_conf_t *out);
ocpp_err_t ocpp21_notify_allowed_energy_transfer_conf_encode(const ocpp21_notify_allowed_energy_transfer_conf_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_notify_allowed_energy_transfer_conf_decode(const char *json, ocpp21_notify_allowed_energy_transfer_conf_t *out);
void ocpp21_notify_allowed_energy_transfer_conf_example(ocpp21_notify_allowed_energy_transfer_conf_t *out);
#ifdef __cplusplus
}
#endif
#endif
