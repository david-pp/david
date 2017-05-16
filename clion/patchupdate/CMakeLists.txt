cmake_minimum_required(VERSION 3.2)
project(clion)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(zpipe zpipe.cpp)
target_link_libraries(zpipe z)

add_executable(crcpipe crcpipe.cpp)
target_link_libraries(crcpipe z)

add_executable(ztcrc ztcrc.cpp)
target_link_libraries(ztcrc z)

add_executable(crc crc.cpp)