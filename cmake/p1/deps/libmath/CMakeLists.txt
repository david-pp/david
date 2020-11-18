cmake_minimum_required(VERSION 3.5)

# set the project name and version
project(tinymath)
#project(libmath VERSION 1.0)

############################################################
# Create a library
############################################################

#Generate the static library from the library sources
add_library(tinymath STATIC 
    src/tinymath.cpp)

target_include_directories(tinymath
    PUBLIC 
    ${PROJECT_SOURCE_DIR}/include
    )

#Generate the shared library from the library sources
add_library(tinymath-dynamic SHARED 
    src/tinymath.cpp)

add_library(my::tinymath ALIAS tinymath-dynamic)
set_target_properties(tinymath-dynamic PROPERTIES OUTPUT_NAME tinymath)

target_include_directories(tinymath-dynamic
    PUBLIC 
    ${PROJECT_SOURCE_DIR}/include
    )

############################################################
# Create an executable
############################################################

# Add an executable with the above sources
add_executable(hello_math
    src/main.cpp
    )

# link the new hello_library target with the hello_binary target
target_link_libraries(hello_math
    PRIVATE 
    my::tinymath
    )


############################################################
# Install
# cmake -DCMAKE_INSTALL_PREFIX=/usr/...
############################################################

# Binaries
install (TARGETS hello_math
    DESTINATION bin)

# Library
#install (TARGETS cmake_examples_inst
#    LIBRARY DESTINATION lib)
install (TARGETS tinymath tinymath-dynamic
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)

# Header files
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ 
    DESTINATION include)

# ReadMe
install (FILES README.md
    DESTINATION  .)

############################################################
# Pack
############################################################

include(InstallRequiredSystemLibraries)
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
#set(CPACK_PACKAGE_VERSION_MAJOR "${libmath_VERSION_MAJOR}")
#set(CPACK_PACKAGE_VERSION_MINOR "${libmath_VERSION_MINOR}")
include(CPack)

