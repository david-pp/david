

# mathlib install path
set(Math_INSTALL_PATH ${CMAKE_SOURCE_DIR}/../libmath/install)
message("try to find mathlib : ${Math_INSTALL_PATH}")

# find include
find_path(Math_INCLUDE_DIR 
          NAMES tinymath.h
          PATHS ${Math_INSTALL_PATH}/include)

# find lib
find_library(Math_LIBRARY
             NAMES tinymath 
             PATHS ${Math_INSTALL_PATH}/lib)

# set directly
# set(Math_INCLUDE_DIR /Users/david/projects/david/cmake/p1/deps/libmath/build/tinymath-0.1.1-Darwin/include)
# set(Math_LIBRARY /Users/david/projects/david/cmake/p1/deps/libmath/build/tinymath-0.1.1-Darwin/lib)

message("math-include: ${Math_INCLUDE_DIR}")
message("math-lib    : ${Math_LIBRARY}")

mark_as_advanced(
    Math_INCLUDE_DIR
    Math_LIBRARY
)

if (Math_INCLUDE_DIR AND Math_LIBRARY)
    set(Math_FOUND TRUE)
endif ()