/* SPDX-License-Identifier: Apache-2.0
 *
 * Linux 宿主默认移植：系统 malloc/free + CLOCK_MONOTONIC + 软件定时器。
 * 单片机若没有堆，再换成本文件：tick 用 SysTick，send 写你们的 WS。
 */
#include "ocpp_port.h"

#include "cJSON.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    int used;
    int periodic;
    uint32_t remaining_ms;
    uint32_t period_ms;
    ocpp_port_timer_fn fn;
    void *ctx;
} ocpp_port_timer_t;

static ocpp_port_send_fn g_send; /* 仅 link.send 为空时的回落 */
static void *g_send_user;
static ocpp_port_timer_t g_timers[OCPP_PORT_TIMER_MAX];
static uint32_t g_now_ms;

/**
 * ocpp_port_init - 启动时调用一次
 *
 * 把 cJSON 的分配函数设成 libc 的 malloc 和 free，并清空软件定时器表。
 * 必须直接传入 malloc/free 这两个符号。cJSON 只有在函数指针就是它们时才使用 realloc。
 */
void ocpp_port_init(void) {
    memset(g_timers, 0, sizeof(g_timers));
    cJSON_Hooks hooks;
    memset(&hooks, 0, sizeof(hooks));
    /*
     * 必须直接挂 malloc/free。cJSON 只有在这两个函数指针就是 libc 的
     * malloc/free 时才会用 realloc，否则它会自己再分配一块再拷贝。
     */
    hooks.malloc_fn = malloc;
    hooks.free_fn = free;
    cJSON_InitHooks(&hooks);
}

/**
 * ocpp_port_arena_reset - 兼容旧的编解码入口
 *
 * 以前这里会把整块静态缓冲丢掉。现在每棵 cJSON 树由 cJSON_Delete 释放，
 * 所以这个函数是空的。不要在这里再 free，否则会和 Delete 重复释放。
 */
void ocpp_port_arena_reset(void) {
    /* 内存由 malloc/free 成对释放。编解码入口仍会调用，这里不再整块丢弃。 */
}

/**
 * ocpp_port_arena_used - 查询旧的静态池用量
 *
 * 系统堆实现不统计在用字节，固定返回 0。
 */
size_t ocpp_port_arena_used(void) {
    return 0;
}

/**
 * ocpp_port_malloc - 分配 size 字节
 * @size: 字节数。0 返回 NULL，避免 malloc(0) 的实现差异
 *
 * 返回 libc malloc 的指针，失败也是 NULL。cJSON 通过自己的钩子直接调用 malloc，
 * 不经过这个函数。业务代码要分配时才用它。
 */
void *ocpp_port_malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    return malloc(size);
}

/**
 * ocpp_port_free - 释放 ocpp_port_malloc 得到的指针
 * @ptr: 可为 NULL，free(NULL) 是空操作
 */
void ocpp_port_free(void *ptr) {
    free(ptr);
}

/**
 * ocpp_port_realloc - 调整已分配块的大小
 * @ptr: 原来的指针。NULL 时等价于 malloc
 * @size: 新的字节数。0 时释放 ptr 并返回 NULL
 *
 * 失败返回 NULL，原块是否保留取决于 libc realloc。
 */
void *ocpp_port_realloc(void *ptr, size_t size) {
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    return realloc(ptr, size);
}

/**
 * ocpp_port_now_ms - 单调毫秒时钟
 *
 * 用 CLOCK_MONOTONIC，不受墙上时间回拨影响。clock_gettime 失败时退回
 * ocpp_port_timer_tick 累加的 g_now_ms。不能把这个实现链进没有 POSIX 时钟的裸机。
 */
uint32_t ocpp_port_now_ms(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return g_now_ms;
    }
    return (uint32_t)((uint64_t)ts.tv_sec * 1000u + (uint64_t)ts.tv_nsec / 1000000u);
}

/**
 * ocpp_port_timer_start - 占用一个软件定时器槽
 * @timer_id: 0 .. OCPP_PORT_TIMER_MAX-1。多路会话不能共用同一个槽
 * @delay_ms: 第一次到期时间，周期定时器的周期也是它
 * @periodic: 非 0 则到期后重新装入 delay_ms
 * @fn: 到期回调，不能为 NULL
 * @ctx: 交给回调的指针，通常是 session
 *
 * 返回 0 表示已经装入。槽号非法或 fn 为空时返回 -1。
 * 真正减时间的是 ocpp_port_timer_tick，本函数不会自己跑。
 */
int ocpp_port_timer_start(int timer_id, uint32_t delay_ms, int periodic, ocpp_port_timer_fn fn, void *ctx) {
    if (timer_id < 0 || timer_id >= OCPP_PORT_TIMER_MAX || fn == NULL) {
        return -1;
    }
    g_timers[timer_id].used = 1;
    g_timers[timer_id].periodic = periodic ? 1 : 0;
    g_timers[timer_id].remaining_ms = delay_ms;
    g_timers[timer_id].period_ms = delay_ms;
    g_timers[timer_id].fn = fn;
    g_timers[timer_id].ctx = ctx;
    return 0;
}

/**
 * ocpp_port_timer_stop - 停掉一个槽
 * @timer_id: 要停的槽。越界则什么都不做
 *
 * 断线时必须停掉该路心跳槽，否则下一次 tick 还会调用发送回调。
 */
void ocpp_port_timer_stop(int timer_id) {
    if (timer_id < 0 || timer_id >= OCPP_PORT_TIMER_MAX) {
        return;
    }
    g_timers[timer_id].used = 0;
}

/**
 * ocpp_port_timer_tick - 把所有在用的槽向前推进
 * @elapsed_ms: 距上次调用过了多少毫秒。调用方要和自己的节拍一致
 *
 * 到期时先重装或清除槽，再调用回调。回调里可以再次 start/stop。
 * 周期槽在回调之前就装回周期，避免回调里忘记续上。
 */
void ocpp_port_timer_tick(uint32_t elapsed_ms) {
    g_now_ms += elapsed_ms;
    for (int i = 0; i < OCPP_PORT_TIMER_MAX; i++) {
        ocpp_port_timer_t *t = &g_timers[i];
        if (!t->used) {
            continue;
        }
        if (t->remaining_ms > elapsed_ms) {
            t->remaining_ms -= elapsed_ms;
            continue;
        }
        ocpp_port_timer_fn fn = t->fn;
        void *ctx = t->ctx;
        if (t->periodic) {
            t->remaining_ms = t->period_ms;
        } else {
            t->used = 0;
        }
        if (fn) {
            fn(i, ctx);
        }
    }
}

/**
 * ocpp_port_set_send - 设置单路时的全局发送函数
 * @fn: 发送一帧 JSON。NULL 表示还没有传输
 * @user: 传给 fn 的指针
 *
 * 多路不要用它。每路应把 send 放进自己的 ocpp_link_t。
 */
void ocpp_port_set_send(ocpp_port_send_fn fn, void *user) {
    g_send = fn;
    g_send_user = user;
}

/**
 * ocpp_port_send - 调用全局发送函数
 * @data: 一帧 JSON
 * @len: 字节数
 *
 * 还没 set_send 时返回 -1。多路发送应走 ocpp_link_send。
 */
int ocpp_port_send(const void *data, size_t len) {
    if (g_send == NULL) {
        return -1;
    }
    return g_send(data, len, g_send_user);
}

/**
 * ocpp_link_init - 填好一条到 CSMS 的链路
 * @link: 输出。NULL 时直接返回
 * @id: 固件自己编的路号，回调里用来区分运营商
 * @send: 写这一路 WebSocket 文本帧的函数
 * @user: 传给 send 的指针，Qt 程序里是 Tx
 * @heartbeat_timer_id: Boot 被接受后使用的定时器槽，全工程唯一
 * @accept_control: 非 0 才执行 Reset 等会改桩状态的 CALL
 */
void ocpp_link_init(ocpp_link_t *link, int id, ocpp_port_send_fn send, void *user,
                    int heartbeat_timer_id, int accept_control) {
    if (link == NULL) {
        return;
    }
    link->id = id;
    link->send = send;
    link->send_user = user;
    link->heartbeat_timer_id = heartbeat_timer_id;
    link->accept_control = accept_control ? 1 : 0;
}

/**
 * ocpp_link_send - 从这一路把一帧发出去
 * @link: 可为 NULL，此时退回全局 ocpp_port_send
 * @data: JSON 文本
 * @len: 字节数
 *
 * 返回 send 回调的返回值。0 成功，非 0 失败。
 */
int ocpp_link_send(const ocpp_link_t *link, const void *data, size_t len) {
    if (link != NULL && link->send != NULL) {
        return link->send(data, len, link->send_user);
    }
    return ocpp_port_send(data, len);
}

/**
 * ocpp_action_is_control - 这个 action 会不会改桩的运行状态
 * @action: OCPP action 名。NULL 返回 0
 *
 * 返回 1 表示 Reset、RemoteStart、SetChargingProfile 这一类。
 * accept_control 为 0 的链路遇到这些 action 时直接回 SecurityError，不进业务回调。
 * 1.6、2.0.1 和 2.1 共用这张表。
 */
int ocpp_action_is_control(const char *action) {
    static const char *const k[] = {
        "Reset",
        "RemoteStartTransaction",
        "RemoteStopTransaction",
        "RequestStartTransaction",
        "RequestStopTransaction",
        "ChangeAvailability",
        "ChangeConfiguration",
        "UnlockConnector",
        "UpdateFirmware",
        "SignedUpdateFirmware",
        "SetChargingProfile",
        "ClearChargingProfile",
        "ReserveNow",
        "CancelReservation",
        "SendLocalList",
        "TriggerMessage",
        "ExtendedTriggerMessage",
        "GetDiagnostics",
        "GetLog",
        "InstallCertificate",
        "DeleteCertificate",
        "CertificateSigned",
        "ClearCache",
        "PublishFirmware",
        "UnpublishFirmware",
        "SetVariables",
        "SetDisplayMessage",
        "ClearDisplayMessage",
        "SetMonitoringBase",
        "SetMonitoringLevel",
        "SetVariableMonitoring",
        "ClearVariableMonitoring",
        "SetNetworkProfile",
        "CostUpdated",
        "CustomerInformation",
        "SetDERControl",
        "ClearDERControl",
        "SetDefaultTariff",
        "ClearTariffs",
        "ChangeTransactionTariff",
        "UsePriorityCharging",
        "RequestBatterySwap",
        "UpdateDynamicSchedule",
        "AFRRSignal",
        "AdjustPeriodicEventStream",
        "OpenPeriodicEventStream",
        "ClosePeriodicEventStream",
        NULL,
    };
    if (action == NULL) {
        return 0;
    }
    for (int i = 0; k[i] != NULL; i++) {
        if (strcmp(action, k[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * ocpp_port_log - 打一行诊断
 * @fmt: printf 格式串
 *
 * 宿主写到 stderr。裸机可以换成空函数，避免把 vfprintf 链进镜像。
 */
void ocpp_port_log(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fputc('\n', stderr);
    va_end(ap);
}
