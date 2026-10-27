#ifndef OCPP201_TYPES_H
#define OCPP201_TYPES_H
/* OCPP 2.0.1 共用嵌套结构（IdToken、Transaction、Variable 等）。 */
#include "cJSON.h"
#include "ocpp_err.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct ocpp201_status_info ocpp201_status_info_t;
typedef struct ocpp201_id_token ocpp201_id_token_t;
typedef struct ocpp201_id_token_info ocpp201_id_token_info_t;
typedef struct ocpp201_charging_station ocpp201_charging_station_t;
typedef struct ocpp201_evse ocpp201_evse_t;
typedef struct ocpp201_component ocpp201_component_t;
typedef struct ocpp201_variable ocpp201_variable_t;
typedef struct ocpp201_sampled_value ocpp201_sampled_value_t;
typedef struct ocpp201_meter_value ocpp201_meter_value_t;
typedef struct ocpp201_cs_period ocpp201_cs_period_t;
typedef struct ocpp201_charging_schedule ocpp201_charging_schedule_t;
typedef struct ocpp201_charging_profile ocpp201_charging_profile_t;
typedef struct ocpp201_ocsp ocpp201_ocsp_t;
typedef struct ocpp201_cert_hash ocpp201_cert_hash_t;
typedef struct ocpp201_log_params ocpp201_log_params_t;
typedef struct ocpp201_firmware ocpp201_firmware_t;
typedef struct ocpp201_network_connection ocpp201_network_connection_t;
typedef struct ocpp201_message_content ocpp201_message_content_t;
typedef struct ocpp201_display_message ocpp201_display_message_t;
typedef struct ocpp201_get_var_data ocpp201_get_var_data_t;
typedef struct ocpp201_get_var_result ocpp201_get_var_result_t;
typedef struct ocpp201_set_var_data ocpp201_set_var_data_t;
typedef struct ocpp201_set_var_result ocpp201_set_var_result_t;
typedef struct ocpp201_charging_limit ocpp201_charging_limit_t;
typedef struct ocpp201_charging_needs ocpp201_charging_needs_t;
typedef struct ocpp201_event_data ocpp201_event_data_t;
typedef struct ocpp201_transaction_info ocpp201_transaction_info_t;
typedef struct ocpp201_monitor_data ocpp201_monitor_data_t;
typedef struct ocpp201_monitor_result ocpp201_monitor_result_t;
typedef struct ocpp201_clear_mon_result ocpp201_clear_mon_result_t;
typedef struct ocpp201_charging_profile_criterion ocpp201_charging_profile_criterion_t;

struct ocpp201_status_info {
    char reasonCode[21];
    int has_additionalInfo;
    char additionalInfo[513];
};

struct ocpp201_id_token {
    char idToken[37];
    char type[21];
};

struct ocpp201_id_token_info {
    char status[21];
};

struct ocpp201_charging_station {
    char model[21];
    char vendorName[21];
    int has_serialNumber;
    char serialNumber[26];
    int has_firmwareVersion;
    char firmwareVersion[51];
};

struct ocpp201_evse {
    int id;
    int has_connectorId;
    int connectorId;
};

struct ocpp201_component {
    char name[51];
    int has_instance;
    char instance[51];
};

struct ocpp201_variable {
    char name[51];
    int has_instance;
    char instance[51];
};

struct ocpp201_sampled_value {
    double value;
    int has_measurand;
    char measurand[49];
};

struct ocpp201_meter_value {
    char timestamp[33];
    int sampledValue_count;
    ocpp201_sampled_value_t sampledValue[8];
};

struct ocpp201_cs_period {
    int startPeriod;
    double limit;
};

struct ocpp201_charging_schedule {
    int id;
    char chargingRateUnit[9];
    int chargingSchedulePeriod_count;
    ocpp201_cs_period_t chargingSchedulePeriod[8];
};

struct ocpp201_charging_profile {
    int id;
    int stackLevel;
    char chargingProfilePurpose[33];
    char chargingProfileKind[21];
    int chargingSchedule_count;
    ocpp201_charging_schedule_t chargingSchedule[2];
};

struct ocpp201_ocsp {
    char hashAlgorithm[17];
    char issuerNameHash[129];
    char issuerKeyHash[129];
    char serialNumber[41];
    char responderURL[513];
};

struct ocpp201_cert_hash {
    char hashAlgorithm[17];
    char issuerNameHash[129];
    char issuerKeyHash[129];
    char serialNumber[41];
};

struct ocpp201_log_params {
    char remoteLocation[513];
};

struct ocpp201_firmware {
    char location[513];
    char retrieveDateTime[33];
};

struct ocpp201_network_connection {
    char ocppVersion[17];
    char ocppTransport[9];
    char ocppCsmsUrl[513];
    int messageTimeout;
    int securityProfile;
    char ocppInterface[21];
};

struct ocpp201_message_content {
    char format[17];
    char content[513];
};

struct ocpp201_display_message {
    int id;
    char priority[21];
    ocpp201_message_content_t message;
};

struct ocpp201_get_var_data {
    ocpp201_component_t component;
    ocpp201_variable_t variable;
};

struct ocpp201_get_var_result {
    char attributeStatus[21];
    ocpp201_component_t component;
    ocpp201_variable_t variable;
    int has_attributeValue;
    char attributeValue[501];
};

struct ocpp201_set_var_data {
    char attributeValue[501];
    ocpp201_component_t component;
    ocpp201_variable_t variable;
};

struct ocpp201_set_var_result {
    char attributeStatus[21];
    ocpp201_component_t component;
    ocpp201_variable_t variable;
};

struct ocpp201_charging_limit {
    char chargingLimitSource[21];
};

struct ocpp201_charging_needs {
    char requestedEnergyTransfer[33];
    int has_der;
    int evSupportedDERControl_count;
    char evSupportedDERControl[4][33];
    int has_maxApparentPower;
    double maxApparentPower;
    int has_v2x;
    int has_minDischargePower;
    double minDischargePower;
};

struct ocpp201_event_data {
    int eventId;
    char timestamp[33];
    char trigger[17];
    char actualValue[129];
    char eventNotificationType[33];
    ocpp201_component_t component;
    ocpp201_variable_t variable;
};

struct ocpp201_transaction_info {
    char transactionId[37];
    int has_chargingState;
    char chargingState[21];
};

struct ocpp201_monitor_data {
    double value;
    char type[21];
    int severity;
    ocpp201_component_t component;
    ocpp201_variable_t variable;
};

struct ocpp201_monitor_result {
    char status[21];
    char type[21];
    int severity;
    ocpp201_component_t component;
    ocpp201_variable_t variable;
};

struct ocpp201_clear_mon_result {
    char status[21];
    int id;
};

struct ocpp201_charging_profile_criterion {
    int has_chargingProfilePurpose;
    char chargingProfilePurpose[33];
};

/**
 * ocpp201_status_info_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_status_info_to_json(const ocpp201_status_info_t *in);
/**
 * ocpp201_status_info_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_status_info_from_json(const cJSON *obj, ocpp201_status_info_t *out);
/**
 * ocpp201_status_info_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_status_info_example(ocpp201_status_info_t *out);
/**
 * ocpp201_id_token_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_id_token_to_json(const ocpp201_id_token_t *in);
/**
 * ocpp201_id_token_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_id_token_from_json(const cJSON *obj, ocpp201_id_token_t *out);
/**
 * ocpp201_id_token_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_id_token_example(ocpp201_id_token_t *out);
/**
 * ocpp201_id_token_info_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_id_token_info_to_json(const ocpp201_id_token_info_t *in);
/**
 * ocpp201_id_token_info_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_id_token_info_from_json(const cJSON *obj, ocpp201_id_token_info_t *out);
/**
 * ocpp201_id_token_info_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_id_token_info_example(ocpp201_id_token_info_t *out);
/**
 * ocpp201_charging_station_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_charging_station_to_json(const ocpp201_charging_station_t *in);
/**
 * ocpp201_charging_station_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_charging_station_from_json(const cJSON *obj, ocpp201_charging_station_t *out);
/**
 * ocpp201_charging_station_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_charging_station_example(ocpp201_charging_station_t *out);
/**
 * ocpp201_evse_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_evse_to_json(const ocpp201_evse_t *in);
/**
 * ocpp201_evse_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_evse_from_json(const cJSON *obj, ocpp201_evse_t *out);
/**
 * ocpp201_evse_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_evse_example(ocpp201_evse_t *out);
/**
 * ocpp201_component_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_component_to_json(const ocpp201_component_t *in);
/**
 * ocpp201_component_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_component_from_json(const cJSON *obj, ocpp201_component_t *out);
/**
 * ocpp201_component_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_component_example(ocpp201_component_t *out);
/**
 * ocpp201_variable_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_variable_to_json(const ocpp201_variable_t *in);
/**
 * ocpp201_variable_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_variable_from_json(const cJSON *obj, ocpp201_variable_t *out);
/**
 * ocpp201_variable_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_variable_example(ocpp201_variable_t *out);
/**
 * ocpp201_sampled_value_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_sampled_value_to_json(const ocpp201_sampled_value_t *in);
/**
 * ocpp201_sampled_value_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_sampled_value_from_json(const cJSON *obj, ocpp201_sampled_value_t *out);
/**
 * ocpp201_sampled_value_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_sampled_value_example(ocpp201_sampled_value_t *out);
/**
 * ocpp201_meter_value_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_meter_value_to_json(const ocpp201_meter_value_t *in);
/**
 * ocpp201_meter_value_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_meter_value_from_json(const cJSON *obj, ocpp201_meter_value_t *out);
/**
 * ocpp201_meter_value_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_meter_value_example(ocpp201_meter_value_t *out);
/**
 * ocpp201_cs_period_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_cs_period_to_json(const ocpp201_cs_period_t *in);
/**
 * ocpp201_cs_period_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_cs_period_from_json(const cJSON *obj, ocpp201_cs_period_t *out);
/**
 * ocpp201_cs_period_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_cs_period_example(ocpp201_cs_period_t *out);
/**
 * ocpp201_charging_schedule_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_charging_schedule_to_json(const ocpp201_charging_schedule_t *in);
/**
 * ocpp201_charging_schedule_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_charging_schedule_from_json(const cJSON *obj, ocpp201_charging_schedule_t *out);
/**
 * ocpp201_charging_schedule_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_charging_schedule_example(ocpp201_charging_schedule_t *out);
/**
 * ocpp201_charging_profile_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_charging_profile_to_json(const ocpp201_charging_profile_t *in);
/**
 * ocpp201_charging_profile_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_charging_profile_from_json(const cJSON *obj, ocpp201_charging_profile_t *out);
/**
 * ocpp201_charging_profile_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_charging_profile_example(ocpp201_charging_profile_t *out);
/**
 * ocpp201_ocsp_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_ocsp_to_json(const ocpp201_ocsp_t *in);
/**
 * ocpp201_ocsp_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_ocsp_from_json(const cJSON *obj, ocpp201_ocsp_t *out);
/**
 * ocpp201_ocsp_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_ocsp_example(ocpp201_ocsp_t *out);
/**
 * ocpp201_cert_hash_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_cert_hash_to_json(const ocpp201_cert_hash_t *in);
/**
 * ocpp201_cert_hash_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_cert_hash_from_json(const cJSON *obj, ocpp201_cert_hash_t *out);
/**
 * ocpp201_cert_hash_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_cert_hash_example(ocpp201_cert_hash_t *out);
/**
 * ocpp201_log_params_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_log_params_to_json(const ocpp201_log_params_t *in);
/**
 * ocpp201_log_params_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_log_params_from_json(const cJSON *obj, ocpp201_log_params_t *out);
/**
 * ocpp201_log_params_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_log_params_example(ocpp201_log_params_t *out);
/**
 * ocpp201_firmware_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_firmware_to_json(const ocpp201_firmware_t *in);
/**
 * ocpp201_firmware_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_firmware_from_json(const cJSON *obj, ocpp201_firmware_t *out);
/**
 * ocpp201_firmware_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_firmware_example(ocpp201_firmware_t *out);
/**
 * ocpp201_network_connection_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_network_connection_to_json(const ocpp201_network_connection_t *in);
/**
 * ocpp201_network_connection_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_network_connection_from_json(const cJSON *obj, ocpp201_network_connection_t *out);
/**
 * ocpp201_network_connection_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_network_connection_example(ocpp201_network_connection_t *out);
/**
 * ocpp201_message_content_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_message_content_to_json(const ocpp201_message_content_t *in);
/**
 * ocpp201_message_content_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_message_content_from_json(const cJSON *obj, ocpp201_message_content_t *out);
/**
 * ocpp201_message_content_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_message_content_example(ocpp201_message_content_t *out);
/**
 * ocpp201_display_message_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_display_message_to_json(const ocpp201_display_message_t *in);
/**
 * ocpp201_display_message_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_display_message_from_json(const cJSON *obj, ocpp201_display_message_t *out);
/**
 * ocpp201_display_message_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_display_message_example(ocpp201_display_message_t *out);
/**
 * ocpp201_get_var_data_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_get_var_data_to_json(const ocpp201_get_var_data_t *in);
/**
 * ocpp201_get_var_data_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_get_var_data_from_json(const cJSON *obj, ocpp201_get_var_data_t *out);
/**
 * ocpp201_get_var_data_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_get_var_data_example(ocpp201_get_var_data_t *out);
/**
 * ocpp201_get_var_result_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_get_var_result_to_json(const ocpp201_get_var_result_t *in);
/**
 * ocpp201_get_var_result_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_get_var_result_from_json(const cJSON *obj, ocpp201_get_var_result_t *out);
/**
 * ocpp201_get_var_result_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_get_var_result_example(ocpp201_get_var_result_t *out);
/**
 * ocpp201_set_var_data_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_set_var_data_to_json(const ocpp201_set_var_data_t *in);
/**
 * ocpp201_set_var_data_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_set_var_data_from_json(const cJSON *obj, ocpp201_set_var_data_t *out);
/**
 * ocpp201_set_var_data_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_set_var_data_example(ocpp201_set_var_data_t *out);
/**
 * ocpp201_set_var_result_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_set_var_result_to_json(const ocpp201_set_var_result_t *in);
/**
 * ocpp201_set_var_result_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_set_var_result_from_json(const cJSON *obj, ocpp201_set_var_result_t *out);
/**
 * ocpp201_set_var_result_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_set_var_result_example(ocpp201_set_var_result_t *out);
/**
 * ocpp201_charging_limit_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_charging_limit_to_json(const ocpp201_charging_limit_t *in);
/**
 * ocpp201_charging_limit_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_charging_limit_from_json(const cJSON *obj, ocpp201_charging_limit_t *out);
/**
 * ocpp201_charging_limit_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_charging_limit_example(ocpp201_charging_limit_t *out);
/**
 * ocpp201_charging_needs_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_charging_needs_to_json(const ocpp201_charging_needs_t *in);
/**
 * ocpp201_charging_needs_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_charging_needs_from_json(const cJSON *obj, ocpp201_charging_needs_t *out);
/**
 * ocpp201_charging_needs_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_charging_needs_example(ocpp201_charging_needs_t *out);
/**
 * ocpp201_event_data_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_event_data_to_json(const ocpp201_event_data_t *in);
/**
 * ocpp201_event_data_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_event_data_from_json(const cJSON *obj, ocpp201_event_data_t *out);
/**
 * ocpp201_event_data_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_event_data_example(ocpp201_event_data_t *out);
/**
 * ocpp201_transaction_info_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_transaction_info_to_json(const ocpp201_transaction_info_t *in);
/**
 * ocpp201_transaction_info_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_transaction_info_from_json(const cJSON *obj, ocpp201_transaction_info_t *out);
/**
 * ocpp201_transaction_info_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_transaction_info_example(ocpp201_transaction_info_t *out);
/**
 * ocpp201_monitor_data_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_monitor_data_to_json(const ocpp201_monitor_data_t *in);
/**
 * ocpp201_monitor_data_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_monitor_data_from_json(const cJSON *obj, ocpp201_monitor_data_t *out);
/**
 * ocpp201_monitor_data_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_monitor_data_example(ocpp201_monitor_data_t *out);
/**
 * ocpp201_monitor_result_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_monitor_result_to_json(const ocpp201_monitor_result_t *in);
/**
 * ocpp201_monitor_result_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_monitor_result_from_json(const cJSON *obj, ocpp201_monitor_result_t *out);
/**
 * ocpp201_monitor_result_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_monitor_result_example(ocpp201_monitor_result_t *out);
/**
 * ocpp201_clear_mon_result_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_clear_mon_result_to_json(const ocpp201_clear_mon_result_t *in);
/**
 * ocpp201_clear_mon_result_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_clear_mon_result_from_json(const cJSON *obj, ocpp201_clear_mon_result_t *out);
/**
 * ocpp201_clear_mon_result_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_clear_mon_result_example(ocpp201_clear_mon_result_t *out);
/**
 * ocpp201_charging_profile_criterion_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_charging_profile_criterion_to_json(const ocpp201_charging_profile_criterion_t *in);
/**
 * ocpp201_charging_profile_criterion_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_charging_profile_criterion_from_json(const cJSON *obj, ocpp201_charging_profile_criterion_t *out);
/**
 * ocpp201_charging_profile_criterion_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_charging_profile_criterion_example(ocpp201_charging_profile_criterion_t *out);
#ifdef __cplusplus
}
#endif
#endif
