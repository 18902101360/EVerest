#ifndef OCPP21_BATTERYSWAP_H
#define OCPP21_BATTERYSWAP_H
#include "ocpp_err.h"
#include "ocpp_port.h"
#include "ocpp_json.h"
#ifdef __cplusplus
extern "C" {
#endif
#define OCPP21_BATTERYSWAP_ACTION "BatterySwap"
typedef struct ocpp21_battery_swap_batteryData {
    int evseId;
    char serialNumber[51];
    double soC;
    double soH;
} ocpp21_battery_swap_batteryData_t;

typedef struct ocpp21_battery_swap_idToken {
    char idToken[256];
    char type[21];
} ocpp21_battery_swap_idToken_t;

typedef struct ocpp21_battery_swap_req {
    int batteryData_count;
    ocpp21_battery_swap_batteryData_t batteryData[2];
    char eventType[21];
    ocpp21_battery_swap_idToken_t idToken;
    int requestId;
} ocpp21_battery_swap_req_t;

typedef struct ocpp21_battery_swap_conf {
    int unused;
} ocpp21_battery_swap_conf_t;
cJSON *ocpp21_battery_swap_batteryData_to_json(const ocpp21_battery_swap_batteryData_t *in);
ocpp_err_t ocpp21_battery_swap_batteryData_from_json(const cJSON *obj, ocpp21_battery_swap_batteryData_t *out);
cJSON *ocpp21_battery_swap_idToken_to_json(const ocpp21_battery_swap_idToken_t *in);
ocpp_err_t ocpp21_battery_swap_idToken_from_json(const cJSON *obj, ocpp21_battery_swap_idToken_t *out);

cJSON *ocpp21_battery_swap_req_to_json(const ocpp21_battery_swap_req_t *in);
ocpp_err_t ocpp21_battery_swap_req_from_json(const cJSON *obj, ocpp21_battery_swap_req_t *out);
ocpp_err_t ocpp21_battery_swap_req_encode(const ocpp21_battery_swap_req_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_battery_swap_req_decode(const char *json, ocpp21_battery_swap_req_t *out);
void ocpp21_battery_swap_req_example(ocpp21_battery_swap_req_t *out);
cJSON *ocpp21_battery_swap_conf_to_json(const ocpp21_battery_swap_conf_t *in);
ocpp_err_t ocpp21_battery_swap_conf_from_json(const cJSON *obj, ocpp21_battery_swap_conf_t *out);
ocpp_err_t ocpp21_battery_swap_conf_encode(const ocpp21_battery_swap_conf_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_battery_swap_conf_decode(const char *json, ocpp21_battery_swap_conf_t *out);
void ocpp21_battery_swap_conf_example(ocpp21_battery_swap_conf_t *out);
#ifdef __cplusplus
}
#endif
#endif
