/* OCPP 2.1 新增报文编解码，以及一条会话同时处理 2.0.1 与 2.1。 */
#include "ocpp21.h"
#include "ocpp_json.h"
#include "ocpp_port.h"
#include <stdio.h>
#include <string.h>

static int g_fail;
static char g_tx[OCPP_FRAME_MAX];
static int mock_send(const void *data, size_t len, void *user) {
    (void)user;
    if (len >= sizeof(g_tx)) return -1;
    memcpy(g_tx, data, len);
    g_tx[len] = 0;
    return 0;
}
static void fail(const char *w) { fprintf(stderr, "FAIL %s\n", w); g_fail++; }

#define RT21(name) do { \
    ocpp21_##name##_req_t req, req2; ocpp21_##name##_conf_t conf, conf2; \
    char buf[OCPP_PAYLOAD_MAX]; \
    ocpp21_##name##_req_example(&req); \
    if (ocpp21_##name##_req_encode(&req, buf, sizeof buf) != OCPP_OK) fail(#name " req encode"); \
    if (ocpp21_##name##_req_decode(buf, &req2) != OCPP_OK) fail(#name " req decode"); \
    ocpp21_##name##_conf_example(&conf); \
    if (ocpp21_##name##_conf_encode(&conf, buf, sizeof buf) != OCPP_OK) fail(#name " conf encode"); \
    if (ocpp21_##name##_conf_decode(buf, &conf2) != OCPP_OK) fail(#name " conf decode"); \
} while (0)

static int g_der;
static int on_der(const ocpp21_set_der_control_req_t *req, ocpp21_set_der_control_conf_t *conf, void *user) {
    (void)user;
    if (!req->has_curve || strcmp(req->controlType, "VoltVar") != 0) return 1;
    ocpp_str_copy(conf->status, sizeof conf->status, "Accepted");
    g_der = 1;
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
    RT21(afrr_signal);
    RT21(adjust_periodic_event_stream);
    RT21(battery_swap);
    RT21(change_transaction_tariff);
    RT21(clear_der_control);
    RT21(clear_tariffs);
    RT21(close_periodic_event_stream);
    RT21(get_certificate_chain_status);
    RT21(get_der_control);
    RT21(get_periodic_event_stream);
    RT21(get_tariffs);
    RT21(notify_allowed_energy_transfer);
    RT21(notify_der_alarm);
    RT21(notify_der_start_stop);
    RT21(notify_periodic_event_stream);
    RT21(notify_priority_charging);
    RT21(notify_settlement);
    RT21(notify_web_payment_started);
    RT21(open_periodic_event_stream);
    RT21(pull_dynamic_schedule_update);
    RT21(report_der_control);
    RT21(request_battery_swap);
    RT21(set_der_control);
    RT21(set_default_tariff);
    RT21(update_dynamic_schedule);
    RT21(use_priority_charging);
    RT21(vat_number_validation);
    if (ocpp21_message_table()[0].action == NULL) fail("table");

    ocpp21_session_t s;
    ocpp21_handlers_t h21;
    memset(&h21, 0, sizeof h21);
    h21.set_der_control_req = on_der;
    ocpp_link_t link;
    ocpp_link_init(&link, 0, mock_send, NULL, 4, 1);
    ocpp21_session_init(&s, NULL, &h21, &link);

    ocpp201_boot_notification_req_t boot;
    ocpp201_boot_notification_req_example(&boot);
    if (ocpp201_session_send_boot_notification(&s.base, &boot) != OCPP_OK) fail("boot send");
    const char *boot_conf = "[3,\"1\",{\"currentTime\":\"2024-01-01T00:00:00Z\",\"interval\":300,\"status\":\"Accepted\"}]";
    if (ocpp21_session_rx(&s, boot_conf, strlen(boot_conf)) != OCPP_OK || !s.base.registered) fail("boot conf");

    ocpp21_set_der_control_req_t der;
    ocpp21_set_der_control_req_example(&der);
    char payload[OCPP_PAYLOAD_MAX];
    if (ocpp21_set_der_control_req_encode(&der, payload, sizeof payload) != OCPP_OK) fail("der encode");
    char frame[OCPP_FRAME_MAX];
    snprintf(frame, sizeof frame, "[2,\"csms-1\",\"SetDERControl\",%s]", payload);
    if (ocpp21_session_rx(&s, frame, strlen(frame)) != OCPP_OK) fail("der rx");
    if (!g_der || strstr(g_tx, "Accepted") == NULL) fail("der conf");

    const char *needs =
        "{\"requestedEnergyTransfer\":\"AC_single_phase\",\"derChargingParameters\":{\"evSupportedDERControl\":[\"VoltVar\"],\"maxApparentPower\":10000},\"v2xChargingParameters\":{\"minDischargePower\":-5000}}";
    cJSON *needs_json = cJSON_Parse(needs);
    ocpp201_charging_needs_t cn;
    if (!needs_json || ocpp201_charging_needs_from_json(needs_json, &cn) != OCPP_OK || !cn.has_der ||
        cn.evSupportedDERControl_count != 1 || strcmp(cn.evSupportedDERControl[0], "VoltVar") != 0 ||
        !cn.has_maxApparentPower || !cn.has_v2x || !cn.has_minDischargePower) {
        fail("charging needs 2.1");
    }
    cJSON_Delete(needs_json);
    ocpp201_transaction_info_t tx;
    cJSON *tx_json = cJSON_Parse("{\"transactionId\":\"tx-1\",\"chargingState\":\"Charging\"}");
    if (!tx_json || ocpp201_transaction_info_from_json(tx_json, &tx) != OCPP_OK || !tx.has_chargingState ||
        strcmp(tx.chargingState, "Charging") != 0) {
        fail("transaction chargingState");
    }
    cJSON_Delete(tx_json);

    if (g_fail) return 1;
    printf("ocpp2.1 codecs and session OK\n");
    return 0;
}
