cmake_minimum_required(VERSION 3.16)
project(parallel_programming_3)

set(CMAKE_CXX_STANDARD 17)

add_executable(parallel_programming_3 main.cpp quickSortMPI.cpp quickSortMPI.h)