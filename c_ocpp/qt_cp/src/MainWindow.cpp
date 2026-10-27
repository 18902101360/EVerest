#include "MainWindow.h"

#include "CpReqExamples.h"

#include <QAbstractSocket>
#include <QCoreApplication>
#include <QDateTime>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QNetworkRequest>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QTabWidget>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>

#include <cstring>

/*
 * session 保存的是 C 函数指针。C++ 成员函数语言链接对不上，所以放在 extern "C"。
 * 只接 Reset：它会改桩状态，必须在窗口里看得见。其余 CSMS CALL 用库里的 example 应答。
 */
extern "C" {
/* 1.6 Boot 应答：记到 1.6 分页的日志。Pending 不算通过。 */
static void on_boot16(const ocpp16_boot_notification_conf_t *conf, void *user) {
    auto *w = static_cast<MainWindow *>(user);
    w->logOn(MainWindow::LINK16, QStringLiteral("BootNotification.conf status=%1 interval=%2s time=%3")
                                     .arg(QString::fromUtf8(conf->status))
                                     .arg(conf->interval)
                                     .arg(QString::fromUtf8(conf->currentTime)));
    w->noteBoot(MainWindow::LINK16, std::strcmp(conf->status, "Accepted") == 0);
}
/* 1.6 心跳应答，只把 CSMS 时间打到日志。 */
static void on_heartbeat16(const ocpp16_heartbeat_conf_t *conf, void *user) {
    static_cast<MainWindow *>(user)->logOn(
        MainWindow::LINK16, QStringLiteral("Heartbeat.conf currentTime=%1").arg(QString::fromUtf8(conf->currentTime)));
}
/* 1.6 状态上报的空应答。能走到这里就说明 CALLRESULT 对上了。 */
static void on_status16(const ocpp16_status_notification_conf_t *, void *user) {
    static_cast<MainWindow *>(user)->logOn(MainWindow::LINK16, QStringLiteral("StatusNotification.conf"));
}
/* 1.6 Reset：接受并填 Accepted。返回非 0 时库会回 InternalError。 */
static int on_reset16(const ocpp16_reset_req_t *req, ocpp16_reset_conf_t *conf, void *user) {
    ocpp_str_copy(conf->status, sizeof(conf->status), "Accepted");
    static_cast<MainWindow *>(user)->logOn(
        MainWindow::LINK16, QStringLiteral("Reset.req type=%1 -> Accepted").arg(QString::fromUtf8(req->type)));
    return 0;
}
/* 2.0.1 Boot 应答，规则与 on_boot16 相同。 */
static void on_boot201(const ocpp201_boot_notification_conf_t *conf, void *user) {
    auto *w = static_cast<MainWindow *>(user);
    w->logOn(MainWindow::LINK201, QStringLiteral("BootNotification.conf status=%1 interval=%2s time=%3")
                                      .arg(QString::fromUtf8(conf->status))
                                      .arg(conf->interval)
                                      .arg(QString::fromUtf8(conf->currentTime)));
    w->noteBoot(MainWindow::LINK201, std::strcmp(conf->status, "Accepted") == 0);
}
/* 2.0.1 心跳应答。 */
static void on_heartbeat201(const ocpp201_heartbeat_conf_t *conf, void *user) {
    static_cast<MainWindow *>(user)->logOn(
        MainWindow::LINK201,
        QStringLiteral("Heartbeat.conf currentTime=%1").arg(QString::fromUtf8(conf->currentTime)));
}
/* 2.0.1 状态上报的空应答。 */
static void on_status201(const ocpp201_status_notification_conf_t *, void *user) {
    static_cast<MainWindow *>(user)->logOn(MainWindow::LINK201, QStringLiteral("StatusNotification.conf"));
}
/* 2.0.1 Reset：接受并填 Accepted。 */
static int on_reset201(const ocpp201_reset_req_t *req, ocpp201_reset_conf_t *conf, void *user) {
    ocpp_str_copy(conf->status, sizeof(conf->status), "Accepted");
    static_cast<MainWindow *>(user)->logOn(
        MainWindow::LINK201, QStringLiteral("Reset.req type=%1 -> Accepted").arg(QString::fromUtf8(req->type)));
    return 0;
}
/*
 * 2.1 的 Boot / Heartbeat / Status / Reset 仍是 2.0.1 的 action。
 * 回调类型用 ocpp201_*，日志写进 2.1 分页。
 */
static void on_boot21(const ocpp201_boot_notification_conf_t *conf, void *user) {
    auto *w = static_cast<MainWindow *>(user);
    w->logOn(MainWindow::LINK21, QStringLiteral("BootNotification.conf status=%1 interval=%2s time=%3")
                                     .arg(QString::fromUtf8(conf->status))
                                     .arg(conf->interval)
                                     .arg(QString::fromUtf8(conf->currentTime)));
    w->noteBoot(MainWindow::LINK21, std::strcmp(conf->status, "Accepted") == 0);
}
static void on_heartbeat21(const ocpp201_heartbeat_conf_t *conf, void *user) {
    static_cast<MainWindow *>(user)->logOn(
        MainWindow::LINK21, QStringLiteral("Heartbeat.conf currentTime=%1").arg(QString::fromUtf8(conf->currentTime)));
}
static void on_status21(const ocpp201_status_notification_conf_t *, void *user) {
    static_cast<MainWindow *>(user)->logOn(MainWindow::LINK21, QStringLiteral("StatusNotification.conf"));
}
static int on_reset21(const ocpp201_reset_req_t *req, ocpp201_reset_conf_t *conf, void *user) {
    ocpp_str_copy(conf->status, sizeof(conf->status), "Accepted");
    static_cast<MainWindow *>(user)->logOn(
        MainWindow::LINK21, QStringLiteral("Reset.req type=%1 -> Accepted").arg(QString::fromUtf8(req->type)));
    return 0;
}
}

namespace {

/* 子协议必须和这一路链上的库一致。CSMS 靠这个头选择版本。 */
const char *kSubprotocol[MainWindow::LINK_COUNT] = {"ocpp1.6", "ocpp2.0.1", "ocpp2.1"};
/* 日志前缀。和分页标题一致。 */
const char *kName[MainWindow::LINK_COUNT] = {"1.6", "2.0.1", "2.1"};

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

/* 2.1 会话里的 2.0.1 回调。日志走 2.1 分页，不能和 2.0.1 那一路共用。 */
void fill21Handlers(ocpp201_handlers_t *h, MainWindow *window) {
    std::memset(h, 0, sizeof(*h));
    h->user = window;
    h->boot_notification_conf = on_boot21;
    h->heartbeat_conf = on_heartbeat21;
    h->status_notification_conf = on_status21;
    h->reset_req = on_reset21;
}

} /* namespace */

/**
 * MainWindow::MainWindow - 建好三个分页和三路套接字，但先不连接
 * @parent: Qt 父对象，一般是空
 *
 * 这里调用 ocpp_port_init，把 cJSON 挂到 malloc/free，并清软件定时器表。
 * 三路 QWebSocket 都挂在 GUI 线程。100 ms 定时器只负责推进 c_ocpp 的心跳。
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      tick_(nullptr),
      sock16_(QString(), QWebSocketProtocol::VersionLatest, this),
      sock201_(QString(), QWebSocketProtocol::VersionLatest, this),
      sock21_(QString(), QWebSocketProtocol::VersionLatest, this),
      autoExit_(false),
      autoOk_(false) {
    for (int i = 0; i < LINK_COUNT; ++i) {
        link_[i].url = nullptr;
        link_[i].id = nullptr;
        link_[i].vendor = nullptr;
        link_[i].model = nullptr;
        link_[i].status = nullptr;
        link_[i].log = nullptr;
        ready_[i] = false;
        bootOk_[i] = false;
    }
    tx_[LINK16].socket = &sock16_;
    tx_[LINK201].socket = &sock201_;
    tx_[LINK21].socket = &sock21_;
    for (int i = 0; i < LINK_COUNT; ++i) {
        tx_[i].window = this;
        tx_[i].link = i;
        tx_[i].name = kName[i];
    }
    std::memset(&sess16_, 0, sizeof(sess16_));
    std::memset(&sess201_, 0, sizeof(sess201_));
    std::memset(&sess21_, 0, sizeof(sess21_));

    /* arena 和软件定时器表是全局的，整个进程只能初始化一次。示例 JSON 也要在这之后编。 */
    ocpp_port_init();

    auto *tabs = new QTabWidget(this);
    tabs->addTab(makePage(LINK16, QStringLiteral("CP001"),
                          QStringLiteral("子协议 ocpp1.6。下面每个按钮发一条桩侧 CALL，文本框里是示例 JSON，可以改。")),
                 QStringLiteral("OCPP 1.6"));
    tabs->addTab(makePage(LINK201, QStringLiteral("CP201"),
                          QStringLiteral("子协议 ocpp2.0.1。文本框是该 Request 的 payload，不含 CALL 外壳。")),
                 QStringLiteral("OCPP 2.0.1"));
    tabs->addTab(makePage(LINK21, QStringLiteral("CP21"),
                          QStringLiteral("子协议 ocpp2.1。上面是 2.1 新增的桩侧请求，下面仍可发 2.0.1 的请求。")),
                 QStringLiteral("OCPP 2.1"));
    setCentralWidget(tabs);
    setWindowTitle(QStringLiteral("c_ocpp 充电桩 — Qt 5.14（1.6 / 2.0.1 / 2.1）"));
    resize(980, 760);

    connect(&sock16_, &QWebSocket::connected, this, [this]() { onConnected(LINK16); });
    connect(&sock16_, &QWebSocket::disconnected, this, [this]() { onDisconnected(LINK16); });
    connect(&sock16_, &QWebSocket::textMessageReceived, this, [this](const QString &m) { onText(LINK16, m); });
    connect(&sock16_, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this,
            [this](QAbstractSocket::SocketError e) { onError(LINK16, e); });

    connect(&sock201_, &QWebSocket::connected, this, [this]() { onConnected(LINK201); });
    connect(&sock201_, &QWebSocket::disconnected, this, [this]() { onDisconnected(LINK201); });
    connect(&sock201_, &QWebSocket::textMessageReceived, this, [this](const QString &m) { onText(LINK201, m); });
    connect(&sock201_, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this,
            [this](QAbstractSocket::SocketError e) { onError(LINK201, e); });

    connect(&sock21_, &QWebSocket::connected, this, [this]() { onConnected(LINK21); });
    connect(&sock21_, &QWebSocket::disconnected, this, [this]() { onDisconnected(LINK21); });
    connect(&sock21_, &QWebSocket::textMessageReceived, this, [this](const QString &m) { onText(LINK21, m); });
    connect(&sock21_, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this,
            [this](QAbstractSocket::SocketError e) { onError(LINK21, e); });

    /*
     * c_ocpp 的心跳不会自己走。ocpp_port_timer_tick 每 100 ms 减一次剩余时间，
     * 间隔必须和 start() 一致，否则 Boot 里的 interval 会被放大或缩短。
     */
    tick_ = new QTimer(this);
    connect(tick_, &QTimer::timeout, this, &MainWindow::onTick);
    tick_->start(100);

    logOn(LINK16, QStringLiteral("本页只走 ocpp1.6。连接、请求和日志都与另外两页无关。"));
    logOn(LINK201, QStringLiteral("本页只走 ocpp2.0.1。"));
    logOn(LINK21, QStringLiteral("本页走 ocpp2.1。2.0.1 的 action 由内嵌会话处理，2.1 新报文走 ocpp21。"));
}

/**
 * MainWindow::makePage - 做出一个版本的整页
 * @link: LINK16 / LINK201 / LINK21
 * @defaultId: 这一页默认桩号，三页不要默认同一个路径
 * @hint: 请求列表上方的说明
 *
 * 上半是连接，中间是可滚动的请求按钮，最下面是这一页自己的日志。
 */
QWidget *MainWindow::makePage(int link, const QString &defaultId, const QString &hint) {
    auto *page = new QWidget(this);
    auto *form = new QFormLayout();
    link_[link].url = new QLineEdit(QStringLiteral("ws://cpmstest.timxon.com:8887"), page);
    link_[link].id = new QLineEdit(defaultId, page);
    link_[link].vendor = new QLineEdit(QStringLiteral("zephyr"), page);
    link_[link].model = new QLineEdit(QStringLiteral("QT-CP"), page);
    link_[link].status = new QLabel(QStringLiteral("未连接"), page);
    link_[link].status->setObjectName(QStringLiteral("linkStatus"));
    form->addRow(QStringLiteral("CSMS 地址"), link_[link].url);
    form->addRow(QStringLiteral("桩号"), link_[link].id);
    /*
     * 「用户」写进 BootNotification 的厂商字段。不要放进 Authorization：
     * 这个平台对 Basic 用户 zephyr 直接 404，桩号只认 URL 路径。
     */
    form->addRow(QStringLiteral("用户"), link_[link].vendor);
    form->addRow(QStringLiteral("型号"), link_[link].model);
    form->addRow(QStringLiteral("状态"), link_[link].status);

    auto *buttons = new QHBoxLayout();
    auto *connectBtn = new QPushButton(QStringLiteral("连接"), page);
    connectBtn->setObjectName(QStringLiteral("connectButton"));
    auto *disconnectBtn = new QPushButton(QStringLiteral("断开"), page);
    buttons->addWidget(connectBtn);
    buttons->addWidget(disconnectBtn);
    buttons->addStretch(1);
    connect(connectBtn, &QPushButton::clicked, this, [this, link]() { connectLink(link); });
    connect(disconnectBtn, &QPushButton::clicked, this, [this, link]() { disconnectLink(link); });

    auto *rows = new QVBoxLayout();
    auto *hintLabel = new QLabel(hint, page);
    hintLabel->setWordWrap(true);
    rows->addWidget(hintLabel);
    int n16 = 0;
    int n201 = 0;
    int n21 = 0;
    const CpReqExample *e16 = cpReqExamples16(&n16);
    const CpReqExample *e201 = cpReqExamples201(&n201);
    const CpReqExample *e21 = cpReqExamples21(&n21);
    if (link == LINK16) {
        addReqRows(link, rows, e16, n16, QString());
    } else if (link == LINK201) {
        addReqRows(link, rows, e201, n201, QString());
    } else {
        addReqRows(link, rows, e21, n21, QStringLiteral("2.1 新增"));
        addReqRows(link, rows, e201, n201, QStringLiteral("2.0.1 报文（本连接仍可发）"));
    }
    rows->addStretch(1);

    auto *rowHost = new QWidget(page);
    rowHost->setLayout(rows);
    auto *scroll = new QScrollArea(page);
    scroll->setWidgetResizable(true);
    scroll->setWidget(rowHost);

    link_[link].log = new QPlainTextEdit(page);
    link_[link].log->setObjectName(QStringLiteral("linkLog"));
    link_[link].log->setReadOnly(true);
    link_[link].log->setMinimumHeight(160);
    link_[link].log->setPlaceholderText(QStringLiteral("这一页的收发日志"));

    auto *layout = new QVBoxLayout(page);
    layout->addLayout(form);
    layout->addLayout(buttons);
    layout->addWidget(scroll, 2);
    layout->addWidget(link_[link].log, 1);
    return page;
}

/* 型号输入框。单独写出来是为了让 makePage 的 addRow 读起来短一些。 */
QLineEdit *modelEditSafe(int link);

/**
 * MainWindow::addReqRows - 追加一组「按钮 + 可编辑 JSON」
 * @link: 点按钮时发给哪一路
 * @rows: 请求区的纵向布局
 * @specs: 桩侧 action 和示例填充函数
 * @count: specs 的条数
 * @caption: 分组标题。空字符串表示不加
 *
 * 文本框初值是库里的 example JSON。用户改过之后，按钮发送文本框里的内容，不再重新取示例。
 */
void MainWindow::addReqRows(int link, QVBoxLayout *rows, const CpReqExample *specs, int count, const QString &caption) {
    if (!caption.isEmpty()) {
        auto *title = new QLabel(caption, rows->parentWidget());
        title->setStyleSheet(QStringLiteral("font-weight: bold;"));
        rows->addWidget(title);
    }
    char buf[OCPP_PAYLOAD_MAX];
    for (int i = 0; i < count; ++i) {
        specs[i].fill(buf, sizeof(buf));
        auto *btn = new QPushButton(QString::fromUtf8(specs[i].action), rows->parentWidget());
        btn->setObjectName(QStringLiteral("reqButton"));
        btn->setFixedWidth(260);
        auto *edit = new QLineEdit(QString::fromUtf8(buf), rows->parentWidget());
        edit->setPlaceholderText(QStringLiteral("Request JSON"));
        auto *line = new QHBoxLayout();
        line->addWidget(btn);
        line->addWidget(edit, 1);
        rows->addLayout(line);
        const QString action = QString::fromUtf8(specs[i].action);
        connect(btn, &QPushButton::clicked, this, [this, link, edit, action]() { sendEditedReq(link, action, edit->text()); });
    }
}

/**
 * MainWindow::applySettings - 用命令行覆盖三个分页
 * @url: CSMS 基地址，不含桩号。空字符串表示不改
 * @cpId: 桩号。空字符串表示各页保持自己的默认桩号
 * @vendor: BootNotification 里的厂商名。空字符串表示不改
 * @model: 型号。空字符串表示不改
 *
 * 不在这里发起连接。连接要等用户点按钮，或 --auto 调用 startConnect。
 */
void MainWindow::applySettings(const QString &url, const QString &cpId, const QString &vendor, const QString &model) {
    for (int i = 0; i < LINK_COUNT; ++i) {
        if (!url.isEmpty()) {
            link_[i].url->setText(url);
        }
        if (!cpId.isEmpty()) {
            link_[i].id->setText(cpId);
        }
        if (!vendor.isEmpty()) {
            link_[i].vendor->setText(vendor);
        }
        if (!model.isEmpty()) {
            link_[i].model->setText(model);
        }
    }
}

/**
 * MainWindow::setAutoExit - 打开或关闭无人值守退出
 * @enabled: 真时，三路 Boot 都 Accepted 才退出 0
 *
 * main 在 --auto 时调用。分页上的按钮不会改这个标志。
 */
void MainWindow::setAutoExit(bool enabled) {
    autoExit_ = enabled;
}

/**
 * MainWindow::logOn - 往指定分页的日志追加一行
 * @link: LINK16 / LINK201 / LINK21
 * @line: 已经格式化好的文本，不要带时间戳
 *
 * 时间戳在这里加。C 回调和套接字槽都通过它输出，所以它不能再调用 session。
 */
void MainWindow::logOn(int link, const QString &line) {
    if (link < 0 || link >= LINK_COUNT || link_[link].log == nullptr) {
        return;
    }
    const QString stamp = QDateTime::currentDateTime().toString(QStringLiteral("hh:mm:ss.zzz"));
    link_[link].log->appendPlainText(stamp + QStringLiteral("  ") + line);
}

/**
 * MainWindow::chargePointUrl - 拼出这一页的 ws://主机:端口/桩号
 * @link: 哪一页的地址和桩号
 *
 * OCPP 用路径区分桩。基地址末尾的斜杠要削掉，否则会多出一个空路径段。
 */
QString MainWindow::chargePointUrl(int link) const {
    QString base = link_[link].url->text().trimmed();
    while (base.endsWith(QLatin1Char('/'))) {
        base.chop(1);
    }
    QUrl url(base);
    url.setPath(QStringLiteral("/") + link_[link].id->text().trimmed());
    return url.toString();
}

/**
 * MainWindow::sendThunk - c_ocpp 发出一帧时的 C 回调
 * @data: 已经编好的 OCPP JSON，不保证以 NUL 结尾
 * @len: data 的字节数
 * @user: 指向 Tx。里面有这一路的 QWebSocket 和分页号
 *
 * 返回 0 表示 QWebSocket 收下了。返回非 0 时 session 记成 OCPP_ERR_IO。
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
    tx->window->logOn(tx->link, QStringLiteral("TX %1").arg(text));
    return 0;
}

/**
 * MainWindow::openLink - 按这一页的子协议发起握手
 * @link: 哪一路。若上次还连着，先 abort，避免两条握手叠在一起
 *
 * Qt 5.14 的 QWebSocket 没有设置子协议的函数，所以写进 Sec-WebSocket-Protocol。
 * 没有这个头时，CSMS 无法知道该用哪一版 OCPP。
 */
void MainWindow::openLink(int link) {
    QWebSocket *sock = tx_[link].socket;
    if (sock->state() != QAbstractSocket::UnconnectedState) {
        sock->abort();
    }
    const QUrl url(chargePointUrl(link));
    QNetworkRequest request(url);
    const QString sub = QString::fromUtf8(kSubprotocol[link]);
    request.setRawHeader("Sec-WebSocket-Protocol", sub.toUtf8());
    logOn(link, QStringLiteral("打开 %1  子协议 %2").arg(request.url().toString(), sub));
    sock->open(request);
}

/**
 * MainWindow::connectLink - 只连接这一页
 * @link: LINK16 / LINK201 / LINK21
 *
 * Boot 要等 connected 信号，不能在 open 之后立刻发。
 * 另外两页保持原样。
 */
void MainWindow::connectLink(int link) {
    bootOk_[link] = false;
    link_[link].status->setText(QStringLiteral("正在连接"));
    openLink(link);
}

/**
 * MainWindow::startConnect - --auto 时把三页都连上
 *
 * 三路桩号可以不同，子协议一定不同，是三条独立的 TCP 连接。
 */
void MainWindow::startConnect() {
    autoOk_ = false;
    for (int i = 0; i < LINK_COUNT; ++i) {
        bootOk_[i] = false;
        connectLink(i);
    }
}

/**
 * MainWindow::disconnectLink - 关掉这一路并停掉它的心跳槽
 * @link: 槽号与 link 相同。0 是 1.6，1 是 2.0.1，2 是 2.1
 *
 * 不停槽的话，下一次 tick 仍会向已经关闭的套接字写心跳。
 */
void MainWindow::disconnectLink(int link) {
    tx_[link].socket->close();
    ocpp_port_timer_stop(link);
    ready_[link] = false;
    link_[link].status->setText(QStringLiteral("已断开"));
    logOn(link, QStringLiteral("已断开"));
}

/**
 * MainWindow::onTick - 把软件定时器向前推 100 ms
 *
 * c_ocpp 的心跳不会自己跑，只有 ocpp_port_timer_tick 会减剩余时间。
 * 这里的 100 必须和构造函数里 QTimer::start(100) 相同。三路共用这一张表。
 */
void MainWindow::onTick() {
    ocpp_port_timer_tick(100);
}

/**
 * MainWindow::sendBoot - 连上后用这一页的厂商和型号发 Boot
 * @link: 哪一路
 *
 * 文本框里的 BootNotification 是给用户改完再点按钮的。
 * 自动这一发走库的 send_boot，Pending 时库会按 interval 重发。
 * 按钮那一发是 session_call，不会登记重试。
 */
void MainWindow::sendBoot(int link) {
    const QByteArray vendor = link_[link].vendor->text().toUtf8();
    const QByteArray model = link_[link].model->text().toUtf8();
    ocpp_err_t rc = OCPP_ERR_ARG;
    if (link == LINK16) {
        ocpp16_boot_notification_req_t boot;
        ocpp16_boot_notification_req_example(&boot);
        ocpp_str_copy(boot.chargePointVendor, sizeof(boot.chargePointVendor), vendor.constData());
        ocpp_str_copy(boot.chargePointModel, sizeof(boot.chargePointModel), model.constData());
        rc = ocpp16_session_send_boot_notification(&sess16_, &boot);
    } else if (link == LINK201) {
        ocpp201_boot_notification_req_t boot;
        ocpp201_boot_notification_req_example(&boot);
        ocpp_str_copy(boot.chargingStation.vendorName, sizeof(boot.chargingStation.vendorName), vendor.constData());
        ocpp_str_copy(boot.chargingStation.model, sizeof(boot.chargingStation.model), model.constData());
        rc = ocpp201_session_send_boot_notification(&sess201_, &boot);
    } else {
        ocpp201_boot_notification_req_t boot;
        ocpp201_boot_notification_req_example(&boot);
        ocpp_str_copy(boot.chargingStation.vendorName, sizeof(boot.chargingStation.vendorName), vendor.constData());
        ocpp_str_copy(boot.chargingStation.model, sizeof(boot.chargingStation.model), model.constData());
        rc = ocpp201_session_send_boot_notification(&sess21_.base, &boot);
    }
    if (rc != OCPP_OK) {
        logOn(link, QStringLiteral("发送 BootNotification 失败 rc=%1").arg(static_cast<int>(rc)));
    }
}

/**
 * MainWindow::sendEditedReq - 把文本框里的 JSON 当作 CALL payload 发出
 * @link: 哪一路已经连上的 session
 * @action: OCPP action 名，与按钮上的字相同
 * @json: 用户改过的 Request 对象文本，不是整帧 [2, uniqueId, ...]
 *
 * 1.6 走 ocpp16_session_call。2.0.1 和 2.1 的 pending 都在 ocpp201_session 上，
 * 所以 2.1 也调用 ocpp201_session_call。2.1 专有 action 的应答由 ocpp21_session_rx 按名字分发。
 */
void MainWindow::sendEditedReq(int link, const QString &action, const QString &json) {
    if (!ready_[link]) {
        logOn(link, QStringLiteral("尚未连接，不能发送 %1").arg(action));
        return;
    }
    const QByteArray act = action.toUtf8();
    const QByteArray body = json.trimmed().toUtf8();
    ocpp_err_t rc = OCPP_ERR_ARG;
    if (link == LINK16) {
        rc = ocpp16_session_call(&sess16_, act.constData(), body.constData());
    } else if (link == LINK201) {
        rc = ocpp201_session_call(&sess201_, act.constData(), body.constData());
    } else {
        rc = ocpp201_session_call(&sess21_.base, act.constData(), body.constData());
    }
    if (rc != OCPP_OK) {
        logOn(link, QStringLiteral("发送 %1 失败 rc=%2").arg(action).arg(static_cast<int>(rc)));
    }
}

/**
 * MainWindow::onConnected - 握手完成后建立 session 并发送 Boot
 * @link: 哪一路刚连上
 *
 * 心跳槽号等于 link。三路共用一张定时器表，槽号重复时后启动的一路会取消前一路。
 * accept_control 为 1，这路接受 Reset 等控制类 CALL。
 */
void MainWindow::onConnected(int link) {
    ocpp_link_t ocppLink;
    ocpp_link_init(&ocppLink, link, &MainWindow::sendThunk, &tx_[link], link, 1);
    if (link == LINK16) {
        ocpp16_handlers_t handlers;
        fill16Handlers(&handlers, this);
        ocpp16_session_init(&sess16_, &handlers, &ocppLink);
    } else if (link == LINK201) {
        ocpp201_handlers_t handlers;
        fill201Handlers(&handlers, this);
        ocpp201_session_init(&sess201_, &handlers, &ocppLink);
    } else {
        ocpp201_handlers_t handlers;
        fill21Handlers(&handlers, this);
        ocpp21_session_init(&sess21_, &handlers, nullptr, &ocppLink);
    }
    ready_[link] = true;
    link_[link].status->setText(QStringLiteral("已连接，发送 Boot"));
    logOn(link, QStringLiteral("WebSocket 已连接"));
    sendBoot(link);
}

/**
 * MainWindow::onDisconnected - 这一路断开
 * @link: 哪一路
 *
 * 停掉对应心跳槽。session 标记为未就绪，避免半包再送进去。
 */
void MainWindow::onDisconnected(int link) {
    ocpp_port_timer_stop(link);
    ready_[link] = false;
    link_[link].status->setText(QStringLiteral("已断开"));
    logOn(link, QStringLiteral("WebSocket 已断开"));
}

/**
 * MainWindow::onText - 把一整帧文本交给这一路的 session
 * @link: 哪一路收到的
 * @message: WebSocket 文本帧。必须是完整的一条 OCPP JSON
 *
 * toUtf8 的结果要活到 session_rx 返回。三路都在 GUI 线程，不会同时改定时器表。
 */
void MainWindow::onText(int link, const QString &message) {
    logOn(link, QStringLiteral("RX %1").arg(message));
    if (!ready_[link]) {
        return;
    }
    const QByteArray bytes = message.toUtf8();
    ocpp_err_t rc = OCPP_ERR_ARG;
    if (link == LINK16) {
        rc = ocpp16_session_rx(&sess16_, bytes.constData(), static_cast<size_t>(bytes.size()));
    } else if (link == LINK201) {
        rc = ocpp201_session_rx(&sess201_, bytes.constData(), static_cast<size_t>(bytes.size()));
    } else {
        rc = ocpp21_session_rx(&sess21_, bytes.constData(), static_cast<size_t>(bytes.size()));
    }
    if (rc != OCPP_OK) {
        logOn(link, QStringLiteral("session_rx 失败 rc=%1").arg(static_cast<int>(rc)));
    }
}

/**
 * MainWindow::onError - 这一路套接字失败
 * @link: 哪一路
 * @error: Qt 的套接字错误码，和 ocpp_err_t 不是同一套
 *
 * --auto 时立刻退出 1。手动运行只改状态标签，留给用户再点连接。
 */
void MainWindow::onError(int link, QAbstractSocket::SocketError error) {
    logOn(link, QStringLiteral("socket 错误 %1 %2").arg(static_cast<int>(error)).arg(tx_[link].socket->errorString()));
    link_[link].status->setText(QStringLiteral("错误"));
    if (autoExit_) {
        QCoreApplication::exit(1);
    }
}

/**
 * MainWindow::noteBoot - 记录这一路 Boot 结果并刷新状态
 * @link: 哪一路
 * @accepted: 仅当 status 字符串是 Accepted 时为真
 *
 * Pending 表示桩已登记但平台还没批准，界面显示「Boot 未接受」，--auto 不会因此退出 0。
 */
void MainWindow::noteBoot(int link, bool accepted) {
    bootOk_[link] = accepted;
    link_[link].status->setText(accepted ? QStringLiteral("Boot Accepted") : QStringLiteral("Boot 未接受"));
    finishAutoIfReady();
}

/**
 * MainWindow::finishAutoIfReady - --auto 只在三路都 Accepted 时退出 0
 *
 * 手动点某一页的连接不会把 autoExit_ 设上，这里直接返回。
 */
void MainWindow::finishAutoIfReady() {
    if (!autoExit_ || autoOk_) {
        return;
    }
    for (int i = 0; i < LINK_COUNT; ++i) {
        if (!bootOk_[i]) {
            return;
        }
    }
    autoOk_ = true;
    logOn(LINK16, QStringLiteral("三路 BootNotification 已接受，自动退出"));
    QTimer::singleShot(0, qApp, []() { QCoreApplication::exit(0); });
}
