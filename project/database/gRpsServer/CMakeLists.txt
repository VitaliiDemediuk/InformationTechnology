cmake_minimum_required(VERSION 3.20)

project(gRpsDatabaseServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_FLAGS "-lboost_program_options")

FIND_PACKAGE(Boost COMPONENTS program_options REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

set(Protobuf_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../proto)

include(../cmake/grpc_common.cmake)
include(../cmake/proto_grpc_generator.cmake)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_SRCS ${Protobuf_INCLUDE_DIR}/database_name.proto
                                            ${Protobuf_INCLUDE_DIR}/table_id.proto
                                            ${Protobuf_INCLUDE_DIR}/table_name.proto
                                            ${Protobuf_INCLUDE_DIR}/table_name.proto
                                            ${Protobuf_INCLUDE_DIR}/empty.proto
                                            ${Protobuf_INCLUDE_DIR}/database_getname_service.proto
                                            ${Protobuf_INCLUDE_DIR}/table_count_service.proto
                                            ${Protobuf_INCLUDE_DIR}/create_table_service.proto)
PROTOBUF_GENERATE_GRPC_CPP(GRPC_SRCS GRPC_HDRS ${Protobuf_INCLUDE_DIR}/database_getname_service.proto
                                               ${Protobuf_INCLUDE_DIR}/table_count_service.proto
                                               ${Protobuf_INCLUDE_DIR}/create_table_service.proto)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../core core)

add_executable(gRpsDatabaseServer
               Services/GetDatabaseNameService.cpp Services/GetDatabaseNameService.h
               Services/GetTableCountService.cpp   Services/GetTableCountService.h
               Services/CreateTableService.cpp     Services/CreateTableService.h
               ServicesHolder.cpp ServicesHolder.h
               ${PROTO_SRCS} ${PROTO_HDRS}
               ${GRPC_SRCS}  ${GRPC_HDRS}
               main.cpp)

target_link_libraries(gRpsDatabaseServer
        core
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF}
        ${Boost_LIBRARIES})

target_include_directories(gRpsDatabaseServer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../core/include)