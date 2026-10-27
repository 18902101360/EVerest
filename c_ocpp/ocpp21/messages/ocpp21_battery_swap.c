#include "ocpp21_battery_swap.h"
#include <string.h>
cJSON *ocpp21_battery_swap_batteryData_to_json(const ocpp21_battery_swap_batteryData_t *in) {
    if (!in) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;
    if (ocpp_json_add_int(obj, "evseId", in->evseId) < 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_str(obj, "serialNumber", in->serialNumber) <= 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_double(obj, "soC", in->soC) < 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_double(obj, "soH", in->soH) < 0) { cJSON_Delete(obj); return NULL; }
    return obj;
}
ocpp_err_t ocpp21_battery_swap_batteryData_from_json(const cJSON *obj, ocpp21_battery_swap_batteryData_t *out) {
    if (!obj || !out) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    if (ocpp_json_get_int(obj, "evseId", &out->evseId, 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "serialNumber", out->serialNumber, sizeof(out->serialNumber), 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_double(obj, "soC", &out->soC, 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_double(obj, "soH", &out->soH, 1) < 0) return OCPP_ERR_PARSE;
    return OCPP_OK;
}

cJSON *ocpp21_battery_swap_idToken_to_json(const ocpp21_battery_swap_idToken_t *in) {
    if (!in) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;
    if (ocpp_json_add_str(obj, "idToken", in->idToken) <= 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_str(obj, "type", in->type) <= 0) { cJSON_Delete(obj); return NULL; }
    return obj;
}
ocpp_err_t ocpp21_battery_swap_idToken_from_json(const cJSON *obj, ocpp21_battery_swap_idToken_t *out) {
    if (!obj || !out) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    if (ocpp_json_get_str(obj, "idToken", out->idToken, sizeof(out->idToken), 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "type", out->type, sizeof(out->type), 1) < 0) return OCPP_ERR_PARSE;
    return OCPP_OK;
}

/**
 * ocpp21_battery_swap_req_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 * 
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp21_battery_swap_req_to_json(const ocpp21_battery_swap_req_t *in) {
    if (!in) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;
    if (in->batteryData_count > 0) {
        cJSON *arr = cJSON_CreateArray();
        if (!arr) { cJSON_Delete(obj); return NULL; }
        for (int i = 0; i < in->batteryData_count && i < 2; i++) {
            cJSON *el = ocpp21_battery_swap_batteryData_to_json(&in->batteryData[i]);
            if (!el) { cJSON_Delete(arr); cJSON_Delete(obj); return NULL; }
            cJSON_AddItemToArray(arr, el);
        }
        cJSON_AddItemToObject(obj, "batteryData", arr);
    }
    if (ocpp_json_add_str(obj, "eventType", in->eventType) <= 0) { cJSON_Delete(obj); return NULL; }
    { cJSON *ch = ocpp21_battery_swap_idToken_to_json(&in->idToken); if (!ch) { cJSON_Delete(obj); return NULL; } cJSON_AddItemToObject(obj, "idToken", ch); }
    if (ocpp_json_add_int(obj, "requestId", in->requestId) < 0) { cJSON_Delete(obj); return NULL; }
    return obj;
}
/**
 * ocpp21_battery_swap_req_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 * 
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp21_battery_swap_req_from_json(const cJSON *obj, ocpp21_battery_swap_req_t *out) {
    if (!obj || !out) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    { cJSON *arr = ocpp_json_get_arr(obj, "batteryData", 1);
      if (!arr || cJSON_GetArraySize(arr) < 1) return OCPP_ERR_PARSE;
      int n = cJSON_GetArraySize(arr); if (n > 2) n = 2;
      out->batteryData_count = n;
      for (int i = 0; i < n; i++) {
        cJSON *el = cJSON_GetArrayItem(arr, i);
        if (!cJSON_IsObject(el) || ocpp21_battery_swap_batteryData_from_json(el, &out->batteryData[i]) != OCPP_OK) return OCPP_ERR_PARSE;
      }
    }
    if (ocpp_json_get_str(obj, "eventType", out->eventType, sizeof(out->eventType), 1) < 0) return OCPP_ERR_PARSE;
    { cJSON *ch = ocpp_json_get_obj(obj, "idToken", 1); if (!ch) return OCPP_ERR_PARSE; if (ocpp21_battery_swap_idToken_from_json(ch, &out->idToken) != OCPP_OK) return OCPP_ERR_PARSE; }
    if (ocpp_json_get_int(obj, "requestId", &out->requestId, 1) < 0) return OCPP_ERR_PARSE;
    return OCPP_OK;
}
/**
 * ocpp21_battery_swap_req_encode - 把结构体编成 JSON 文本
 * @in: 已填好的结构
 * @buf: 输出文本
 * @buflen: buf 的字节数
 * 
 * 成功返回 OCPP_OK。缓冲不够返回 OCPP_ERR_OVERFLOW。树在 ocpp_json_dump 里释放。
 */
ocpp_err_t ocpp21_battery_swap_req_encode(const ocpp21_battery_swap_req_t *in, char *buf, size_t buflen) {
    ocpp_port_arena_reset();
    cJSON *o = ocpp21_battery_swap_req_to_json(in);
    if (!o) return OCPP_ERR_NOMEM;
    return ocpp_json_dump(o, buf, buflen);
}
/**
 * ocpp21_battery_swap_req_decode - 把 JSON 文本解析进结构体
 * @json: JSON 对象文本
 * @out: 输出结构
 * 
 * 成功返回 OCPP_OK。文本不是对象或必填字段缺失时返回 OCPP_ERR_PARSE。解析树在返回前 Delete。
 */
ocpp_err_t ocpp21_battery_swap_req_decode(const char *json, ocpp21_battery_swap_req_t *out) {
    if (!json || !out) return OCPP_ERR_ARG;
    ocpp_port_arena_reset();
    cJSON *o = cJSON_Parse(json);
    if (!o) return OCPP_ERR_PARSE;
    ocpp_err_t rc = ocpp21_battery_swap_req_from_json(o, out);
    cJSON_Delete(o);
    return rc;
}
/**
 * ocpp21_battery_swap_req_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 * 
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp21_battery_swap_req_example(ocpp21_battery_swap_req_t *out) {
    memset(out, 0, sizeof(*out));
    out->batteryData_count = 1;
    out->batteryData[0].evseId = 1;
    ocpp_str_copy(out->batteryData[0].serialNumber, sizeof(out->batteryData[0].serialNumber), "BAT001");
    out->batteryData[0].soC = 80.0;
    out->batteryData[0].soH = 98.0;
    ocpp_str_copy(out->eventType, sizeof(out->eventType), "BatteryIn");
    ocpp_str_copy(out->idToken.idToken, sizeof(out->idToken.idToken), "TAG001");
    ocpp_str_copy(out->idToken.type, sizeof(out->idToken.type), "ISO14443");
    out->requestId = 1;
}


/**
 * ocpp21_battery_swap_conf_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 * 
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp21_battery_swap_conf_to_json(const ocpp21_battery_swap_conf_t *in) {
    if (!in) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;

    return obj;
}
/**
 * ocpp21_battery_swap_conf_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 * 
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp21_battery_swap_conf_from_json(const cJSON *obj, ocpp21_battery_swap_conf_t *out) {
    if (!obj || !out) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));

    return OCPP_OK;
}
/**
 * ocpp21_battery_swap_conf_encode - 把结构体编成 JSON 文本
 * @in: 已填好的结构
 * @buf: 输出文本
 * @buflen: buf 的字节数
 * 
 * 成功返回 OCPP_OK。缓冲不够返回 OCPP_ERR_OVERFLOW。树在 ocpp_json_dump 里释放。
 */
ocpp_err_t ocpp21_battery_swap_conf_encode(const ocpp21_battery_swap_conf_t *in, char *buf, size_t buflen) {
    ocpp_port_arena_reset();
    cJSON *o = ocpp21_battery_swap_conf_to_json(in);
    if (!o) return OCPP_ERR_NOMEM;
    return ocpp_json_dump(o, buf, buflen);
}
/**
 * ocpp21_battery_swap_conf_decode - 把 JSON 文本解析进结构体
 * @json: JSON 对象文本
 * @out: 输出结构
 * 
 * 成功返回 OCPP_OK。文本不是对象或必填字段缺失时返回 OCPP_ERR_PARSE。解析树在返回前 Delete。
 */
ocpp_err_t ocpp21_battery_swap_conf_decode(const char *json, ocpp21_battery_swap_conf_t *out) {
    if (!json || !out) return OCPP_ERR_ARG;
    ocpp_port_arena_reset();
    cJSON *o = cJSON_Parse(json);
    if (!o) return OCPP_ERR_PARSE;
    ocpp_err_t rc = ocpp21_battery_swap_conf_from_json(o, out);
    cJSON_Delete(o);
    return rc;
}
/**
 * ocpp21_battery_swap_conf_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 * 
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp21_battery_swap_conf_example(ocpp21_battery_swap_conf_t *out) {
    memset(out, 0, sizeof(*out));
    (void)out;
}

