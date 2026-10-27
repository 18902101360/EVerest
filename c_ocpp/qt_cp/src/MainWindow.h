/*
 * 充电桩窗口。一条 QWebSocket 对一个 c_ocpp session。
 * 套接字和软件定时器表都不是线程安全的，收发留在 GUI 线程。
 */
#ifndef QT_OCPP_MAINWINDOW_H
#define QT_OCPP_MAINWINDOW_H

#include "ocpp16.h"
#include "ocpp201.h"

#include <QMainWindow>
#include <QWebSocket>

class QCheckBox;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QTimer;

/**
 * MainWindow - OCPP 1.6 与 2.0.1 各一条 WebSocket
 *
 * 桩号是 URL 路径，不是 HTTP Basic 用户。这个测试平台给未登记的路径回 404。
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    /** 建界面和两路套接字，初始化 c_ocpp 的 malloc 钩子和定时器表。先不连接。 */
    explicit MainWindow(QWidget *parent = nullptr);

    /** 用命令行覆盖地址、桩号、厂商和型号。空字符串表示保持界面当前值。 */
    void applySettings(const QString &url, const QString &cpId, const QString &vendor, const QString &model);
    /** --auto 时为真。两路需要的 Boot 都 Accepted 才退出 0。 */
    void setAutoExit(bool enabled);
    /** --auto 已经因为 Accepted 安排退出时为真。 */
    bool autoSucceeded() const { return autoOk_; }
    /** 追加一行日志。内部会加时间戳，不要从这里再调用 session。 */
    void logLine(const QString &line);
    /**
     * noteBoot16 - 记录 1.6 Boot 是否被接受
     * @accepted: status 为 Accepted 时为真。Pending 也是合法应答，但不能当作已批准。
     */
    void noteBoot16(bool accepted);
    /** 同 noteBoot16，对应 2.0.1。 */
    void noteBoot201(bool accepted);

public slots:
    /** 按勾选打开 1.6 和/或 2.0.1。Boot 在 connected 之后发。 */
    void startConnect();

private slots:
    /** 关闭两路套接字，并停止心跳槽 0 和 1。 */
    void onDisconnect();
    /** 给已经建立 session 的每一路发 StatusNotification。 */
    void onSendStatus();
    /** 每 100 ms 调用一次 ocpp_port_timer_tick(100)。 */
    void onTick();
    /** 1.6 握手成功：绑定槽 0，发送 Boot。 */
    void on16Connected();
    /** 1.6 断开：停槽 0，不再把后续帧送进 session。 */
    void on16Disconnected();
    /** 1.6 收到一整帧文本，交给 ocpp16_session_rx。 */
    void on16Text(const QString &message);
    /** 1.6 套接字错误。--auto 时退出码 1。 */
    void on16Error(QAbstractSocket::SocketError error);
    /** 2.0.1 握手成功：绑定槽 1，发送 Boot。 */
    void on201Connected();
    /** 2.0.1 断开：停槽 1。 */
    void on201Disconnected();
    /** 2.0.1 收到一整帧文本，交给 ocpp201_session_rx。 */
    void on201Text(const QString &message);
    /** 2.0.1 套接字错误。--auto 时退出码 1。 */
    void on201Error(QAbstractSocket::SocketError error);

private:
    /**
     * Tx - ocpp_link_t.send 的 user 指针
     *
     * 链接里只能存一个 void *。套接字和窗口都要，所以包在一起。
     * 对象必须活过 session，send 回调会在心跳定时器里被调用。
     */
    struct Tx {
        QWebSocket *socket;
        MainWindow *window;
        const char *name;
    };

    /**
     * sendThunk - c_ocpp 的发送回调
     * @data: 一帧已经编好的 OCPP JSON
     * @len: 字节数，不含额外的结尾 NUL
     * @user: Tx *
     *
     * 返回 0 表示交给了 QWebSocket。非 0 会被 session 记成 OCPP_ERR_IO。
     * 必须是不捕获的函数：ocpp_port_send_fn 是 C 函数指针。
     */
    static int sendThunk(const void *data, size_t len, void *user);

    /** 用 Sec-WebSocket-Protocol 打开一路。Qt 5.14 没有别的子协议接口。 */
    void openLink(QWebSocket &sock, const QString &subprotocol);
    /** 发送 1.6 Boot。厂商和型号来自界面。 */
    void sendBoot16();
    /** 发送 2.0.1 Boot。厂商和型号来自界面。 */
    void sendBoot201();
    /** session 已建立时发送 1.6 StatusNotification。 */
    void sendStatus16();
    /** session 已建立时发送 2.0.1 StatusNotification。 */
    void sendStatus201();
    /** --auto 下两路该等的 Boot 都 Accepted 时退出 0。Pending 不算。 */
    void finishAutoIfReady();
    /** 把基地址和桩号拼成 ws://主机:端口/桩号。 */
    QString chargePointUrl() const;
    /** 更新某一路的状态标签。 */
    void setLinkStatus(QLabel *label, const QString &text);

    QLineEdit *urlEdit_;
    QLineEdit *idEdit_;
    QLineEdit *vendorEdit_;
    QLineEdit *modelEdit_;
    QCheckBox *use16_;
    QCheckBox *use201_;
    QLabel *status16_;
    QLabel *status201_;
    QPlainTextEdit *log_;
    QTimer *tick_;

    QWebSocket sock16_;
    QWebSocket sock201_;
    Tx tx16_;
    Tx tx201_;
    ocpp16_session_t sess16_;
    ocpp201_session_t sess201_;
    bool sess16Ready_;
    bool sess201Ready_;
    bool boot16Ok_;
    bool boot201Ok_;
    bool autoExit_;
    bool autoOk_;
};

#endif
