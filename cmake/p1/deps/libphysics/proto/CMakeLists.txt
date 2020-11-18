# find the protobuf compiler and libraries
message("use protobuf: ${USE_MY_PROTOBUF}")
if (USE_MY_PROTOBUF)
    message("use my protobuf")
    set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
    find_package(MyProtobuf REQUIRED)
else(USE_MY_PROTOBUF)
    message("use system protobuf")
    find_package(Protobuf REQUIRED)
endif(USE_MY_PROTOBUF)

# check if protobuf was found
if(Protobuf_FOUND)
    message ("protobuf is found - include: ${Protobuf_INCLUDE_DIR}")
    message ("protobuf is found - libs: ${Protobuf_LIBRARY}")
    message ("protobuf is found - protoc: ${Protobuf_PROTOC_EXECUTABLE}")
else()
    message (FATAL_ERROR "Cannot find Protobuf")
endif()

# Generate the .h and .cxx files
PROTOBUF_GENERATE_CPP(
            PROTO_SRCS PROTO_HDRS AddressBook.proto Person.proto 
            PROTOC_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Print path to generated files
message ("PROTO_SRCS = ${PROTO_SRCS}")
message ("PROTO_HDRS = ${PROTO_HDRS}")

# Package .h and .cxx as OBJECT library target
add_library(physics_proto OBJECT ${PROTO_SRCS})
target_link_libraries(physics_proto PUBLIC protobuf::libprotobuf )

# Way1: Add an executable
add_executable(protobuf_example
    main.cpp
    ${PROTO_SRCS})

target_include_directories(protobuf_example PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(protobuf_example PRIVATE protobuf::libprotobuf)

# Way2 : Add an executable
add_executable(protobuf_example2  main.cpp)
target_include_directories(protobuf_example2 PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(protobuf_example2 PUBLIC physics_proto)