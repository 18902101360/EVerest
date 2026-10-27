#ifndef OCPP201_SET_DISPLAY_MESSAGE_H
#define OCPP201_SET_DISPLAY_MESSAGE_H
#include "ocpp201_types.h"
#include "ocpp_err.h"
#include "ocpp_port.h"
#include "ocpp_json.h"
#ifdef __cplusplus
extern "C" {
#endif
#define OCPP201_SET_DISPLAY_MESSAGE_ACTION "SetDisplayMessage"
typedef struct ocpp201_set_display_message_req {
    ocpp201_display_message_t message;
} ocpp201_set_display_message_req_t;
typedef struct ocpp201_set_display_message_conf {
    char status[17];
} ocpp201_set_display_message_conf_t;
/**
 * ocpp201_set_display_message_req_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_set_display_message_req_to_json(const ocpp201_set_display_message_req_t *in);
/**
 * ocpp201_set_display_message_req_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_set_display_message_req_from_json(const cJSON *obj, ocpp201_set_display_message_req_t *out);
/**
 * ocpp201_set_display_message_req_encode - 把结构体编成 JSON 文本
 * @in: 已填好的结构
 * @buf: 输出文本
 * @buflen: buf 的字节数
 *
 * 成功返回 OCPP_OK。缓冲不够返回 OCPP_ERR_OVERFLOW。树在 ocpp_json_dump 里释放。
 */
ocpp_err_t ocpp201_set_display_message_req_encode(const ocpp201_set_display_message_req_t *in, char *buf, size_t buflen);
/**
 * ocpp201_set_display_message_req_decode - 把 JSON 文本解析进结构体
 * @json: JSON 对象文本
 * @out: 输出结构
 *
 * 成功返回 OCPP_OK。文本非法或必填字段缺失时返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_set_display_message_req_decode(const char *json, ocpp201_set_display_message_req_t *out);
/**
 * ocpp201_set_display_message_req_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_set_display_message_req_example(ocpp201_set_display_message_req_t *out);
/**
 * ocpp201_set_display_message_conf_to_json - 结构体编成 cJSON 对象
 * @in: 源结构，不能为 NULL
 *
 * 失败返回 NULL。调用方负责 cJSON_Delete。空的可选字段不会写进去。
 */
cJSON *ocpp201_set_display_message_conf_to_json(const ocpp201_set_display_message_conf_t *in);
/**
 * ocpp201_set_display_message_conf_from_json - 从 cJSON 对象填结构体
 * @obj: JSON 对象
 * @out: 输出结构，函数内会先清零
 *
 * 成功返回 OCPP_OK。缺必填字段返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_set_display_message_conf_from_json(const cJSON *obj, ocpp201_set_display_message_conf_t *out);
/**
 * ocpp201_set_display_message_conf_encode - 把结构体编成 JSON 文本
 * @in: 已填好的结构
 * @buf: 输出文本
 * @buflen: buf 的字节数
 *
 * 成功返回 OCPP_OK。缓冲不够返回 OCPP_ERR_OVERFLOW。树在 ocpp_json_dump 里释放。
 */
ocpp_err_t ocpp201_set_display_message_conf_encode(const ocpp201_set_display_message_conf_t *in, char *buf, size_t buflen);
/**
 * ocpp201_set_display_message_conf_decode - 把 JSON 文本解析进结构体
 * @json: JSON 对象文本
 * @out: 输出结构
 *
 * 成功返回 OCPP_OK。文本非法或必填字段缺失时返回 OCPP_ERR_PARSE。
 */
ocpp_err_t ocpp201_set_display_message_conf_decode(const char *json, ocpp201_set_display_message_conf_t *out);
/**
 * ocpp201_set_display_message_conf_example - 填一份可直接发出去的示例
 * @out: 输出，函数内会先清零
 *
 * 测试和未实现的 CSMS 回调用这份默认值。正式桩应换成自己的数据。
 */
void ocpp201_set_display_message_conf_example(ocpp201_set_display_message_conf_t *out);
#ifdef __cplusplus
}
#endif
#endif
