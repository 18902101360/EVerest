/*
 * 充电桩窗口。三个分页各自一条 QWebSocket，对一个 c_ocpp session。
 * 套接字和软件定时器表都不是线程安全的，收发留在 GUI 线程。
 */
#ifndef QT_OCPP_MAINWINDOW_H
#define QT_OCPP_MAINWINDOW_H

#include "ocpp16.h"
#include "ocpp201.h"
#include "ocpp21.h"

#include <QMainWindow>
#include <QWebSocket>

class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QTimer;

/**
 * MainWindow - OCPP 1.6、2.0.1、2.1 各一个分页
 *
 * 每一页自己填地址和桩号、自己连接、自己发桩侧 Request。
 * 桩号是 URL 路径，不是 HTTP Basic 用户。
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    /** 0=1.6，1=2.0.1，2=2.1。心跳槽号与这个下标相同，三路不能共用一个槽。 */
    enum LinkId { LINK16 = 0, LINK201 = 1, LINK21 = 2, LINK_COUNT = 3 };

    /** 建三个分页和三路套接字，初始化 c_ocpp。先不连接。 */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * applySettings - 用命令行覆盖三个分页的地址、桩号、厂商和型号
     * 空字符串表示保持该分页当前值。非空时三页写成同一个值。
     */
    void applySettings(const QString &url, const QString &cpId, const QString &vendor, const QString &model);
    /** --auto 时为真。三路 Boot 都 Accepted 才退出 0。 */
    void setAutoExit(bool enabled);
    /** --auto 已经因为 Accepted 安排退出时为真。 */
    bool autoSucceeded() const { return autoOk_; }
    /**
     * logOn - 往指定分页的日志追加一行
     * @link: LINK16 / LINK201 / LINK21
     * @line: 已经格式化好的文本，不要带时间戳
     *
     * 不能从这里再调用 session。C 回调和套接字槽都走这个函数。
     */
    void logOn(int link, const QString &line);
    /** 记录这一路 Boot 是否 Accepted。Pending 不算通过。 */
    void noteBoot(int link, bool accepted);

public slots:
    /** --auto 用。依次连接三个分页。 */
    void startConnect();

private slots:
    /** 每 100 ms 调用一次 ocpp_port_timer_tick(100)。 */
    void onTick();

private:
    /**
     * LinkUi - 一个分页上和连接有关的控件
     *
     * url 是 CSMS 基地址，不含桩号。id 会变成 URL 路径。
     * log 只属于这一页，另外两页的收发不会写进来。
     */
    struct LinkUi {
        QLineEdit *url;
        QLineEdit *id;
        QLineEdit *vendor;
        QLineEdit *model;
        QLabel *status;
        QPlainTextEdit *log;
    };

    /**
     * Tx - ocpp_link_t.send 的 user 指针
     *
     * 链接里只能存一个 void *。套接字、窗口和这一页的日志都要，所以包在一起。
     * 对象必须活过 session，心跳定时器里也会调用 send。
     */
    struct Tx {
        QWebSocket *socket;
        MainWindow *window;
        int link;
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

    /** 做出一个分页：连接栏、请求按钮和底部日志。 */
    QWidget *makePage(int link, const QString &defaultId, const QString &hint);
    /** 在请求区追加一组按钮。caption 为空时不加小标题。 */
    void addReqRows(int link, class QVBoxLayout *rows, const struct CpReqExample *specs, int count, const QString &caption);
    /** 按这一页的地址和桩号拼出 ws://主机:端口/桩号。 */
    QString chargePointUrl(int link) const;
    /** 用 Sec-WebSocket-Protocol 打开这一路。子协议必须和 session 版本一致。 */
    void openLink(int link);
    /** 这一页的连接按钮。 */
    void connectLink(int link);
    /** 关掉这一路并停掉它的心跳槽。 */
    void disconnectLink(int link);
    /** 握手成功后建立 session 并发送 Boot。 */
    void onConnected(int link);
    /** 断开后停心跳槽，不再把后续帧送进 session。 */
    void onDisconnected(int link);
    /** 一整帧文本交给对应 session。 */
    void onText(int link, const QString &message);
    /** 套接字错误。--auto 时退出码 1。 */
    void onError(int link, QAbstractSocket::SocketError error);
    /** 把文本框里的 JSON 当作该 action 的 CALL payload 发出去。 */
    void sendEditedReq(int link, const QString &action, const QString &json);
    /** 连上后用这一页的厂商和型号发 Boot。文本框里的 Boot 留给用户再点按钮。 */
    void sendBoot(int link);
    /** --auto 下三路 Boot 都 Accepted 时退出 0。 */
    void finishAutoIfReady();

    LinkUi link_[LINK_COUNT];
    QTimer *tick_;

    QWebSocket sock16_;
    QWebSocket sock201_;
    QWebSocket sock21_;
    Tx tx_[LINK_COUNT];
    ocpp16_session_t sess16_;
    ocpp201_session_t sess201_;
    ocpp21_session_t sess21_;
    bool ready_[LINK_COUNT];
    bool bootOk_[LINK_COUNT];
    bool autoExit_;
    bool autoOk_;
};

#endif
