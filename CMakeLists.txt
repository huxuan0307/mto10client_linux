# 最低版本号要求
cmake_minimum_required(VERSION 2.8)
# 项目信息
project(mto10client)

aux_source_directory(. DIR_SRCS)

add_subdirectory(src)
include_directories(./include)
add_executable(mto10client main.cpp)

target_link_libraries(mto10client Functions)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

