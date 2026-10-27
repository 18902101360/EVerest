#ifndef OCPP21_OPENPERIODICEVENTSTREAM_H
#define OCPP21_OPENPERIODICEVENTSTREAM_H
#include "ocpp_err.h"
#include "ocpp_port.h"
#include "ocpp_json.h"
#ifdef __cplusplus
extern "C" {
#endif
#define OCPP21_OPENPERIODICEVENTSTREAM_ACTION "OpenPeriodicEventStream"
typedef struct ocpp21_open_periodic_event_stream_constantStreamData_params {
    int has_interval;
    int interval;
    int has_values;
    int values;
} ocpp21_open_periodic_event_stream_constantStreamData_params_t;

typedef struct ocpp21_open_periodic_event_stream_constantStreamData {
    int id;
    ocpp21_open_periodic_event_stream_constantStreamData_params_t params;
    int variableMonitoringId;
} ocpp21_open_periodic_event_stream_constantStreamData_t;

typedef struct ocpp21_open_periodic_event_stream_req {
    ocpp21_open_periodic_event_stream_constantStreamData_t constantStreamData;
} ocpp21_open_periodic_event_stream_req_t;

typedef struct ocpp21_open_periodic_event_stream_conf {
    char status[21];
} ocpp21_open_periodic_event_stream_conf_t;
cJSON *ocpp21_open_periodic_event_stream_constantStreamData_params_to_json(const ocpp21_open_periodic_event_stream_constantStreamData_params_t *in);
ocpp_err_t ocpp21_open_periodic_event_stream_constantStreamData_params_from_json(const cJSON *obj, ocpp21_open_periodic_event_stream_constantStreamData_params_t *out);
cJSON *ocpp21_open_periodic_event_stream_constantStreamData_to_json(const ocpp21_open_periodic_event_stream_constantStreamData_t *in);
ocpp_err_t ocpp21_open_periodic_event_stream_constantStreamData_from_json(const cJSON *obj, ocpp21_open_periodic_event_stream_constantStreamData_t *out);

cJSON *ocpp21_open_periodic_event_stream_req_to_json(const ocpp21_open_periodic_event_stream_req_t *in);
ocpp_err_t ocpp21_open_periodic_event_stream_req_from_json(const cJSON *obj, ocpp21_open_periodic_event_stream_req_t *out);
ocpp_err_t ocpp21_open_periodic_event_stream_req_encode(const ocpp21_open_periodic_event_stream_req_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_open_periodic_event_stream_req_decode(const char *json, ocpp21_open_periodic_event_stream_req_t *out);
void ocpp21_open_periodic_event_stream_req_example(ocpp21_open_periodic_event_stream_req_t *out);
cJSON *ocpp21_open_periodic_event_stream_conf_to_json(const ocpp21_open_periodic_event_stream_conf_t *in);
ocpp_err_t ocpp21_open_periodic_event_stream_conf_from_json(const cJSON *obj, ocpp21_open_periodic_event_stream_conf_t *out);
ocpp_err_t ocpp21_open_periodic_event_stream_conf_encode(const ocpp21_open_periodic_event_stream_conf_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_open_periodic_event_stream_conf_decode(const char *json, ocpp21_open_periodic_event_stream_conf_t *out);
void ocpp21_open_periodic_event_stream_conf_example(ocpp21_open_periodic_event_stream_conf_t *out);
#ifdef __cplusplus
}
#endif
#endif
