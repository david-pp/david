cmake_minimum_required(VERSION 3.5)

# set the project name and version
project(libphysics)
#project(libphysics VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)

############################################################
# Create a library
############################################################

#Generate the static library from the library sources
add_library(physics STATIC 
    src/physics.cpp)

target_include_directories(physics
    PUBLIC 
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/../libmath/include
    )

target_link_directories(physics
    PUBLIC
    ${PROJECT_SOURCE_DIR}/../libmath/lib
    )

target_link_libraries(physics PRIVATE math)

#Generate the shared library from the library sources
add_library(physics-dynamic SHARED 
    src/physics.cpp)

add_library(my::physics ALIAS physics-dynamic)
set_target_properties(physics-dynamic PROPERTIES OUTPUT_NAME physics)

target_link_directories(physics-dynamic
    PUBLIC
    ${PROJECT_SOURCE_DIR}/../libmath/lib
    )

target_include_directories(physics-dynamic
    PUBLIC 
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/../libmath/include
    )

target_link_libraries(physics-dynamic PRIVATE math)

############################################################
# Create an executable
############################################################

# Add an executable with the above sources
add_executable(hello_physics
    src/main.cpp
    )

#target_link_directories(hello_physics
#   PUBLIC
#   ${PROJECT_SOURCE_DIR}/../libmath/lib
#   )

# link the new hello_library target with the hello_binary target
target_link_libraries(hello_physics
    PRIVATE 
    physics
    #    math
    )

add_subdirectory(proto)


############################################################
# Install
# cmake -DCMAKE_INSTALL_PREFIX=/usr/...
############################################################

# Binaries
install (TARGETS hello_physics
    DESTINATION bin)

# Library
#install (TARGETS cmake_examples_inst
#    LIBRARY DESTINATION lib)
install (TARGETS physics physics-dynamic
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)

# Header files
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ 
    DESTINATION include)

# ReadMe
install (FILES README.md
    DESTINATION  ${CMAKE_INSTALL_PREFIX})

############################################################
# Pack
############################################################

include(InstallRequiredSystemLibraries)
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
#set(CPACK_PACKAGE_VERSION_MAJOR "${libphysics_VERSION_MAJOR}")
#set(CPACK_PACKAGE_VERSION_MINOR "${libphysics_VERSION_MINOR}")
include(CPack)

add_subdirectory(3rd_party/catch2)
add_subdirectory(tests)

add_subdirectory(3rd_party/google-test)
add_subdirectory(tests2)