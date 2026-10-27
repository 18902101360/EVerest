/* SPDX-License-Identifier: Apache-2.0
 * 协议层返回码（负数为失败）。与 OCPP CallError 字符串不是同一套。
 */
#ifndef OCPP_ERR_H
#define OCPP_ERR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ocpp_err {
    OCPP_OK = 0,
    OCPP_ERR_ARG = -1,       /* 空指针或非法参数 */
    OCPP_ERR_NOMEM = -2,     /* arena / 输出缓冲不够 */
    OCPP_ERR_PARSE = -3,     /* JSON 或字段解析失败 */
    OCPP_ERR_STATE = -4,     /* 会话状态不允许该操作 */
    OCPP_ERR_IO = -5,        /* send 回调失败 */
    OCPP_ERR_OVERFLOW = -6,  /* pending 表满或字符串截断溢出 */
    OCPP_ERR_NOTFOUND = -7,  /* CALLRESULT 对不上 pending uniqueId */
    OCPP_ERR_PROTOCOL = -8   /* RPC 帧格式非法 */
} ocpp_err_t;

#ifdef __cplusplus
}
#endif

#endif
