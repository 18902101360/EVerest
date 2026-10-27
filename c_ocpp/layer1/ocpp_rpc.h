/* SPDX-License-Identifier: Apache-2.0
 * 第 1 层：OCPP JSON-RPC 外壳 [2|3|4, uniqueId, …]。不管业务 payload 语义。
 */
#ifndef OCPP_RPC_H
#define OCPP_RPC_H

#include "cJSON.h"
#include "ocpp_err.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OCPP_RPC_CALL 2       /* Charge Point 或 CSMS 发起的请求 */
#define OCPP_RPC_CALLRESULT 3 /* 对 CALL 的成功应答 */
#define OCPP_RPC_CALLERROR 4  /* 对 CALL 的错误应答 */

#define OCPP_UID_MAX 36
#define OCPP_ACTION_MAX 48
#define OCPP_ERROR_CODE_MAX 32

typedef struct ocpp_rpc_msg {
    int type; /* OCPP_RPC_CALL / CALLRESULT / CALLERROR */
    char unique_id[OCPP_UID_MAX + 1];
    char action[OCPP_ACTION_MAX + 1];                 /* 仅 CALL */
    char error_code[OCPP_ERROR_CODE_MAX + 1];         /* 仅 CALLERROR */
    char error_description[64];
    cJSON *payload; /* unpack 摘出来的树，调用方必须 cJSON_Delete */
} ocpp_rpc_msg_t;

/**
 * ocpp_rpc_pack_call - 组类型号 2 的 CALL
 * @payload_json: 已经是合法 JSON 对象，函数不再转义
 * @out: 整帧输出
 *
 * 成功返回 OCPP_OK。参数空或缓冲不够返回负的 ocpp_err_t。
 */
ocpp_err_t ocpp_rpc_pack_call(const char *unique_id, const char *action, const char *payload_json, char *out,
                              size_t outlen);
/**
 * ocpp_rpc_pack_callresult - 组类型号 3 的 CALLRESULT
 * @unique_id: 必须等于对端 CALL 的 uniqueId
 */
ocpp_err_t ocpp_rpc_pack_callresult(const char *unique_id, const char *payload_json, char *out, size_t outlen);
/**
 * ocpp_rpc_pack_callerror - 组类型号 4 的 CALLERROR
 * @code: 例如 SecurityError、InternalError、NotImplemented
 *
 * payload 固定写成 {}。
 */
ocpp_err_t ocpp_rpc_pack_callerror(const char *unique_id, const char *code, const char *desc, char *out, size_t outlen);

/**
 * ocpp_rpc_unpack - 解析一整帧
 * @frame: JSON 数组文本
 * @len: 字节数，不依赖结尾 NUL
 * @out: payload 是摘出来的树，调用方必须 cJSON_Delete
 *
 * 成功返回 OCPP_OK。格式不对返回 OCPP_ERR_PARSE 或 OCPP_ERR_PROTOCOL。
 */
ocpp_err_t ocpp_rpc_unpack(const char *frame, size_t len, ocpp_rpc_msg_t *out);

#ifdef __cplusplus
}
#endif

#endif
