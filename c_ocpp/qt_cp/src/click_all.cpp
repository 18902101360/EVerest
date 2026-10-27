#include "click_all.h"

#include "MainWindow.h"

#include <QElapsedTimer>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QTabWidget>
#include <QTest>

#include <functional>

/**
 * pageLog - 取出这一页底部的日志框
 * @page: 一个协议分页
 *
 * 请求文本框是 QLineEdit，日志才是 QPlainTextEdit，所以这里只会找到日志。
 */
static QPlainTextEdit *pageLog(QWidget *page) {
    return page->findChild<QPlainTextEdit *>(QStringLiteral("linkLog"));
}

/**
 * pageStatus - 取出这一页的连接状态标签
 * @page: 一个协议分页
 */
static QLabel *pageStatus(QWidget *page) {
    return page->findChild<QLabel *>(QStringLiteral("linkStatus"));
}

/**
 * waitUntil - 在超时前反复跑事件循环，直到 pred 为真
 * @timeoutMs: 最长等待，毫秒
 * @pred: 返回真就停
 *
 * 套接字的 connected 和 CSMS 的 CALLRESULT 都要靠事件循环送进来。
 * 返回真表示等到了。超时返回假。
 */
static bool waitUntil(int timeoutMs, const std::function<bool()> &pred) {
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < timeoutMs) {
        if (pred()) {
            return true;
        }
        QTest::qWait(30);
    }
    return false;
}

/**
 * clickButton - 把按钮滚进视野再点左键
 * @page: 按钮所在的分页，用来找滚动区
 * @button: 要点的按钮
 *
 * ensureWidgetVisible 让长列表里靠下的按钮也能被点到。
 */
static void clickButton(QWidget *page, QPushButton *button) {
    if (QScrollArea *scroll = page->findChild<QScrollArea *>()) {
        scroll->ensureWidgetVisible(button);
    }
    QTest::mouseClick(button, Qt::LeftButton);
}

/**
 * waitBoot - 等这一页的 Boot 被 CSMS 接受
 * @page: 刚点过「连接」的分页
 *
 * 状态标签变成 Boot Accepted 才算连上。错误或超时都返回假。
 */
static bool waitBoot(QWidget *page) {
    QLabel *status = pageStatus(page);
    return waitUntil(8000, [status]() {
        const QString text = status->text();
        return text == QStringLiteral("Boot Accepted") || text == QStringLiteral("错误") ||
               text == QStringLiteral("Boot 未接受");
    }) && status->text() == QStringLiteral("Boot Accepted");
}

/**
 * waitReqReply - 等这次点击对应的 TX 和后面的 RX
 * @log: 这一页的日志
 * @action: 按钮上的 OCPP action 名
 * @logBefore: 点击前的日志全文，用来只看新增的行
 *
 * TX 行里带着 action，说明按钮把这条 CALL 发出去了。
 * 紧接着的 RX 是 CSMS 的 CALLRESULT。发送失败的行也算没通过。
 */
static bool waitReqReply(QPlainTextEdit *log, const QString &action, const QString &logBefore) {
    return waitUntil(8000, [log, action, logBefore]() {
        const QString all = log->toPlainText();
        const QString added = all.mid(logBefore.size());
        if (added.contains(QStringLiteral("发送 %1 失败").arg(action)) || added.contains(QStringLiteral("尚未连接"))) {
            return true;
        }
        const int tx = added.indexOf(QStringLiteral("TX"));
        if (tx < 0 || !added.mid(tx).contains(action)) {
            return false;
        }
        return added.mid(tx).contains(QStringLiteral("RX"));
    }) && !log->toPlainText().mid(logBefore.size()).contains(QStringLiteral("失败"));
}

/**
 * clickAllRequests - 三个协议分页各点一遍
 * @window: 已经 show 出来的主窗口
 *
 * 返回 0 表示每个协议的连接按钮和全部桩侧请求按钮都点过，并且日志里有应答。
 */
int clickAllRequests(MainWindow *window) {
    QTabWidget *tabs = window->findChild<QTabWidget *>();
    if (tabs == nullptr || tabs->count() != 3) {
        qWarning("找不到三个协议分页");
        return 1;
    }
    int failed = 0;
    for (int i = 0; i < tabs->count(); ++i) {
        tabs->setCurrentIndex(i);
        QWidget *page = tabs->widget(i);
        const QString title = tabs->tabText(i);
        QPushButton *connectBtn = page->findChild<QPushButton *>(QStringLiteral("connectButton"));
        QPlainTextEdit *log = pageLog(page);
        if (connectBtn == nullptr || log == nullptr) {
            qWarning("%s 缺少连接按钮或日志", qUtf8Printable(title));
            return 1;
        }
        clickButton(page, connectBtn);
        if (!waitBoot(page)) {
            qWarning("%s 连接后没有 Boot Accepted\n%s", qUtf8Printable(title), qUtf8Printable(log->toPlainText()));
            ++failed;
            continue;
        }
        const auto buttons = page->findChildren<QPushButton *>(QStringLiteral("reqButton"));
        qInfo("%s 连接成功，开始点 %d 个请求按钮", qUtf8Printable(title), buttons.size());
        for (QPushButton *btn : buttons) {
            const QString action = btn->text();
            const QString before = log->toPlainText();
            clickButton(page, btn);
            if (!waitReqReply(log, action, before)) {
                qWarning("%s 按钮 %s 没有看到 TX/RX", qUtf8Printable(title), qUtf8Printable(action));
                ++failed;
            } else {
                qInfo("OK %s %s", qUtf8Printable(title), qUtf8Printable(action));
            }
        }
    }
    if (failed != 0) {
        qWarning("点击测试失败 %d 项", failed);
        return 1;
    }
    qInfo("三个协议的请求按钮全部点过");
    return 0;
}
