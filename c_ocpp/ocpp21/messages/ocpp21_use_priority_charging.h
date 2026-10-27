#ifndef OCPP21_USEPRIORITYCHARGING_H
#define OCPP21_USEPRIORITYCHARGING_H
#include "ocpp_err.h"
#include "ocpp_port.h"
#include "ocpp_json.h"
#ifdef __cplusplus
extern "C" {
#endif
#define OCPP21_USEPRIORITYCHARGING_ACTION "UsePriorityCharging"

typedef struct ocpp21_use_priority_charging_req {
    char transactionId[37];
    int activate;
} ocpp21_use_priority_charging_req_t;

typedef struct ocpp21_use_priority_charging_conf {
    char status[21];
} ocpp21_use_priority_charging_conf_t;


cJSON *ocpp21_use_priority_charging_req_to_json(const ocpp21_use_priority_charging_req_t *in);
ocpp_err_t ocpp21_use_priority_charging_req_from_json(const cJSON *obj, ocpp21_use_priority_charging_req_t *out);
ocpp_err_t ocpp21_use_priority_charging_req_encode(const ocpp21_use_priority_charging_req_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_use_priority_charging_req_decode(const char *json, ocpp21_use_priority_charging_req_t *out);
void ocpp21_use_priority_charging_req_example(ocpp21_use_priority_charging_req_t *out);
cJSON *ocpp21_use_priority_charging_conf_to_json(const ocpp21_use_priority_charging_conf_t *in);
ocpp_err_t ocpp21_use_priority_charging_conf_from_json(const cJSON *obj, ocpp21_use_priority_charging_conf_t *out);
ocpp_err_t ocpp21_use_priority_charging_conf_encode(const ocpp21_use_priority_charging_conf_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_use_priority_charging_conf_decode(const char *json, ocpp21_use_priority_charging_conf_t *out);
void ocpp21_use_priority_charging_conf_example(ocpp21_use_priority_charging_conf_t *out);
#ifdef __cplusplus
}
#endif
#endif
