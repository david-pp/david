cmake_minimum_required(VERSION 3.2)
project(clion)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-elide-constructors")


add_subdirectory(zmq)
add_subdirectory(learn-cpp)

set(SOURCE_FILES main.cpp)
add_executable(clion ${SOURCE_FILES})

