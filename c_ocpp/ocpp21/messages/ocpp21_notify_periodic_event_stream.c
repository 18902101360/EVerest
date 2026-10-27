#include "ocpp21_notify_periodic_event_stream.h"
#include <string.h>
cJSON *ocpp21_notify_periodic_event_stream_data_to_json(const ocpp21_notify_periodic_event_stream_data_t *in) {
    if (!in) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;
    if (ocpp_json_add_double(obj, "t", in->t) < 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_str(obj, "v", in->v) <= 0) { cJSON_Delete(obj); return NULL; }
    return obj;
}
ocpp_err_t ocpp21_notify_periodic_event_stream_data_from_json(const cJSON *obj, ocpp21_notify_periodic_event_stream_data_t *out) {
    if (!obj || !out) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    if (ocpp_json_get_double(obj, "t", &out->t, 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "v", out->v, sizeof(out->v), 1) < 0) return OCPP_ERR_PARSE;
    return OCPP_OK;
}

/**
 * ocpp21_notify_periodic_event_stream_req_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 * 
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp21_notify_periodic_event_stream_req_to_json(const ocpp21_notify_periodic_event_stream_req_t *in) {
    if (!in) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;
    if (in->data_count > 0) {
        cJSON *arr = cJSON_CreateArray();
        if (!arr) { cJSON_Delete(obj); return NULL; }
        for (int i = 0; i < in->data_count && i < 2; i++) {
            cJSON *el = ocpp21_notify_periodic_event_stream_data_to_json(&in->data[i]);
            if (!el) { cJSON_Delete(arr); cJSON_Delete(obj); return NULL; }
            cJSON_AddItemToArray(arr, el);
        }
        cJSON_AddItemToObject(obj, "data", arr);
    }
    if (ocpp_json_add_int(obj, "id", in->id) < 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_int(obj, "pending", in->pending) < 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_str(obj, "basetime", in->basetime) <= 0) { cJSON_Delete(obj); return NULL; }
    return obj;
}
/**
 * ocpp21_notify_periodic_event_stream_req_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 * 
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp21_notify_periodic_event_stream_req_from_json(const cJSON *obj, ocpp21_notify_periodic_event_stream_req_t *out) {
    if (!obj || !out) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    { cJSON *arr = ocpp_json_get_arr(obj, "data", 1);
      if (!arr || cJSON_GetArraySize(arr) < 1) return OCPP_ERR_PARSE;
      int n = cJSON_GetArraySize(arr); if (n > 2) n = 2;
      out->data_count = n;
      for (int i = 0; i < n; i++) {
        cJSON *el = cJSON_GetArrayItem(arr, i);
        if (!cJSON_IsObject(el) || ocpp21_notify_periodic_event_stream_data_from_json(el, &out->data[i]) != OCPP_OK) return OCPP_ERR_PARSE;
      }
    }
    if (ocpp_json_get_int(obj, "id", &out->id, 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_int(obj, "pending", &out->pending, 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "basetime", out->basetime, sizeof(out->basetime), 1) < 0) return OCPP_ERR_PARSE;
    return OCPP_OK;
}
/**
 * ocpp21_notify_periodic_event_stream_req_encode - 把结构体编成 JSON 文本
 * @in: 已填好的结构
 * @buf: 输出文本
 * @buflen: buf 的字节数
 * 
 * 成功返回 OCPP_OK。缓冲不够返回 OCPP_ERR_OVERFLOW。树在 ocpp_json_dump 里释放。
 */
ocpp_err_t ocpp21_notify_periodic_event_stream_req_encode(const ocpp21_notify_periodic_event_stream_req_t *in, char *buf, size_t buflen) {
    ocpp_port_arena_reset();
    cJSON *o = ocpp21_notify_periodic_event_stream_req_to_json(in);
    if (!o) return OCPP_ERR_NOMEM;
    return ocpp_json_dump(o, buf, buflen);
}
/**
 * ocpp21_notify_periodic_event_stream_req_decode - 把 JSON 文本解析进结构体
 * @json: JSON 对象文本
 * @out: 输出结构
 * 
 * 成功返回 OCPP_OK。文本不是对象或必填字段缺失时返回 OCPP_ERR_PARSE。解析树在返回前 Delete。
 */
ocpp_err_t ocpp21_notify_periodic_event_stream_req_decode(const char *json, ocpp21_notify_periodic_event_stream_req_t *out) {
    if (!json || !out) return OCPP_ERR_ARG;
    ocpp_port_arena_reset();
    cJSON *o = cJSON_Parse(json);
    if (!o) return OCPP_ERR_PARSE;
    ocpp_err_t rc = ocpp21_notify_periodic_event_stream_req_from_json(o, out);
    cJSON_Delete(o);
    return rc;
}
/**
 * ocpp21_notify_periodic_event_stream_req_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 * 
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp21_notify_periodic_event_stream_req_example(ocpp21_notify_periodic_event_stream_req_t *out) {
    memset(out, 0, sizeof(*out));
    out->data_count = 1;
    out->data[0].t = 0.0;
    ocpp_str_copy(out->data[0].v, sizeof(out->data[0].v), "1");
    out->id = 1;
    out->pending = 0;
    ocpp_str_copy(out->basetime, sizeof(out->basetime), "2024-01-01T00:00:00Z");
}


/**
 * ocpp21_notify_periodic_event_stream_conf_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 * 
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp21_notify_periodic_event_stream_conf_to_json(const ocpp21_notify_periodic_event_stream_conf_t *in) {
    if (!in) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;

    return obj;
}
/**
 * ocpp21_notify_periodic_event_stream_conf_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 * 
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp21_notify_periodic_event_stream_conf_from_json(const cJSON *obj, ocpp21_notify_periodic_event_stream_conf_t *out) {
    if (!obj || !out) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));

    return OCPP_OK;
}
/**
 * ocpp21_notify_periodic_event_stream_conf_encode - 把结构体编成 JSON 文本
 * @in: 已填好的结构
 * @buf: 输出文本
 * @buflen: buf 的字节数
 * 
 * 成功返回 OCPP_OK。缓冲不够返回 OCPP_ERR_OVERFLOW。树在 ocpp_json_dump 里释放。
 */
ocpp_err_t ocpp21_notify_periodic_event_stream_conf_encode(const ocpp21_notify_periodic_event_stream_conf_t *in, char *buf, size_t buflen) {
    ocpp_port_arena_reset();
    cJSON *o = ocpp21_notify_periodic_event_stream_conf_to_json(in);
    if (!o) return OCPP_ERR_NOMEM;
    return ocpp_json_dump(o, buf, buflen);
}
/**
 * ocpp21_notify_periodic_event_stream_conf_decode - 把 JSON 文本解析进结构体
 * @json: JSON 对象文本
 * @out: 输出结构
 * 
 * 成功返回 OCPP_OK。文本不是对象或必填字段缺失时返回 OCPP_ERR_PARSE。解析树在返回前 Delete。
 */
ocpp_err_t ocpp21_notify_periodic_event_stream_conf_decode(const char *json, ocpp21_notify_periodic_event_stream_conf_t *out) {
    if (!json || !out) return OCPP_ERR_ARG;
    ocpp_port_arena_reset();
    cJSON *o = cJSON_Parse(json);
    if (!o) return OCPP_ERR_PARSE;
    ocpp_err_t rc = ocpp21_notify_periodic_event_stream_conf_from_json(o, out);
    cJSON_Delete(o);
    return rc;
}
/**
 * ocpp21_notify_periodic_event_stream_conf_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 * 
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp21_notify_periodic_event_stream_conf_example(ocpp21_notify_periodic_event_stream_conf_t *out) {
    memset(out, 0, sizeof(*out));
    (void)out;
}

