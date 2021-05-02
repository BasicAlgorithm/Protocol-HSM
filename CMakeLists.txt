cmake_minimum_required(VERSION 3.15)

project("HSMP")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(HSMP_SOURCES src/HSMP/HSMPRequest.cpp src/HSMP/HSMPResponse.cpp)

add_executable(server src/server.cpp ${HSMP_SOURCES})
target_link_libraries(server pthread)

add_executable(client src/client.cpp ${HSMP_SOURCES})
target_link_libraries(client pthread)
