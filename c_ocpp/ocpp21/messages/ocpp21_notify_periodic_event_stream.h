#ifndef OCPP21_NOTIFYPERIODICEVENTSTREAM_H
#define OCPP21_NOTIFYPERIODICEVENTSTREAM_H
#include "ocpp_err.h"
#include "ocpp_port.h"
#include "ocpp_json.h"
#ifdef __cplusplus
extern "C" {
#endif
#define OCPP21_NOTIFYPERIODICEVENTSTREAM_ACTION "NotifyPeriodicEventStream"
typedef struct ocpp21_notify_periodic_event_stream_data {
    double t;
    char v[64];
} ocpp21_notify_periodic_event_stream_data_t;

typedef struct ocpp21_notify_periodic_event_stream_req {
    int data_count;
    ocpp21_notify_periodic_event_stream_data_t data[2];
    int id;
    int pending;
    char basetime[33];
} ocpp21_notify_periodic_event_stream_req_t;

typedef struct ocpp21_notify_periodic_event_stream_conf {
    int unused;
} ocpp21_notify_periodic_event_stream_conf_t;
cJSON *ocpp21_notify_periodic_event_stream_data_to_json(const ocpp21_notify_periodic_event_stream_data_t *in);
ocpp_err_t ocpp21_notify_periodic_event_stream_data_from_json(const cJSON *obj, ocpp21_notify_periodic_event_stream_data_t *out);

cJSON *ocpp21_notify_periodic_event_stream_req_to_json(const ocpp21_notify_periodic_event_stream_req_t *in);
ocpp_err_t ocpp21_notify_periodic_event_stream_req_from_json(const cJSON *obj, ocpp21_notify_periodic_event_stream_req_t *out);
ocpp_err_t ocpp21_notify_periodic_event_stream_req_encode(const ocpp21_notify_periodic_event_stream_req_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_notify_periodic_event_stream_req_decode(const char *json, ocpp21_notify_periodic_event_stream_req_t *out);
void ocpp21_notify_periodic_event_stream_req_example(ocpp21_notify_periodic_event_stream_req_t *out);
cJSON *ocpp21_notify_periodic_event_stream_conf_to_json(const ocpp21_notify_periodic_event_stream_conf_t *in);
ocpp_err_t ocpp21_notify_periodic_event_stream_conf_from_json(const cJSON *obj, ocpp21_notify_periodic_event_stream_conf_t *out);
ocpp_err_t ocpp21_notify_periodic_event_stream_conf_encode(const ocpp21_notify_periodic_event_stream_conf_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_notify_periodic_event_stream_conf_decode(const char *json, ocpp21_notify_periodic_event_stream_conf_t *out);
void ocpp21_notify_periodic_event_stream_conf_example(ocpp21_notify_periodic_event_stream_conf_t *out);
#ifdef __cplusplus
}
#endif
#endif
