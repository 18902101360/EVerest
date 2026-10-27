#ifndef OCPP21_GETPERIODICEVENTSTREAM_H
#define OCPP21_GETPERIODICEVENTSTREAM_H
#include "ocpp_err.h"
#include "ocpp_port.h"
#include "ocpp_json.h"
#ifdef __cplusplus
extern "C" {
#endif
#define OCPP21_GETPERIODICEVENTSTREAM_ACTION "GetPeriodicEventStream"

typedef struct ocpp21_get_periodic_event_stream_req {
    int unused;
} ocpp21_get_periodic_event_stream_req_t;
typedef struct ocpp21_get_periodic_event_stream_constantStreamData_params {
    int has_interval;
    int interval;
    int has_values;
    int values;
} ocpp21_get_periodic_event_stream_constantStreamData_params_t;

typedef struct ocpp21_get_periodic_event_stream_constantStreamData {
    int id;
    ocpp21_get_periodic_event_stream_constantStreamData_params_t params;
    int variableMonitoringId;
} ocpp21_get_periodic_event_stream_constantStreamData_t;

typedef struct ocpp21_get_periodic_event_stream_conf {
    int constantStreamData_count;
    ocpp21_get_periodic_event_stream_constantStreamData_t constantStreamData[2];
} ocpp21_get_periodic_event_stream_conf_t;

cJSON *ocpp21_get_periodic_event_stream_constantStreamData_params_to_json(const ocpp21_get_periodic_event_stream_constantStreamData_params_t *in);
ocpp_err_t ocpp21_get_periodic_event_stream_constantStreamData_params_from_json(const cJSON *obj, ocpp21_get_periodic_event_stream_constantStreamData_params_t *out);
cJSON *ocpp21_get_periodic_event_stream_constantStreamData_to_json(const ocpp21_get_periodic_event_stream_constantStreamData_t *in);
ocpp_err_t ocpp21_get_periodic_event_stream_constantStreamData_from_json(const cJSON *obj, ocpp21_get_periodic_event_stream_constantStreamData_t *out);
cJSON *ocpp21_get_periodic_event_stream_req_to_json(const ocpp21_get_periodic_event_stream_req_t *in);
ocpp_err_t ocpp21_get_periodic_event_stream_req_from_json(const cJSON *obj, ocpp21_get_periodic_event_stream_req_t *out);
ocpp_err_t ocpp21_get_periodic_event_stream_req_encode(const ocpp21_get_periodic_event_stream_req_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_get_periodic_event_stream_req_decode(const char *json, ocpp21_get_periodic_event_stream_req_t *out);
void ocpp21_get_periodic_event_stream_req_example(ocpp21_get_periodic_event_stream_req_t *out);
cJSON *ocpp21_get_periodic_event_stream_conf_to_json(const ocpp21_get_periodic_event_stream_conf_t *in);
ocpp_err_t ocpp21_get_periodic_event_stream_conf_from_json(const cJSON *obj, ocpp21_get_periodic_event_stream_conf_t *out);
ocpp_err_t ocpp21_get_periodic_event_stream_conf_encode(const ocpp21_get_periodic_event_stream_conf_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_get_periodic_event_stream_conf_decode(const char *json, ocpp21_get_periodic_event_stream_conf_t *out);
void ocpp21_get_periodic_event_stream_conf_example(ocpp21_get_periodic_event_stream_conf_t *out);
#ifdef __cplusplus
}
#endif
#endif
