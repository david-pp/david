cmake_minimum_required(VERSION 3.2)
project(clion)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-elide-constructors")

add_executable(variadic variadic.cpp)

add_executable(func func.cpp)
add_executable(func-pointer func-pointer.cpp)
add_executable(func-functor func-functor.cpp)
add_executable(func-lambda func-lambda.cpp)

add_executable(life life.cpp)
add_executable(life-lrvalue life-lrvalue.cpp)
add_executable(life-move life-move.cpp)
add_executable(life-forward life-forward.cpp)
add_executable(construct construct.cpp)

add_executable(reflection-preq reflection-preq.cpp)
add_executable(reflection reflection.cpp)
#target_link_libraries(life-move ncurses)

add_executable(loop loop.cpp)