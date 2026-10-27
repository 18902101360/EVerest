#ifndef OCPP21_SETDERCONTROL_H
#define OCPP21_SETDERCONTROL_H
#include "ocpp_err.h"
#include "ocpp_port.h"
#include "ocpp_json.h"
#ifdef __cplusplus
extern "C" {
#endif
#define OCPP21_SETDERCONTROL_ACTION "SetDERControl"
typedef struct ocpp21_set_der_control_curve_curveData {
    double x;
    double y;
} ocpp21_set_der_control_curve_curveData_t;

typedef struct ocpp21_set_der_control_curve {
    int priority;
    char yUnit[9];
    int curveData_count;
    ocpp21_set_der_control_curve_curveData_t curveData[2];
} ocpp21_set_der_control_curve_t;

typedef struct ocpp21_set_der_control_req {
    int isDefault;
    char controlId[37];
    char controlType[33];
    int has_curve;
    ocpp21_set_der_control_curve_t curve;
} ocpp21_set_der_control_req_t;

typedef struct ocpp21_set_der_control_conf {
    char status[21];
} ocpp21_set_der_control_conf_t;
cJSON *ocpp21_set_der_control_curve_curveData_to_json(const ocpp21_set_der_control_curve_curveData_t *in);
ocpp_err_t ocpp21_set_der_control_curve_curveData_from_json(const cJSON *obj, ocpp21_set_der_control_curve_curveData_t *out);
cJSON *ocpp21_set_der_control_curve_to_json(const ocpp21_set_der_control_curve_t *in);
ocpp_err_t ocpp21_set_der_control_curve_from_json(const cJSON *obj, ocpp21_set_der_control_curve_t *out);

cJSON *ocpp21_set_der_control_req_to_json(const ocpp21_set_der_control_req_t *in);
ocpp_err_t ocpp21_set_der_control_req_from_json(const cJSON *obj, ocpp21_set_der_control_req_t *out);
ocpp_err_t ocpp21_set_der_control_req_encode(const ocpp21_set_der_control_req_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_set_der_control_req_decode(const char *json, ocpp21_set_der_control_req_t *out);
void ocpp21_set_der_control_req_example(ocpp21_set_der_control_req_t *out);
cJSON *ocpp21_set_der_control_conf_to_json(const ocpp21_set_der_control_conf_t *in);
ocpp_err_t ocpp21_set_der_control_conf_from_json(const cJSON *obj, ocpp21_set_der_control_conf_t *out);
ocpp_err_t ocpp21_set_der_control_conf_encode(const ocpp21_set_der_control_conf_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_set_der_control_conf_decode(const char *json, ocpp21_set_der_control_conf_t *out);
void ocpp21_set_der_control_conf_example(ocpp21_set_der_control_conf_t *out);
#ifdef __cplusplus
}
#endif
#endif
