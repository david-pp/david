cmake_minimum_required(VERSION 3.2)
project(clion)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(multi-index multi-index.cpp)
add_executable(reflection reflection.cpp)