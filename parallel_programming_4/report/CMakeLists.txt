cmake_minimum_required(VERSION 3.16)
project(parallel_programming_4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

#add_executable(parallel_programming_4 main.cpp)
add_executable(parallel_programming_4 dijkstrareduction.cpp)