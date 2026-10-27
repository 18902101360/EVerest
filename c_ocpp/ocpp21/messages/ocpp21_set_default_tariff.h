#ifndef OCPP21_SETDEFAULTTARIFF_H
#define OCPP21_SETDEFAULTTARIFF_H
#include "ocpp_err.h"
#include "ocpp_port.h"
#include "ocpp_json.h"
#ifdef __cplusplus
extern "C" {
#endif
#define OCPP21_SETDEFAULTTARIFF_ACTION "SetDefaultTariff"
typedef struct ocpp21_set_default_tariff_tariff {
    char tariffId[61];
    char currency[4];
} ocpp21_set_default_tariff_tariff_t;

typedef struct ocpp21_set_default_tariff_req {
    int evseId;
    ocpp21_set_default_tariff_tariff_t tariff;
} ocpp21_set_default_tariff_req_t;

typedef struct ocpp21_set_default_tariff_conf {
    char status[21];
} ocpp21_set_default_tariff_conf_t;
cJSON *ocpp21_set_default_tariff_tariff_to_json(const ocpp21_set_default_tariff_tariff_t *in);
ocpp_err_t ocpp21_set_default_tariff_tariff_from_json(const cJSON *obj, ocpp21_set_default_tariff_tariff_t *out);

cJSON *ocpp21_set_default_tariff_req_to_json(const ocpp21_set_default_tariff_req_t *in);
ocpp_err_t ocpp21_set_default_tariff_req_from_json(const cJSON *obj, ocpp21_set_default_tariff_req_t *out);
ocpp_err_t ocpp21_set_default_tariff_req_encode(const ocpp21_set_default_tariff_req_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_set_default_tariff_req_decode(const char *json, ocpp21_set_default_tariff_req_t *out);
void ocpp21_set_default_tariff_req_example(ocpp21_set_default_tariff_req_t *out);
cJSON *ocpp21_set_default_tariff_conf_to_json(const ocpp21_set_default_tariff_conf_t *in);
ocpp_err_t ocpp21_set_default_tariff_conf_from_json(const cJSON *obj, ocpp21_set_default_tariff_conf_t *out);
ocpp_err_t ocpp21_set_default_tariff_conf_encode(const ocpp21_set_default_tariff_conf_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_set_default_tariff_conf_decode(const char *json, ocpp21_set_default_tariff_conf_t *out);
void ocpp21_set_default_tariff_conf_example(ocpp21_set_default_tariff_conf_t *out);
#ifdef __cplusplus
}
#endif
#endif
