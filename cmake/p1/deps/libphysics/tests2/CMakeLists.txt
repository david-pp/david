cmake_minimum_required(VERSION 3.5)

# Set the project name
project (google_test_example)

# Add an library for the example classes
add_library(example_google_test 
    Reverse.cpp
    Palindrome.cpp
)


#############################################
# Unit tests

# enable CTest testing
enable_testing()

# Add a testing executable
add_executable(unit_tests2 unit_tests.cpp)

target_link_libraries(unit_tests2
    example_google_test
    GTest::GTest 
    GTest::Main
)

add_test(test_all unit_tests2)
