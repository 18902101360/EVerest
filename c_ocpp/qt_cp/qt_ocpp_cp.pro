# Qt Creator 5.14：用本文件打开工程。影子构建目录不要放在源码树外的 c_ocpp 里面。
QT += core gui widgets websockets network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11 warn_on
CONFIG -= app_bundle
TEMPLATE = app
TARGET = qt_ocpp_cp

!equals(QT_MAJOR_VERSION, 5) | lessThan(QT_MINOR_VERSION, 14) {
    error("This program is written for Qt 5.14 (QWebSocket). Current Qt is $$QT_VERSION")
}

C_OCPP = $$PWD/..
INCLUDEPATH += \
    $$C_OCPP \
    $$C_OCPP/port \
    $$C_OCPP/layer1 \
    $$C_OCPP/third_party/cjson \
    $$C_OCPP/ocpp16 \
    $$C_OCPP/ocpp16/messages \
    $$C_OCPP/ocpp201 \
    $$C_OCPP/ocpp201/messages \
    $$C_OCPP/ocpp21 \
    $$C_OCPP/ocpp21/messages

QMAKE_CFLAGS += -std=c11 -Wall -Wextra
QMAKE_CXXFLAGS += -Wall -Wextra

SOURCES += \
    src/main.cpp \
    src/MainWindow.cpp \
    src/CpReqExamples.cpp \
    $$C_OCPP/port/ocpp_port.c \
    $$C_OCPP/layer1/ocpp_json.c \
    $$C_OCPP/layer1/ocpp_rpc.c \
    $$C_OCPP/third_party/cjson/cJSON.c \
    $$C_OCPP/ocpp16/ocpp16_session.c \
    $$C_OCPP/ocpp201/ocpp201_session.c \
    $$C_OCPP/ocpp21/ocpp21_session.c

SOURCES += $$files($$C_OCPP/ocpp16/messages/*.c)
SOURCES += $$files($$C_OCPP/ocpp201/messages/*.c)
SOURCES += $$files($$C_OCPP/ocpp21/messages/*.c)

HEADERS += src/MainWindow.h src/CpReqExamples.h
