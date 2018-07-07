cmake_minimum_required(VERSION 3.2)
project(clion)

include_directories(/usr/local/include)
link_directories(/usr/local/lib/)

add_executable(ev_1 ev_1.cpp)
target_link_libraries(ev_1 ev)

add_executable(ev_my ev_my.cpp eventloop.cpp)
target_link_libraries(ev_my ev)