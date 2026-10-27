/* SPDX-License-Identifier: Apache-2.0
 * 组/解 JSON-RPC 数组。payload 原样嵌入，不转义内部引号（调用方须保证 payload 是合法 JSON 对象）。
 */
#include "ocpp_rpc.h"

#include "ocpp_json.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* 用 vsnprintf 组一帧。写不下返回 OCPP_ERR_OVERFLOW，避免截断的 JSON 被发出去。 */
static ocpp_err_t pack4(char *out, size_t outlen, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(out, outlen, fmt, ap);
    va_end(ap);
    if (n < 0) {
        return OCPP_ERR_ARG;
    }
    if ((size_t)n >= outlen) {
        return OCPP_ERR_OVERFLOW;
    }
    return OCPP_OK;
}

/**
 * ocpp_rpc_pack_call - 组一条 CALL，类型号 2
 * @unique_id: 本端生成的报文号
 * @action: OCPP action 名
 * @payload_json: 已经编好的对象文本。NULL 时用 {}
 * @out: 输出整帧
 * @outlen: out 的容量
 *
 * 返回 OCPP_OK，或 OCPP_ERR_ARG / OCPP_ERR_OVERFLOW。
 * payload 原样嵌进去，不会再转义，所以它必须已经是合法 JSON。
 */
ocpp_err_t ocpp_rpc_pack_call(const char *unique_id, const char *action, const char *payload_json, char *out,
                              size_t outlen) {
    if (!unique_id || !action || !out) {
        return OCPP_ERR_ARG;
    }
    return pack4(out, outlen, "[2,\"%s\",\"%s\",%s]", unique_id, action, payload_json ? payload_json : "{}");
}

/**
 * ocpp_rpc_pack_callresult - 组一条 CALLRESULT，类型号 3
 * @unique_id: 必须等于对端 CALL 里的 uniqueId
 * @payload_json: 应答对象。NULL 时用 {}
 *
 * 返回值约定与 ocpp_rpc_pack_call 相同。
 */
ocpp_err_t ocpp_rpc_pack_callresult(const char *unique_id, const char *payload_json, char *out, size_t outlen) {
    if (!unique_id || !out) {
        return OCPP_ERR_ARG;
    }
    return pack4(out, outlen, "[3,\"%s\",%s]", unique_id, payload_json ? payload_json : "{}");
}

/**
 * ocpp_rpc_pack_callerror - 组一条 CALLERROR，类型号 4
 * @code: OCPP 错误码，例如 SecurityError、InternalError
 * @desc: 给人看的说明。NULL 时写成空串
 *
 * 错误帧的 payload 固定是 {}。
 */
ocpp_err_t ocpp_rpc_pack_callerror(const char *unique_id, const char *code, const char *desc, char *out,
                                   size_t outlen) {
    if (!unique_id || !code || !out) {
        return OCPP_ERR_ARG;
    }
    return pack4(out, outlen, "[4,\"%s\",\"%s\",\"%s\",{}]", unique_id, code, desc ? desc : "");
}

/**
 * ocpp_rpc_unpack - 解析一整帧 CALL / CALLRESULT / CALLERROR
 * @frame: JSON 数组文本
 * @len: 字节数，不依赖结尾 NUL
 * @out: 输出。payload 是从根上摘下来的 cJSON，调用方必须 cJSON_Delete
 *
 * 成功返回 OCPP_OK。格式不对返回 OCPP_ERR_PARSE 或 OCPP_ERR_PROTOCOL。
 * 失败时不会把半截树留给调用方。
 */
ocpp_err_t ocpp_rpc_unpack(const char *frame, size_t len, ocpp_rpc_msg_t *out) {
    if (!frame || !out) {
        return OCPP_ERR_ARG;
    }
    memset(out, 0, sizeof(*out));
    cJSON *root = cJSON_ParseWithLength(frame, len);
    if (!root || !cJSON_IsArray(root)) {
        cJSON_Delete(root);
        return OCPP_ERR_PARSE;
    }
    int n = cJSON_GetArraySize(root);
    cJSON *t = cJSON_GetArrayItem(root, 0);
    cJSON *id = cJSON_GetArrayItem(root, 1);
    if (!cJSON_IsNumber(t) || !cJSON_IsString(id)) {
        cJSON_Delete(root);
        return OCPP_ERR_PARSE;
    }
    out->type = t->valueint;
    ocpp_str_copy(out->unique_id, sizeof(out->unique_id), id->valuestring);

    if (out->type == OCPP_RPC_CALL) {
        cJSON *action = n > 2 ? cJSON_GetArrayItem(root, 2) : NULL;
        if (!cJSON_IsString(action)) {
            cJSON_Delete(root);
            return OCPP_ERR_PARSE;
        }
        ocpp_str_copy(out->action, sizeof(out->action), action->valuestring);
        cJSON *payload = n > 3 ? cJSON_GetArrayItem(root, 3) : NULL;
        out->payload = payload ? cJSON_DetachItemFromArray(root, 3) : cJSON_CreateObject();
    } else if (out->type == OCPP_RPC_CALLRESULT) {
        cJSON *payload = n > 2 ? cJSON_GetArrayItem(root, 2) : NULL;
        out->payload = payload ? cJSON_DetachItemFromArray(root, 2) : cJSON_CreateObject();
    } else if (out->type == OCPP_RPC_CALLERROR) {
        cJSON *code = n > 2 ? cJSON_GetArrayItem(root, 2) : NULL;
        cJSON *desc = n > 3 ? cJSON_GetArrayItem(root, 3) : NULL;
        if (cJSON_IsString(code)) {
            ocpp_str_copy(out->error_code, sizeof(out->error_code), code->valuestring);
        }
        if (cJSON_IsString(desc)) {
            ocpp_str_copy(out->error_description, sizeof(out->error_description), desc->valuestring);
        }
        out->payload = cJSON_CreateObject();
    } else {
        cJSON_Delete(root);
        return OCPP_ERR_PROTOCOL;
    }
    cJSON_Delete(root);
    return out->payload ? OCPP_OK : OCPP_ERR_NOMEM;
}
