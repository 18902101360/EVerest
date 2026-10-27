# Host 构建：静态库 + 无网络单元测试。MCU 工程不必用本文件，按 README 列源文件即可。
cmake_minimum_required(VERSION 3.14)
project(c_ocpp C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra)

set(CJSON_SRC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/cjson/cJSON.c)

# 公共层只编进 c_ocpp_core 一次。1.6 与 2.0.1 各自的 .a 再 PUBLIC 链接它，
# 避免同一固件里 cJSON / ocpp_port 符号重复。
add_library(c_ocpp_core STATIC
    ${CJSON_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/layer1/ocpp_json.c
    ${CMAKE_CURRENT_SOURCE_DIR}/layer1/ocpp_rpc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port/ocpp_port.c
)
target_include_directories(c_ocpp_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/port
    ${CMAKE_CURRENT_SOURCE_DIR}/layer1
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/cjson
)

file(GLOB OCPP16_MSG_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ocpp16/messages/*.c)
file(GLOB OCPP201_MSG_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ocpp201/messages/*.c)
file(GLOB OCPP21_MSG_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ocpp21/messages/*.c)

add_library(c_ocpp16 STATIC
    ${OCPP16_MSG_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/ocpp16/ocpp16_session.c
)
target_include_directories(c_ocpp16
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/ocpp16
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/ocpp16/messages
)
target_link_libraries(c_ocpp16 PUBLIC c_ocpp_core)

add_library(c_ocpp201 STATIC
    ${OCPP201_MSG_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/ocpp201/ocpp201_session.c
)
target_include_directories(c_ocpp201
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/ocpp201
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/ocpp201/messages
)
target_link_libraries(c_ocpp201 PUBLIC c_ocpp_core)

add_library(c_ocpp21 STATIC
    ${OCPP21_MSG_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/ocpp21/ocpp21_session.c
)
target_include_directories(c_ocpp21
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/ocpp21
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/ocpp21/messages
)
target_link_libraries(c_ocpp21 PUBLIC c_ocpp201)

enable_testing()
add_executable(test_ocpp16 tests/test_ocpp16.c)
target_link_libraries(test_ocpp16 PRIVATE c_ocpp16)
add_test(NAME test_ocpp16 COMMAND test_ocpp16)

add_executable(test_ocpp201 tests/test_ocpp201.c)
target_link_libraries(test_ocpp201 PRIVATE c_ocpp201)
add_test(NAME test_ocpp201 COMMAND test_ocpp201)

add_executable(test_ocpp_both tests/test_ocpp_both.c)
target_link_libraries(test_ocpp_both PRIVATE c_ocpp16 c_ocpp201 c_ocpp21)
add_test(NAME test_ocpp_both COMMAND test_ocpp_both)

add_executable(test_ocpp21 tests/test_ocpp21.c)
target_link_libraries(test_ocpp21 PRIVATE c_ocpp21)
add_test(NAME test_ocpp21 COMMAND test_ocpp21)

# Python CSMS (stdlib) + c_ocpp WebSocket client. Not linked into MCU firmware.
find_package(Python3 COMPONENTS Interpreter REQUIRED)
add_executable(test_ocpp_csms tests/test_ocpp_csms.c tests/ws_client.c)
target_link_libraries(test_ocpp_csms PRIVATE c_ocpp16 c_ocpp201 c_ocpp21)
add_test(
    NAME test_ocpp_csms_py
    COMMAND ${Python3_EXECUTABLE} -m unittest discover -s tests -v
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/csms
)
set_tests_properties(test_ocpp_csms_py PROPERTIES
    ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}/csms"
)
add_test(
    NAME test_ocpp_csms
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/csms/run_live_test.py $<TARGET_FILE:test_ocpp_csms>
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/csms
)
set_tests_properties(test_ocpp_csms PROPERTIES
    ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}/csms"
    TIMEOUT 150
)
