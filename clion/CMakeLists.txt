cmake_minimum_required(VERSION 3.6)
project(clion)

set(CMAKE_LEGACY_CYGWIN_WIN32 0) # Remove when CMake >= 2.8.4 is require

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wno-deprecated-declarations")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")


include_directories(/usr/local/include)
include_directories(./)


add_subdirectory(lua)

add_subdirectory(zmq)
add_subdirectory(cpp)
add_subdirectory(snippet)
add_subdirectory(db)
add_subdirectory(protobuf)
add_subdirectory(curl)
add_subdirectory(patchupdate)
add_subdirectory(fsm)
add_subdirectory(thread)
add_subdirectory(ev)
add_subdirectory(tinyhttp)
add_subdirectory(pattern)

add_definitions(-DCATCH_PLATFORM_WINDOWS)

set(SOURCE_FILES main.cpp)
add_executable(clion ${SOURCE_FILES})
add_executable(catch catch.cpp)

