cmake_minimum_required(VERSION 3.10)

project(qr-service)

set(CMAKE_CXX_STANDARD 17)

add_executable(qr-service
    src/main.cpp
    src/tcp_server.cpp
    src/logger.cpp
    src/command_handler.cpp
    src/qr_reader.cpp
)