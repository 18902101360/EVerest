/* OCPP 2.0.1 编解码 + session。覆盖方式同 test_ocpp16。 */
#include "ocpp201.h"
#include "ocpp_port.h"
#include <stdio.h>
#include <string.h>

static int g_fail;
static char g_tx[OCPP_FRAME_MAX];
/* 测试用发送回调：把整帧拷进全局缓冲，不走网络。 */
static int mock_send(const void *data, size_t len, void *user) {
    (void)user;
    if (len >= sizeof(g_tx)) return -1;
    memcpy(g_tx, data, len); g_tx[len]=0; return 0;
}
/* 记一次失败。测试结束时用 g_fail 决定退出码。 */
static void fail(const char *w) { fprintf(stderr, "FAIL %s\n", w); g_fail++; }

#define RT201(name) do { \
    ocpp201_##name##_req_t req; ocpp201_##name##_conf_t conf; \
    ocpp201_##name##_req_t req2; ocpp201_##name##_conf_t conf2; \
    char buf[OCPP_PAYLOAD_MAX]; \
    ocpp201_##name##_req_example(&req); \
    if (ocpp201_##name##_req_encode(&req, buf, sizeof buf) != OCPP_OK) fail(#name " req encode"); \
    if (ocpp201_##name##_req_decode(buf, &req2) != OCPP_OK) fail(#name " req decode"); \
    ocpp201_##name##_conf_example(&conf); \
    if (ocpp201_##name##_conf_encode(&conf, buf, sizeof buf) != OCPP_OK) fail(#name " conf encode"); \
    if (ocpp201_##name##_conf_decode(buf, &conf2) != OCPP_OK) fail(#name " conf decode"); \
} while (0)

static int g_boot_ok;
/* Boot 应答是 Accepted 且 interval 为 300 时记下成功。 */
static void on_boot(const ocpp201_boot_notification_conf_t *c, void *u) {
    (void)u; if (strcmp(c->status, "Accepted")==0) g_boot_ok=1;
}
/* Reset 类型必须是 Soft，否则返回非 0，让库回 InternalError。 */
static int on_reset(const ocpp201_reset_req_t *req, ocpp201_reset_conf_t *conf, void *u) {
    (void)u; ocpp_str_copy(conf->status, sizeof conf->status, "Accepted"); (void)req; return 0;
}

typedef struct { char tx[OCPP_FRAME_MAX]; int reset_hits; } sink_t;
static sink_t g_sink[3];
/* 多路测试的发送回调。user 指向这一路自己的缓冲。 */
static int sink_send(const void *data, size_t len, void *user) {
    sink_t *sk = user;
    if (len >= sizeof sk->tx) return -1;
    memcpy(sk->tx, data, len); sk->tx[len] = 0; return 0;
}
/* 统计这一路收到的 Reset。三路测试用来确认 accept_control。 */
static int reset_count(const ocpp201_reset_req_t *req, ocpp201_reset_conf_t *conf, void *user) {
    sink_t *sk = user; (void)req; sk->reset_hits++;
    ocpp_str_copy(conf->status, sizeof conf->status, "Accepted"); return 0;
}

/**
 * main - 本文件的测试入口
 *
 * 返回 0 表示全部检查通过。失败时向 stderr 打 FAIL，并返回非 0。
 */
int main(void) {
    ocpp_port_init();
    ocpp_port_set_send(mock_send, NULL);
    RT201(authorize);
    RT201(boot_notification);
    RT201(cancel_reservation);
    RT201(certificate_signed);
    RT201(change_availability);
    RT201(clear_cache);
    RT201(clear_charging_profile);
    RT201(clear_display_message);
    RT201(cleared_charging_limit);
    RT201(clear_variable_monitoring);
    RT201(cost_updated);
    RT201(customer_information);
    RT201(data_transfer);
    RT201(delete_certificate);
    RT201(firmware_status_notification);
    RT201(get15118_ev_certificate);
    RT201(get_base_report);
    RT201(get_certificate_status);
    RT201(get_charging_profiles);
    RT201(get_composite_schedule);
    RT201(get_display_messages);
    RT201(get_installed_certificate_ids);
    RT201(get_local_list_version);
    RT201(get_log);
    RT201(get_monitoring_report);
    RT201(get_report);
    RT201(get_transaction_status);
    RT201(get_variables);
    RT201(heartbeat);
    RT201(install_certificate);
    RT201(log_status_notification);
    RT201(meter_values);
    RT201(notify_charging_limit);
    RT201(notify_customer_information);
    RT201(notify_display_messages);
    RT201(notify_ev_charging_needs);
    RT201(notify_ev_charging_schedule);
    RT201(notify_event);
    RT201(notify_monitoring_report);
    RT201(notify_report);
    RT201(publish_firmware);
    RT201(publish_firmware_status_notification);
    RT201(report_charging_profiles);
    RT201(request_start_transaction);
    RT201(request_stop_transaction);
    RT201(reservation_status_update);
    RT201(reserve_now);
    RT201(reset);
    RT201(security_event_notification);
    RT201(send_local_list);
    RT201(set_charging_profile);
    RT201(set_display_message);
    RT201(set_monitoring_base);
    RT201(set_monitoring_level);
    RT201(set_network_profile);
    RT201(set_variable_monitoring);
    RT201(set_variables);
    RT201(sign_certificate);
    RT201(status_notification);
    RT201(transaction_event);
    RT201(trigger_message);
    RT201(unlock_connector);
    RT201(unpublish_firmware);
    RT201(update_firmware);

    ocpp201_session_t s; ocpp201_handlers_t h; memset(&h,0,sizeof h);
    h.boot_notification_conf = on_boot; h.reset_req = on_reset;
    ocpp201_session_init(&s, &h, NULL);
    ocpp201_boot_notification_req_t boot; ocpp201_boot_notification_req_example(&boot);
    if (ocpp201_session_send_boot_notification(&s, &boot) != OCPP_OK) fail("send boot");
    ocpp_rpc_msg_t msg;
    if (ocpp_rpc_unpack(g_tx, strlen(g_tx), &msg) != OCPP_OK) fail("unpack boot");
    ocpp201_boot_notification_conf_t bc; ocpp201_boot_notification_conf_example(&bc);
    char pbuf[OCPP_PAYLOAD_MAX]; ocpp201_boot_notification_conf_encode(&bc, pbuf, sizeof pbuf);
    char frame[OCPP_FRAME_MAX]; ocpp_rpc_pack_callresult(msg.unique_id, pbuf, frame, sizeof frame);
    if (ocpp201_session_rx(&s, frame, strlen(frame)) != OCPP_OK || !g_boot_ok) fail("boot conf");

    ocpp201_reset_req_t rr; ocpp201_reset_req_example(&rr);
    ocpp201_reset_req_encode(&rr, pbuf, sizeof pbuf);
    ocpp_rpc_pack_call("csms-1", "Reset", pbuf, frame, sizeof frame);
    if (ocpp201_session_rx(&s, frame, strlen(frame)) != OCPP_OK) fail("reset rx");
    if (ocpp_rpc_unpack(g_tx, strlen(g_tx), &msg) != OCPP_OK || msg.type != OCPP_RPC_CALLRESULT) fail("reset conf");

    memset(g_sink, 0, sizeof g_sink);
    ocpp_link_t link[3];
    ocpp201_session_t sess[3];
    ocpp201_handlers_t hh[3];
    const int accept[3] = {1, 0, 0};
    memset(hh, 0, sizeof hh);
    for (int i = 0; i < 3; i++) {
        ocpp_link_init(&link[i], i, sink_send, &g_sink[i], i, accept[i]);
        hh[i].reset_req = reset_count;
        hh[i].user = &g_sink[i];
        ocpp201_session_init(&sess[i], &hh[i], &link[i]);
    }
    if (ocpp201_session_send_boot_notification(&sess[0], &boot) != OCPP_OK) fail("multi 0 boot");
    if (g_sink[1].tx[0] || g_sink[2].tx[0]) fail("multi 201 boot leaked");
    for (int i = 1; i < 3; i++) {
        if (ocpp201_session_send_boot_notification(&sess[i], &boot) != OCPP_OK) fail("multi boot send");
        if (strstr(g_sink[i].tx, "BootNotification") == NULL) fail("multi boot missing");
    }

    ocpp_rpc_msg_t mu[3];
    for (int i = 0; i < 3; i++) ocpp_rpc_unpack(g_sink[i].tx, strlen(g_sink[i].tx), &mu[i]);
    bc.interval = 1;
    ocpp201_boot_notification_conf_encode(&bc, pbuf, sizeof pbuf);
    ocpp_rpc_pack_callresult(mu[0].unique_id, pbuf, frame, sizeof frame);
    if (ocpp201_session_rx(&sess[0], frame, strlen(frame)) != OCPP_OK || !sess[0].registered ||
        sess[1].registered || sess[2].registered) fail("multi 201 boot conf isolated");
    for (int i = 1; i < 3; i++) {
        ocpp_rpc_pack_callresult(mu[i].unique_id, pbuf, frame, sizeof frame);
        if (ocpp201_session_rx(&sess[i], frame, strlen(frame)) != OCPP_OK || !sess[i].registered) fail("multi 201 boot conf");
    }

    for (int i = 0; i < 3; i++) g_sink[i].tx[0] = 0;
    ocpp_port_timer_tick(1000);
    for (int i = 0; i < 3; i++) {
        if (strstr(g_sink[i].tx, "Heartbeat") == NULL) fail("multi 201 heartbeat timer");
    }

    ocpp201_reset_req_encode(&rr, pbuf, sizeof pbuf);
    ocpp_rpc_pack_call("csms-0", "Reset", pbuf, frame, sizeof frame);
    if (ocpp201_session_rx(&sess[0], frame, strlen(frame)) != OCPP_OK || g_sink[0].reset_hits != 1) fail("multi 201 primary reset");
    ocpp_rpc_unpack(g_sink[0].tx, strlen(g_sink[0].tx), &mu[0]);
    if (mu[0].type != OCPP_RPC_CALLRESULT) fail("multi 201 primary reset result");
    for (int i = 1; i < 3; i++) {
        ocpp_rpc_pack_call("csms-x", "Reset", pbuf, frame, sizeof frame);
        if (ocpp201_session_rx(&sess[i], frame, strlen(frame)) != OCPP_OK || g_sink[i].reset_hits != 0) fail("multi 201 telemetry reset");
        ocpp_rpc_unpack(g_sink[i].tx, strlen(g_sink[i].tx), &mu[i]);
        if (mu[i].type != OCPP_RPC_CALLERROR) fail("multi 201 telemetry CallError");
    }

    ocpp201_get_variables_req_t gv;
    ocpp201_get_variables_req_example(&gv);
    ocpp201_get_variables_req_encode(&gv, pbuf, sizeof pbuf);
    ocpp_rpc_pack_call("csms-2", "GetVariables", pbuf, frame, sizeof frame);
    if (ocpp201_session_rx(&sess[2], frame, strlen(frame)) != OCPP_OK) fail("multi 201 get variables");
    ocpp_rpc_unpack(g_sink[2].tx, strlen(g_sink[2].tx), &mu[2]);
    if (mu[2].type != OCPP_RPC_CALLRESULT) fail("multi 201 get variables result");

    if (g_fail) { fprintf(stderr, "%d failures\n", g_fail); return 1; }
    printf("ocpp2.0.1: 64 message req/conf codecs + boot/reset + 3-context session OK\n");
    return 0;
}
