#ifndef OCPP21_CLOSEPERIODICEVENTSTREAM_H
#define OCPP21_CLOSEPERIODICEVENTSTREAM_H
#include "ocpp_err.h"
#include "ocpp_port.h"
#include "ocpp_json.h"
#ifdef __cplusplus
extern "C" {
#endif
#define OCPP21_CLOSEPERIODICEVENTSTREAM_ACTION "ClosePeriodicEventStream"

typedef struct ocpp21_close_periodic_event_stream_req {
    int id;
} ocpp21_close_periodic_event_stream_req_t;

typedef struct ocpp21_close_periodic_event_stream_conf {
    int unused;
} ocpp21_close_periodic_event_stream_conf_t;


cJSON *ocpp21_close_periodic_event_stream_req_to_json(const ocpp21_close_periodic_event_stream_req_t *in);
ocpp_err_t ocpp21_close_periodic_event_stream_req_from_json(const cJSON *obj, ocpp21_close_periodic_event_stream_req_t *out);
ocpp_err_t ocpp21_close_periodic_event_stream_req_encode(const ocpp21_close_periodic_event_stream_req_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_close_periodic_event_stream_req_decode(const char *json, ocpp21_close_periodic_event_stream_req_t *out);
void ocpp21_close_periodic_event_stream_req_example(ocpp21_close_periodic_event_stream_req_t *out);
cJSON *ocpp21_close_periodic_event_stream_conf_to_json(const ocpp21_close_periodic_event_stream_conf_t *in);
ocpp_err_t ocpp21_close_periodic_event_stream_conf_from_json(const cJSON *obj, ocpp21_close_periodic_event_stream_conf_t *out);
ocpp_err_t ocpp21_close_periodic_event_stream_conf_encode(const ocpp21_close_periodic_event_stream_conf_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_close_periodic_event_stream_conf_decode(const char *json, ocpp21_close_periodic_event_stream_conf_t *out);
void ocpp21_close_periodic_event_stream_conf_example(ocpp21_close_periodic_event_stream_conf_t *out);
#ifdef __cplusplus
}
#endif
#endif
