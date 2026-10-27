/* Generated OCPP message: StopTransaction */
#include "ocpp16_stop_transaction.h"
#include <string.h>


/**
 * ocpp16_stop_transaction_req_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_stop_transaction_req_to_json(const ocpp16_stop_transaction_req_t *in) {
    if (in == NULL) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL) return NULL;
    ocpp_json_add_int(obj, "transactionId", in->transactionId);
    if (ocpp_json_add_str(obj, "timestamp", in->timestamp) < 0) { cJSON_Delete(obj); return NULL; }
    ocpp_json_add_int(obj, "meterStop", in->meterStop);
    if (in->has_idTag) ocpp_json_add_str(obj, "idTag", in->idTag);
    if (in->has_reason) ocpp_json_add_str(obj, "reason", in->reason);
    return obj;
}
/**
 * ocpp16_stop_transaction_req_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_stop_transaction_req_from_json(const cJSON *obj, ocpp16_stop_transaction_req_t *out) {
    if (obj == NULL || out == NULL) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    if (ocpp_json_get_int(obj, "transactionId", &out->transactionId, 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "timestamp", out->timestamp, sizeof(out->timestamp), 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_int(obj, "meterStop", &out->meterStop, 1) < 0) return OCPP_ERR_PARSE;
    out->has_idTag = ocpp_json_get_str(obj, "idTag", out->idTag, sizeof(out->idTag), 0) > 0;
    out->has_reason = ocpp_json_get_str(obj, "reason", out->reason, sizeof(out->reason), 0) > 0;
    return OCPP_OK;
}
/**
 * ocpp16_stop_transaction_req_encode - 把结构体编成 JSON 文本
 * @in: 已填好的结构
 * @buf: 输出文本
 * @buflen: buf 的字节数
 *
 * 成功返回 OCPP_OK。缓冲不够返回 OCPP_ERR_OVERFLOW。树在 ocpp_json_dump 里释放。
 */
ocpp_err_t ocpp16_stop_transaction_req_encode(const ocpp16_stop_transaction_req_t *in, char *buf, size_t buflen) {
    ocpp_port_arena_reset();
    cJSON *o = ocpp16_stop_transaction_req_to_json(in);
    if (o == NULL) return OCPP_ERR_NOMEM;
    return ocpp_json_dump(o, buf, buflen);
}
/**
 * ocpp16_stop_transaction_req_decode - 把 JSON 文本解析进结构体
 * @json: JSON 对象文本
 * @out: 输出结构
 *
 * 成功返回 OCPP_OK。文本不是对象或必填字段缺失时返回 OCPP_ERR_PARSE。解析树在返回前 Delete。
 */
ocpp_err_t ocpp16_stop_transaction_req_decode(const char *json, ocpp16_stop_transaction_req_t *out) {
    if (json == NULL || out == NULL) return OCPP_ERR_ARG;
    ocpp_port_arena_reset();
    cJSON *o = cJSON_Parse(json);
    if (o == NULL) return OCPP_ERR_PARSE;
    ocpp_err_t rc = ocpp16_stop_transaction_req_from_json(o, out);
    cJSON_Delete(o);
    return rc;
}
/**
 * ocpp16_stop_transaction_req_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_stop_transaction_req_example(ocpp16_stop_transaction_req_t *out) {
    memset(out, 0, sizeof(*out));
    out->transactionId = 42;
    ocpp_str_copy(out->timestamp, sizeof(out->timestamp), "2024-01-01T00:10:00Z");
    out->meterStop = 1500;
}


/**
 * ocpp16_stop_transaction_conf_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp16_stop_transaction_conf_to_json(const ocpp16_stop_transaction_conf_t *in) {
    if (in == NULL) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL) return NULL;
    if (in->has_idTagInfo) { cJSON *ch = ocpp16_id_tag_info_to_json(&in->idTagInfo); if (ch) cJSON_AddItemToObject(obj, "idTagInfo", ch); }
    return obj;
}
/**
 * ocpp16_stop_transaction_conf_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp16_stop_transaction_conf_from_json(const cJSON *obj, ocpp16_stop_transaction_conf_t *out) {
    if (obj == NULL || out == NULL) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    { cJSON *ch = ocpp_json_get_obj(obj, "idTagInfo", 0);
      if (ch) { out->has_idTagInfo = 1; if (ocpp16_id_tag_info_from_json(ch, &out->idTagInfo) != OCPP_OK) return OCPP_ERR_PARSE; }
      else { out->has_idTagInfo = 0; }
    }
    return OCPP_OK;
}
/**
 * ocpp16_stop_transaction_conf_encode - 把结构体编成 JSON 文本
 * @in: 已填好的结构
 * @buf: 输出文本
 * @buflen: buf 的字节数
 *
 * 成功返回 OCPP_OK。缓冲不够返回 OCPP_ERR_OVERFLOW。树在 ocpp_json_dump 里释放。
 */
ocpp_err_t ocpp16_stop_transaction_conf_encode(const ocpp16_stop_transaction_conf_t *in, char *buf, size_t buflen) {
    ocpp_port_arena_reset();
    cJSON *o = ocpp16_stop_transaction_conf_to_json(in);
    if (o == NULL) return OCPP_ERR_NOMEM;
    return ocpp_json_dump(o, buf, buflen);
}
/**
 * ocpp16_stop_transaction_conf_decode - 把 JSON 文本解析进结构体
 * @json: JSON 对象文本
 * @out: 输出结构
 *
 * 成功返回 OCPP_OK。文本不是对象或必填字段缺失时返回 OCPP_ERR_PARSE。解析树在返回前 Delete。
 */
ocpp_err_t ocpp16_stop_transaction_conf_decode(const char *json, ocpp16_stop_transaction_conf_t *out) {
    if (json == NULL || out == NULL) return OCPP_ERR_ARG;
    ocpp_port_arena_reset();
    cJSON *o = cJSON_Parse(json);
    if (o == NULL) return OCPP_ERR_PARSE;
    ocpp_err_t rc = ocpp16_stop_transaction_conf_from_json(o, out);
    cJSON_Delete(o);
    return rc;
}
/**
 * ocpp16_stop_transaction_conf_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp16_stop_transaction_conf_example(ocpp16_stop_transaction_conf_t *out) {
    memset(out, 0, sizeof(*out));
}

