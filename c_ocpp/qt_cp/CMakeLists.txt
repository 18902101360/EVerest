cmake_minimum_required(VERSION 3.14)
project(qt_ocpp_cp LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 5.14 REQUIRED COMPONENTS Widgets WebSockets Test)

set(C_OCPP ${CMAKE_CURRENT_SOURCE_DIR}/..)
file(GLOB OCPP16_MSG ${C_OCPP}/ocpp16/messages/*.c)
file(GLOB OCPP201_MSG ${C_OCPP}/ocpp201/messages/*.c)
file(GLOB OCPP21_MSG ${C_OCPP}/ocpp21/messages/*.c)

add_executable(qt_ocpp_cp
    src/main.cpp
    src/MainWindow.cpp
    src/CpReqExamples.cpp
    src/click_all.cpp
    ${C_OCPP}/port/ocpp_port.c
    ${C_OCPP}/layer1/ocpp_json.c
    ${C_OCPP}/layer1/ocpp_rpc.c
    ${C_OCPP}/third_party/cjson/cJSON.c
    ${C_OCPP}/ocpp16/ocpp16_session.c
    ${C_OCPP}/ocpp201/ocpp201_session.c
    ${C_OCPP}/ocpp21/ocpp21_session.c
    ${OCPP16_MSG}
    ${OCPP201_MSG}
    ${OCPP21_MSG}
)
target_include_directories(qt_ocpp_cp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${C_OCPP}
    ${C_OCPP}/port
    ${C_OCPP}/layer1
    ${C_OCPP}/third_party/cjson
    ${C_OCPP}/ocpp16
    ${C_OCPP}/ocpp16/messages
    ${C_OCPP}/ocpp201
    ${C_OCPP}/ocpp201/messages
    ${C_OCPP}/ocpp21
    ${C_OCPP}/ocpp21/messages
)
target_link_libraries(qt_ocpp_cp PRIVATE Qt5::Widgets Qt5::WebSockets Qt5::Test)
if(MSVC)
    target_compile_options(qt_ocpp_cp PRIVATE /W3)
else()
    target_compile_options(qt_ocpp_cp PRIVATE -Wall -Wextra)
endif()
