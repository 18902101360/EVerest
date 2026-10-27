#ifndef OCPP21_GETCERTIFICATECHAINSTATUS_H
#define OCPP21_GETCERTIFICATECHAINSTATUS_H
#include "ocpp_err.h"
#include "ocpp_port.h"
#include "ocpp_json.h"
#ifdef __cplusplus
extern "C" {
#endif
#define OCPP21_GETCERTIFICATECHAINSTATUS_ACTION "GetCertificateChainStatus"
typedef struct ocpp21_get_certificate_chain_status_certificateStatusRequests {
    char hashAlgorithm[17];
    char issuerNameHash[129];
    char issuerKeyHash[129];
    char serialNumber[41];
    char source[17];
    int urls_count;
    char urls[2][129];
} ocpp21_get_certificate_chain_status_certificateStatusRequests_t;

typedef struct ocpp21_get_certificate_chain_status_req {
    int certificateStatusRequests_count;
    ocpp21_get_certificate_chain_status_certificateStatusRequests_t certificateStatusRequests[2];
} ocpp21_get_certificate_chain_status_req_t;
typedef struct ocpp21_get_certificate_chain_status_certificateStatus {
    char hashAlgorithm[17];
    char issuerNameHash[129];
    char issuerKeyHash[129];
    char serialNumber[41];
    char source[17];
    char status[21];
    char nextUpdate[33];
} ocpp21_get_certificate_chain_status_certificateStatus_t;

typedef struct ocpp21_get_certificate_chain_status_conf {
    int certificateStatus_count;
    ocpp21_get_certificate_chain_status_certificateStatus_t certificateStatus[2];
} ocpp21_get_certificate_chain_status_conf_t;
cJSON *ocpp21_get_certificate_chain_status_certificateStatusRequests_to_json(const ocpp21_get_certificate_chain_status_certificateStatusRequests_t *in);
ocpp_err_t ocpp21_get_certificate_chain_status_certificateStatusRequests_from_json(const cJSON *obj, ocpp21_get_certificate_chain_status_certificateStatusRequests_t *out);
cJSON *ocpp21_get_certificate_chain_status_certificateStatus_to_json(const ocpp21_get_certificate_chain_status_certificateStatus_t *in);
ocpp_err_t ocpp21_get_certificate_chain_status_certificateStatus_from_json(const cJSON *obj, ocpp21_get_certificate_chain_status_certificateStatus_t *out);
cJSON *ocpp21_get_certificate_chain_status_req_to_json(const ocpp21_get_certificate_chain_status_req_t *in);
ocpp_err_t ocpp21_get_certificate_chain_status_req_from_json(const cJSON *obj, ocpp21_get_certificate_chain_status_req_t *out);
ocpp_err_t ocpp21_get_certificate_chain_status_req_encode(const ocpp21_get_certificate_chain_status_req_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_get_certificate_chain_status_req_decode(const char *json, ocpp21_get_certificate_chain_status_req_t *out);
void ocpp21_get_certificate_chain_status_req_example(ocpp21_get_certificate_chain_status_req_t *out);
cJSON *ocpp21_get_certificate_chain_status_conf_to_json(const ocpp21_get_certificate_chain_status_conf_t *in);
ocpp_err_t ocpp21_get_certificate_chain_status_conf_from_json(const cJSON *obj, ocpp21_get_certificate_chain_status_conf_t *out);
ocpp_err_t ocpp21_get_certificate_chain_status_conf_encode(const ocpp21_get_certificate_chain_status_conf_t *in, char *buf, size_t buflen);
ocpp_err_t ocpp21_get_certificate_chain_status_conf_decode(const char *json, ocpp21_get_certificate_chain_status_conf_t *out);
void ocpp21_get_certificate_chain_status_conf_example(ocpp21_get_certificate_chain_status_conf_t *out);
#ifdef __cplusplus
}
#endif
#endif
