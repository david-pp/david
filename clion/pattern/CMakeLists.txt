cmake_minimum_required(VERSION 3.2)
project(clion)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)


add_executable(adapter adapter.cpp)
add_executable(factory-method factory-method.cpp)
add_executable(factory-method2 factory-method2.cpp)