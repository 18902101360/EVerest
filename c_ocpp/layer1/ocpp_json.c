/* SPDX-License-Identifier: Apache-2.0
 * cJSON 辅助：字符串进定长数组，数值/bool 读写。dump 用 PrintPreallocated，避免再 malloc。
 */
#include "ocpp_json.h"

#include <string.h>

/**
 * ocpp_str_copy - 把 src 拷进定长数组
 * @dst: 目标。NULL 或 dst_size 为 0 时直接返回
 * @dst_size: 目标容量，含结尾的 NUL
 * @src: 源。NULL 时把 dst 收成空串
 *
 * 超长就截断，保证 dst 以 NUL 结尾。业务字符串都走这里，避免 strcpy 写穿。
 */
void ocpp_str_copy(char *dst, size_t dst_size, const char *src) {
    if (dst == NULL || dst_size == 0) {
        return;
    }
    if (src == NULL) {
        dst[0] = '\0';
        return;
    }
    size_t n = strlen(src);
    if (n >= dst_size) {
        n = dst_size - 1;
    }
    memcpy(dst, src, n);
    dst[n] = '\0';
}

/**
 * ocpp_json_dump - 把 cJSON 树打进调用方缓冲，然后释放这棵树
 * @obj: 要打印的对象。函数返回后不能再使用
 * @buf: 输出文本
 * @buflen: buf 的字节数，至少 3
 *
 * 成功返回 OCPP_OK。参数不合法返回 OCPP_ERR_ARG，缓冲不够返回 OCPP_ERR_OVERFLOW。
 * 两种失败都会 cJSON_Delete。用 PrintPreallocated，避免再为打印结果 malloc 一块。
 */
ocpp_err_t ocpp_json_dump(const cJSON *obj, char *buf, size_t buflen) {
    if (obj == NULL || buf == NULL || buflen < 3) {
        cJSON_Delete((cJSON *)obj);
        return OCPP_ERR_ARG;
    }
    /* PrintPreallocated 只写调用方的 buf。树本身要用 free 还回去。 */
    int ok = cJSON_PrintPreallocated((cJSON *)obj, buf, (int)buflen, 0);
    cJSON_Delete((cJSON *)obj);
    if (!ok) {
        return OCPP_ERR_OVERFLOW;
    }
    return OCPP_OK;
}

/**
 * ocpp_json_get_str - 读一个字符串字段
 * @obj: JSON 对象
 * @key: 字段名，区分大小写
 * @dst: 输出，超长截断
 * @dst_size: dst 容量
 * @required: 非 0 时缺字段算失败
 *
 * 返回 1 表示读到了。可选字段缺失返回 0。类型不对或必填缺失返回 -1。
 */
int ocpp_json_get_str(const cJSON *obj, const char *key, char *dst, size_t dst_size, int required) {
    cJSON *it = cJSON_GetObjectItemCaseSensitive((cJSON *)obj, key);
    if (it == NULL) {
        if (dst && dst_size) {
            dst[0] = '\0';
        }
        return required ? -1 : 0;
    }
    if (!cJSON_IsString(it) || it->valuestring == NULL) {
        return -1;
    }
    ocpp_str_copy(dst, dst_size, it->valuestring);
    return 1;
}

/**
 * ocpp_json_get_int - 读一个整数段
 * @out: 可为 NULL，只检查字段在不在
 * @required: 非 0 时缺字段返回 -1
 *
 * 返回值约定与 ocpp_json_get_str 相同。数字以外的类型返回 -1。
 */
int ocpp_json_get_int(const cJSON *obj, const char *key, int *out, int required) {
    cJSON *it = cJSON_GetObjectItemCaseSensitive((cJSON *)obj, key);
    if (it == NULL) {
        return required ? -1 : 0;
    }
    if (!cJSON_IsNumber(it)) {
        return -1;
    }
    if (out) {
        *out = it->valueint;
    }
    return 1;
}

/**
 * ocpp_json_get_bool - 读一个布尔字段
 * @out: 真写成 1，假写成 0
 *
 * JSON 的 true/false 才算布尔。0 和 1 这种数字返回 -1。
 */
int ocpp_json_get_bool(const cJSON *obj, const char *key, int *out, int required) {
    cJSON *it = cJSON_GetObjectItemCaseSensitive((cJSON *)obj, key);
    if (it == NULL) {
        return required ? -1 : 0;
    }
    if (!cJSON_IsBool(it)) {
        return -1;
    }
    if (out) {
        *out = cJSON_IsTrue(it) ? 1 : 0;
    }
    return 1;
}

/**
 * ocpp_json_get_double - 读一个浮点字段
 *
 * cJSON 把整数也存成数字，所以整数字段也能读出来。
 */
int ocpp_json_get_double(const cJSON *obj, const char *key, double *out, int required) {
    cJSON *it = cJSON_GetObjectItemCaseSensitive((cJSON *)obj, key);
    if (it == NULL) {
        return required ? -1 : 0;
    }
    if (!cJSON_IsNumber(it)) {
        return -1;
    }
    if (out) {
        *out = it->valuedouble;
    }
    return 1;
}

/**
 * ocpp_json_get_obj - 取出一个嵌套对象
 * @required: 目前只影响缺字段时的语义，类型不对一律返回 NULL
 *
 * 返回的指针属于 obj 这棵树，不要单独 Delete。
 */
cJSON *ocpp_json_get_obj(const cJSON *obj, const char *key, int required) {
    cJSON *it = cJSON_GetObjectItemCaseSensitive((cJSON *)obj, key);
    if (it == NULL) {
        return required ? NULL : NULL;
    }
    if (!cJSON_IsObject(it)) {
        return NULL;
    }
    (void)required;
    return it;
}

/**
 * ocpp_json_get_arr - 取出一个数组字段
 *
 * 不是数组就返回 NULL。指针属于父对象，不要单独 Delete。
 */
cJSON *ocpp_json_get_arr(const cJSON *obj, const char *key, int required) {
    cJSON *it = cJSON_GetObjectItemCaseSensitive((cJSON *)obj, key);
    if (it == NULL) {
        return NULL;
    }
    (void)required;
    return cJSON_IsArray(it) ? it : NULL;
}

/**
 * ocpp_json_add_str - 往对象里加字符串
 *
 * 空串不加，返回 0。这是为了让可选字段在空的时候从 JSON 里消失。
 * 成功返回 1，cJSON 分配失败返回 -1。
 */
int ocpp_json_add_str(cJSON *obj, const char *key, const char *value) {
    if (obj == NULL || key == NULL || value == NULL || value[0] == '\0') {
        return 0;
    }
    return cJSON_AddStringToObject(obj, key, value) ? 1 : -1;
}

/** 加一个整数段。成功 1，分配失败 -1。 */
int ocpp_json_add_int(cJSON *obj, const char *key, int value) {
    return cJSON_AddNumberToObject(obj, key, value) ? 1 : -1;
}

/** 加一个布尔段。value 非 0 写成 true。成功 1，失败 -1。 */
int ocpp_json_add_bool(cJSON *obj, const char *key, int value) {
    return cJSON_AddBoolToObject(obj, key, value ? 1 : 0) ? 1 : -1;
}

/** 加一个浮点段。成功 1，分配失败 -1。 */
int ocpp_json_add_double(cJSON *obj, const char *key, double value) {
    return cJSON_AddNumberToObject(obj, key, value) ? 1 : -1;
}
