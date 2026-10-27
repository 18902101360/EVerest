# Qt 5.14 充电桩（c_ocpp 1.6 / 2.0.1 / 2.1）

这个程序把 `c_ocpp` 接到 **Qt 5.14 `QWebSocket`** 上。三个分页各自一条连接，互不影响：

| 分页 | 子协议 | 库 |
| --- | --- | --- |
| OCPP 1.6 | `ocpp1.6` | `ocpp16_session_t` |
| OCPP 2.0.1 | `ocpp2.0.1` | `ocpp201_session_t` |
| OCPP 2.1 | `ocpp2.1` | `ocpp21_session_t`（内嵌 2.0.1 会话） |

每一页自己填地址和桩号，自己点连接和断开。中间是桩侧 Request：按钮上是 action 名，旁边的文本框是示例 JSON，可以改完再发。页底是这一页自己的日志。

连上之后自动发一次 `BootNotification`，厂商和型号用这一页顶部的输入框。文本框里的 Boot 留给用户再点按钮。CSMS 的 `Reset` 由窗口回调接受；其它平台报文用库里的 `*_example()` 默认 Confirmation。心跳由 `QTimer` 每 100 ms 调用 `ocpp_port_timer_tick(100)`。三路心跳槽分别是 0、1、2。

## Qt Creator 5.14

1. 用 Qt 5.14 kit 打开 `c_ocpp/qt_cp/qt_ocpp_cp.pro`。
2. 影子构建，不要把产物写进 `ocpp16/messages/`。
3. 运行。默认地址 `ws://cpmstest.timxon.com:8887`，三页桩号分别是 `CP001`、`CP201`、`CP21`，厂商是 `zephyr`。

## 命令行（CMake，Qt 5.14）

```bash
export CMAKE_PREFIX_PATH=/path/to/Qt/5.14.2/gcc_64
cmake -S c_ocpp/qt_cp -B c_ocpp/qt_cp/build
cmake --build c_ocpp/qt_cp/build -j$(nproc)
./c_ocpp/qt_cp/build/qt_ocpp_cp
```

无界面自检（CSMS 要同时接受三个子协议）：

```bash
cd c_ocpp/csms
PYTHONPATH=. python3 -m ocpp_csms --host 127.0.0.1 --port 9000 --interval 3600
```

```bash
./c_ocpp/qt_cp/build/qt_ocpp_cp --auto --url ws://127.0.0.1:9000 --vendor zephyr
```

三页的 `BootNotification.conf` 都是 `Accepted` 时进程退出码 0。20 秒内没有完成则退出码 2。

参数：`--url`、`--id`、`--vendor`、`--model`、`--auto`。没写 `--id` 时三页保持各自的默认桩号。`--help` 可看说明。

`ws://` 不需要 TLS。Qt 5.14 官方包按 OpenSSL 1.1 编译，在较新的系统上走 `wss://` 可能提示 `QSslSocket: cannot resolve ...`，需要本机有与该 Qt 匹配的 OpenSSL 1.1。
