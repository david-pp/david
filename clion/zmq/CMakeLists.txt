cmake_minimum_required(VERSION 3.2)
project(clion)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(main main.cpp)
target_link_libraries(main zmq)


add_executable(proxy proxy.cpp)
target_link_libraries(proxy zmq)