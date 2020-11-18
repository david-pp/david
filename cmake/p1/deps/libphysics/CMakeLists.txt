cmake_minimum_required(VERSION 3.5)

# set the project name and version
project(libphysics)
#project(libphysics VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(src)
add_subdirectory(proto)


############################################################
# Install
# cmake -DCMAKE_INSTALL_PREFIX=/usr/...
############################################################

# Binaries
# install (TARGETS hello_physics
#     DESTINATION bin)

# Library
#install (TARGETS cmake_examples_inst
#    LIBRARY DESTINATION lib)
install (TARGETS physics
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