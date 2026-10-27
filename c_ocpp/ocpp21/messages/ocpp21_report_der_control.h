#ifndef OCPP21_REPORTDERCONTROL_H
#define OCPP21_REPORTDERCONTROL_H
#include "ocpp_err.h"
#include "ocpp_port.h"
#include "ocpp_json.h"
#ifdef __cplusplus
extern "C" {
#endif
#define OCPP21_REPORTDERCONTROL_ACTION "ReportDERControl"

typedef struct ocpp21_report_der_control_req {
    int requestId;
    int has_tbc;
    int tbc;
} ocpp21_report_der_control_req_t;

typedef struct ocpp21_report_der_control_conf {
    int unused;
} ocpp21_report_der_control_conf_t;


cJSON *ocpp21_report_der_control_req_to_json(const ocpp21_report_der_control_req_t *in);
ocpp_err_t ocpp21_report_der_control_req_from_json(const cJSON *obj, ocpp21_report_der_control_req_t *out);
ocpp_err_t ocpp21_report_der_control_req_encode(const ocpp21_report_der_control_req_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_report_der_control_req_decode(const char *json, ocpp21_report_der_control_req_t *out);
void ocpp21_report_der_control_req_example(ocpp21_report_der_control_req_t *out);
cJSON *ocpp21_report_der_control_conf_to_json(const ocpp21_report_der_control_conf_t *in);
ocpp_err_t ocpp21_report_der_control_conf_from_json(const cJSON *obj, ocpp21_report_der_control_conf_t *out);
ocpp_err_t ocpp21_report_der_control_conf_encode(const ocpp21_report_der_control_conf_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_report_der_control_conf_decode(const char *json, ocpp21_report_der_control_conf_t *out);
void ocpp21_report_der_control_conf_example(ocpp21_report_der_control_conf_t *out);
#ifdef __cplusplus
}
#endif
#endif
