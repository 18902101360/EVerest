/* SPDX-License-Identifier: Apache-2.0
 * 定长缓冲上的 JSON 字段读写。业务字符串不要 malloc。
 * cJSON 节点用系统 malloc/free，用完由 cJSON_Delete 释放。
 */
#ifndef OCPP_JSON_H
#define OCPP_JSON_H

#include "cJSON.h"
#include "ocpp_err.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ocpp_str_copy - 拷进定长数组并保证结尾是 NUL
 * @dst: 目标。NULL 或容量为 0 时直接返回
 * @dst_size: 含 NUL 的容量。超长会截断
 * @src: 源。NULL 时把目标收成空串
 */
void ocpp_str_copy(char *dst, size_t dst_size, const char *src);

/**
 * ocpp_json_dump - 打印后释放这棵树
 * @obj: 打印完不能再使用
 * @buf: 调用方提供的输出缓冲
 * @buflen: 至少 3 字节
 *
 * 成功返回 OCPP_OK。失败也要 Delete，避免改用 malloc 之后泄漏。
 */
ocpp_err_t ocpp_json_dump(const cJSON *obj, char *buf, size_t buflen);

/**
 * ocpp_json_get_str - 读字符串字段
 * @required: 非 0 时缺字段失败
 *
 * 读到返回 1，可选字段缺失返回 0，类型不对返回 -1。超长截断。
 */
int ocpp_json_get_str(const cJSON *obj, const char *key, char *dst, size_t dst_size, int required);
/** 读整数。返回值约定与 ocpp_json_get_str 相同。 */
int ocpp_json_get_int(const cJSON *obj, const char *key, int *out, int required);
/** 读布尔。数字 0/1 不算布尔，返回 -1。 */
int ocpp_json_get_bool(const cJSON *obj, const char *key, int *out, int required);
/** 读浮点。cJSON 的整数也是数字，可以读出来。 */
int ocpp_json_get_double(const cJSON *obj, const char *key, double *out, int required);
/** 取嵌套对象。指针属于父树，不要单独 Delete。类型不对返回 NULL。 */
cJSON *ocpp_json_get_obj(const cJSON *obj, const char *key, int required);
/** 取数组。不是数组返回 NULL。指针属于父树。 */
cJSON *ocpp_json_get_arr(const cJSON *obj, const char *key, int required);

/** 加字符串。空串不加并返回 0，这是为了让可选字段消失。成功 1，分配失败 -1。 */
int ocpp_json_add_str(cJSON *obj, const char *key, const char *value);
/** 加整数。成功 1，分配失败 -1。 */
int ocpp_json_add_int(cJSON *obj, const char *key, int value);
/** 加布尔。value 非 0 写成 true。成功 1，失败 -1。 */
int ocpp_json_add_bool(cJSON *obj, const char *key, int value);
/** 加浮点。成功 1，分配失败 -1。 */
int ocpp_json_add_double(cJSON *obj, const char *key, double value);

#ifdef __cplusplus
}
#endif

#endif
