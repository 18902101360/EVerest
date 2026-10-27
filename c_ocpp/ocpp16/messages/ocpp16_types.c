/* Generated */
#include "ocpp16_types.h"
#include "ocpp_json.h"
#include <string.h>

/**
 * ocpp16_id_tag_info_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_id_tag_info_to_json(const ocpp16_id_tag_info_t *in) {
    if (in == NULL) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL) return NULL;
    if (ocpp_json_add_str(obj, "status", in->status) < 0) { cJSON_Delete(obj); return NULL; }
    if (in->has_expiryDate) ocpp_json_add_str(obj, "expiryDate", in->expiryDate);
    if (in->has_parentIdTag) ocpp_json_add_str(obj, "parentIdTag", in->parentIdTag);
    return obj;
}
/**
 * ocpp16_id_tag_info_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_id_tag_info_from_json(const cJSON *obj, ocpp16_id_tag_info_t *out) {
    if (obj == NULL || out == NULL) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    if (ocpp_json_get_str(obj, "status", out->status, sizeof(out->status), 1) < 0) return OCPP_ERR_PARSE;
    out->has_expiryDate = ocpp_json_get_str(obj, "expiryDate", out->expiryDate, sizeof(out->expiryDate), 0) > 0;
    out->has_parentIdTag = ocpp_json_get_str(obj, "parentIdTag", out->parentIdTag, sizeof(out->parentIdTag), 0) > 0;
    return OCPP_OK;
}
/**
 * ocpp16_id_tag_info_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_id_tag_info_example(ocpp16_id_tag_info_t *out) {
    memset(out, 0, sizeof(*out));
    ocpp_str_copy(out->status, sizeof(out->status), "Accepted");
}

/**
 * ocpp16_sampled_value_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_sampled_value_to_json(const ocpp16_sampled_value_t *in) {
    if (in == NULL) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL) return NULL;
    if (ocpp_json_add_str(obj, "value", in->value) < 0) { cJSON_Delete(obj); return NULL; }
    if (in->has_measurand) ocpp_json_add_str(obj, "measurand", in->measurand);
    if (in->has_unit) ocpp_json_add_str(obj, "unit", in->unit);
    return obj;
}
/**
 * ocpp16_sampled_value_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_sampled_value_from_json(const cJSON *obj, ocpp16_sampled_value_t *out) {
    if (obj == NULL || out == NULL) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    if (ocpp_json_get_str(obj, "value", out->value, sizeof(out->value), 1) < 0) return OCPP_ERR_PARSE;
    out->has_measurand = ocpp_json_get_str(obj, "measurand", out->measurand, sizeof(out->measurand), 0) > 0;
    out->has_unit = ocpp_json_get_str(obj, "unit", out->unit, sizeof(out->unit), 0) > 0;
    return OCPP_OK;
}
/**
 * ocpp16_sampled_value_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_sampled_value_example(ocpp16_sampled_value_t *out) {
    memset(out, 0, sizeof(*out));
    ocpp_str_copy(out->value, sizeof(out->value), "1234");
}

/**
 * ocpp16_meter_value_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_meter_value_to_json(const ocpp16_meter_value_t *in) {
    if (in == NULL) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL) return NULL;
    if (ocpp_json_add_str(obj, "timestamp", in->timestamp) < 0) { cJSON_Delete(obj); return NULL; }
    { cJSON *arr = cJSON_CreateArray();
      int n = in->sampledValue_count; if (n > 8) n = 8;
      for (int i = 0; i < n; i++) {
        cJSON *ch = ocpp16_sampled_value_to_json(&in->sampledValue[i]); if (ch) cJSON_AddItemToArray(arr, ch);
      }
      cJSON_AddItemToObject(obj, "sampledValue", arr); }
    return obj;
}
/**
 * ocpp16_meter_value_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_meter_value_from_json(const cJSON *obj, ocpp16_meter_value_t *out) {
    if (obj == NULL || out == NULL) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    if (ocpp_json_get_str(obj, "timestamp", out->timestamp, sizeof(out->timestamp), 1) < 0) return OCPP_ERR_PARSE;
    { cJSON *arr = ocpp_json_get_arr(obj, "sampledValue", 1); out->sampledValue_count = 0;
      if (arr == NULL) return OCPP_ERR_PARSE;
      if (arr) {
        int n = cJSON_GetArraySize(arr); if (n > 8) n = 8;
        for (int i = 0; i < n; i++) {
          if (ocpp16_sampled_value_from_json(cJSON_GetArrayItem(arr, i), &out->sampledValue[i]) != OCPP_OK) return OCPP_ERR_PARSE;
          out->sampledValue_count++;
        }
      }
    }
    return OCPP_OK;
}
/**
 * ocpp16_meter_value_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_meter_value_example(ocpp16_meter_value_t *out) {
    memset(out, 0, sizeof(*out));
    ocpp_str_copy(out->timestamp, sizeof(out->timestamp), "2024-01-01T00:00:00Z");
    out->sampledValue_count = 1;
    ocpp16_sampled_value_example(&out->sampledValue[0]);
}

/**
 * ocpp16_cs_period_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_cs_period_to_json(const ocpp16_cs_period_t *in) {
    if (in == NULL) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL) return NULL;
    ocpp_json_add_int(obj, "startPeriod", in->startPeriod);
    ocpp_json_add_double(obj, "limit", in->limit);
    if (in->has_numberPhases) ocpp_json_add_int(obj, "numberPhases", in->numberPhases);
    return obj;
}
/**
 * ocpp16_cs_period_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_cs_period_from_json(const cJSON *obj, ocpp16_cs_period_t *out) {
    if (obj == NULL || out == NULL) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    if (ocpp_json_get_int(obj, "startPeriod", &out->startPeriod, 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_double(obj, "limit", &out->limit, 1) < 0) return OCPP_ERR_PARSE;
    out->has_numberPhases = ocpp_json_get_int(obj, "numberPhases", &out->numberPhases, 0) > 0;
    return OCPP_OK;
}
/**
 * ocpp16_cs_period_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_cs_period_example(ocpp16_cs_period_t *out) {
    memset(out, 0, sizeof(*out));
    out->startPeriod = 0;
    out->limit = 16.0;
}

/**
 * ocpp16_charging_schedule_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_charging_schedule_to_json(const ocpp16_charging_schedule_t *in) {
    if (in == NULL) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL) return NULL;
    if (ocpp_json_add_str(obj, "chargingRateUnit", in->chargingRateUnit) < 0) { cJSON_Delete(obj); return NULL; }
    { cJSON *arr = cJSON_CreateArray();
      int n = in->chargingSchedulePeriod_count; if (n > 8) n = 8;
      for (int i = 0; i < n; i++) {
        cJSON *ch = ocpp16_cs_period_to_json(&in->chargingSchedulePeriod[i]); if (ch) cJSON_AddItemToArray(arr, ch);
      }
      cJSON_AddItemToObject(obj, "chargingSchedulePeriod", arr); }
    if (in->has_duration) ocpp_json_add_int(obj, "duration", in->duration);
    return obj;
}
/**
 * ocpp16_charging_schedule_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_charging_schedule_from_json(const cJSON *obj, ocpp16_charging_schedule_t *out) {
    if (obj == NULL || out == NULL) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    if (ocpp_json_get_str(obj, "chargingRateUnit", out->chargingRateUnit, sizeof(out->chargingRateUnit), 1) < 0) return OCPP_ERR_PARSE;
    { cJSON *arr = ocpp_json_get_arr(obj, "chargingSchedulePeriod", 1); out->chargingSchedulePeriod_count = 0;
      if (arr == NULL) return OCPP_ERR_PARSE;
      if (arr) {
        int n = cJSON_GetArraySize(arr); if (n > 8) n = 8;
        for (int i = 0; i < n; i++) {
          if (ocpp16_cs_period_from_json(cJSON_GetArrayItem(arr, i), &out->chargingSchedulePeriod[i]) != OCPP_OK) return OCPP_ERR_PARSE;
          out->chargingSchedulePeriod_count++;
        }
      }
    }
    out->has_duration = ocpp_json_get_int(obj, "duration", &out->duration, 0) > 0;
    return OCPP_OK;
}
/**
 * ocpp16_charging_schedule_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_charging_schedule_example(ocpp16_charging_schedule_t *out) {
    memset(out, 0, sizeof(*out));
    ocpp_str_copy(out->chargingRateUnit, sizeof(out->chargingRateUnit), "A");
    out->chargingSchedulePeriod_count = 1;
    ocpp16_cs_period_example(&out->chargingSchedulePeriod[0]);
}

/**
 * ocpp16_charging_profile_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_charging_profile_to_json(const ocpp16_charging_profile_t *in) {
    if (in == NULL) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL) return NULL;
    ocpp_json_add_int(obj, "chargingProfileId", in->chargingProfileId);
    ocpp_json_add_int(obj, "stackLevel", in->stackLevel);
    if (ocpp_json_add_str(obj, "chargingProfilePurpose", in->chargingProfilePurpose) < 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_str(obj, "chargingProfileKind", in->chargingProfileKind) < 0) { cJSON_Delete(obj); return NULL; }
    { cJSON *ch = ocpp16_charging_schedule_to_json(&in->chargingSchedule); if (!ch) { cJSON_Delete(obj); return NULL; } cJSON_AddItemToObject(obj, "chargingSchedule", ch); }
    if (in->has_transactionId) ocpp_json_add_int(obj, "transactionId", in->transactionId);
    return obj;
}
/**
 * ocpp16_charging_profile_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_charging_profile_from_json(const cJSON *obj, ocpp16_charging_profile_t *out) {
    if (obj == NULL || out == NULL) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    if (ocpp_json_get_int(obj, "chargingProfileId", &out->chargingProfileId, 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_int(obj, "stackLevel", &out->stackLevel, 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "chargingProfilePurpose", out->chargingProfilePurpose, sizeof(out->chargingProfilePurpose), 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "chargingProfileKind", out->chargingProfileKind, sizeof(out->chargingProfileKind), 1) < 0) return OCPP_ERR_PARSE;
    { cJSON *ch = ocpp_json_get_obj(obj, "chargingSchedule", 1);
      if (ch == NULL) return OCPP_ERR_PARSE;
      if (ocpp16_charging_schedule_from_json(ch, &out->chargingSchedule) != OCPP_OK) return OCPP_ERR_PARSE;
    }
    out->has_transactionId = ocpp_json_get_int(obj, "transactionId", &out->transactionId, 0) > 0;
    return OCPP_OK;
}
/**
 * ocpp16_charging_profile_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_charging_profile_example(ocpp16_charging_profile_t *out) {
    memset(out, 0, sizeof(*out));
    out->chargingProfileId = 1;
    out->stackLevel = 0;
    ocpp_str_copy(out->chargingProfilePurpose, sizeof(out->chargingProfilePurpose), "TxDefaultProfile");
    ocpp_str_copy(out->chargingProfileKind, sizeof(out->chargingProfileKind), "Absolute");
    ocpp16_charging_schedule_example(&out->chargingSchedule);
}

/**
 * ocpp16_key_value_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_key_value_to_json(const ocpp16_key_value_t *in) {
    if (in == NULL) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL) return NULL;
    if (ocpp_json_add_str(obj, "key", in->key) < 0) { cJSON_Delete(obj); return NULL; }
    ocpp_json_add_bool(obj, "readonly", in->readonly);
    if (in->has_value) ocpp_json_add_str(obj, "value", in->value);
    return obj;
}
/**
 * ocpp16_key_value_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_key_value_from_json(const cJSON *obj, ocpp16_key_value_t *out) {
    if (obj == NULL || out == NULL) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    if (ocpp_json_get_str(obj, "key", out->key, sizeof(out->key), 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_bool(obj, "readonly", &out->readonly, 1) < 0) return OCPP_ERR_PARSE;
    out->has_value = ocpp_json_get_str(obj, "value", out->value, sizeof(out->value), 0) > 0;
    return OCPP_OK;
}
/**
 * ocpp16_key_value_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_key_value_example(ocpp16_key_value_t *out) {
    memset(out, 0, sizeof(*out));
    ocpp_str_copy(out->key, sizeof(out->key), "HeartbeatInterval");
    out->readonly = 0;
    out->has_value = 1;
    ocpp_str_copy(out->value, sizeof(out->value), "300");
}

/**
 * ocpp16_auth_data_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_auth_data_to_json(const ocpp16_auth_data_t *in) {
    if (in == NULL) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL) return NULL;
    if (ocpp_json_add_str(obj, "idTag", in->idTag) < 0) { cJSON_Delete(obj); return NULL; }
    if (in->has_idTagInfo) { cJSON *ch = ocpp16_id_tag_info_to_json(&in->idTagInfo); if (ch) cJSON_AddItemToObject(obj, "idTagInfo", ch); }
    return obj;
}
/**
 * ocpp16_auth_data_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_auth_data_from_json(const cJSON *obj, ocpp16_auth_data_t *out) {
    if (obj == NULL || out == NULL) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    if (ocpp_json_get_str(obj, "idTag", out->idTag, sizeof(out->idTag), 1) < 0) return OCPP_ERR_PARSE;
    { cJSON *ch = ocpp_json_get_obj(obj, "idTagInfo", 0);
      if (ch) { out->has_idTagInfo = 1; if (ocpp16_id_tag_info_from_json(ch, &out->idTagInfo) != OCPP_OK) return OCPP_ERR_PARSE; }
      else { out->has_idTagInfo = 0; }
    }
    return OCPP_OK;
}
/**
 * ocpp16_auth_data_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_auth_data_example(ocpp16_auth_data_t *out) {
    memset(out, 0, sizeof(*out));
    ocpp_str_copy(out->idTag, sizeof(out->idTag), "TAG001");
}

/**
 * ocpp16_cert_hash_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_cert_hash_to_json(const ocpp16_cert_hash_t *in) {
    if (in == NULL) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL) return NULL;
    if (ocpp_json_add_str(obj, "hashAlgorithm", in->hashAlgorithm) < 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_str(obj, "issuerNameHash", in->issuerNameHash) < 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_str(obj, "issuerKeyHash", in->issuerKeyHash) < 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_str(obj, "serialNumber", in->serialNumber) < 0) { cJSON_Delete(obj); return NULL; }
    return obj;
}
/**
 * ocpp16_cert_hash_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_cert_hash_from_json(const cJSON *obj, ocpp16_cert_hash_t *out) {
    if (obj == NULL || out == NULL) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    if (ocpp_json_get_str(obj, "hashAlgorithm", out->hashAlgorithm, sizeof(out->hashAlgorithm), 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "issuerNameHash", out->issuerNameHash, sizeof(out->issuerNameHash), 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "issuerKeyHash", out->issuerKeyHash, sizeof(out->issuerKeyHash), 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "serialNumber", out->serialNumber, sizeof(out->serialNumber), 1) < 0) return OCPP_ERR_PARSE;
    return OCPP_OK;
}
/**
 * ocpp16_cert_hash_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_cert_hash_example(ocpp16_cert_hash_t *out) {
    memset(out, 0, sizeof(*out));
    ocpp_str_copy(out->hashAlgorithm, sizeof(out->hashAlgorithm), "SHA256");
    ocpp_str_copy(out->issuerNameHash, sizeof(out->issuerNameHash), "aa");
    ocpp_str_copy(out->issuerKeyHash, sizeof(out->issuerKeyHash), "bb");
    ocpp_str_copy(out->serialNumber, sizeof(out->serialNumber), "1");
}

/**
 * ocpp16_log_params_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_log_params_to_json(const ocpp16_log_params_t *in) {
    if (in == NULL) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL) return NULL;
    if (ocpp_json_add_str(obj, "remoteLocation", in->remoteLocation) < 0) { cJSON_Delete(obj); return NULL; }
    return obj;
}
/**
 * ocpp16_log_params_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_log_params_from_json(const cJSON *obj, ocpp16_log_params_t *out) {
    if (obj == NULL || out == NULL) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    if (ocpp_json_get_str(obj, "remoteLocation", out->remoteLocation, sizeof(out->remoteLocation), 1) < 0) return OCPP_ERR_PARSE;
    return OCPP_OK;
}
/**
 * ocpp16_log_params_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_log_params_example(ocpp16_log_params_t *out) {
    memset(out, 0, sizeof(*out));
    ocpp_str_copy(out->remoteLocation, sizeof(out->remoteLocation), "ftp://example/log");
}

/**
 * ocpp16_firmware_signed_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_firmware_signed_to_json(const ocpp16_firmware_signed_t *in) {
    if (in == NULL) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL) return NULL;
    if (ocpp_json_add_str(obj, "location", in->location) < 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_str(obj, "retrieveDateTime", in->retrieveDateTime) < 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_str(obj, "signingCertificate", in->signingCertificate) < 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_str(obj, "signature", in->signature) < 0) { cJSON_Delete(obj); return NULL; }
    return obj;
}
/**
 * ocpp16_firmware_signed_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_firmware_signed_from_json(const cJSON *obj, ocpp16_firmware_signed_t *out) {
    if (obj == NULL || out == NULL) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    if (ocpp_json_get_str(obj, "location", out->location, sizeof(out->location), 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "retrieveDateTime", out->retrieveDateTime, sizeof(out->retrieveDateTime), 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "signingCertificate", out->signingCertificate, sizeof(out->signingCertificate), 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "signature", out->signature, sizeof(out->signature), 1) < 0) return OCPP_ERR_PARSE;
    return OCPP_OK;
}
/**
 * ocpp16_firmware_signed_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_firmware_signed_example(ocpp16_firmware_signed_t *out) {
    memset(out, 0, sizeof(*out));
    ocpp_str_copy(out->location, sizeof(out->location), "https://example/fw.bin");
    ocpp_str_copy(out->retrieveDateTime, sizeof(out->retrieveDateTime), "2024-01-01T00:00:00Z");
    ocpp_str_copy(out->signingCertificate, sizeof(out->signingCertificate), "CERT");
    ocpp_str_copy(out->signature, sizeof(out->signature), "c2ln");
}
