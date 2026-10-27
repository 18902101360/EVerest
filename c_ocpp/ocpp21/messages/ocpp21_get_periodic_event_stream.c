#include "ocpp21_get_periodic_event_stream.h"
#include <string.h>

/**
 * ocpp21_get_periodic_event_stream_req_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 * 
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp21_get_periodic_event_stream_req_to_json(const ocpp21_get_periodic_event_stream_req_t *in) {
    if (!in) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;

    return obj;
}
/**
 * ocpp21_get_periodic_event_stream_req_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 * 
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp21_get_periodic_event_stream_req_from_json(const cJSON *obj, ocpp21_get_periodic_event_stream_req_t *out) {
    if (!obj || !out) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));

    return OCPP_OK;
}
/**
 * ocpp21_get_periodic_event_stream_req_encode - 把结构体编成 JSON 文本
 * @in: 已填好的结构
 * @buf: 输出文本
 * @buflen: buf 的字节数
 * 
 * 成功返回 OCPP_OK。缓冲不够返回 OCPP_ERR_OVERFLOW。树在 ocpp_json_dump 里释放。
 */
ocpp_err_t ocpp21_get_periodic_event_stream_req_encode(const ocpp21_get_periodic_event_stream_req_t *in, char *buf, size_t buflen) {
    ocpp_port_arena_reset();
    cJSON *o = ocpp21_get_periodic_event_stream_req_to_json(in);
    if (!o) return OCPP_ERR_NOMEM;
    return ocpp_json_dump(o, buf, buflen);
}
/**
 * ocpp21_get_periodic_event_stream_req_decode - 把 JSON 文本解析进结构体
 * @json: JSON 对象文本
 * @out: 输出结构
 * 
 * 成功返回 OCPP_OK。文本不是对象或必填字段缺失时返回 OCPP_ERR_PARSE。解析树在返回前 Delete。
 */
ocpp_err_t ocpp21_get_periodic_event_stream_req_decode(const char *json, ocpp21_get_periodic_event_stream_req_t *out) {
    if (!json || !out) return OCPP_ERR_ARG;
    ocpp_port_arena_reset();
    cJSON *o = cJSON_Parse(json);
    if (!o) return OCPP_ERR_PARSE;
    ocpp_err_t rc = ocpp21_get_periodic_event_stream_req_from_json(o, out);
    cJSON_Delete(o);
    return rc;
}
/**
 * ocpp21_get_periodic_event_stream_req_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 * 
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp21_get_periodic_event_stream_req_example(ocpp21_get_periodic_event_stream_req_t *out) {
    memset(out, 0, sizeof(*out));
    (void)out;
}

cJSON *ocpp21_get_periodic_event_stream_constantStreamData_params_to_json(const ocpp21_get_periodic_event_stream_constantStreamData_params_t *in) {
    if (!in) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;
    if (in->has_interval) { if (ocpp_json_add_int(obj, "interval", in->interval) < 0) { cJSON_Delete(obj); return NULL; } }
    if (in->has_values) { if (ocpp_json_add_int(obj, "values", in->values) < 0) { cJSON_Delete(obj); return NULL; } }
    return obj;
}
ocpp_err_t ocpp21_get_periodic_event_stream_constantStreamData_params_from_json(const cJSON *obj, ocpp21_get_periodic_event_stream_constantStreamData_params_t *out) {
    if (!obj || !out) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    { int gr = ocpp_json_get_int(obj, "interval", &out->interval, 0); if (gr < 0) return OCPP_ERR_PARSE; out->has_interval = gr > 0; }
    { int gr = ocpp_json_get_int(obj, "values", &out->values, 0); if (gr < 0) return OCPP_ERR_PARSE; out->has_values = gr > 0; }
    return OCPP_OK;
}

cJSON *ocpp21_get_periodic_event_stream_constantStreamData_to_json(const ocpp21_get_periodic_event_stream_constantStreamData_t *in) {
    if (!in) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;
    if (ocpp_json_add_int(obj, "id", in->id) < 0) { cJSON_Delete(obj); return NULL; }
    { cJSON *ch = ocpp21_get_periodic_event_stream_constantStreamData_params_to_json(&in->params); if (!ch) { cJSON_Delete(obj); return NULL; } cJSON_AddItemToObject(obj, "params", ch); }
    if (ocpp_json_add_int(obj, "variableMonitoringId", in->variableMonitoringId) < 0) { cJSON_Delete(obj); return NULL; }
    return obj;
}
ocpp_err_t ocpp21_get_periodic_event_stream_constantStreamData_from_json(const cJSON *obj, ocpp21_get_periodic_event_stream_constantStreamData_t *out) {
    if (!obj || !out) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    if (ocpp_json_get_int(obj, "id", &out->id, 1) < 0) return OCPP_ERR_PARSE;
    { cJSON *ch = ocpp_json_get_obj(obj, "params", 1); if (!ch) return OCPP_ERR_PARSE; if (ocpp21_get_periodic_event_stream_constantStreamData_params_from_json(ch, &out->params) != OCPP_OK) return OCPP_ERR_PARSE; }
    if (ocpp_json_get_int(obj, "variableMonitoringId", &out->variableMonitoringId, 1) < 0) return OCPP_ERR_PARSE;
    return OCPP_OK;
}

/**
 * ocpp21_get_periodic_event_stream_conf_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 * 
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp21_get_periodic_event_stream_conf_to_json(const ocpp21_get_periodic_event_stream_conf_t *in) {
    if (!in) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;
    if (in->constantStreamData_count > 0) {
        cJSON *arr = cJSON_CreateArray();
        if (!arr) { cJSON_Delete(obj); return NULL; }
        for (int i = 0; i < in->constantStreamData_count && i < 2; i++) {
            cJSON *el = ocpp21_get_periodic_event_stream_constantStreamData_to_json(&in->constantStreamData[i]);
            if (!el) { cJSON_Delete(arr); cJSON_Delete(obj); return NULL; }
            cJSON_AddItemToArray(arr, el);
        }
        cJSON_AddItemToObject(obj, "constantStreamData", arr);
    }
    return obj;
}
/**
 * ocpp21_get_periodic_event_stream_conf_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 * 
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp21_get_periodic_event_stream_conf_from_json(const cJSON *obj, ocpp21_get_periodic_event_stream_conf_t *out) {
    if (!obj || !out) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    { cJSON *arr = ocpp_json_get_arr(obj, "constantStreamData", 0);
      if (arr) {
        int n = cJSON_GetArraySize(arr); if (n > 2) n = 2;
        out->constantStreamData_count = n;
        for (int i = 0; i < n; i++) {
          cJSON *el = cJSON_GetArrayItem(arr, i);
          if (!cJSON_IsObject(el) || ocpp21_get_periodic_event_stream_constantStreamData_from_json(el, &out->constantStreamData[i]) != OCPP_OK) return OCPP_ERR_PARSE;
        }
      }
    }
    return OCPP_OK;
}
/**
 * ocpp21_get_periodic_event_stream_conf_encode - 把结构体编成 JSON 文本
 * @in: 已填好的结构
 * @buf: 输出文本
 * @buflen: buf 的字节数
 * 
 * 成功返回 OCPP_OK。缓冲不够返回 OCPP_ERR_OVERFLOW。树在 ocpp_json_dump 里释放。
 */
ocpp_err_t ocpp21_get_periodic_event_stream_conf_encode(const ocpp21_get_periodic_event_stream_conf_t *in, char *buf, size_t buflen) {
    ocpp_port_arena_reset();
    cJSON *o = ocpp21_get_periodic_event_stream_conf_to_json(in);
    if (!o) return OCPP_ERR_NOMEM;
    return ocpp_json_dump(o, buf, buflen);
}
/**
 * ocpp21_get_periodic_event_stream_conf_decode - 把 JSON 文本解析进结构体
 * @json: JSON 对象文本
 * @out: 输出结构
 * 
 * 成功返回 OCPP_OK。文本不是对象或必填字段缺失时返回 OCPP_ERR_PARSE。解析树在返回前 Delete。
 */
ocpp_err_t ocpp21_get_periodic_event_stream_conf_decode(const char *json, ocpp21_get_periodic_event_stream_conf_t *out) {
    if (!json || !out) return OCPP_ERR_ARG;
    ocpp_port_arena_reset();
    cJSON *o = cJSON_Parse(json);
    if (!o) return OCPP_ERR_PARSE;
    ocpp_err_t rc = ocpp21_get_periodic_event_stream_conf_from_json(o, out);
    cJSON_Delete(o);
    return rc;
}
/**
 * ocpp21_get_periodic_event_stream_conf_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 * 
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp21_get_periodic_event_stream_conf_example(ocpp21_get_periodic_event_stream_conf_t *out) {
    memset(out, 0, sizeof(*out));
    out->constantStreamData_count = 1;
    out->constantStreamData[0].id = 1;
    out->constantStreamData[0].params.has_interval = 1;
    out->constantStreamData[0].params.interval = 60;
    out->constantStreamData[0].variableMonitoringId = 7;
}

