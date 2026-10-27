#ifndef OCPP21_NOTIFYSETTLEMENT_H
#define OCPP21_NOTIFYSETTLEMENT_H
#include "ocpp_err.h"
#include "ocpp_port.h"
#include "ocpp_json.h"
#ifdef __cplusplus
extern "C" {
#endif
#define OCPP21_NOTIFYSETTLEMENT_ACTION "NotifySettlement"

typedef struct ocpp21_notify_settlement_req {
    char pspRef[256];
    char status[21];
    double settlementAmount;
    char settlementTime[33];
    int has_transactionId;
    char transactionId[37];
} ocpp21_notify_settlement_req_t;

typedef struct ocpp21_notify_settlement_conf {
    int unused;
} ocpp21_notify_settlement_conf_t;


cJSON *ocpp21_notify_settlement_req_to_json(const ocpp21_notify_settlement_req_t *in);
ocpp_err_t ocpp21_notify_settlement_req_from_json(const cJSON *obj, ocpp21_notify_settlement_req_t *out);
ocpp_err_t ocpp21_notify_settlement_req_encode(const ocpp21_notify_settlement_req_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_notify_settlement_req_decode(const char *json, ocpp21_notify_settlement_req_t *out);
void ocpp21_notify_settlement_req_example(ocpp21_notify_settlement_req_t *out);
cJSON *ocpp21_notify_settlement_conf_to_json(const ocpp21_notify_settlement_conf_t *in);
ocpp_err_t ocpp21_notify_settlement_conf_from_json(const cJSON *obj, ocpp21_notify_settlement_conf_t *out);
ocpp_err_t ocpp21_notify_settlement_conf_encode(const ocpp21_notify_settlement_conf_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_notify_settlement_conf_decode(const char *json, ocpp21_notify_settlement_conf_t *out);
void ocpp21_notify_settlement_conf_example(ocpp21_notify_settlement_conf_t *out);
#ifdef __cplusplus
}
#endif
#endif
