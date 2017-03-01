cmake_minimum_required(VERSION 3.2)
project(clion)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

include_directories(/usr/local/include/soci)
include_directories(/usr/local/include/soci/mysql)
include_directories(/usr/include/mysql)
#link_directories(/usr/local/lib)
link_directories(/usr/local/lib64)

add_executable(soci soci.cpp)
target_link_libraries(soci soci_core soci_mysql)