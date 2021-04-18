cmake_minimum_required(VERSION 3.12)

project(HSMProtocol)

message( "Configuring ...${PROJECT_NAME}" )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCE_MAIN main.cpp)
set(SOURCE_HSMP HSMP-src/HSMP.hpp)
set(HSMP HSMProtocol)

add_library(HSMProtocol STATIC
HSMP-src/HSMP.hpp
)

target_include_directories(HSMProtocol
PUBLIC HSMP-src
)

add_executable(main ${SOURCE_MAIN})


set_target_properties(${HSMP} PROPERTIES LINKER_LANGUAGE C)