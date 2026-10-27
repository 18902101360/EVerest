#include "ocpp21_set_der_control.h"
#include <string.h>
cJSON *ocpp21_set_der_control_curve_curveData_to_json(const ocpp21_set_der_control_curve_curveData_t *in) {
    if (!in) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;
    if (ocpp_json_add_double(obj, "x", in->x) < 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_double(obj, "y", in->y) < 0) { cJSON_Delete(obj); return NULL; }
    return obj;
}
ocpp_err_t ocpp21_set_der_control_curve_curveData_from_json(const cJSON *obj, ocpp21_set_der_control_curve_curveData_t *out) {
    if (!obj || !out) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    if (ocpp_json_get_double(obj, "x", &out->x, 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_double(obj, "y", &out->y, 1) < 0) return OCPP_ERR_PARSE;
    return OCPP_OK;
}

cJSON *ocpp21_set_der_control_curve_to_json(const ocpp21_set_der_control_curve_t *in) {
    if (!in) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;
    if (ocpp_json_add_int(obj, "priority", in->priority) < 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_str(obj, "yUnit", in->yUnit) <= 0) { cJSON_Delete(obj); return NULL; }
    if (in->curveData_count > 0) {
        cJSON *arr = cJSON_CreateArray();
        if (!arr) { cJSON_Delete(obj); return NULL; }
        for (int i = 0; i < in->curveData_count && i < 2; i++) {
            cJSON *el = ocpp21_set_der_control_curve_curveData_to_json(&in->curveData[i]);
            if (!el) { cJSON_Delete(arr); cJSON_Delete(obj); return NULL; }
            cJSON_AddItemToArray(arr, el);
        }
        cJSON_AddItemToObject(obj, "curveData", arr);
    }
    return obj;
}
ocpp_err_t ocpp21_set_der_control_curve_from_json(const cJSON *obj, ocpp21_set_der_control_curve_t *out) {
    if (!obj || !out) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    if (ocpp_json_get_int(obj, "priority", &out->priority, 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "yUnit", out->yUnit, sizeof(out->yUnit), 1) < 0) return OCPP_ERR_PARSE;
    { cJSON *arr = ocpp_json_get_arr(obj, "curveData", 1);
      if (!arr || cJSON_GetArraySize(arr) < 1) return OCPP_ERR_PARSE;
      int n = cJSON_GetArraySize(arr); if (n > 2) n = 2;
      out->curveData_count = n;
      for (int i = 0; i < n; i++) {
        cJSON *el = cJSON_GetArrayItem(arr, i);
        if (!cJSON_IsObject(el) || ocpp21_set_der_control_curve_curveData_from_json(el, &out->curveData[i]) != OCPP_OK) return OCPP_ERR_PARSE;
      }
    }
    return OCPP_OK;
}

/**
 * ocpp21_set_der_control_req_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 * 
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp21_set_der_control_req_to_json(const ocpp21_set_der_control_req_t *in) {
    if (!in) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;
    if (ocpp_json_add_bool(obj, "isDefault", in->isDefault) < 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_str(obj, "controlId", in->controlId) <= 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_str(obj, "controlType", in->controlType) <= 0) { cJSON_Delete(obj); return NULL; }
    if (in->has_curve) { cJSON *ch = ocpp21_set_der_control_curve_to_json(&in->curve); if (!ch) { cJSON_Delete(obj); return NULL; } cJSON_AddItemToObject(obj, "curve", ch); }
    return obj;
}
/**
 * ocpp21_set_der_control_req_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 * 
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp21_set_der_control_req_from_json(const cJSON *obj, ocpp21_set_der_control_req_t *out) {
    if (!obj || !out) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    if (ocpp_json_get_bool(obj, "isDefault", &out->isDefault, 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "controlId", out->controlId, sizeof(out->controlId), 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "controlType", out->controlType, sizeof(out->controlType), 1) < 0) return OCPP_ERR_PARSE;
    { cJSON *ch = ocpp_json_get_obj(obj, "curve", 0); if (ch) { if (ocpp21_set_der_control_curve_from_json(ch, &out->curve) != OCPP_OK) return OCPP_ERR_PARSE; out->has_curve = 1; } }
    return OCPP_OK;
}
/**
 * ocpp21_set_der_control_req_encode - 把结构体编成 JSON 文本
 * @in: 已填好的结构
 * @buf: 输出文本
 * @buflen: buf 的字节数
 * 
 * 成功返回 OCPP_OK。缓冲不够返回 OCPP_ERR_OVERFLOW。树在 ocpp_json_dump 里释放。
 */
ocpp_err_t ocpp21_set_der_control_req_encode(const ocpp21_set_der_control_req_t *in, char *buf, size_t buflen) {
    ocpp_port_arena_reset();
    cJSON *o = ocpp21_set_der_control_req_to_json(in);
    if (!o) return OCPP_ERR_NOMEM;
    return ocpp_json_dump(o, buf, buflen);
}
/**
 * ocpp21_set_der_control_req_decode - 把 JSON 文本解析进结构体
 * @json: JSON 对象文本
 * @out: 输出结构
 * 
 * 成功返回 OCPP_OK。文本不是对象或必填字段缺失时返回 OCPP_ERR_PARSE。解析树在返回前 Delete。
 */
ocpp_err_t ocpp21_set_der_control_req_decode(const char *json, ocpp21_set_der_control_req_t *out) {
    if (!json || !out) return OCPP_ERR_ARG;
    ocpp_port_arena_reset();
    cJSON *o = cJSON_Parse(json);
    if (!o) return OCPP_ERR_PARSE;
    ocpp_err_t rc = ocpp21_set_der_control_req_from_json(o, out);
    cJSON_Delete(o);
    return rc;
}
/**
 * ocpp21_set_der_control_req_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 * 
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp21_set_der_control_req_example(ocpp21_set_der_control_req_t *out) {
    memset(out, 0, sizeof(*out));
    out->isDefault = 0;
    ocpp_str_copy(out->controlId, sizeof(out->controlId), "der-1");
    ocpp_str_copy(out->controlType, sizeof(out->controlType), "VoltVar");
    out->has_curve = 1;
    out->curve.priority = 0;
    ocpp_str_copy(out->curve.yUnit, sizeof(out->curve.yUnit), "V");
    out->curve.curveData_count = 1;
    out->curve.curveData[0].x = 0.0;
    out->curve.curveData[0].y = 1.0;
}


/**
 * ocpp21_set_der_control_conf_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 * 
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp21_set_der_control_conf_to_json(const ocpp21_set_der_control_conf_t *in) {
    if (!in) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;
    if (ocpp_json_add_str(obj, "status", in->status) <= 0) { cJSON_Delete(obj); return NULL; }
    return obj;
}
/**
 * ocpp21_set_der_control_conf_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 * 
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp21_set_der_control_conf_from_json(const cJSON *obj, ocpp21_set_der_control_conf_t *out) {
    if (!obj || !out) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    if (ocpp_json_get_str(obj, "status", out->status, sizeof(out->status), 1) < 0) return OCPP_ERR_PARSE;
    return OCPP_OK;
}
/**
 * ocpp21_set_der_control_conf_encode - 把结构体编成 JSON 文本
 * @in: 已填好的结构
 * @buf: 输出文本
 * @buflen: buf 的字节数
 * 
 * 成功返回 OCPP_OK。缓冲不够返回 OCPP_ERR_OVERFLOW。树在 ocpp_json_dump 里释放。
 */
ocpp_err_t ocpp21_set_der_control_conf_encode(const ocpp21_set_der_control_conf_t *in, char *buf, size_t buflen) {
    ocpp_port_arena_reset();
    cJSON *o = ocpp21_set_der_control_conf_to_json(in);
    if (!o) return OCPP_ERR_NOMEM;
    return ocpp_json_dump(o, buf, buflen);
}
/**
 * ocpp21_set_der_control_conf_decode - 把 JSON 文本解析进结构体
 * @json: JSON 对象文本
 * @out: 输出结构
 * 
 * 成功返回 OCPP_OK。文本不是对象或必填字段缺失时返回 OCPP_ERR_PARSE。解析树在返回前 Delete。
 */
ocpp_err_t ocpp21_set_der_control_conf_decode(const char *json, ocpp21_set_der_control_conf_t *out) {
    if (!json || !out) return OCPP_ERR_ARG;
    ocpp_port_arena_reset();
    cJSON *o = cJSON_Parse(json);
    if (!o) return OCPP_ERR_PARSE;
    ocpp_err_t rc = ocpp21_set_der_control_conf_from_json(o, out);
    cJSON_Delete(o);
    return rc;
}
/**
 * ocpp21_set_der_control_conf_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 * 
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp21_set_der_control_conf_example(ocpp21_set_der_control_conf_t *out) {
    memset(out, 0, sizeof(*out));
    ocpp_str_copy(out->status, sizeof(out->status), "Accepted");
}

