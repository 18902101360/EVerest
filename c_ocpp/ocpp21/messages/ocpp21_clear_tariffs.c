#include "ocpp21_clear_tariffs.h"
#include <string.h>

/**
 * ocpp21_clear_tariffs_req_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 * 
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp21_clear_tariffs_req_to_json(const ocpp21_clear_tariffs_req_t *in) {
    if (!in) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;
    if (in->tariffIds_count > 0) {
        cJSON *arr = cJSON_CreateArray();
        if (!arr) { cJSON_Delete(obj); return NULL; }
        for (int i = 0; i < in->tariffIds_count && i < 4; i++) {
            cJSON *el = cJSON_CreateString(in->tariffIds[i]);
            if (!el) { cJSON_Delete(arr); cJSON_Delete(obj); return NULL; }
            cJSON_AddItemToArray(arr, el);
        }
        cJSON_AddItemToObject(obj, "tariffIds", arr);
    }
    return obj;
}
/**
 * ocpp21_clear_tariffs_req_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 * 
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp21_clear_tariffs_req_from_json(const cJSON *obj, ocpp21_clear_tariffs_req_t *out) {
    if (!obj || !out) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    { cJSON *arr = ocpp_json_get_arr(obj, "tariffIds", 0);
      if (arr) {
        int n = cJSON_GetArraySize(arr); if (n > 4) n = 4;
        out->tariffIds_count = n;
        for (int i = 0; i < n; i++) {
          cJSON *el = cJSON_GetArrayItem(arr, i);
          if (!cJSON_IsString(el)) return OCPP_ERR_PARSE;
          ocpp_str_copy(out->tariffIds[i], sizeof(out->tariffIds[i]), el->valuestring);
        }
      }
    }
    return OCPP_OK;
}
/**
 * ocpp21_clear_tariffs_req_encode - 把结构体编成 JSON 文本
 * @in: 已填好的结构
 * @buf: 输出文本
 * @buflen: buf 的字节数
 * 
 * 成功返回 OCPP_OK。缓冲不够返回 OCPP_ERR_OVERFLOW。树在 ocpp_json_dump 里释放。
 */
ocpp_err_t ocpp21_clear_tariffs_req_encode(const ocpp21_clear_tariffs_req_t *in, char *buf, size_t buflen) {
    ocpp_port_arena_reset();
    cJSON *o = ocpp21_clear_tariffs_req_to_json(in);
    if (!o) return OCPP_ERR_NOMEM;
    return ocpp_json_dump(o, buf, buflen);
}
/**
 * ocpp21_clear_tariffs_req_decode - 把 JSON 文本解析进结构体
 * @json: JSON 对象文本
 * @out: 输出结构
 * 
 * 成功返回 OCPP_OK。文本不是对象或必填字段缺失时返回 OCPP_ERR_PARSE。解析树在返回前 Delete。
 */
ocpp_err_t ocpp21_clear_tariffs_req_decode(const char *json, ocpp21_clear_tariffs_req_t *out) {
    if (!json || !out) return OCPP_ERR_ARG;
    ocpp_port_arena_reset();
    cJSON *o = cJSON_Parse(json);
    if (!o) return OCPP_ERR_PARSE;
    ocpp_err_t rc = ocpp21_clear_tariffs_req_from_json(o, out);
    cJSON_Delete(o);
    return rc;
}
/**
 * ocpp21_clear_tariffs_req_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 * 
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp21_clear_tariffs_req_example(ocpp21_clear_tariffs_req_t *out) {
    memset(out, 0, sizeof(*out));
    out->tariffIds_count = 1;
    ocpp_str_copy(out->tariffIds[0], sizeof(out->tariffIds[0]), "TARIFF-1");
}

cJSON *ocpp21_clear_tariffs_clearTariffsResult_to_json(const ocpp21_clear_tariffs_clearTariffsResult_t *in) {
    if (!in) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;
    if (ocpp_json_add_str(obj, "status", in->status) <= 0) { cJSON_Delete(obj); return NULL; }
    if (in->has_tariffId) { if (ocpp_json_add_str(obj, "tariffId", in->tariffId) < 0) { cJSON_Delete(obj); return NULL; } }
    return obj;
}
ocpp_err_t ocpp21_clear_tariffs_clearTariffsResult_from_json(const cJSON *obj, ocpp21_clear_tariffs_clearTariffsResult_t *out) {
    if (!obj || !out) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    if (ocpp_json_get_str(obj, "status", out->status, sizeof(out->status), 1) < 0) return OCPP_ERR_PARSE;
    { int gr = ocpp_json_get_str(obj, "tariffId", out->tariffId, sizeof(out->tariffId), 0); if (gr < 0) return OCPP_ERR_PARSE; out->has_tariffId = gr > 0; }
    return OCPP_OK;
}

/**
 * ocpp21_clear_tariffs_conf_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 * 
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp21_clear_tariffs_conf_to_json(const ocpp21_clear_tariffs_conf_t *in) {
    if (!in) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;
    if (in->clearTariffsResult_count > 0) {
        cJSON *arr = cJSON_CreateArray();
        if (!arr) { cJSON_Delete(obj); return NULL; }
        for (int i = 0; i < in->clearTariffsResult_count && i < 2; i++) {
            cJSON *el = ocpp21_clear_tariffs_clearTariffsResult_to_json(&in->clearTariffsResult[i]);
            if (!el) { cJSON_Delete(arr); cJSON_Delete(obj); return NULL; }
            cJSON_AddItemToArray(arr, el);
        }
        cJSON_AddItemToObject(obj, "clearTariffsResult", arr);
    }
    return obj;
}
/**
 * ocpp21_clear_tariffs_conf_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 * 
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp21_clear_tariffs_conf_from_json(const cJSON *obj, ocpp21_clear_tariffs_conf_t *out) {
    if (!obj || !out) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    { cJSON *arr = ocpp_json_get_arr(obj, "clearTariffsResult", 1);
      if (!arr || cJSON_GetArraySize(arr) < 1) return OCPP_ERR_PARSE;
      int n = cJSON_GetArraySize(arr); if (n > 2) n = 2;
      out->clearTariffsResult_count = n;
      for (int i = 0; i < n; i++) {
        cJSON *el = cJSON_GetArrayItem(arr, i);
        if (!cJSON_IsObject(el) || ocpp21_clear_tariffs_clearTariffsResult_from_json(el, &out->clearTariffsResult[i]) != OCPP_OK) return OCPP_ERR_PARSE;
      }
    }
    return OCPP_OK;
}
/**
 * ocpp21_clear_tariffs_conf_encode - 把结构体编成 JSON 文本
 * @in: 已填好的结构
 * @buf: 输出文本
 * @buflen: buf 的字节数
 * 
 * 成功返回 OCPP_OK。缓冲不够返回 OCPP_ERR_OVERFLOW。树在 ocpp_json_dump 里释放。
 */
ocpp_err_t ocpp21_clear_tariffs_conf_encode(const ocpp21_clear_tariffs_conf_t *in, char *buf, size_t buflen) {
    ocpp_port_arena_reset();
    cJSON *o = ocpp21_clear_tariffs_conf_to_json(in);
    if (!o) return OCPP_ERR_NOMEM;
    return ocpp_json_dump(o, buf, buflen);
}
/**
 * ocpp21_clear_tariffs_conf_decode - 把 JSON 文本解析进结构体
 * @json: JSON 对象文本
 * @out: 输出结构
 * 
 * 成功返回 OCPP_OK。文本不是对象或必填字段缺失时返回 OCPP_ERR_PARSE。解析树在返回前 Delete。
 */
ocpp_err_t ocpp21_clear_tariffs_conf_decode(const char *json, ocpp21_clear_tariffs_conf_t *out) {
    if (!json || !out) return OCPP_ERR_ARG;
    ocpp_port_arena_reset();
    cJSON *o = cJSON_Parse(json);
    if (!o) return OCPP_ERR_PARSE;
    ocpp_err_t rc = ocpp21_clear_tariffs_conf_from_json(o, out);
    cJSON_Delete(o);
    return rc;
}
/**
 * ocpp21_clear_tariffs_conf_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 * 
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp21_clear_tariffs_conf_example(ocpp21_clear_tariffs_conf_t *out) {
    memset(out, 0, sizeof(*out));
    out->clearTariffsResult_count = 1;
    ocpp_str_copy(out->clearTariffsResult[0].status, sizeof(out->clearTariffsResult[0].status), "Accepted");
    out->clearTariffsResult[0].has_tariffId = 1;
    ocpp_str_copy(out->clearTariffsResult[0].tariffId, sizeof(out->clearTariffsResult[0].tariffId), "TARIFF-1");
}

