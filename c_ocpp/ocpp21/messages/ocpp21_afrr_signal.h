#ifndef OCPP21_AFRRSIGNAL_H
#define OCPP21_AFRRSIGNAL_H
#include "ocpp_err.h"
#include "ocpp_port.h"
#include "ocpp_json.h"
#ifdef __cplusplus
extern "C" {
#endif
#define OCPP21_AFRRSIGNAL_ACTION "AFRRSignal"

typedef struct ocpp21_afrr_signal_req {
    char timestamp[33];
    int signal;
} ocpp21_afrr_signal_req_t;

typedef struct ocpp21_afrr_signal_conf {
    char status[21];
} ocpp21_afrr_signal_conf_t;


cJSON *ocpp21_afrr_signal_req_to_json(const ocpp21_afrr_signal_req_t *in);
ocpp_err_t ocpp21_afrr_signal_req_from_json(const cJSON *obj, ocpp21_afrr_signal_req_t *out);
ocpp_err_t ocpp21_afrr_signal_req_encode(const ocpp21_afrr_signal_req_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_afrr_signal_req_decode(const char *json, ocpp21_afrr_signal_req_t *out);
void ocpp21_afrr_signal_req_example(ocpp21_afrr_signal_req_t *out);
cJSON *ocpp21_afrr_signal_conf_to_json(const ocpp21_afrr_signal_conf_t *in);
ocpp_err_t ocpp21_afrr_signal_conf_from_json(const cJSON *obj, ocpp21_afrr_signal_conf_t *out);
ocpp_err_t ocpp21_afrr_signal_conf_encode(const ocpp21_afrr_signal_conf_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_afrr_signal_conf_decode(const char *json, ocpp21_afrr_signal_conf_t *out);
void ocpp21_afrr_signal_conf_example(ocpp21_afrr_signal_conf_t *out);
#ifdef __cplusplus
}
#endif
#endif
