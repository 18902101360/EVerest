/*
 * Qt 5.14 充电桩入口。协议在 c_ocpp 里，这里只把命令行交给窗口。
 * --auto 用来在没有人点按钮时确认 Boot 已被 CSMS 接受。
 */
#include "MainWindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QTimer>

/**
 * main - 启动充电桩窗口
 * @argc: 命令行参数个数
 * @argv: 参数，支持 --url --id --vendor --model --auto
 *
 * 普通运行打开界面，用户自己点连接。--auto 在事件循环开始后立刻连接，
 * 两路 BootNotification 都是 Accepted 时退出码 0；超时或连不上时退出码 2。
 * 返回值交给操作系统。
 */
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("qt_ocpp_cp"));
    QCoreApplication::setApplicationVersion(QStringLiteral("1.0"));
    QCoreApplication::setOrganizationName(QStringLiteral("c_ocpp"));

    QCommandLineParser parser;
    parser.setApplicationDescription(
        QStringLiteral("Qt 5.14 charge point: c_ocpp OCPP 1.6 and 2.0.1 over QWebSocket"));
    parser.addHelpOption();
    QCommandLineOption urlOpt(QStringList() << QStringLiteral("u") << QStringLiteral("url"),
                              QStringLiteral("CSMS base URL, without charge-point id"),
                              QStringLiteral("url"), QStringLiteral("ws://cpmstest.timxon.com:8887"));
    QCommandLineOption idOpt(QStringList() << QStringLiteral("i") << QStringLiteral("id"),
                             QStringLiteral("Charge point id (URL path)"),
                             QStringLiteral("id"), QStringLiteral("CP001"));
    QCommandLineOption vendorOpt(QStringLiteral("vendor"), QStringLiteral("chargePointVendor / vendorName (station user)"),
                                 QStringLiteral("name"), QStringLiteral("zephyr"));
    QCommandLineOption modelOpt(QStringLiteral("model"), QStringLiteral("chargePointModel / model"),
                                QStringLiteral("name"), QStringLiteral("QT-CP"));
    QCommandLineOption autoOpt(QStringLiteral("auto"),
                               QStringLiteral("Connect both versions, exit 0 after BootNotification Accepted"));
    parser.addOption(urlOpt);
    parser.addOption(idOpt);
    parser.addOption(vendorOpt);
    parser.addOption(modelOpt);
    parser.addOption(autoOpt);
    parser.process(app);

    MainWindow window;
    window.applySettings(parser.value(urlOpt), parser.value(idOpt), parser.value(vendorOpt), parser.value(modelOpt));
    const bool autoExit = parser.isSet(autoOpt);
    window.setAutoExit(autoExit);
    window.show();

    if (autoExit) {
        /* 等窗口进入事件循环再连，否则套接字还没有线程。 */
        QTimer::singleShot(0, &window, &MainWindow::startConnect);
        /*
         * 超时用 exit(2)，和 Boot 成功的 0 分开。关窗口会让事件循环以 0 结束，
         * 若那时还没 Accepted，下面同样记成 2。
         */
        QTimer::singleShot(20000, &app, []() { QCoreApplication::exit(2); });
    }

    const int rc = app.exec();
    if (autoExit && !window.autoSucceeded()) {
        return rc == 0 ? 2 : rc;
    }
    return rc;
}
