/*
 * 桩侧 CALL 的示例 JSON。界面上每个请求按钮旁边的文本框用这里的初值，用户可以改完再发。
 * 只列出充电桩会主动发出的 Request，平台下发的 CALL 不在这里。
 */
#ifndef QT_OCPP_CP_REQ_EXAMPLES_H
#define QT_OCPP_CP_REQ_EXAMPLES_H

#include <stddef.h>

/**
 * CpReqExample - 一个桩侧 action 和它的示例 JSON
 *
 * action 是 OCPP CALL 的名字，例如 "BootNotification"。
 * fill 把库里的 *_example() 编成 JSON 写进 buf。buf 至少 OCPP_PAYLOAD_MAX 字节。
 * 编失败时 buf 收成空串。
 */
struct CpReqExample {
    const char *action;
    void (*fill)(char *buf, size_t len);
};

/** 1.6 桩侧 Request。count 写出条数。返回的表是静态的，不要释放。 */
const CpReqExample *cpReqExamples16(int *count);
/** 2.0.1 桩侧 Request。2.1 连接上这些 action 仍然由桩发出。 */
const CpReqExample *cpReqExamples201(int *count);
/** 只在 2.1 里才有的桩侧 Request。 */
const CpReqExample *cpReqExamples21(int *count);

#endif
