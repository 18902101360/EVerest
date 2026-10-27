/* 编解码 + session 收发。无 socket，send 接到 mock_send。 */
#include "ocpp16.h"
#include "ocpp_port.h"

#include <stdio.h>
#include <string.h>

static int g_fail;
static char g_tx[OCPP_FRAME_MAX];

/* 测试用发送回调：把整帧拷进全局缓冲，不走网络。 */
static int mock_send(const void *data, size_t len, void *user) {
    (void)user;
    if (len >= sizeof(g_tx)) {
        return -1;
    }
    memcpy(g_tx, data, len);
    g_tx[len] = '\0';
    return 0;
}

/* 记一次失败。测试结束时用 g_fail 决定退出码。 */
static void fail(const char *what) {
    fprintf(stderr, "FAIL %s\n", what);
    g_fail++;
}

#define RT16(name)                                                                                                 \
    do {                                                                                                           \
        ocpp16_##name##_req_t req;                                                                               \
        ocpp16_##name##_conf_t conf;                                                                             \
        ocpp16_##name##_req_t req2;                                                                              \
        ocpp16_##name##_conf_t conf2;                                                                            \
        char buf[OCPP_PAYLOAD_MAX];                                                                                \
        ocpp16_##name##_req_example(&req);                                                                       \
        if (ocpp16_##name##_req_encode(&req, buf, sizeof(buf)) != OCPP_OK)                                     \
            fail(#name " req encode");                                                                             \
        if (ocpp16_##name##_req_decode(buf, &req2) != OCPP_OK)                                                 \
            fail(#name " req decode");                                                                             \
        ocpp16_##name##_conf_example(&conf);                                                                     \
        if (ocpp16_##name##_conf_encode(&conf, buf, sizeof(buf)) != OCPP_OK)                                   \
            fail(#name " conf encode");                                                                            \
        if (ocpp16_##name##_conf_decode(buf, &conf2) != OCPP_OK)                                               \
            fail(#name " conf decode");                                                                            \
    } while (0)

static int g_boot_ok;
/* 1.6 Boot 应答是 Accepted 且 interval 为 300 时记下成功。 */
static void on_boot_conf(const ocpp16_boot_notification_conf_t *conf, void *user) {
    (void)user;
    if (strcmp(conf->status, "Accepted") == 0 && conf->interval == 300) {
        g_boot_ok = 1;
    }
}

/* Reset 类型必须是 Soft，否则返回非 0，让库回 InternalError。 */
static int on_reset(const ocpp16_reset_req_t *req, ocpp16_reset_conf_t *conf, void *user) {
    (void)user;
    if (strcmp(req->type, "Soft") != 0) {
        return -1;
    }
    ocpp_str_copy(conf->status, sizeof(conf->status), "Accepted");
    return 0;
}

typedef struct {
    char tx[OCPP_FRAME_MAX];
    int reset_hits;
} sink_t;

static sink_t g_sink[3];

/* 多路测试的发送回调。user 指向这一路自己的缓冲。 */
static int sink_send(const void *data, size_t len, void *user) {
    sink_t *sk = (sink_t *)user;
    if (len >= sizeof(sk->tx)) {
        return -1;
    }
    memcpy(sk->tx, data, len);
    sk->tx[len] = '\0';
    return 0;
}

/* 统计这一路收到的 Reset。三路测试用来确认 accept_control。 */
/* 模拟链路写失败，用来确认 pending 不会被占住。 */
static int fail_send(const void *data, size_t len, void *user) {
    (void)data;
    (void)len;
    (void)user;
    return -1;
}

static int reset_count(const ocpp16_reset_req_t *req, ocpp16_reset_conf_t *conf, void *user) {
    sink_t *sk = (sink_t *)user;
    (void)req;
    sk->reset_hits++;
    ocpp_str_copy(conf->status, sizeof(conf->status), "Accepted");
    return 0;
}

/**
 * main - 本文件的测试入口
 *
 * 返回 0 表示全部检查通过。失败时向 stderr 打 FAIL，并返回非 0。
 */
int main(void) {
    ocpp_port_init();
    ocpp_port_set_send(mock_send, NULL);

    RT16(authorize);
    RT16(boot_notification);
    RT16(data_transfer);
    RT16(diagnostics_status_notification);
    RT16(firmware_status_notification);
    RT16(heartbeat);
    RT16(meter_values);
    RT16(start_transaction);
    RT16(status_notification);
    RT16(stop_transaction);
    RT16(cancel_reservation);
    RT16(change_availability);
    RT16(change_configuration);
    RT16(clear_cache);
    RT16(clear_charging_profile);
    RT16(get_composite_schedule);
    RT16(get_configuration);
    RT16(get_diagnostics);
    RT16(get_local_list_version);
    RT16(remote_start_transaction);
    RT16(remote_stop_transaction);
    RT16(reserve_now);
    RT16(reset);
    RT16(send_local_list);
    RT16(set_charging_profile);
    RT16(trigger_message);
    RT16(unlock_connector);
    RT16(update_firmware);
    RT16(certificate_signed);
    RT16(delete_certificate);
    RT16(extended_trigger_message);
    RT16(get_installed_certificate_ids);
    RT16(get_log);
    RT16(install_certificate);
    RT16(log_status_notification);
    RT16(security_event_notification);
    RT16(sign_certificate);
    RT16(signed_firmware_status_notification);
    RT16(signed_update_firmware);

    /* Layer2: CP BootNotification.req -> CSMS conf */
    ocpp16_session_t s;
    ocpp16_handlers_t h;
    memset(&h, 0, sizeof(h));
    h.boot_notification_conf = on_boot_conf;
    h.reset_req = on_reset;
    ocpp16_session_init(&s, &h, NULL);

    ocpp16_boot_notification_req_t boot;
    ocpp16_boot_notification_req_example(&boot);
    if (ocpp16_session_send_boot_notification(&s, &boot) != OCPP_OK) {
        fail("send boot");
    }
    ocpp_rpc_msg_t msg;
    if (ocpp_rpc_unpack(g_tx, strlen(g_tx), &msg) != OCPP_OK || strcmp(msg.action, "BootNotification") != 0) {
        fail("unpack boot call");
    }
    ocpp16_boot_notification_conf_t bconf;
    ocpp16_boot_notification_conf_example(&bconf);
    char pbuf[512];
    ocpp16_boot_notification_conf_encode(&bconf, pbuf, sizeof(pbuf));
    char frame[OCPP_FRAME_MAX];
    ocpp_rpc_pack_callresult(msg.unique_id, pbuf, frame, sizeof(frame));
    if (ocpp16_session_rx(&s, frame, strlen(frame)) != OCPP_OK || !g_boot_ok) {
        fail("boot conf");
    }

    /* Layer2: CSMS Reset.req -> CP conf */
    ocpp16_reset_req_t rreq;
    ocpp16_reset_req_example(&rreq);
    ocpp16_reset_req_encode(&rreq, pbuf, sizeof(pbuf));
    ocpp_rpc_pack_call("csms-1", "Reset", pbuf, frame, sizeof(frame));
    if (ocpp16_session_rx(&s, frame, strlen(frame)) != OCPP_OK) {
        fail("reset rx");
    }
    if (ocpp_rpc_unpack(g_tx, strlen(g_tx), &msg) != OCPP_OK || msg.type != OCPP_RPC_CALLRESULT) {
        fail("reset conf frame");
    }
    ocpp16_reset_conf_t rconf;
    if (ocpp16_reset_conf_from_json(msg.payload, &rconf) != OCPP_OK || strcmp(rconf.status, "Accepted") != 0) {
        fail("reset conf payload");
    }

    /* Remaining CSMS-originated CALLs: default handler replies with example conf */
    const ocpp16_message_info_t *tab = ocpp16_message_table();
    for (int i = 0; i < OCPP16_MESSAGE_COUNT; i++) {
        if (!tab[i].from_csms) {
            continue;
        }
        /* Reset already covered with a custom handler */
        if (strcmp(tab[i].action, "Reset") == 0) {
            continue;
        }
    }

    /* Three independent CSMS contexts: own send, pending, seq, timers, control flag. */
    memset(g_sink, 0, sizeof g_sink);
    ocpp_link_t link[3];
    ocpp16_session_t sess[3];
    ocpp16_handlers_t hh[3];
    const int accept[3] = {1, 0, 0};
    memset(hh, 0, sizeof hh);
    for (int i = 0; i < 3; i++) {
        ocpp_link_init(&link[i], i, sink_send, &g_sink[i], i, accept[i]);
        hh[i].reset_req = reset_count;
        hh[i].user = &g_sink[i];
        ocpp16_session_init(&sess[i], &hh[i], &link[i]);
    }

    if (ocpp16_session_send_boot_notification(&sess[0], &boot) != OCPP_OK) {
        fail("multi 0 boot send");
    }
    if (strstr(g_sink[0].tx, "BootNotification") == NULL || g_sink[1].tx[0] || g_sink[2].tx[0]) {
        fail("multi boot routed to 0 only");
    }
    for (int i = 1; i < 3; i++) {
        if (ocpp16_session_send_boot_notification(&sess[i], &boot) != OCPP_OK ||
            strstr(g_sink[i].tx, "BootNotification") == NULL) {
            fail("multi boot send");
        }
    }

    ocpp_rpc_msg_t mu[3];
    for (int i = 0; i < 3; i++) {
        ocpp_rpc_unpack(g_sink[i].tx, strlen(g_sink[i].tx), &mu[i]);
    }
    if (strcmp(mu[0].unique_id, mu[1].unique_id) != 0 || strcmp(mu[0].unique_id, mu[2].unique_id) != 0) {
        fail("multi independent seq should all start at 1");
    }

    bconf.interval = 1;
    ocpp16_boot_notification_conf_encode(&bconf, pbuf, sizeof pbuf);
    ocpp_rpc_pack_callresult(mu[0].unique_id, pbuf, frame, sizeof frame);
    if (ocpp16_session_rx(&sess[0], frame, strlen(frame)) != OCPP_OK || !sess[0].registered ||
        sess[1].registered || sess[2].registered) {
        fail("multi boot conf isolated");
    }
    for (int i = 1; i < 3; i++) {
        ocpp_rpc_pack_callresult(mu[i].unique_id, pbuf, frame, sizeof frame);
        if (ocpp16_session_rx(&sess[i], frame, strlen(frame)) != OCPP_OK || !sess[i].registered) {
            fail("multi boot conf");
        }
    }

    for (int i = 0; i < 3; i++) {
        g_sink[i].tx[0] = 0;
    }
    ocpp_port_timer_tick(1000);
    for (int i = 0; i < 3; i++) {
        if (strstr(g_sink[i].tx, "Heartbeat") == NULL) {
            fail("multi heartbeat timer");
        }
    }

    ocpp16_reset_req_encode(&rreq, pbuf, sizeof pbuf);
    ocpp_rpc_pack_call("csms-0", "Reset", pbuf, frame, sizeof frame);
    if (ocpp16_session_rx(&sess[0], frame, strlen(frame)) != OCPP_OK || g_sink[0].reset_hits != 1) {
        fail("multi primary reset");
    }
    ocpp_rpc_unpack(g_sink[0].tx, strlen(g_sink[0].tx), &mu[0]);
    if (mu[0].type != OCPP_RPC_CALLRESULT) {
        fail("multi primary reset result");
    }
    for (int i = 1; i < 3; i++) {
        ocpp_rpc_pack_call("csms-x", "Reset", pbuf, frame, sizeof frame);
        if (ocpp16_session_rx(&sess[i], frame, strlen(frame)) != OCPP_OK || g_sink[i].reset_hits != 0) {
            fail("multi telemetry reset must not run handler");
        }
        ocpp_rpc_unpack(g_sink[i].tx, strlen(g_sink[i].tx), &mu[i]);
        if (mu[i].type != OCPP_RPC_CALLERROR) {
            fail("multi telemetry reset CallError");
        }
    }

    ocpp16_get_configuration_req_t greq;
    ocpp16_get_configuration_req_example(&greq);
    ocpp16_get_configuration_req_encode(&greq, pbuf, sizeof pbuf);
    ocpp_rpc_pack_call("csms-2", "GetConfiguration", pbuf, frame, sizeof frame);
    if (ocpp16_session_rx(&sess[2], frame, strlen(frame)) != OCPP_OK) {
        fail("multi telemetry get config");
    }
    ocpp_rpc_unpack(g_sink[2].tx, strlen(g_sink[2].tx), &mu[2]);
    if (mu[2].type != OCPP_RPC_CALLRESULT) {
        fail("multi telemetry get config result");
    }

    /* Pending 不登记、不心跳，到点重发 Boot。发送失败不占 pending。 */
    ocpp_port_timer_stop(0);
    memset(&g_sink[0], 0, sizeof g_sink[0]);
    ocpp_link_init(&link[0], 0, sink_send, &g_sink[0], 0, 1);
    ocpp16_session_init(&sess[0], NULL, &link[0]);
    if (ocpp16_session_send_boot_notification(&sess[0], &boot) != OCPP_OK) {
        fail("pending boot send");
    }
    ocpp_rpc_unpack(g_sink[0].tx, strlen(g_sink[0].tx), &mu[0]);
    ocpp_str_copy(bconf.status, sizeof bconf.status, "Pending");
    bconf.interval = 1;
    ocpp16_boot_notification_conf_encode(&bconf, pbuf, sizeof pbuf);
    ocpp_rpc_pack_callresult(mu[0].unique_id, pbuf, frame, sizeof frame);
    g_sink[0].tx[0] = 0;
    if (ocpp16_session_rx(&sess[0], frame, strlen(frame)) != OCPP_OK || sess[0].registered) {
        fail("pending must not register");
    }
    ocpp_port_timer_tick(1000);
    if (strstr(g_sink[0].tx, "BootNotification") == NULL || strstr(g_sink[0].tx, "Heartbeat") != NULL) {
        fail("pending retries boot");
    }
    ocpp_link_init(&link[0], 0, fail_send, NULL, 0, 1);
    ocpp16_session_init(&sess[0], NULL, &link[0]);
    if (ocpp16_session_send_boot_notification(&sess[0], &boot) == OCPP_OK) {
        fail("io should fail");
    }
    for (int i = 0; i < OCPP16_PENDING_MAX; i++) {
        if (sess[0].pending[i].used) {
            fail("failed send left pending");
        }
    }

    if (g_fail) {
        fprintf(stderr, "%d failures\n", g_fail);
        return 1;
    }
    printf("ocpp1.6: 39 message req/conf codecs + boot/reset + 3-context session OK\n");
    return 0;
}
