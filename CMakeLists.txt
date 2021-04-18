cmake_minimum_required(VERSION 3.15)

project("HSMP")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(hsmp src/main.cpp)
