cmake_minimum_required(VERSION 3.20)

project(gRpsDatabaseServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(Protobuf_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../proto)

include(../cmake/grpc_common.cmake)
include(../cmake/proto_grpc_generator.cmake)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_SRCS ${Protobuf_INCLUDE_DIR}/create_new_database.proto)
PROTOBUF_GENERATE_GRPC_CPP(GRPC_SRCS GRPC_HDRS ${Protobuf_INCLUDE_DIR}/create_new_database.proto)

add_executable(gRpsDatabaseServer
               Services/DatabaseCreatorService.cpp Services/DatabaseCreatorService.h
               ServicesHolder.cpp ServicesHolder.h
               ${PROTO_SRCS} ${PROTO_HDRS}
               ${GRPC_SRCS}  ${GRPC_HDRS}
               main.cpp)

target_link_libraries(gRpsDatabaseServer
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF})
