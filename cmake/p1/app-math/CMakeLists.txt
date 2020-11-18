cmake_minimum_required(VERSION 3.5)
project(myapp)

# specify libmath install path
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${PROJECT_SOURCE_DIR}/../deps/libmath/install)
# message(${CMAKE_PREFIX_PATH})

# find tinymath
find_package(tinymath REQUIRED)

# create exe
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} tiny::tinymath)
