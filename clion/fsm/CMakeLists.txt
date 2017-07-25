cmake_minimum_required(VERSION 3.2)
project(clion)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(fsm main.cpp)