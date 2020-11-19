cmake_minimum_required(VERSION 3.5)

# set the project name and version
project(libphysics)
#project(libphysics VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)

option(USE_MY_PROTOBUF "use my protobuf" ON)

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


############################################################
# Git Submodule
############################################################
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/repo/CMakeLists.txt")
    # message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()


############################################################
# Subdirectory
############################################################

add_subdirectory(3rd_party/catch2)
add_subdirectory(tests)

add_subdirectory(3rd_party/google-test)
add_subdirectory(tests2)

add_subdirectory(3rd_party/fmt)
add_subdirectory(app-fmt)
