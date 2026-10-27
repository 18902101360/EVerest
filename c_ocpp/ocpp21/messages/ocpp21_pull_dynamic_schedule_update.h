#ifndef OCPP21_PULLDYNAMICSCHEDULEUPDATE_H
#define OCPP21_PULLDYNAMICSCHEDULEUPDATE_H
#include "ocpp_err.h"
#include "ocpp_port.h"
#include "ocpp_json.h"
#ifdef __cplusplus
extern "C" {
#endif
#define OCPP21_PULLDYNAMICSCHEDULEUPDATE_ACTION "PullDynamicScheduleUpdate"

typedef struct ocpp21_pull_dynamic_schedule_update_req {
    int chargingProfileId;
} ocpp21_pull_dynamic_schedule_update_req_t;
typedef struct ocpp21_pull_dynamic_schedule_update_scheduleUpdate {
    int has_limit;
    double limit;
} ocpp21_pull_dynamic_schedule_update_scheduleUpdate_t;

typedef struct ocpp21_pull_dynamic_schedule_update_conf {
    char status[33];
    int has_scheduleUpdate;
    ocpp21_pull_dynamic_schedule_update_scheduleUpdate_t scheduleUpdate;
} ocpp21_pull_dynamic_schedule_update_conf_t;

cJSON *ocpp21_pull_dynamic_schedule_update_scheduleUpdate_to_json(const ocpp21_pull_dynamic_schedule_update_scheduleUpdate_t *in);
ocpp_err_t ocpp21_pull_dynamic_schedule_update_scheduleUpdate_from_json(const cJSON *obj, ocpp21_pull_dynamic_schedule_update_scheduleUpdate_t *out);
cJSON *ocpp21_pull_dynamic_schedule_update_req_to_json(const ocpp21_pull_dynamic_schedule_update_req_t *in);
ocpp_err_t ocpp21_pull_dynamic_schedule_update_req_from_json(const cJSON *obj, ocpp21_pull_dynamic_schedule_update_req_t *out);
ocpp_err_t ocpp21_pull_dynamic_schedule_update_req_encode(const ocpp21_pull_dynamic_schedule_update_req_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_pull_dynamic_schedule_update_req_decode(const char *json, ocpp21_pull_dynamic_schedule_update_req_t *out);
void ocpp21_pull_dynamic_schedule_update_req_example(ocpp21_pull_dynamic_schedule_update_req_t *out);
cJSON *ocpp21_pull_dynamic_schedule_update_conf_to_json(const ocpp21_pull_dynamic_schedule_update_conf_t *in);
ocpp_err_t ocpp21_pull_dynamic_schedule_update_conf_from_json(const cJSON *obj, ocpp21_pull_dynamic_schedule_update_conf_t *out);
ocpp_err_t ocpp21_pull_dynamic_schedule_update_conf_encode(const ocpp21_pull_dynamic_schedule_update_conf_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_pull_dynamic_schedule_update_conf_decode(const char *json, ocpp21_pull_dynamic_schedule_update_conf_t *out);
void ocpp21_pull_dynamic_schedule_update_conf_example(ocpp21_pull_dynamic_schedule_update_conf_t *out);
#ifdef __cplusplus
}
#endif
#endif
