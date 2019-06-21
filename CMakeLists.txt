cmake_minimum_required(VERSION 3.14)
project(matrix)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pthread")

add_executable(matrix main.cpp matrix.h matrix_threads.h)
