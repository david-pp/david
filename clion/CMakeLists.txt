cmake_minimum_required(VERSION 3.2)
project(clion)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(zmq)

set(SOURCE_FILES main.cpp)
add_executable(exam ${SOURCE_FILES})

add_executable(variadic variadic.cpp)