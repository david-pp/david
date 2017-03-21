cmake_minimum_required(VERSION 2.8)
project(clion)

set(PROTOC              /usr/local/bin/protoc)
set(PROTOS_DIR          ${CMAKE_CURRENT_SOURCE_DIR})
set(PROTOS_CPPOUT_DIR   ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE PROTOS *.proto)

#message("${PROTOS}")

foreach(PB ${PROTOS})
    get_filename_component(PB_NAME ${PB} NAME_WE)
    set(PB_H   ${PB_NAME}.pb.h)
    set(PB_CPP ${PB_NAME}.pb.cc)

    set(PB_CPPOUT
            ${PROTOS_CPPOUT_DIR}/${PB_H}
            ${PROTOS_CPPOUT_DIR}/${PB_CPP})

    set(PB_CPPOUTS
            ${PB_CPPOUTS}
            ${PB_CPPOUT})

    add_custom_command(
            OUTPUT  ${PB_CPPOUT}
            COMMAND ${PROTOC} -I=${PROTOS_DIR} --cpp_out=${PROTOS_CPPOUT_DIR} ${PB}
            DEPENDS  ${PB})
endforeach(PB ${PROTOS})

include_directories(/usr/local/include)
link_directories(/usr/local/lib/)

add_executable(pb_simple simple.cpp ${PB_CPPOUTS})
target_link_libraries(pb_simple protobuf)

add_executable(pb_reflect reflect.cpp ${PB_CPPOUTS})
target_link_libraries(pb_reflect protobuf)