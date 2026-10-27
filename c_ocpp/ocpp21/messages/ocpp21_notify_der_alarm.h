#ifndef OCPP21_NOTIFYDERALARM_H
#define OCPP21_NOTIFYDERALARM_H
#include "ocpp_err.h"
#include "ocpp_port.h"
#include "ocpp_json.h"
#ifdef __cplusplus
extern "C" {
#endif
#define OCPP21_NOTIFYDERALARM_ACTION "NotifyDERAlarm"

typedef struct ocpp21_notify_der_alarm_req {
    char controlType[33];
    char timestamp[33];
    int has_gridEventFault;
    char gridEventFault[33];
    int has_alarmEnded;
    int alarmEnded;
} ocpp21_notify_der_alarm_req_t;

typedef struct ocpp21_notify_der_alarm_conf {
    int unused;
} ocpp21_notify_der_alarm_conf_t;


cJSON *ocpp21_notify_der_alarm_req_to_json(const ocpp21_notify_der_alarm_req_t *in);
ocpp_err_t ocpp21_notify_der_alarm_req_from_json(const cJSON *obj, ocpp21_notify_der_alarm_req_t *out);
ocpp_err_t ocpp21_notify_der_alarm_req_encode(const ocpp21_notify_der_alarm_req_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_notify_der_alarm_req_decode(const char *json, ocpp21_notify_der_alarm_req_t *out);
void ocpp21_notify_der_alarm_req_example(ocpp21_notify_der_alarm_req_t *out);
cJSON *ocpp21_notify_der_alarm_conf_to_json(const ocpp21_notify_der_alarm_conf_t *in);
ocpp_err_t ocpp21_notify_der_alarm_conf_from_json(const cJSON *obj, ocpp21_notify_der_alarm_conf_t *out);
ocpp_err_t ocpp21_notify_der_alarm_conf_encode(const ocpp21_notify_der_alarm_conf_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_notify_der_alarm_conf_decode(const char *json, ocpp21_notify_der_alarm_conf_t *out);
void ocpp21_notify_der_alarm_conf_example(ocpp21_notify_der_alarm_conf_t *out);
#ifdef __cplusplus
}
#endif
#endif
