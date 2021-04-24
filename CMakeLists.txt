cmake_minimum_required(VERSION 3.15)

project("HSMP")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# add_executable(hsmp src/main.cpp)

add_executable(server src/server.cpp)

add_executable(client src/client.cpp)
