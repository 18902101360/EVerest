/* 同一镜像同时链 1.6 与 2.0.1：类型/符号不得冲突。 */
#include "ocpp16.h"
#include "ocpp201.h"
#include "ocpp21.h"
#include "ocpp_port.h"

#include <stdio.h>
#include <string.h>

static char g_tx16[OCPP_FRAME_MAX];
static char g_tx201[OCPP_FRAME_MAX];
static char g_tx21[OCPP_FRAME_MAX];

/* 1.6 那一路的假发送，只保留最后一帧。 */
static int send16(const void *data, size_t len, void *user) {
    (void)user;
    if (len >= sizeof(g_tx16)) {
        return -1;
    }
    memcpy(g_tx16, data, len);
    g_tx16[len] = '\0';
    return 0;
}

/* 2.1 那一路的假发送，只保留最后一帧。 */
static int send21(const void *data, size_t len, void *user) {
    (void)user;
    if (len >= sizeof(g_tx21)) {
        return -1;
    }
    memcpy(g_tx21, data, len);
    g_tx21[len] = '\0';
    return 0;
}

/* 2.0.1 那一路的假发送，只保留最后一帧。 */
static int send201(const void *data, size_t len, void *user) {
    (void)user;
    if (len >= sizeof(g_tx201)) {
        return -1;
    }
    memcpy(g_tx201, data, len);
    g_tx201[len] = '\0';
    return 0;
}

/**
 * main - 本文件的测试入口
 *
 * 返回 0 表示全部检查通过。失败时向 stderr 打 FAIL，并返回非 0。
 */
int main(void) {
    ocpp_port_init();

    ocpp_link_t l16, l201;
    ocpp_link_init(&l16, 0, send16, NULL, 0, 1);
    ocpp_link_init(&l201, 1, send201, NULL, 1, 1);

    ocpp16_session_t s16;
    ocpp201_session_t s201;
    ocpp16_handlers_t h16;
    ocpp201_handlers_t h201;
    memset(&h16, 0, sizeof h16);
    memset(&h201, 0, sizeof h201);
    ocpp16_session_init(&s16, &h16, &l16);
    ocpp201_session_init(&s201, &h201, &l201);

    ocpp16_boot_notification_req_t b16;
    ocpp201_boot_notification_req_t b201;
    ocpp16_boot_notification_req_example(&b16);
    ocpp201_boot_notification_req_example(&b201);
    if (ocpp16_session_send_boot_notification(&s16, &b16) != OCPP_OK) {
        fprintf(stderr, "FAIL 1.6 boot\n");
        return 1;
    }
    if (ocpp201_session_send_boot_notification(&s201, &b201) != OCPP_OK) {
        fprintf(stderr, "FAIL 2.0.1 boot\n");
        return 1;
    }
    if (strstr(g_tx16, "BootNotification") == NULL || strstr(g_tx201, "BootNotification") == NULL) {
        fprintf(stderr, "FAIL boot action\n");
        return 1;
    }
    if (strcmp(g_tx16, g_tx201) == 0) {
        fprintf(stderr, "FAIL isolated sends\n");
        return 1;
    }

    ocpp_link_t l21;
    ocpp_link_init(&l21, 2, send21, NULL, 2, 1);
    ocpp21_session_t s21;
    ocpp21_session_init(&s21, NULL, NULL, &l21);
    ocpp21_notify_der_alarm_req_t alarm;
    ocpp21_notify_der_alarm_req_example(&alarm);
    if (ocpp21_session_send_notify_der_alarm(&s21, &alarm) != OCPP_OK || strstr(g_tx21, "NotifyDERAlarm") == NULL) {
        fprintf(stderr, "FAIL 2.1 alarm\n");
        return 1;
    }

    printf("ocpp1.6 + ocpp2.0.1 + ocpp2.1 linked together OK\n");
    return 0;
}
