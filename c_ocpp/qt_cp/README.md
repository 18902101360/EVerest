# Qt 5.14 充电桩（c_ocpp 1.6 / 2.0.1）

这个程序把单片机用的 `c_ocpp` 接到 **Qt 5.14 `QWebSocket`** 上。协议库仍然不包含 TCP。两条连接各自一个 session：

| 连接 | 子协议 | 库 |
| --- | --- | --- |
| WebSocket 1 | `ocpp1.6` | `ocpp16_session_t` |
| WebSocket 2 | `ocpp2.0.1` | `ocpp201_session_t` |

仓库里的 C 库是 **OCPP 1.6 和 2.0.1**，没有 OCPP 1.2。界面和子协议按 1.6 / 2.0.1 实现。

连上之后自动发 `BootNotification`。CSMS 的 CALL 里，`Reset` 由窗口回调接受；其它 CSMS 报文用库里的 `*_example()` 默认 Confirmation。心跳由 `QTimer` 每 100 ms 调用 `ocpp_port_timer_tick(100)` 推动。cJSON arena 只有一份，收发都在 GUI 线程。

注释按仓库里的 `.cursor/skills/linux-kernel-style/SKILL.md` 写：只说明为什么，块注释用 `/* */`，函数用 kernel-doc。该 skill 来自 [luban-skill 的 linux-kernel.skill](https://github.com/happydog-intj/luban-skill/tree/master/skills/linux-kernel.skill)。

## Qt Creator 5.14

1. 用 Qt 5.14 kit 打开 `c_ocpp/qt_cp/qt_ocpp_cp.pro`。
2. 影子构建，不要把产物写进 `ocpp16/messages/`。
3. 运行。默认连 `ws://cpmstest.timxon.com:8887/CP001`，BootNotification 里的厂商/用户名是 `zephyr`。

## 命令行（CMake，Qt 5.14）

```bash
export CMAKE_PREFIX_PATH=/path/to/Qt/5.14.2/gcc_64
cmake -S c_ocpp/qt_cp -B c_ocpp/qt_cp/build
cmake --build c_ocpp/qt_cp/build -j$(nproc)
./c_ocpp/qt_cp/build/qt_ocpp_cp
```

无界面自检（先另开终端起 CSMS）：

```bash
cd c_ocpp/csms
PYTHONPATH=. python3 -m ocpp_csms --host 127.0.0.1 --port 9000 --interval 3600
```

```bash
./c_ocpp/qt_cp/build/qt_ocpp_cp --auto --url ws://cpmstest.timxon.com:8887 --id CP001 --vendor zephyr
```

两条连接的 `BootNotification.conf` 都是 `Accepted` 时进程退出码 0。20 秒内没有完成则退出码 2。

参数：`--url`、`--id`、`--vendor`、`--model`、`--auto`。`--help` 可看说明。

`ws://` 不需要 TLS。Qt 5.14 官方包按 OpenSSL 1.1 编译，在较新的系统上走 `wss://` 可能提示 `QSslSocket: cannot resolve ...`，需要本机有与该 Qt 匹配的 OpenSSL 1.1。
