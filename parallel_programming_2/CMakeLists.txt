cmake_minimum_required(VERSION 3.16)
project(parallel_programming_2)

set(CMAKE_CXX_STANDARD 20)

add_executable(parallel_programming_2 main.cpp matrix.cpp matrix.h jacobiMPI.cpp jacobiMPI.h)