cmake_minimum_required(VERSION 3.5)
project(myapp)

# options: default ON
# can change by : cmake -DUSE_STD_MATH=OFF
option(USE_STD_MATH "use std cmath" ON)
message("USE_STD_MATH = ${USE_STD_MATH}")

# specify libmath install path
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${PROJECT_SOURCE_DIR}/../deps/libmath/install)
# message(${CMAKE_PREFIX_PATH})

# find tinymath
find_package(tinymath REQUIRED)

# create exe
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} tiny::tinymath)

if (USE_STD_MATH)
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DUSE_STD_MATH)
endif(USE_STD_MATH)