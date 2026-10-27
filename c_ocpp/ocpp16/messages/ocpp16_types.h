/* OCPP 1.6 共用嵌套结构（IdTagInfo、MeterValue、ChargingProfile 等）。 */
#ifndef OCPP16_TYPES_H
#define OCPP16_TYPES_H

#include "cJSON.h"
#include "ocpp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ocpp16_id_tag_info ocpp16_id_tag_info_t;
typedef struct ocpp16_sampled_value ocpp16_sampled_value_t;
typedef struct ocpp16_meter_value ocpp16_meter_value_t;
typedef struct ocpp16_cs_period ocpp16_cs_period_t;
typedef struct ocpp16_charging_schedule ocpp16_charging_schedule_t;
typedef struct ocpp16_charging_profile ocpp16_charging_profile_t;
typedef struct ocpp16_key_value ocpp16_key_value_t;
typedef struct ocpp16_auth_data ocpp16_auth_data_t;
typedef struct ocpp16_cert_hash ocpp16_cert_hash_t;
typedef struct ocpp16_log_params ocpp16_log_params_t;
typedef struct ocpp16_firmware_signed ocpp16_firmware_signed_t;

struct ocpp16_id_tag_info {
    char status[21];
    int has_expiryDate;
    char expiryDate[33];
    int has_parentIdTag;
    char parentIdTag[22];
};

struct ocpp16_sampled_value {
    char value[33];
    int has_measurand;
    char measurand[49];
    int has_unit;
    char unit[17];
};

struct ocpp16_meter_value {
    char timestamp[33];
    int sampledValue_count;
    ocpp16_sampled_value_t sampledValue[8];
};

struct ocpp16_cs_period {
    int startPeriod;
    double limit;
    int has_numberPhases;
    int numberPhases;
};

struct ocpp16_charging_schedule {
    char chargingRateUnit[9];
    int chargingSchedulePeriod_count;
    ocpp16_cs_period_t chargingSchedulePeriod[8];
    int has_duration;
    int duration;
};

struct ocpp16_charging_profile {
    int chargingProfileId;
    int stackLevel;
    char chargingProfilePurpose[33];
    char chargingProfileKind[21];
    ocpp16_charging_schedule_t chargingSchedule;
    int has_transactionId;
    int transactionId;
};

struct ocpp16_key_value {
    char key[51];
    int readonly;
    int has_value;
    char value[501];
};

struct ocpp16_auth_data {
    char idTag[22];
    int has_idTagInfo;
    ocpp16_id_tag_info_t idTagInfo;
};

struct ocpp16_cert_hash {
    char hashAlgorithm[17];
    char issuerNameHash[129];
    char issuerKeyHash[129];
    char serialNumber[41];
};

struct ocpp16_log_params {
    char remoteLocation[513];
};

struct ocpp16_firmware_signed {
    char location[513];
    char retrieveDateTime[33];
    char signingCertificate[801];
    char signature[257];
};

/**
 * ocpp16_id_tag_info_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_id_tag_info_to_json(const ocpp16_id_tag_info_t *in);
/**
 * ocpp16_id_tag_info_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_id_tag_info_from_json(const cJSON *obj, ocpp16_id_tag_info_t *out);
/**
 * ocpp16_id_tag_info_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_id_tag_info_example(ocpp16_id_tag_info_t *out);

/**
 * ocpp16_sampled_value_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_sampled_value_to_json(const ocpp16_sampled_value_t *in);
/**
 * ocpp16_sampled_value_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_sampled_value_from_json(const cJSON *obj, ocpp16_sampled_value_t *out);
/**
 * ocpp16_sampled_value_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_sampled_value_example(ocpp16_sampled_value_t *out);

/**
 * ocpp16_meter_value_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_meter_value_to_json(const ocpp16_meter_value_t *in);
/**
 * ocpp16_meter_value_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_meter_value_from_json(const cJSON *obj, ocpp16_meter_value_t *out);
/**
 * ocpp16_meter_value_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_meter_value_example(ocpp16_meter_value_t *out);

/**
 * ocpp16_cs_period_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_cs_period_to_json(const ocpp16_cs_period_t *in);
/**
 * ocpp16_cs_period_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_cs_period_from_json(const cJSON *obj, ocpp16_cs_period_t *out);
/**
 * ocpp16_cs_period_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_cs_period_example(ocpp16_cs_period_t *out);

/**
 * ocpp16_charging_schedule_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_charging_schedule_to_json(const ocpp16_charging_schedule_t *in);
/**
 * ocpp16_charging_schedule_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_charging_schedule_from_json(const cJSON *obj, ocpp16_charging_schedule_t *out);
/**
 * ocpp16_charging_schedule_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_charging_schedule_example(ocpp16_charging_schedule_t *out);

/**
 * ocpp16_charging_profile_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_charging_profile_to_json(const ocpp16_charging_profile_t *in);
/**
 * ocpp16_charging_profile_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_charging_profile_from_json(const cJSON *obj, ocpp16_charging_profile_t *out);
/**
 * ocpp16_charging_profile_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_charging_profile_example(ocpp16_charging_profile_t *out);

/**
 * ocpp16_key_value_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_key_value_to_json(const ocpp16_key_value_t *in);
/**
 * ocpp16_key_value_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_key_value_from_json(const cJSON *obj, ocpp16_key_value_t *out);
/**
 * ocpp16_key_value_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_key_value_example(ocpp16_key_value_t *out);

/**
 * ocpp16_auth_data_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_auth_data_to_json(const ocpp16_auth_data_t *in);
/**
 * ocpp16_auth_data_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_auth_data_from_json(const cJSON *obj, ocpp16_auth_data_t *out);
/**
 * ocpp16_auth_data_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_auth_data_example(ocpp16_auth_data_t *out);

/**
 * ocpp16_cert_hash_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_cert_hash_to_json(const ocpp16_cert_hash_t *in);
/**
 * ocpp16_cert_hash_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_cert_hash_from_json(const cJSON *obj, ocpp16_cert_hash_t *out);
/**
 * ocpp16_cert_hash_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_cert_hash_example(ocpp16_cert_hash_t *out);

/**
 * ocpp16_log_params_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_log_params_to_json(const ocpp16_log_params_t *in);
/**
 * ocpp16_log_params_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_log_params_from_json(const cJSON *obj, ocpp16_log_params_t *out);
/**
 * ocpp16_log_params_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_log_params_example(ocpp16_log_params_t *out);

/**
 * ocpp16_firmware_signed_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_firmware_signed_to_json(const ocpp16_firmware_signed_t *in);
/**
 * ocpp16_firmware_signed_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_firmware_signed_from_json(const cJSON *obj, ocpp16_firmware_signed_t *out);
/**
 * ocpp16_firmware_signed_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_firmware_signed_example(ocpp16_firmware_signed_t *out);

#ifdef __cplusplus
}
#endif

#endif
