#include "ocpp21_get_certificate_chain_status.h"
#include <string.h>
cJSON *ocpp21_get_certificate_chain_status_certificateStatusRequests_to_json(const ocpp21_get_certificate_chain_status_certificateStatusRequests_t *in) {
    if (!in) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;
    if (ocpp_json_add_str(obj, "hashAlgorithm", in->hashAlgorithm) <= 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_str(obj, "issuerNameHash", in->issuerNameHash) <= 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_str(obj, "issuerKeyHash", in->issuerKeyHash) <= 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_str(obj, "serialNumber", in->serialNumber) <= 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_str(obj, "source", in->source) <= 0) { cJSON_Delete(obj); return NULL; }
    if (in->urls_count > 0) {
        cJSON *arr = cJSON_CreateArray();
        if (!arr) { cJSON_Delete(obj); return NULL; }
        for (int i = 0; i < in->urls_count && i < 2; i++) {
            cJSON *el = cJSON_CreateString(in->urls[i]);
            if (!el) { cJSON_Delete(arr); cJSON_Delete(obj); return NULL; }
            cJSON_AddItemToArray(arr, el);
        }
        cJSON_AddItemToObject(obj, "urls", arr);
    }
    return obj;
}
ocpp_err_t ocpp21_get_certificate_chain_status_certificateStatusRequests_from_json(const cJSON *obj, ocpp21_get_certificate_chain_status_certificateStatusRequests_t *out) {
    if (!obj || !out) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    if (ocpp_json_get_str(obj, "hashAlgorithm", out->hashAlgorithm, sizeof(out->hashAlgorithm), 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "issuerNameHash", out->issuerNameHash, sizeof(out->issuerNameHash), 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "issuerKeyHash", out->issuerKeyHash, sizeof(out->issuerKeyHash), 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "serialNumber", out->serialNumber, sizeof(out->serialNumber), 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "source", out->source, sizeof(out->source), 1) < 0) return OCPP_ERR_PARSE;
    { cJSON *arr = ocpp_json_get_arr(obj, "urls", 1);
      if (!arr || cJSON_GetArraySize(arr) < 1) return OCPP_ERR_PARSE;
      int n = cJSON_GetArraySize(arr); if (n > 2) n = 2;
      out->urls_count = n;
      for (int i = 0; i < n; i++) {
        cJSON *el = cJSON_GetArrayItem(arr, i);
        if (!cJSON_IsString(el)) return OCPP_ERR_PARSE;
        ocpp_str_copy(out->urls[i], sizeof(out->urls[i]), el->valuestring);
      }
    }
    return OCPP_OK;
}

/**
 * ocpp21_get_certificate_chain_status_req_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 * 
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp21_get_certificate_chain_status_req_to_json(const ocpp21_get_certificate_chain_status_req_t *in) {
    if (!in) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;
    if (in->certificateStatusRequests_count > 0) {
        cJSON *arr = cJSON_CreateArray();
        if (!arr) { cJSON_Delete(obj); return NULL; }
        for (int i = 0; i < in->certificateStatusRequests_count && i < 2; i++) {
            cJSON *el = ocpp21_get_certificate_chain_status_certificateStatusRequests_to_json(&in->certificateStatusRequests[i]);
            if (!el) { cJSON_Delete(arr); cJSON_Delete(obj); return NULL; }
            cJSON_AddItemToArray(arr, el);
        }
        cJSON_AddItemToObject(obj, "certificateStatusRequests", arr);
    }
    return obj;
}
/**
 * ocpp21_get_certificate_chain_status_req_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 * 
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp21_get_certificate_chain_status_req_from_json(const cJSON *obj, ocpp21_get_certificate_chain_status_req_t *out) {
    if (!obj || !out) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    { cJSON *arr = ocpp_json_get_arr(obj, "certificateStatusRequests", 1);
      if (!arr || cJSON_GetArraySize(arr) < 1) return OCPP_ERR_PARSE;
      int n = cJSON_GetArraySize(arr); if (n > 2) n = 2;
      out->certificateStatusRequests_count = n;
      for (int i = 0; i < n; i++) {
        cJSON *el = cJSON_GetArrayItem(arr, i);
        if (!cJSON_IsObject(el) || ocpp21_get_certificate_chain_status_certificateStatusRequests_from_json(el, &out->certificateStatusRequests[i]) != OCPP_OK) return OCPP_ERR_PARSE;
      }
    }
    return OCPP_OK;
}
/**
 * ocpp21_get_certificate_chain_status_req_encode - 把结构体编成 JSON 文本
 * @in: 已填好的结构
 * @buf: 输出文本
 * @buflen: buf 的字节数
 * 
 * 成功返回 OCPP_OK。缓冲不够返回 OCPP_ERR_OVERFLOW。树在 ocpp_json_dump 里释放。
 */
ocpp_err_t ocpp21_get_certificate_chain_status_req_encode(const ocpp21_get_certificate_chain_status_req_t *in, char *buf, size_t buflen) {
    ocpp_port_arena_reset();
    cJSON *o = ocpp21_get_certificate_chain_status_req_to_json(in);
    if (!o) return OCPP_ERR_NOMEM;
    return ocpp_json_dump(o, buf, buflen);
}
/**
 * ocpp21_get_certificate_chain_status_req_decode - 把 JSON 文本解析进结构体
 * @json: JSON 对象文本
 * @out: 输出结构
 * 
 * 成功返回 OCPP_OK。文本不是对象或必填字段缺失时返回 OCPP_ERR_PARSE。解析树在返回前 Delete。
 */
ocpp_err_t ocpp21_get_certificate_chain_status_req_decode(const char *json, ocpp21_get_certificate_chain_status_req_t *out) {
    if (!json || !out) return OCPP_ERR_ARG;
    ocpp_port_arena_reset();
    cJSON *o = cJSON_Parse(json);
    if (!o) return OCPP_ERR_PARSE;
    ocpp_err_t rc = ocpp21_get_certificate_chain_status_req_from_json(o, out);
    cJSON_Delete(o);
    return rc;
}
/**
 * ocpp21_get_certificate_chain_status_req_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 * 
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp21_get_certificate_chain_status_req_example(ocpp21_get_certificate_chain_status_req_t *out) {
    memset(out, 0, sizeof(*out));
    out->certificateStatusRequests_count = 1;
    ocpp_str_copy(out->certificateStatusRequests[0].hashAlgorithm, sizeof(out->certificateStatusRequests[0].hashAlgorithm), "SHA256");
    ocpp_str_copy(out->certificateStatusRequests[0].issuerNameHash, sizeof(out->certificateStatusRequests[0].issuerNameHash), "aa");
    ocpp_str_copy(out->certificateStatusRequests[0].issuerKeyHash, sizeof(out->certificateStatusRequests[0].issuerKeyHash), "bb");
    ocpp_str_copy(out->certificateStatusRequests[0].serialNumber, sizeof(out->certificateStatusRequests[0].serialNumber), "1");
    ocpp_str_copy(out->certificateStatusRequests[0].source, sizeof(out->certificateStatusRequests[0].source), "OCSP");
    out->certificateStatusRequests[0].urls_count = 1;
    ocpp_str_copy(out->certificateStatusRequests[0].urls[0], sizeof(out->certificateStatusRequests[0].urls[0]), "http://ocsp.example");
}

cJSON *ocpp21_get_certificate_chain_status_certificateStatus_to_json(const ocpp21_get_certificate_chain_status_certificateStatus_t *in) {
    if (!in) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;
    if (ocpp_json_add_str(obj, "hashAlgorithm", in->hashAlgorithm) <= 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_str(obj, "issuerNameHash", in->issuerNameHash) <= 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_str(obj, "issuerKeyHash", in->issuerKeyHash) <= 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_str(obj, "serialNumber", in->serialNumber) <= 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_str(obj, "source", in->source) <= 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_str(obj, "status", in->status) <= 0) { cJSON_Delete(obj); return NULL; }
    if (ocpp_json_add_str(obj, "nextUpdate", in->nextUpdate) <= 0) { cJSON_Delete(obj); return NULL; }
    return obj;
}
ocpp_err_t ocpp21_get_certificate_chain_status_certificateStatus_from_json(const cJSON *obj, ocpp21_get_certificate_chain_status_certificateStatus_t *out) {
    if (!obj || !out) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    if (ocpp_json_get_str(obj, "hashAlgorithm", out->hashAlgorithm, sizeof(out->hashAlgorithm), 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "issuerNameHash", out->issuerNameHash, sizeof(out->issuerNameHash), 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "issuerKeyHash", out->issuerKeyHash, sizeof(out->issuerKeyHash), 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "serialNumber", out->serialNumber, sizeof(out->serialNumber), 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "source", out->source, sizeof(out->source), 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "status", out->status, sizeof(out->status), 1) < 0) return OCPP_ERR_PARSE;
    if (ocpp_json_get_str(obj, "nextUpdate", out->nextUpdate, sizeof(out->nextUpdate), 1) < 0) return OCPP_ERR_PARSE;
    return OCPP_OK;
}

/**
 * ocpp21_get_certificate_chain_status_conf_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 * 
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp21_get_certificate_chain_status_conf_to_json(const ocpp21_get_certificate_chain_status_conf_t *in) {
    if (!in) return NULL;
    cJSON *obj = cJSON_CreateObject();
    if (!obj) return NULL;
    if (in->certificateStatus_count > 0) {
        cJSON *arr = cJSON_CreateArray();
        if (!arr) { cJSON_Delete(obj); return NULL; }
        for (int i = 0; i < in->certificateStatus_count && i < 2; i++) {
            cJSON *el = ocpp21_get_certificate_chain_status_certificateStatus_to_json(&in->certificateStatus[i]);
            if (!el) { cJSON_Delete(arr); cJSON_Delete(obj); return NULL; }
            cJSON_AddItemToArray(arr, el);
        }
        cJSON_AddItemToObject(obj, "certificateStatus", arr);
    }
    return obj;
}
/**
 * ocpp21_get_certificate_chain_status_conf_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 * 
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp21_get_certificate_chain_status_conf_from_json(const cJSON *obj, ocpp21_get_certificate_chain_status_conf_t *out) {
    if (!obj || !out) return OCPP_ERR_ARG;
    memset(out, 0, sizeof(*out));
    { cJSON *arr = ocpp_json_get_arr(obj, "certificateStatus", 1);
      if (!arr || cJSON_GetArraySize(arr) < 1) return OCPP_ERR_PARSE;
      int n = cJSON_GetArraySize(arr); if (n > 2) n = 2;
      out->certificateStatus_count = n;
      for (int i = 0; i < n; i++) {
        cJSON *el = cJSON_GetArrayItem(arr, i);
        if (!cJSON_IsObject(el) || ocpp21_get_certificate_chain_status_certificateStatus_from_json(el, &out->certificateStatus[i]) != OCPP_OK) return OCPP_ERR_PARSE;
      }
    }
    return OCPP_OK;
}
/**
 * ocpp21_get_certificate_chain_status_conf_encode - 把结构体编成 JSON 文本
 * @in: 已填好的结构
 * @buf: 输出文本
 * @buflen: buf 的字节数
 * 
 * 成功返回 OCPP_OK。缓冲不够返回 OCPP_ERR_OVERFLOW。树在 ocpp_json_dump 里释放。
 */
ocpp_err_t ocpp21_get_certificate_chain_status_conf_encode(const ocpp21_get_certificate_chain_status_conf_t *in, char *buf, size_t buflen) {
    ocpp_port_arena_reset();
    cJSON *o = ocpp21_get_certificate_chain_status_conf_to_json(in);
    if (!o) return OCPP_ERR_NOMEM;
    return ocpp_json_dump(o, buf, buflen);
}
/**
 * ocpp21_get_certificate_chain_status_conf_decode - 把 JSON 文本解析进结构体
 * @json: JSON 对象文本
 * @out: 输出结构
 * 
 * 成功返回 OCPP_OK。文本不是对象或必填字段缺失时返回 OCPP_ERR_PARSE。解析树在返回前 Delete。
 */
ocpp_err_t ocpp21_get_certificate_chain_status_conf_decode(const char *json, ocpp21_get_certificate_chain_status_conf_t *out) {
    if (!json || !out) return OCPP_ERR_ARG;
    ocpp_port_arena_reset();
    cJSON *o = cJSON_Parse(json);
    if (!o) return OCPP_ERR_PARSE;
    ocpp_err_t rc = ocpp21_get_certificate_chain_status_conf_from_json(o, out);
    cJSON_Delete(o);
    return rc;
}
/**
 * ocpp21_get_certificate_chain_status_conf_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 * 
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp21_get_certificate_chain_status_conf_example(ocpp21_get_certificate_chain_status_conf_t *out) {
    memset(out, 0, sizeof(*out));
    out->certificateStatus_count = 1;
    ocpp_str_copy(out->certificateStatus[0].hashAlgorithm, sizeof(out->certificateStatus[0].hashAlgorithm), "SHA256");
    ocpp_str_copy(out->certificateStatus[0].issuerNameHash, sizeof(out->certificateStatus[0].issuerNameHash), "aa");
    ocpp_str_copy(out->certificateStatus[0].issuerKeyHash, sizeof(out->certificateStatus[0].issuerKeyHash), "bb");
    ocpp_str_copy(out->certificateStatus[0].serialNumber, sizeof(out->certificateStatus[0].serialNumber), "1");
    ocpp_str_copy(out->certificateStatus[0].source, sizeof(out->certificateStatus[0].source), "OCSP");
    ocpp_str_copy(out->certificateStatus[0].status, sizeof(out->certificateStatus[0].status), "Good");
    ocpp_str_copy(out->certificateStatus[0].nextUpdate, sizeof(out->certificateStatus[0].nextUpdate), "2024-01-01T00:00:00Z");
}

