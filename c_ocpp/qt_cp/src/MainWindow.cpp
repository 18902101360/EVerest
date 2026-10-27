#include "MainWindow.h"

#include <QAbstractSocket>
#include <QCheckBox>
#include <QCoreApplication>
#include <QDateTime>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QNetworkRequest>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>

#include <cstring>

/*
 * session 保存的是 C 函数指针。C++ 成员函数语言链接对不上，所以放在 extern "C"。
 * 只接 Reset：它会改桩状态，必须在窗口里看得见。其余 CSMS CALL 用库里的 example 应答。
 */
extern "C" {
/* 1.6 Boot 应答：记日志，并把是否 Accepted 交给窗口。Pending 不算通过。 */
static void on_boot16(const ocpp16_boot_notification_conf_t *conf, void *user) {
    auto *w = static_cast<MainWindow *>(user);
    w->logLine(QStringLiteral("1.6 BootNotification.conf status=%1 interval=%2s time=%3")
                   .arg(QString::fromUtf8(conf->status))
                   .arg(conf->interval)
                   .arg(QString::fromUtf8(conf->currentTime)));
    w->noteBoot16(std::strcmp(conf->status, "Accepted") == 0);
}
/* 1.6 心跳应答，只把 CSMS 时间打到日志。 */
static void on_heartbeat16(const ocpp16_heartbeat_conf_t *conf, void *user) {
    static_cast<MainWindow *>(user)->logLine(
        QStringLiteral("1.6 Heartbeat.conf currentTime=%1").arg(QString::fromUtf8(conf->currentTime)));
}
/* 1.6 状态上报的空应答。能走到这里就说明 CALLRESULT 对上了。 */
static void on_status16(const ocpp16_status_notification_conf_t *, void *user) {
    static_cast<MainWindow *>(user)->logLine(QStringLiteral("1.6 StatusNotification.conf"));
}
/* 1.6 Reset：接受并填 Accepted。返回非 0 时库会回 InternalError。 */
static int on_reset16(const ocpp16_reset_req_t *req, ocpp16_reset_conf_t *conf, void *user) {
    ocpp_str_copy(conf->status, sizeof(conf->status), "Accepted");
    static_cast<MainWindow *>(user)->logLine(
        QStringLiteral("1.6 Reset.req type=%1 -> Accepted").arg(QString::fromUtf8(req->type)));
    return 0;
}
/* 2.0.1 Boot 应答，规则与 on_boot16 相同。 */
static void on_boot201(const ocpp201_boot_notification_conf_t *conf, void *user) {
    auto *w = static_cast<MainWindow *>(user);
    w->logLine(QStringLiteral("2.0.1 BootNotification.conf status=%1 interval=%2s time=%3")
                   .arg(QString::fromUtf8(conf->status))
                   .arg(conf->interval)
                   .arg(QString::fromUtf8(conf->currentTime)));
    w->noteBoot201(std::strcmp(conf->status, "Accepted") == 0);
}
/* 2.0.1 心跳应答。 */
static void on_heartbeat201(const ocpp201_heartbeat_conf_t *conf, void *user) {
    static_cast<MainWindow *>(user)->logLine(
        QStringLiteral("2.0.1 Heartbeat.conf currentTime=%1").arg(QString::fromUtf8(conf->currentTime)));
}
/* 2.0.1 状态上报的空应答。 */
static void on_status201(const ocpp201_status_notification_conf_t *, void *user) {
    static_cast<MainWindow *>(user)->logLine(QStringLiteral("2.0.1 StatusNotification.conf"));
}
/* 2.0.1 Reset：接受并填 Accepted。 */
static int on_reset201(const ocpp201_reset_req_t *req, ocpp201_reset_conf_t *conf, void *user) {
    ocpp_str_copy(conf->status, sizeof(conf->status), "Accepted");
    static_cast<MainWindow *>(user)->logLine(
        QStringLiteral("2.0.1 Reset.req type=%1 -> Accepted").arg(QString::fromUtf8(req->type)));
    return 0;
}
}

namespace {

/* 清掉 1.6 回调表，只挂本窗口要看的几项。user 用来从 C 回调回到窗口。 */
void fill16Handlers(ocpp16_handlers_t *h, MainWindow *window) {
    std::memset(h, 0, sizeof(*h));
    h->user = window;
    h->boot_notification_conf = on_boot16;
    h->heartbeat_conf = on_heartbeat16;
    h->status_notification_conf = on_status16;
    h->reset_req = on_reset16;
}

/* 清掉 2.0.1 回调表，只挂本窗口要看的几项。 */
void fill201Handlers(ocpp201_handlers_t *h, MainWindow *window) {
    std::memset(h, 0, sizeof(*h));
    h->user = window;
    h->boot_notification_conf = on_boot201;
    h->heartbeat_conf = on_heartbeat201;
    h->status_notification_conf = on_status201;
    h->reset_req = on_reset201;
}

} /* namespace */

/**
 * MainWindow::MainWindow - 建好两路套接字和界面，但先不连接
 * @parent: Qt 父对象，一般是空
 *
 * 这里调用 ocpp_port_init，把 cJSON 挂到 malloc/free，并清软件定时器表。
 * 两路 QWebSocket 都挂在 GUI 线程。100 ms 定时器只负责推进 c_ocpp 的心跳，
 * 间隔必须和 onTick 里传入的毫秒数一致。
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      urlEdit_(nullptr),
      idEdit_(nullptr),
      vendorEdit_(nullptr),
      modelEdit_(nullptr),
      use16_(nullptr),
      use201_(nullptr),
      status16_(nullptr),
      status201_(nullptr),
      log_(nullptr),
      tick_(nullptr),
      sock16_(QString(), QWebSocketProtocol::VersionLatest, this),
      sock201_(QString(), QWebSocketProtocol::VersionLatest, this),
      sess16Ready_(false),
      sess201Ready_(false),
      boot16Ok_(false),
      boot201Ok_(false),
      autoExit_(false),
      autoOk_(false) {
    tx16_.socket = &sock16_;
    tx16_.window = this;
    tx16_.name = "1.6";
    tx201_.socket = &sock201_;
    tx201_.window = this;
    tx201_.name = "2.0.1";
    memset(&sess16_, 0, sizeof(sess16_));
    memset(&sess201_, 0, sizeof(sess201_));

    /* arena 和软件定时器表是全局的，整个进程只能初始化一次。 */
    ocpp_port_init();

    auto *central = new QWidget(this);
    auto *form = new QFormLayout();
    urlEdit_ = new QLineEdit(QStringLiteral("ws://cpmstest.timxon.com:8887"), central);
    idEdit_ = new QLineEdit(QStringLiteral("CP001"), central);
    vendorEdit_ = new QLineEdit(QStringLiteral("zephyr"), central);
    modelEdit_ = new QLineEdit(QStringLiteral("QT-CP"), central);
    use16_ = new QCheckBox(QStringLiteral("连接 OCPP 1.6（子协议 ocpp1.6）"), central);
    use201_ = new QCheckBox(QStringLiteral("连接 OCPP 2.0.1（子协议 ocpp2.0.1）"), central);
    use16_->setChecked(true);
    use201_->setChecked(true);
    status16_ = new QLabel(QStringLiteral("未连接"), central);
    status201_ = new QLabel(QStringLiteral("未连接"), central);
    form->addRow(QStringLiteral("CSMS 地址"), urlEdit_);
    form->addRow(QStringLiteral("桩号"), idEdit_);
    /*
     * 「用户」写进 BootNotification 的厂商字段。不要放进 Authorization：
     * 这个平台对 Basic 用户 zephyr 直接 404，桩号只认 URL 路径。
     */
    form->addRow(QStringLiteral("用户"), vendorEdit_);
    form->addRow(QStringLiteral("型号"), modelEdit_);
    form->addRow(use16_);
    form->addRow(QStringLiteral("1.6 状态"), status16_);
    form->addRow(use201_);
    form->addRow(QStringLiteral("2.0.1 状态"), status201_);

    auto *buttons = new QHBoxLayout();
    auto *connectBtn = new QPushButton(QStringLiteral("连接并 Boot"), central);
    auto *disconnectBtn = new QPushButton(QStringLiteral("断开"), central);
    auto *statusBtn = new QPushButton(QStringLiteral("上报 StatusNotification"), central);
    buttons->addWidget(connectBtn);
    buttons->addWidget(disconnectBtn);
    buttons->addWidget(statusBtn);

    log_ = new QPlainTextEdit(central);
    log_->setReadOnly(true);
    log_->setPlaceholderText(QStringLiteral("收发的 OCPP JSON 显示在这里"));

    auto *layout = new QVBoxLayout(central);
    layout->addLayout(form);
    layout->addLayout(buttons);
    layout->addWidget(log_, 1);
    setCentralWidget(central);
    setWindowTitle(QStringLiteral("c_ocpp 充电桩 — Qt 5.14 WebSocket（OCPP 1.6 / 2.0.1）"));
    resize(860, 640);

    connect(connectBtn, &QPushButton::clicked, this, &MainWindow::startConnect);
    connect(disconnectBtn, &QPushButton::clicked, this, &MainWindow::onDisconnect);
    connect(statusBtn, &QPushButton::clicked, this, &MainWindow::onSendStatus);

    connect(&sock16_, &QWebSocket::connected, this, &MainWindow::on16Connected);
    connect(&sock16_, &QWebSocket::disconnected, this, &MainWindow::on16Disconnected);
    connect(&sock16_, &QWebSocket::textMessageReceived, this, &MainWindow::on16Text);
    connect(&sock16_, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &MainWindow::on16Error);

    connect(&sock201_, &QWebSocket::connected, this, &MainWindow::on201Connected);
    connect(&sock201_, &QWebSocket::disconnected, this, &MainWindow::on201Disconnected);
    connect(&sock201_, &QWebSocket::textMessageReceived, this, &MainWindow::on201Text);
    connect(&sock201_, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &MainWindow::on201Error);

    /*
     * c_ocpp 的心跳不会自己走。ocpp_port_timer_tick 每 100 ms 减一次剩余时间，
     * 间隔必须和 start() 一致，否则 Boot 里的 interval 会被放大或缩短。
     */
    tick_ = new QTimer(this);
    connect(tick_, &QTimer::timeout, this, &MainWindow::onTick);
    tick_->start(100);

    logLine(QStringLiteral("协议栈是 c_ocpp（OCPP 1.6 与 2.0.1）。本窗口只提供 QWebSocket。"));
    logLine(QStringLiteral("仓库里没有 OCPP 1.2。1.6 用子协议 ocpp1.6，2.0.1 用 ocpp2.0.1。"));
}

/**
 * MainWindow::applySettings - 用命令行覆盖界面默认值
 * @url: CSMS 基地址，不含桩号。空字符串表示不改
 * @cpId: 桩号，会变成 URL 路径。空字符串表示不改
 * @vendor: BootNotification 里的厂商名。空字符串表示不改
 * @model: 型号。空字符串表示不改
 *
 * 不在这里发起连接。连接要等 startConnect。
 */
void MainWindow::applySettings(const QString &url, const QString &cpId, const QString &vendor, const QString &model) {
    if (!url.isEmpty()) {
        urlEdit_->setText(url);
    }
    if (!cpId.isEmpty()) {
        idEdit_->setText(cpId);
    }
    if (!vendor.isEmpty()) {
        vendorEdit_->setText(vendor);
    }
    if (!model.isEmpty()) {
        modelEdit_->setText(model);
    }
}

/**
 * MainWindow::setAutoExit - 打开或关闭无人值守退出
 * @enabled: 真时，两路需要的 Boot 都 Accepted 才退出 0
 *
 * main 在 --auto 时调用。界面按钮不会改这个标志。
 */
void MainWindow::setAutoExit(bool enabled) {
    autoExit_ = enabled;
}

/**
 * MainWindow::logLine - 往窗口日志追加一行
 * @line: 已经格式化好的文本，不要带时间戳
 *
 * 时间戳在这里加。C 回调和套接字槽都通过它输出，所以它不能再调用 session。
 */
void MainWindow::logLine(const QString &line) {
    const QString stamp = QDateTime::currentDateTime().toString(QStringLiteral("hh:mm:ss.zzz"));
    log_->appendPlainText(stamp + QStringLiteral("  ") + line);
}

/* 改某一路状态标签的文字。label 必须是本窗口的 status16_ 或 status201_。 */
void MainWindow::setLinkStatus(QLabel *label, const QString &text) {
    label->setText(text);
}

/**
 * MainWindow::chargePointUrl - 拼出 ws://主机:端口/桩号
 *
 * OCPP 用路径区分桩，不把桩号放在 HTTP Basic 里。基地址末尾的斜杠要削掉，
 * 否则会多出一个空路径段。返回可直接交给 QUrl 的字符串。
 */
QString MainWindow::chargePointUrl() const {
    QString base = urlEdit_->text().trimmed();
    while (base.endsWith(QLatin1Char('/'))) {
        base.chop(1);
    }
    QUrl url(base);
    const QString id = idEdit_->text().trimmed();
    url.setPath(QStringLiteral("/") + id);
    return url.toString();
}

/**
 * MainWindow::sendThunk - c_ocpp 发出一帧时的 C 回调
 * @data: 已经编好的 OCPP JSON，不保证以 NUL 结尾
 * @len: data 的字节数
 * @user: 指向 Tx。里面有这一路的 QWebSocket 和窗口
 *
 * 返回 0 表示 QWebSocket 收下了。返回非 0 时 session 记成 OCPP_ERR_IO。
 * 不能改成捕获 lambda：ocpp_port_send_fn 是 C 函数指针。
 * 套接字还没 Connected 时直接失败，避免把心跳写进一个正在关闭的连接。
 */
int MainWindow::sendThunk(const void *data, size_t len, void *user) {
    auto *tx = static_cast<Tx *>(user);
    if (tx == nullptr || tx->socket == nullptr || tx->socket->state() != QAbstractSocket::ConnectedState) {
        return -1;
    }
    const QString text = QString::fromUtf8(static_cast<const char *>(data), static_cast<int>(len));
    if (tx->socket->sendTextMessage(text) < 0) {
        return -1;
    }
    tx->window->logLine(QStringLiteral("%1 TX %2").arg(QString::fromUtf8(tx->name), text));
    return 0;
}

/**
 * MainWindow::openLink - 按指定子协议发起握手
 * @sock: 这一路的套接字。若上次还连着，先 abort，避免两条握手叠在一起
 * @subprotocol: "ocpp1.6" 或 "ocpp2.0.1"，必须和这条链路上的 session 版本一致
 *
 * Qt 5.14 的 QWebSocket 没有设置子协议的函数，所以写进 Sec-WebSocket-Protocol。
 * 没有这个头时，CSMS 无法知道该用哪一版 OCPP。
 */
void MainWindow::openLink(QWebSocket &sock, const QString &subprotocol) {
    if (sock.state() != QAbstractSocket::UnconnectedState) {
        sock.abort();
    }
    const QUrl url(chargePointUrl());
    QNetworkRequest request(url);
    request.setRawHeader("Sec-WebSocket-Protocol", subprotocol.toUtf8());
    logLine(QStringLiteral("打开 %1  子协议 %2").arg(request.url().toString(), subprotocol));
    sock.open(request);
}

/**
 * MainWindow::startConnect - 按勾选的版本打开 WebSocket
 *
 * 两路可以同时连。桩号相同、子协议不同，是两条独立的 TCP 连接。
 * 都没勾选时不连接；--auto 下这种情况退出码 1。
 * Boot 要等 connected 信号，不能在 open 之后立刻发。
 */
void MainWindow::startConnect() {
    boot16Ok_ = false;
    boot201Ok_ = false;
    autoOk_ = false;
    const bool want16 = use16_->isChecked();
    const bool want201 = use201_->isChecked();
    if (!want16 && !want201) {
        logLine(QStringLiteral("至少勾选一个 OCPP 版本"));
        if (autoExit_) {
            QCoreApplication::exit(1);
        }
        return;
    }
    if (want16) {
        setLinkStatus(status16_, QStringLiteral("正在连接"));
        openLink(sock16_, QStringLiteral("ocpp1.6"));
    } else {
        setLinkStatus(status16_, QStringLiteral("未启用"));
    }
    if (want201) {
        setLinkStatus(status201_, QStringLiteral("正在连接"));
        openLink(sock201_, QStringLiteral("ocpp2.0.1"));
    } else {
        setLinkStatus(status201_, QStringLiteral("未启用"));
    }
}

/**
 * MainWindow::onDisconnect - 关掉两路并停掉心跳槽
 *
 * 槽 0 是 1.6，槽 1 是 2.0.1。不停的话，下一次 tick 仍会调用 send，
 * 向已经关闭的套接字写心跳。
 */
void MainWindow::onDisconnect() {
    sock16_.close();
    sock201_.close();
    /* 0 和 1 是两路心跳槽。停掉，避免断线后 tick 仍去写已经关闭的套接字。 */
    ocpp_port_timer_stop(0);
    ocpp_port_timer_stop(1);
    sess16Ready_ = false;
    sess201Ready_ = false;
    setLinkStatus(status16_, QStringLiteral("已断开"));
    setLinkStatus(status201_, QStringLiteral("已断开"));
    logLine(QStringLiteral("已断开两条连接"));
}

/**
 * MainWindow::onTick - 把软件定时器向前推 100 ms
 *
 * c_ocpp 的心跳不会自己跑，只有 ocpp_port_timer_tick 会减剩余时间。
 * 这里的 100 必须和构造函数里 QTimer::start(100) 相同。
 */
void MainWindow::onTick() {
    ocpp_port_timer_tick(100);
}

/* 用界面上的用户名和型号填 1.6 Boot，其余字段用库里的示例。 */
void MainWindow::sendBoot16() {
    ocpp16_boot_notification_req_t boot;
    ocpp16_boot_notification_req_example(&boot);
    const QByteArray vendor = vendorEdit_->text().toUtf8();
    const QByteArray model = modelEdit_->text().toUtf8();
    ocpp_str_copy(boot.chargePointVendor, sizeof(boot.chargePointVendor), vendor.constData());
    ocpp_str_copy(boot.chargePointModel, sizeof(boot.chargePointModel), model.constData());
    const ocpp_err_t rc = ocpp16_session_send_boot_notification(&sess16_, &boot);
    if (rc != OCPP_OK) {
        logLine(QStringLiteral("1.6 发送 BootNotification 失败 rc=%1").arg(static_cast<int>(rc)));
    }
}

/* 用界面上的用户名和型号填 2.0.1 Boot。reason 仍是示例里的 PowerUp。 */
void MainWindow::sendBoot201() {
    ocpp201_boot_notification_req_t boot;
    ocpp201_boot_notification_req_example(&boot);
    const QByteArray vendor = vendorEdit_->text().toUtf8();
    const QByteArray model = modelEdit_->text().toUtf8();
    ocpp_str_copy(boot.chargingStation.vendorName, sizeof(boot.chargingStation.vendorName), vendor.constData());
    ocpp_str_copy(boot.chargingStation.model, sizeof(boot.chargingStation.model), model.constData());
    const ocpp_err_t rc = ocpp201_session_send_boot_notification(&sess201_, &boot);
    if (rc != OCPP_OK) {
        logLine(QStringLiteral("2.0.1 发送 BootNotification 失败 rc=%1").arg(static_cast<int>(rc)));
    }
}

/* 1.6 还没完成 session_init 时不发。状态内容用库里的示例。 */
void MainWindow::sendStatus16() {
    if (!sess16Ready_) {
        return;
    }
    ocpp16_status_notification_req_t req;
    ocpp16_status_notification_req_example(&req);
    const ocpp_err_t rc = ocpp16_session_send_status_notification(&sess16_, &req);
    if (rc != OCPP_OK) {
        logLine(QStringLiteral("1.6 StatusNotification 发送失败 rc=%1").arg(static_cast<int>(rc)));
    }
}

/* 2.0.1 还没完成 session_init 时不发。 */
void MainWindow::sendStatus201() {
    if (!sess201Ready_) {
        return;
    }
    ocpp201_status_notification_req_t req;
    ocpp201_status_notification_req_example(&req);
    const ocpp_err_t rc = ocpp201_session_send_status_notification(&sess201_, &req);
    if (rc != OCPP_OK) {
        logLine(QStringLiteral("2.0.1 StatusNotification 发送失败 rc=%1").arg(static_cast<int>(rc)));
    }
}

/**
 * MainWindow::onSendStatus - 给已经连上的每一路发 StatusNotification
 *
 * 没勾选或还没连上的那一路在 sendStatus* 里直接返回。
 */
void MainWindow::onSendStatus() {
    sendStatus16();
    sendStatus201();
}

/**
 * MainWindow::on16Connected - 1.6 握手完成后建立 session 并发送 Boot
 *
 * 心跳槽固定用 0。2.0.1 用槽 1。两路共用一张定时器表，槽号重复时后启动的一路会取消前一路。
 * accept_control 为 1，这路接受 Reset 等控制类 CALL。
 */
void MainWindow::on16Connected() {
    ocpp16_handlers_t handlers;
    fill16Handlers(&handlers, this);
    /*
     * 心跳槽必须和 2.0.1 的槽 1 错开。两路共用一张定时器表，
     * 都用 0 的话后启动的一路会取消前一路的心跳。
     */
    ocpp_link_t link;
    ocpp_link_init(&link, 0, &MainWindow::sendThunk, &tx16_, 0, 1);
    ocpp16_session_init(&sess16_, &handlers, &link);
    sess16Ready_ = true;
    setLinkStatus(status16_, QStringLiteral("已连接，发送 Boot"));
    logLine(QStringLiteral("1.6 WebSocket 已连接"));
    sendBoot16();
}

/**
 * MainWindow::on201Connected - 2.0.1 握手完成后建立 session 并发送 Boot
 *
 * 心跳槽固定用 1，避免和 1.6 的槽 0 冲突。其余约束与 on16Connected 相同。
 */
void MainWindow::on201Connected() {
    ocpp201_handlers_t handlers;
    fill201Handlers(&handlers, this);
    ocpp_link_t link;
    ocpp_link_init(&link, 1, &MainWindow::sendThunk, &tx201_, 1, 1);
    ocpp201_session_init(&sess201_, &handlers, &link);
    sess201Ready_ = true;
    setLinkStatus(status201_, QStringLiteral("已连接，发送 Boot"));
    logLine(QStringLiteral("2.0.1 WebSocket 已连接"));
    sendBoot201();
}

/* 1.6 断开后停槽 0。session 标记为未就绪，避免半包再送进去。 */
void MainWindow::on16Disconnected() {
    ocpp_port_timer_stop(0);
    sess16Ready_ = false;
    setLinkStatus(status16_, QStringLiteral("已断开"));
    logLine(QStringLiteral("1.6 WebSocket 已断开"));
}

/* 2.0.1 断开后停槽 1。 */
void MainWindow::on201Disconnected() {
    ocpp_port_timer_stop(1);
    sess201Ready_ = false;
    setLinkStatus(status201_, QStringLiteral("已断开"));
    logLine(QStringLiteral("2.0.1 WebSocket 已断开"));
}

/**
 * MainWindow::on16Text - 把一整帧文本交给 session
 *
 * bytes 必须活到 session_rx 返回，解析期间还要读这块缓冲。
 * 2.0.1 的槽也在这个线程，两路不会同时改同一份定时器表。
 */
void MainWindow::on16Text(const QString &message) {
    logLine(QStringLiteral("1.6 RX %1").arg(message));
    if (!sess16Ready_) {
        return;
    }
    const QByteArray bytes = message.toUtf8();
    const ocpp_err_t rc = ocpp16_session_rx(&sess16_, bytes.constData(), static_cast<size_t>(bytes.size()));
    if (rc != OCPP_OK) {
        logLine(QStringLiteral("1.6 session_rx 失败 rc=%1").arg(static_cast<int>(rc)));
    }
}

/**
 * MainWindow::on201Text - 把 2.0.1 的一整帧交给 session
 * @message: WebSocket 文本帧。必须是完整的一条 OCPP JSON，不能是半包
 *
 * toUtf8 的结果要活到 session_rx 返回，解析时还要读这块缓冲。
 */
void MainWindow::on201Text(const QString &message) {
    logLine(QStringLiteral("2.0.1 RX %1").arg(message));
    if (!sess201Ready_) {
        return;
    }
    const QByteArray bytes = message.toUtf8();
    const ocpp_err_t rc = ocpp201_session_rx(&sess201_, bytes.constData(), static_cast<size_t>(bytes.size()));
    if (rc != OCPP_OK) {
        logLine(QStringLiteral("2.0.1 session_rx 失败 rc=%1").arg(static_cast<int>(rc)));
    }
}

/**
 * MainWindow::on16Error - 1.6 套接字失败
 * @error: Qt 的套接字错误码，和 ocpp_err_t 不是同一套
 *
 * --auto 时立刻退出 1。手动运行只改状态标签，留给用户再点连接。
 */
void MainWindow::on16Error(QAbstractSocket::SocketError error) {
    logLine(QStringLiteral("1.6 socket 错误 %1 %2").arg(static_cast<int>(error)).arg(sock16_.errorString()));
    setLinkStatus(status16_, QStringLiteral("错误"));
    if (autoExit_) {
        QCoreApplication::exit(1);
    }
}

/**
 * MainWindow::on201Error - 2.0.1 套接字失败
 * @error: Qt 的套接字错误码
 *
 * 处理与 on16Error 相同。
 */
void MainWindow::on201Error(QAbstractSocket::SocketError error) {
    logLine(QStringLiteral("2.0.1 socket 错误 %1 %2").arg(static_cast<int>(error)).arg(sock201_.errorString()));
    setLinkStatus(status201_, QStringLiteral("错误"));
    if (autoExit_) {
        QCoreApplication::exit(1);
    }
}

/**
 * MainWindow::noteBoot16 - 记录 1.6 Boot 结果并刷新状态
 * @accepted: 仅当 status 字符串是 Accepted 时为真
 *
 * Pending 表示桩已登记但平台还没批准，界面显示「Boot 未接受」，--auto 不会因此退出 0。
 */
void MainWindow::noteBoot16(bool accepted) {
    boot16Ok_ = accepted;
    setLinkStatus(status16_, accepted ? QStringLiteral("Boot Accepted") : QStringLiteral("Boot 未接受"));
    finishAutoIfReady();
}

/**
 * MainWindow::noteBoot201 - 记录 2.0.1 Boot 结果并刷新状态
 * @accepted: 仅当 status 字符串是 Accepted 时为真
 *
 * 含义与 noteBoot16 相同。
 */
void MainWindow::noteBoot201(bool accepted) {
    boot201Ok_ = accepted;
    setLinkStatus(status201_, accepted ? QStringLiteral("Boot Accepted") : QStringLiteral("Boot 未接受"));
    finishAutoIfReady();
}

/**
 * MainWindow::finishAutoIfReady - --auto 只在两路都 Accepted 时退出 0
 *
 * Pending 表示桩已登记但还没批准。把它当成成功会让未批准的桩也通过检查。
 */
void MainWindow::finishAutoIfReady() {
    if (!autoExit_ || autoOk_) {
        return;
    }
    const bool need16 = use16_->isChecked();
    const bool need201 = use201_->isChecked();
    if ((need16 && !boot16Ok_) || (need201 && !boot201Ok_)) {
        return;
    }
    autoOk_ = true;
    logLine(QStringLiteral("BootNotification 已接受，自动退出"));
    QTimer::singleShot(0, qApp, []() { QCoreApplication::exit(0); });
}
