cmake_minimum_required(VERSION 3.5)

# Add an library for the example classes
add_library(example_unit_test
    Reverse.cpp
    Palindrome.cpp
)

#############################################
# Unit tests

# enable CTest testing
enable_testing()

# Add a testing executable
add_executable(unit_tests unit_tests.cpp)

target_link_libraries(unit_tests
    example_unit_test
    Catch2::Test
)

add_test(test_all unit_tests)
