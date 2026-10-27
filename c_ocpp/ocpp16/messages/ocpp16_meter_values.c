/* Generated OCPP message: MeterValues */
#include "ocpp16_meter_values.h"
#include <string.h>


/**
 * ocpp16_meter_values_req_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_meter_values_req_to_json(const ocpp16_meter_values_req_t *in) {
    if (in == NULL) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL) return NULL;
    ocpp_json_add_int(obj, "connectorId", in->connectorId);
    { cJSON *arr = cJSON_CreateArray();
      int n = in->meterValue_count; if (n > 4) n = 4;
      for (int i = 0; i < n; i++) {
        cJSON *ch = ocpp16_meter_value_to_json(&in->meterValue[i]); if (ch) cJSON_AddItemToArray(arr, ch);
      }
      cJSON_AddItemToObject(obj, "meterValue", arr); }
    if (in->has_transactionId) ocpp_json_add_int(obj, "transactionId", in->transactionId);
    return obj;
}
/**
 * ocpp16_meter_values_req_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_meter_values_req_from_json(const cJSON *obj, ocpp16_meter_values_req_t *out) {
    if (obj == NULL || out == NULL) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    if (ocpp_json_get_int(obj, "connectorId", &out->connectorId, 1) < 0) return OCPP_ERR_PARSE;
    { cJSON *arr = ocpp_json_get_arr(obj, "meterValue", 1); out->meterValue_count = 0;
      if (arr == NULL) return OCPP_ERR_PARSE;
      if (arr) {
        int n = cJSON_GetArraySize(arr); if (n > 4) n = 4;
        for (int i = 0; i < n; i++) {
          if (ocpp16_meter_value_from_json(cJSON_GetArrayItem(arr, i), &out->meterValue[i]) != OCPP_OK) return OCPP_ERR_PARSE;
          out->meterValue_count++;
        }
      }
    }
    out->has_transactionId = ocpp_json_get_int(obj, "transactionId", &out->transactionId, 0) > 0;
    return OCPP_OK;
}
/**
 * ocpp16_meter_values_req_encode - 把结构体编成 JSON 文本
 * @in: 已填好的结构
 * @buf: 输出文本
 * @buflen: buf 的字节数
 *
 * 成功返回 OCPP_OK。缓冲不够返回 OCPP_ERR_OVERFLOW。树在 ocpp_json_dump 里释放。
 */
ocpp_err_t ocpp16_meter_values_req_encode(const ocpp16_meter_values_req_t *in, char *buf, size_t buflen) {
    ocpp_port_arena_reset();
    cJSON *o = ocpp16_meter_values_req_to_json(in);
    if (o == NULL) return OCPP_ERR_NOMEM;
    return ocpp_json_dump(o, buf, buflen);
}
/**
 * ocpp16_meter_values_req_decode - 把 JSON 文本解析进结构体
 * @json: JSON 对象文本
 * @out: 输出结构
 *
 * 成功返回 OCPP_OK。文本不是对象或必填字段缺失时返回 OCPP_ERR_PARSE。解析树在返回前 Delete。
 */
ocpp_err_t ocpp16_meter_values_req_decode(const char *json, ocpp16_meter_values_req_t *out) {
    if (json == NULL || out == NULL) return OCPP_ERR_ARG;
    ocpp_port_arena_reset();
    cJSON *o = cJSON_Parse(json);
    if (o == NULL) return OCPP_ERR_PARSE;
    ocpp_err_t rc = ocpp16_meter_values_req_from_json(o, out);
    cJSON_Delete(o);
    return rc;
}
/**
 * ocpp16_meter_values_req_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_meter_values_req_example(ocpp16_meter_values_req_t *out) {
    memset(out, 0, sizeof(*out));
    out->connectorId = 1;
    out->meterValue_count = 1;
    ocpp16_meter_value_example(&out->meterValue[0]);
}


/**
 * ocpp16_meter_values_conf_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_meter_values_conf_to_json(const ocpp16_meter_values_conf_t *in) {
    if (in == NULL) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL) return NULL;
    (void)in;
    return obj;
}
/**
 * ocpp16_meter_values_conf_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_meter_values_conf_from_json(const cJSON *obj, ocpp16_meter_values_conf_t *out) {
    if (obj == NULL || out == NULL) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    (void)obj;
    (void)out;
    return OCPP_OK;
}
/**
 * ocpp16_meter_values_conf_encode - 把结构体编成 JSON 文本
 * @in: 已填好的结构
 * @buf: 输出文本
 * @buflen: buf 的字节数
 *
 * 成功返回 OCPP_OK。缓冲不够返回 OCPP_ERR_OVERFLOW。树在 ocpp_json_dump 里释放。
 */
ocpp_err_t ocpp16_meter_values_conf_encode(const ocpp16_meter_values_conf_t *in, char *buf, size_t buflen) {
    ocpp_port_arena_reset();
    cJSON *o = ocpp16_meter_values_conf_to_json(in);
    if (o == NULL) return OCPP_ERR_NOMEM;
    return ocpp_json_dump(o, buf, buflen);
}
/**
 * ocpp16_meter_values_conf_decode - 把 JSON 文本解析进结构体
 * @json: JSON 对象文本
 * @out: 输出结构
 *
 * 成功返回 OCPP_OK。文本不是对象或必填字段缺失时返回 OCPP_ERR_PARSE。解析树在返回前 Delete。
 */
ocpp_err_t ocpp16_meter_values_conf_decode(const char *json, ocpp16_meter_values_conf_t *out) {
    if (json == NULL || out == NULL) return OCPP_ERR_ARG;
    ocpp_port_arena_reset();
    cJSON *o = cJSON_Parse(json);
    if (o == NULL) return OCPP_ERR_PARSE;
    ocpp_err_t rc = ocpp16_meter_values_conf_from_json(o, out);
    cJSON_Delete(o);
    return rc;
}
/**
 * ocpp16_meter_values_conf_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_meter_values_conf_example(ocpp16_meter_values_conf_t *out) {
    memset(out, 0, sizeof(*out));
    (void)out;
}

