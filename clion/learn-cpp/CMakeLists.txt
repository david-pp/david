cmake_minimum_required(VERSION 3.2)
project(clion)


add_executable(variadic variadic.cpp)
add_executable(func func.cpp)
add_executable(func-pointer func-pointer.cpp)
add_executable(func-functor func-functor.cpp)
add_executable(func-lambda func-lambda.cpp)
add_executable(life life.cpp)
add_executable(life-lrvalue life-lrvalue.cpp)
add_executable(life-move life-move.cpp)
add_executable(life-forward life-forward.cpp)
#target_link_libraries(life-move ncurses)