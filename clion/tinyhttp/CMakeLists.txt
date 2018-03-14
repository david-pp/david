cmake_minimum_required(VERSION 3.2)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(example00 example00.cpp connection.cc helpers.cc restclient.cc tinyhttp.cpp)
target_link_libraries(example00 curl)