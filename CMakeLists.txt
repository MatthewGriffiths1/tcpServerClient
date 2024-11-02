cmake_minimum_required(VERSION 3.5)
project(tcpserver)
include_directories(headers)
enable_testing()
file(GLOB SERVER_SOURCES "src/server/*.cpp" )
file(GLOB CLIENT_SOURCES "src/client/*.cpp")
add_executable(server ${SERVER_SOURCES})
add_executable(client ${CLIENT_SOURCES})

