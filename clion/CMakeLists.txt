cmake_minimum_required(VERSION 2.8)
project(clion)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wno-deprecated-declarations")

include_directories(/usr/local/include)
include_directories(./)

add_subdirectory(zmq)
add_subdirectory(cpp)
add_subdirectory(snippet)
add_subdirectory(db)
add_subdirectory(protobuf)

add_definitions(-DCATCH_PLATFORM_WINDOWS)

set(SOURCE_FILES main.cpp)
add_executable(clion ${SOURCE_FILES})
add_executable(catch catch.cpp)

