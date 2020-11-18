############################################################
# Create a library
############################################################

# Generate the static library from the library sources
add_library(physics STATIC physics.cpp)
target_include_directories(physics PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Depends on libmath
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
find_package(Math REQUIRED)
if(Math_FOUND)
    message(STATUS "math library is found: ${Math_INCLUDE_DIR}")
    target_include_directories(physics PUBLIC ${Math_INCLUDE_DIR})
    target_link_libraries(physics PRIVATE ${Math_LIBRARY})
else(Math_FOUND)
    message(FATAL_ERROR "math library is not found")
endif(Math_FOUND)

############################################################
# Create an executable
############################################################

# Add an executable with the above sources
add_executable(hello_physics main.cpp)
# target_include_directories(hello_physics PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(hello_physics physics)
