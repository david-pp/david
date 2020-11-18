
message("try to find mathlib")

# find include
find_path(Math_INCLUDE_DIR 
          NAMES tinymath.h
          PATHS /Users/david/projects/david/cmake/p1/deps/libmath/build/tinymath-0.1.1-Darwin/include)

# find lib
find_library(Math_LIBRARY
             NAMES tinymath 
             PATHS /Users/david/projects/david/cmake/p1/deps/libmath/build/tinymath-0.1.1-Darwin/lib)

# set directly
# set(Math_INCLUDE_DIR /Users/david/projects/david/cmake/p1/deps/libmath/build/tinymath-0.1.1-Darwin/include)
# set(Math_LIBRARY /Users/david/projects/david/cmake/p1/deps/libmath/build/tinymath-0.1.1-Darwin/lib)

message("math-include: ${Math_INCLUDE_DIR}")
message("math-lib    : ${Math_LIBRARY}")

if (Math_INCLUDE_DIR AND Math_LIBRARY)
    set(Math_FOUND TRUE)
endif ()