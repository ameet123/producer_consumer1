cmake_minimum_required(VERSION 3.6)
project(producer_consumer)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.c util.c util.h)
add_executable(producer_consumer ${SOURCE_FILES})