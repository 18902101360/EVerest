/*
 * 逐个点击三个分页上的「连接」和每一条桩侧 Request 按钮。
 * 用的是 QTest 的鼠标左键，事件进到按钮自己的 clicked，和人点是同一条槽。
 */
#ifndef QT_OCPP_CLICK_ALL_H
#define QT_OCPP_CLICK_ALL_H

class MainWindow;

/**
 * clickAllRequests - 三个协议分页各点一遍
 * @window: 已经 show 出来的主窗口
 *
 * 先点该页的「连接」，等到 Boot Accepted，再从上到下点每个请求按钮。
 * 每个按钮都要在该页日志里看到 TX（含 action 名）以及随后的 RX。
 * 返回 0 表示全部点过且都有应答。失败返回 1。
 */
int clickAllRequests(MainWindow *window);

#endif
