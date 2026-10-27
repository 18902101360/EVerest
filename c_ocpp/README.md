# c_ocpp — 充电桩侧 OCPP 1.6 / 2.0.1 / 2.1 C 库（面向单片机）

本目录是充电桩（Charge Point）侧协议栈。它负责：

- 把结构体编成 OCPP JSON CALL / CALLRESULT / CALLERROR
- 解析对端发来的一帧 JSON
- 维护 **每条** 到 CSMS 的会话：uniqueId、pending、心跳、`accept_control`

它 **不包含** TCP、TLS、WebSocket、HTTP。链路上的收发由你的固件（或主机测试里的 `tests/ws_client.c`）完成。

仓库根目录还有上游 C++ 项目 `libocpp`。**本文件只讲 `c_ocpp/`。** 不要把根目录 CMake 当成 MCU 库的构建入口。

| 你要做的事 | 看哪一节 |
| --- | --- |
| 第一次把代码拉下来并在 Linux 上编过、测过 | [从零：下载、编译、测试](#从零下载编译测试) |
| 一键脚本到底跑了什么、会不会起 Python 服务器 | [一键脚本会做什么](#一键脚本会做什么) |
| 六条 `ctest` 各自测什么、联调覆盖哪些报文 | [六条测试分别做什么](#六条测试分别做什么) |
| 自己起 CSMS，用 C 客户端连 | [手动起 Python CSMS](#手动起-python-csms) |
| 多运营商 / 多条 WebSocket | [分层与多路连接](#分层与多路连接) |
| 编进 Keil / 交叉编译 / 自己写 `port.c` | [移植与编译框架](#移植与编译框架) |
| 用 Qt 5.14 WebSocket 把本库跑成充电桩 | [Qt 5.14 充电桩](#qt-514-充电桩)（`c_ocpp/qt_cp/`） |

---

## 从零：下载、编译、测试

下面假设你用 **Linux**（Debian / Ubuntu 或同类发行版）。Windows 可用 WSL2。macOS 把 `apt` 换成 Homebrew 的 `cmake python3`，其余命令相同。

所有带路径的命令默认在 **仓库根目录** 执行（能看到 `c_ocpp/` 文件夹）。不要在 `c_ocpp/` 里面再写 `c_ocpp/...`。

### 1. 装工具

| 工具 | 用途 | 建议版本 |
| --- | --- | --- |
| `git` | 拉代码 | 任意近期版本 |
| C 编译器 | 编 `c_ocpp` | gcc 或 clang，**C11** |
| `cmake` | 生成 Makefile / Ninja | **≥ 3.14** |
| `make` 或 `ninja` | 真正编译 | CMake 默认一般是 Make |
| `python3` | 单元测试 + 联调用 CSMS | **3.8+**；**不需要 pip**（只用标准库） |
| `cc` | 脚本用来探测编译器是否存在 | 通常随 `build-essential` 安装 |

Debian / Ubuntu：

```bash
sudo apt update
sudo apt install -y git build-essential cmake python3
```

装完自检：

```bash
git --version
cc --version
cmake --version    # 应 ≥ 3.14
python3 --version  # 应 ≥ 3.8
```

`Could not find Python3` 时，配置阶段加上 `-DPython3_EXECUTABLE=/usr/bin/python3`（见下文）。

### 2. 下载源码

MCU 库在分支 **`cursor/c-ocpp-mcu-76bd`** 上。`main` 是上游 `libocpp`，**没有** 这套 `c_ocpp` 主机测试脚本。

**方式 A — git clone（推荐）**

```bash
git clone -b cursor/c-ocpp-mcu-76bd https://github.com/18902101360/libocpp.git
cd libocpp
```

确认目录里有这些文件：

```bash
ls c_ocpp/CMakeLists.txt c_ocpp/build_and_test.sh c_ocpp/csms/run_live_test.py
git branch --show-current    # 应为 cursor/c-ocpp-mcu-76bd
```

已经 clone 过整个仓库、但当前在 `main` 上：

```bash
cd libocpp
git fetch origin cursor/c-ocpp-mcu-76bd
git checkout cursor/c-ocpp-mcu-76bd
```

**方式 B — GitHub ZIP**

打开该分支页面 → Code → Download ZIP → 解压。ZIP 没有 `.git`，不影响编译；以后要更新请重新下或改用 git。

### 3. 一键编 + 测（推荐）

脚本在仓库根或 `c_ocpp/` 下都能跑（它按脚本自己的路径定位源码）：

```bash
chmod +x c_ocpp/build_and_test.sh   # 克隆后只需一次；git 已标可执行则可省略
./c_ocpp/build_and_test.sh
```

成功结束时终端类似：

```text
==> 配置  cmake -S .../c_ocpp -B .../c_ocpp/build
==> 编译  cmake --build ... -jN
==> 测试  ctest --test-dir ... --output-on-failure
...
100% tests passed, 0 tests failed out of 6
==> 完成
```

`6` 条里最后一条会 **启动 Python OCPP 服务器**，再用 C 库经 WebSocket **测完 1.6、2.0.1 和 2.1 的每一条 Request / Confirmation**。细节见 [一键脚本会做什么](#一键脚本会做什么)。

常用参数：

```bash
./c_ocpp/build_and_test.sh --help
./c_ocpp/build_and_test.sh --clean                 # 删掉构建目录后重配
./c_ocpp/build_and_test.sh -j 8                    # 并行编译数
./c_ocpp/build_and_test.sh --build-dir /tmp/c_ocpp_build
./c_ocpp/build_and_test.sh -- -G Ninja             # -- 后面交给 cmake 配置
./c_ocpp/build_and_test.sh -- -DPython3_EXECUTABLE=/usr/bin/python3
./c_ocpp/build_and_test.sh -- -DCMAKE_C_COMPILER=gcc
```

环境变量 `BUILD_DIR` 也可指定构建目录（默认 `c_ocpp/build`）。脚本会检查 `cc`、`cmake`、`ctest`、`python3`，缺哪个就退出并提示 `apt` 包名。

### 4. 不用脚本时：逐步 cmake

`c_ocpp/CMakeLists.txt` **只编 Linux 主机**：三份静态库 + 测试程序。单片机工程不要用这套 CMake 去链 `test_*`，固件编法见 [移植与编译框架](#移植与编译框架)。

**配置**（生成 `c_ocpp/build/`，不要把该目录提交进 git）：

```bash
cmake -S c_ocpp -B c_ocpp/build
```

可选：

```bash
cmake -S c_ocpp -B c_ocpp/build -DCMAKE_C_COMPILER=gcc
cmake -S c_ocpp -B c_ocpp/build -G Ninja
cmake -S c_ocpp -B c_ocpp/build -DPython3_EXECUTABLE=/usr/bin/python3
```

`messages/*.c` 用了 `file(GLOB …)`。你 **增删** 了某个报文 `.c` 之后，必须 **再跑一遍这条 cmake**，然后才 `--build`，否则新文件进不了 `.a`。

**编译：**

```bash
cmake --build c_ocpp/build
cmake --build c_ocpp/build -j$(nproc)    # 多核
```

只编某一个目标：

```bash
cmake --build c_ocpp/build --target test_ocpp16
cmake --build c_ocpp/build --target test_ocpp_csms
```

产物都在 `c_ocpp/build/`：

| 文件 | 内容 |
| --- | --- |
| `libc_ocpp_core.a` | cJSON + `ocpp_json.c` + `ocpp_rpc.c` + **主机** `port/ocpp_port.c` |
| `libc_ocpp16.a` | 1.6 session + `ocpp16/messages/*.c`，PUBLIC 链 core |
| `libc_ocpp201.a` | 2.0.1 同上 |
| `test_ocpp16` / `test_ocpp201` / `test_ocpp_both` | 无 socket 的编解码与 session |
| `test_ocpp_csms` | C 客户端（`tests/test_ocpp_csms.c` + `tests/ws_client.c`） |

`c_ocpp_core` 只编一次，避免 1.6 和 2.0.1 各链一份 cJSON / `ocpp_port` 造成符号重复。主机 `ocpp_port.c` 用了 `clock_gettime`，**不能**原样链进裸机。

**跑测试：**

```bash
ctest --test-dir c_ocpp/build --output-on-failure
```

应 `100% tests passed, 0 tests failed out of 6`。

只跑一类，或直接跑无网络二进制：

```bash
ctest --test-dir c_ocpp/build -R test_ocpp16 --output-on-failure
./c_ocpp/build/test_ocpp16
./c_ocpp/build/test_ocpp201
./c_ocpp/build/test_ocpp_both
```

**不要**只启动 `./c_ocpp/build/test_ocpp_csms` 就指望联调成功：它需要已经在听的 CSMS。CTest 会执行：

```text
python3 c_ocpp/csms/run_live_test.py <build>/test_ocpp_csms
```

`run_live_test.py` 会：占一个空闲回环端口 → `python3 -m ocpp_csms --host 127.0.0.1 --port <port> --interval 3600 --quiet` → 等端口就绪 → 跑 C 客户端 `test_ocpp_csms 127.0.0.1 <port>`（超时 90 秒）→ 结束时杀掉服务器。

干净重编：

```bash
rm -rf c_ocpp/build
cmake -S c_ocpp -B c_ocpp/build
cmake --build c_ocpp/build -j$(nproc)
ctest --test-dir c_ocpp/build --output-on-failure
```

---

## 一键脚本会做什么

`c_ocpp/build_and_test.sh` 按顺序：

1. 检查 `cmake` / `ctest` / `python3` / `cc`
2. 可选 `--clean` 删除构建目录
3. `cmake -S <c_ocpp> -B <build>`
4. `cmake --build <build> -j<jobs>`
5. `ctest --test-dir <build> --output-on-failure`（**全部 6 项**）

第 5 步里的 `test_ocpp_csms` **会**启动 Python 服务器，并对每个 Request / Confirmation 走 WebSocket。协议库本身仍然没有 TCP；网络只存在于：

- `c_ocpp/csms/`（Python CSMS）
- `c_ocpp/tests/ws_client.c`（仅主机测试）

固件里你自己提供 WebSocket text 帧的收发。

---

## 六条测试分别做什么

| CTest 名称 | 程序 / 命令 | 网络 | 做什么 |
| --- | --- | --- | --- |
| `test_ocpp16` | `./c_ocpp/build/test_ocpp16` | 无 | 1.6 全部报文编解码 + Boot / Reset / 三路 session |
| `test_ocpp201` | `./c_ocpp/build/test_ocpp201` | 无 | 2.0.1 同样 |
| `test_ocpp21` | `./c_ocpp/build/test_ocpp21` | 无 | 2.1 新增报文编解码 + SetDERControl / Boot |
| `test_ocpp_both` | `./c_ocpp/build/test_ocpp_both` | 无 | **同一进程**同时链接 1.6、2.0.1 与 2.1 |
| `test_ocpp_csms_py` | `PYTHONPATH=c_ocpp/csms python3 -m unittest discover -s tests -v` | 无 | Python CSMS 的 rpc / handler 单测 |
| `test_ocpp_csms` | `run_live_test.py` + C 客户端 | **有**（本机回环 WebSocket） | 拉起 CSMS，覆盖 1.6 / 2.0.1 / 2.1 的 Request 与 Confirmation |

### 联调覆盖的报文数量

C 客户端 `tests/test_ocpp_csms.c` 期望：

| 版本 | 桩发出的 CALL（收到 Confirmation） | CSMS 发出的 CALL（桩回 Confirmation） |
| --- | --- | --- |
| OCPP 1.6（含 security whitepaper） | **14** | **26** |
| OCPP 2.0.1 | **25** | **40** |
| OCPP 2.1（含全部 2.0.1 action，再加 2.1 才有的） | **37** | **55** |

成功时打印：

```text
c_ocpp live CSMS: all req/conf OCPP 1.6 (14+26), 2.0.1 (25+40) and 2.1 (37+55) OK
```

2.1 的 37 是 2.0.1 的 25 条桩侧 CALL 加上 12 条 2.1 新增桩侧 CALL。55 是 2.0.1 的 40 条平台侧 CALL 加上 15 条 2.1 新增平台侧 CALL。子协议是 `ocpp2.1`。2.0.1 已有的 action 仍由内嵌的 `ocpp201_session` 处理。

流程（每个版本各连一次，子协议分别为 `ocpp1.6` / `ocpp2.0.1` / `ocpp2.1`）：

1. 桩发 `BootNotification`，CSMS 回 Accepted。
2. 桩把 **本版本 FROM_CP 列表**里每一条 CALL 发出去（含 Boot 已发过的），等 Confirmation。
3. 桩再发 `DataTransfer`，`vendorId=PROBE_CSMS`。
4. CSMS 按 `c_ocpp/csms/ocpp_csms/catalog.py` 把 **FROM_CSMS 列表**里每一条 CALL 发给桩；桩用 session 编 Confirmation。
5. CSMS 最后发 `DataTransfer`，`vendorId=ALL_CSMS_DONE`，C 客户端核对计数后退出。

1.6 桩侧 14 条：Authorize, BootNotification, DataTransfer, DiagnosticsStatusNotification, FirmwareStatusNotification, Heartbeat, MeterValues, StartTransaction, StatusNotification, StopTransaction, LogStatusNotification, SecurityEventNotification, SignCertificate, SignedFirmwareStatusNotification。

1.6 CSMS 侧 26 条：CancelReservation, ChangeAvailability, ChangeConfiguration, ClearCache, ClearChargingProfile, GetCompositeSchedule, GetConfiguration, GetDiagnostics, GetLocalListVersion, RemoteStartTransaction, RemoteStopTransaction, ReserveNow, Reset, SendLocalList, SetChargingProfile, TriggerMessage, UnlockConnector, UpdateFirmware, CertificateSigned, DeleteCertificate, ExtendedTriggerMessage, GetInstalledCertificateIds, GetLog, InstallCertificate, SignedUpdateFirmware, DataTransfer。

2.0.1 列表见 `catalog.py` 的 `FROM_CP_201` / `FROM_CSMS_201`。示例 JSON 与 C 侧 `*_example()` 编码器对齐。

`--no-probe` 会关掉第 4 步（仅开发服务器时用；一键测试 **不会** 加这个参数）。

---

## 手动起 Python CSMS

`c_ocpp/csms` 是模块化的 1.6 + 2.0.1 服务器，给联调用，不是固件的一部分。更细的模块说明见 `c_ocpp/csms/README.md`。

终端 A（仓库根）：

```bash
cd c_ocpp/csms
PYTHONPATH=. python3 -m ocpp_csms --host 127.0.0.1 --port 9000 --interval 3600
```

常用参数：

| 参数 | 默认 | 含义 |
| --- | --- | --- |
| `--host` | `0.0.0.0` | 监听地址；本机联调用 `127.0.0.1` |
| `--port` | `9000` | 端口 |
| `--interval` | `300` | BootNotification.conf 里的心跳间隔（秒） |
| `--quiet` | 关 | 不打印每帧 RPC |
| `--no-probe` | 关 | 收到 `PROBE_CSMS` 后不要扫全部 CSMS CALL |
| `--reset-after-boot` | 关 | Boot 后再额外发一条 Reset（联调默认关，避免和 probe 抢） |

终端 B（仓库根，且已编译）：

```bash
./c_ocpp/build/test_ocpp_csms 127.0.0.1 9000
```

桩 URL：`ws://127.0.0.1:9000/<chargePointId>`，WebSocket **子协议**必须是 `ocpp1.6`、`ocpp2.0.1` 或 `ocpp2.1`（与你链的库一致）。同一条连接只能跑一个版本。

只跑 Python handler 单测：

```bash
cd c_ocpp/csms
PYTHONPATH=. python3 -m unittest discover -s tests -v
```

---

## 目录导航（源码）

```
c_ocpp/
  README.md                 本文件
  build_and_test.sh         主机一键配置 + 编译 + ctest
  CMakeLists.txt            仅 Linux 主机
  port/                     malloc/free、时钟、定时器；MCU 替换实现
  layer1/                   RPC 组帧 + JSON 辅助（cJSON）
  ocpp16/                   1.6 session + messages
  ocpp201/                  2.0.1 session + messages
  ocpp21/                   2.1 新增报文 + session（内嵌 2.0.1）
  third_party/cjson/        随仓库携带的 cJSON
  tests/                    仅主机；MCU 不要加入编译
  csms/                     主机 Python CSMS + run_live_test.py
```

对外入口：

- 1.6：`#include "ocpp16.h"`
- 2.0.1：`#include "ocpp201.h"`
- 公共 port：`#include "ocpp_port.h"`

报文文件一律带版本前缀，例如 `ocpp16_authorize.c` / `ocpp201_authorize.c`，避免 Keil 把两路都编成 `authorize.o`。

---

## 分层与多路连接

```
WS[i] rx  -->  session[i]（第 2 层）  -->  RPC 组帧（第 1 层）  -->  session.link.send
                  | uniqueId / pending / 定时器 / accept_control
```

1. **port**（`port/ocpp_port.c`）— `malloc` / `free`、时钟、软件定时器。芯片上换实现。
2. **`ocpp_link_t`** — 把一个 session 绑到一条传输和策略（send、心跳槽、是否接受控制）。
3. **第 1 层** — 拼 `[2|3|4, uniqueId, …]`；payload 编解码在每个报文自己的 `.c`。
4. **第 2 层** — **每个 CSMS 一个** `ocpp16_session_t` 或 `ocpp201_session_t`。

结构体用 **定长缓冲**。业务字段不要自己 `malloc`。cJSON 树用系统 `malloc` / `free`，用完 `cJSON_Delete`。软件定时器表没有锁，encode / `session_rx` 不要并行。

### 多运营商（2、3、N 路）

没有「双路专用类型」。N 路就是 `session[N]` + `link[N]` + N 条 WebSocket。常见策略：一路控制 CSMS，其余只收遥测。

```c
#define OCPP_CSMS_N 3
ocpp16_session_t ctx[OCPP_CSMS_N]; /* 放 BSS — 每路大约数 KiB（payload+frame） */

static int ws_send(const void *data, size_t len, void *user); /* user = &ws[i] */

void charger_ocpp_start(void) {
    ocpp_port_init();
    ocpp16_handlers_t h = {0};
    h.reset_req = my_reset;
    h.boot_notification_conf = my_boot_conf;

    ocpp_link_t link[OCPP_CSMS_N];
    const int accept_control[OCPP_CSMS_N] = {1, 0, 0}; /* 仅 CSMS 0 可 Reset / RemoteStart */
    ocpp16_boot_notification_req_t boot;
    ocpp16_boot_notification_req_example(&boot);

    for (int i = 0; i < OCPP_CSMS_N; i++) {
        ocpp_link_init(&link[i], i, ws_send, &ws[i], /*heartbeat_timer*/ i, accept_control[i]);
        ocpp16_session_init(&ctx[i], &h, &link[i]);
        ocpp16_session_send_boot_notification(&ctx[i], &boot);
    }
}

void on_ws_text(int i, const char *data, size_t len) {
    ocpp16_session_rx(&ctx[i], data, len);
}

void fanout_status(const ocpp16_status_notification_req_t *req) {
    for (int i = 0; i < OCPP_CSMS_N; i++) {
        ocpp16_session_send_status_notification(&ctx[i], req);
    }
}
```

- **定时器 id** 必须在全部上下文里唯一（`0 .. OCPP_PORT_TIMER_MAX-1`，默认 16）。3 路心跳用 `0,1,2` 即可；不要每路都用 `OCPP_PORT_TIMER_HEARTBEAT`（值为 0）。
- **RAM**：`N * sizeof(ocpp16_session_t)`。cJSON 树临时走堆，编完就释放。若每路还要 boot-retry / call 定时器，加大 `OCPP_PORT_TIMER_MAX`，例如 `i*3+0` 心跳、`i*3+1` retry。
- **`accept_control=0`**：Reset / RemoteStart / SetChargingProfile 等回 `CallError` `SecurityError`，不跑业务 handler。查询类如 `GetConfiguration` 仍应答。主备切换：`ocpp16_session_bind()` 改哪一路 `accept_control=1`。
- **遥测扇出**：固件自己循环 `send_*`。
- 单路仍可用 `ocpp_port_set_send()` 和 `session_init(s, h, NULL)`。

把 `ocpp16_session_t` / `ocpp201_session_t` 放 BSS/全局，不要放任务栈。尺寸宏：`OCPP_PORT_ARENA_SIZE`、`OCPP_PAYLOAD_MAX`、`OCPP_FRAME_MAX`、`OCPP_PORT_TIMER_MAX`。

---

## 移植与编译框架

主机 CMake **不要** `enable_testing` 进设备镜像。协议层不依赖操作系统、TCP 或 WebSocket。

### 必须编译的源文件

公共（任何版本都要，且 **只编一份**）：

| 文件 | 说明 |
| --- | --- |
| `third_party/cjson/cJSON.c` | JSON 树，内存走 `ocpp_port_malloc` |
| `layer1/ocpp_json.c` | 定长字符串拷贝、字段读写 |
| `layer1/ocpp_rpc.c` | `[2\|3\|4, uniqueId, …]` |
| `port/ocpp_port.c` | **主机默认实现**；MCU 换成自己的 `.c`，头文件仍用 `ocpp_port.h` |

OCPP 1.6 再加 `ocpp16/ocpp16_session.c` 和 `ocpp16/messages/*.c`（含各 action、`ocpp16_types.c`、`ocpp16_messages.c`）。

OCPP 2.0.1 再加 `ocpp201/ocpp201_session.c` 和 `ocpp201/messages/*.c`。

两套可以同时链进固件（两路运营商各用一个版本）。**同一条 WebSocket 只能跑一个版本。** 不要把 `tests/*.c`、`c_ocpp/build/`、`csms/` 编进镜像。

### 1.6 与 2.0.1 文件名会不会冲突

**不会。** 消息 `.c` / `.h` 都带版本前缀，Keil 按文件名生成 `.o` 时不会合成同一个 `authorize.o`。C 符号是 `ocpp16_*` / `ocpp201_*`。同一 `.c` 里可以同时持有两种 session（见 `tests/test_ocpp_both.c`）。

### Include 路径

全局（应用 + 两个版本一起用时）：

```
c_ocpp
c_ocpp/port
c_ocpp/layer1
c_ocpp/third_party/cjson
c_ocpp/ocpp16
c_ocpp/ocpp201
```

**不要**把 `ocpp16/messages` 和 `ocpp201/messages` **同时**放进全局 Include。只在编译该目录下 `.c` 时加自己的 `messages/`。

语言：**C11**。不要用 C++ 名修饰去编这些 `.c`。需要 `stddef.h` / `stdint.h` / `string.h` / `stdio.h`（`snprintf`）。MCU 上可把 `ocpp_port_log` 做成空函数，避免拉 `vfprintf`。

### 编译期尺寸（按 RAM 改）

在工程预处理宏里覆盖，不要改业务 `.c`：

| 宏 | 默认 | 含义 |
| --- | --- | --- |
| `OCPP_PORT_ARENA_SIZE` | 8192 | 仅静态池移植使用。宿主 `ocpp_port.c` 改走 `malloc`，不读这个宏 |
| `OCPP_PAYLOAD_MAX` | 2048 | session 里 payload 缓冲 |
| `OCPP_FRAME_MAX` | 4096 | 整帧 `[2,…,payload]` |
| `OCPP_PORT_TIMER_MAX` | 16 | 软件定时器槽，N 路心跳 id 必须互斥 |
| `OCPP16_PENDING_MAX` / `OCPP201_PENDING_MAX` | 8 | 每路未完成 CALL 数 |

N 路 RAM 大约 `N × (PAYLOAD + FRAME)`，外加编解码当次的堆分配。

### MCU 替换 `port.c`

保留 `ocpp_port.h` 的函数签名，芯片工程提供自己的 `ocpp_port.c`（或 `board/ocpp_port_mcu.c`），编译列表里拿掉 host 的 `port/ocpp_port.c`。

| 函数 | 单片机上做什么 |
| --- | --- |
| `ocpp_port_init` | 把 cJSON hook 指到 `malloc` / `free`（可抄 host 里 `cJSON_InitHooks`） |
| `ocpp_port_malloc/free/realloc` | 宿主实现调用 `malloc` / `free` / `realloc`。cJSON 树在 `ocpp_json_dump` 和 `session_rx` 里 `cJSON_Delete` |
| `ocpp_port_now_ms` | SysTick / `HAL_GetTick` |
| `ocpp_port_timer_start/stop` | 硬件周期中断，或在节拍里调 `ocpp_port_timer_tick(elapsed_ms)` |
| `ocpp_link.send` | 为 **这一路** CSMS 写一条 WebSocket **text** 帧；返回 0 成功 |
| `ocpp_port_log` | RTT/UART，或空函数 |

Host 的 `ocpp_port_now_ms` 用了 `clock_gettime`，**不能**原样链到裸机。**不要并行** encode/rx 两个 session。多路不要依赖全局 `ocpp_port_set_send`。

启动顺序：`ocpp_port_init()` → 每路 `ocpp_link_init`（send + 唯一 `heartbeat_timer_id`）→ `ocpp16_session_init` / `ocpp201_session_init` → 连接成功后 `send_boot_notification`。WS **收齐一帧** JSON 后再 `session_rx`（不要半包）。

### 嵌进现有固件 CMake（交叉编译）

```cmake
set(C_OCPP ${CMAKE_SOURCE_DIR}/c_ocpp)

add_library(c_ocpp_core STATIC
    ${C_OCPP}/third_party/cjson/cJSON.c
    ${C_OCPP}/layer1/ocpp_json.c
    ${C_OCPP}/layer1/ocpp_rpc.c
    ${CMAKE_SOURCE_DIR}/port/ocpp_port_mcu.c)
target_include_directories(c_ocpp_core PUBLIC
    ${C_OCPP} ${C_OCPP}/port ${C_OCPP}/layer1 ${C_OCPP}/third_party/cjson)

add_library(c_ocpp16 STATIC
    ${C_OCPP}/ocpp16/ocpp16_session.c
    ${C_OCPP}/ocpp16/messages/ocpp16_authorize.c
    # … 其余 ocpp16/messages/*.c
)
target_include_directories(c_ocpp16 PUBLIC ${C_OCPP}/ocpp16
    PRIVATE ${C_OCPP}/ocpp16/messages)
target_link_libraries(c_ocpp16 PUBLIC c_ocpp_core)
target_compile_definitions(c_ocpp_core PUBLIC
    OCPP_PORT_ARENA_SIZE=4096
    OCPP_PAYLOAD_MAX=1536
    OCPP_FRAME_MAX=3072)
```

ARM GCC 工具链文件示例：

```cmake
# toolchain-arm-none-eabi.cmake
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
```

```bash
cmake -S . -B build-mcu -DCMAKE_TOOLCHAIN_FILE=toolchain-arm-none-eabi.cmake
cmake --build build-mcu
```

### Makefile / Keil / IAR

**Makefile（摘录）：**

```makefile
C_OCPP := c_ocpp
INCLUDES := -I$(C_OCPP) -I$(C_OCPP)/port -I$(C_OCPP)/layer1 -I$(C_OCPP)/third_party/cjson \
            -I$(C_OCPP)/ocpp16
# 编译 ocpp16/messages/*.c 时再加： -I$(C_OCPP)/ocpp16/messages
# 同时编 2.0.1：公共 INCLUDES 加 -I$(C_OCPP)/ocpp201，
# messages 的 -I 只加在对应那一组 .c 上
CFLAGS += -std=c11 -Wall -Wextra -DOCPP_PORT_ARENA_SIZE=4096
PORT_SRC := board/ocpp_port_mcu.c
CORE_SRC := $(C_OCPP)/third_party/cjson/cJSON.c \
            $(C_OCPP)/layer1/ocpp_json.c $(C_OCPP)/layer1/ocpp_rpc.c
LIB16_SRC := $(C_OCPP)/ocpp16/ocpp16_session.c \
             $(wildcard $(C_OCPP)/ocpp16/messages/*.c)
```

**Keil / IAR：** 建组 `c_ocpp_core` / `c_ocpp16_msg` / `c_ocpp201_msg` / `port`。两组 messages 用**不同**的组 Include（各自的 `messages/`）。应用组不要加两个 `messages/`。

### 移植检查单

- [ ] MCU `port` 已替换，`clock_gettime` / `vfprintf` 不再进镜像（除非你自愿）
- [ ] `ocpp_port_init` 在任何 encode/rx 之前调用
- [ ] 每路 `heartbeat_timer_id` 唯一，且 `< OCPP_PORT_TIMER_MAX`
- [ ] WS text 完整一帧再 `session_rx`
- [ ] session 对象在 BSS；定时器表只在一个任务里推进
- [ ] 子协议与库一致：`ocpp1.6` ↔ `c_ocpp16`，`ocpp2.0.1` ↔ `c_ocpp201`，`ocpp2.1` ↔ `c_ocpp21`
- [ ] 同时用 1.6 和 2.0.1 时 `cJSON`/`port`/`layer1` 只编一份；全局 Include 不含两套 `messages/`
- [ ] 主机 `ctest` 仍通过后再切交叉编译

---

## Qt 5.14 充电桩

`c_ocpp/qt_cp/` 是主机上的充电桩窗口：三个分页分别是 OCPP 1.6、2.0.1 和 2.1。每页自己连接，桩侧 Request 做成按钮，旁边的文本框里是可修改的示例 JSON，页底是这一页的日志。打开 `qt_ocpp_cp.pro`，或按 `c_ocpp/qt_cp/README.md` 用 CMake 编。连上后自动发 BootNotification。

## 常见问题

| 现象 | 处理 |
| --- | --- |
| clone 后没有 `c_ocpp/build_and_test.sh` | 当前不在 `cursor/c-ocpp-mcu-76bd`，见上文 checkout |
| `缺少命令: cc` / `cmake` | `sudo apt install -y build-essential cmake python3` |
| `Could not find Python3` | 安装 `python3`，或 `cmake … -DPython3_EXECUTABLE=/usr/bin/python3` |
| 新增 `messages/*.c` 没进库 | 再执行一次 `cmake -S c_ocpp -B c_ocpp/build` |
| `test_ocpp_csms` 连不上 / 超时 | 本机 `127.0.0.1` 要能 listen；防火墙不要拦回环；90 秒超时 |
| 直接跑 `test_ocpp_csms` 立刻失败 | 先起 CSMS，或用 `run_live_test.py` / 一键脚本 |
| 想看 CSMS 收发 | 不要 `--quiet`：`PYTHONPATH=. python3 -m ocpp_csms --port 9000` |
| 链接报 cJSON / `ocpp_port` 重复定义 | `cJSON.c` 和 `ocpp_port.c` 只编进一份 `c_ocpp_core` |
| 固件里 Keil 报 `authorize.c` 重复 | 确认用的是带前缀的 `ocpp16_authorize.c` / `ocpp201_authorize.c` |
| 心跳错乱 / 多路互相取消 | 每路 `heartbeat_timer_id` 不同 |
| 半包解析失败 | 等 WebSocket **完整 text 帧**再 `session_rx` |
| 栈溢出 | session 放到 BSS；减小 `OCPP_PAYLOAD_MAX` / `OCPP_FRAME_MAX` 或加大任务栈 |

脚本里 `chmod +x` 只需一次。若 `bash: Permission denied`，检查文件是否可执行，或用 `bash c_ocpp/build_and_test.sh`。
