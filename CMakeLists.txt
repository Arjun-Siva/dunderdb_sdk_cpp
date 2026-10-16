cmake_minimum_required(VERSION 3.28)
project(dunderdb_sdk_cpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(ZMQ REQUIRED IMPORTED_TARGET libzmq)

# third party libraries
add_library(rapidjson INTERFACE)

target_include_directories(rapidjson
        INTERFACE
        ${CMAKE_SOURCE_DIR}/third_party/rapidjson/include
)

add_library(dunderdb_sdk_cpp STATIC
        src/field.cpp
        src/message.cpp
        src/send_request.cpp
        src/reply.cpp
        src/query_request.cpp
        src/client.cpp
        src/schema.cpp
        include/dunderdb/util.h
)

target_include_directories(dunderdb_sdk_cpp PUBLIC
        include
)

target_link_libraries(dunderdb_sdk_cpp PRIVATE
        PkgConfig::ZMQ
        rapidjson
)

target_link_libraries(dunderdb_sdk_cpp
        PRIVATE
        rapidjson
)

add_executable(insert_test
        tests/insert_test.cpp
)

target_link_libraries(insert_test PRIVATE
        dunderdb_sdk_cpp
)

add_executable(query_test
        tests/query_test.cpp
)

target_link_libraries(query_test PRIVATE
        dunderdb_sdk_cpp
)

add_executable(ddl_test
        tests/ddl_test.cpp
)

target_link_libraries(ddl_test PRIVATE
        dunderdb_sdk_cpp
)