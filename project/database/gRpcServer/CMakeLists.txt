cmake_minimum_required(VERSION 3.20)

project(gRpcDatabaseServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

FIND_PACKAGE(Boost COMPONENTS program_options REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

set(Protobuf_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../proto)

include(../cmake/grpc_common.cmake)
include(../cmake/proto_grpc_generator.cmake)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_SRCS     ${Protobuf_INCLUDE_DIR}/database_name.proto
                                                ${Protobuf_INCLUDE_DIR}/table_id.proto
                                                ${Protobuf_INCLUDE_DIR}/table_name.proto
                                                ${Protobuf_INCLUDE_DIR}/all_tables_id.proto
                                                ${Protobuf_INCLUDE_DIR}/empty.proto
                                                ${Protobuf_INCLUDE_DIR}/database_getname_service.proto
                                                ${Protobuf_INCLUDE_DIR}/get_tables_count_service.proto
                                                ${Protobuf_INCLUDE_DIR}/create_table_service.proto
                                                ${Protobuf_INCLUDE_DIR}/get_all_tables_service.proto
                                                ${Protobuf_INCLUDE_DIR}/get_table_name_service.proto
                                                ${Protobuf_INCLUDE_DIR}/delete_table_service.proto
                                                ${Protobuf_INCLUDE_DIR}/create_cartesian_product_service.proto
                                                ${Protobuf_INCLUDE_DIR}/column_id.proto
                                                ${Protobuf_INCLUDE_DIR}/columns_service.proto
                                                ${Protobuf_INCLUDE_DIR}/get_columns_count_service.proto
                                                ${Protobuf_INCLUDE_DIR}/get_rows_count_service.proto
                                                ${Protobuf_INCLUDE_DIR}/rename_table_service.proto
                                                ${Protobuf_INCLUDE_DIR}/row_id.proto
                                                ${Protobuf_INCLUDE_DIR}/rows_service.proto
                                                ${Protobuf_INCLUDE_DIR}/get_all_rows_service.proto)

PROTOBUF_GENERATE_GRPC_CPP(GRPC_SRCS GRPC_HDRS  ${Protobuf_INCLUDE_DIR}/database_getname_service.proto
                                                ${Protobuf_INCLUDE_DIR}/get_tables_count_service.proto
                                                ${Protobuf_INCLUDE_DIR}/create_table_service.proto
                                                ${Protobuf_INCLUDE_DIR}/get_all_tables_service.proto
                                                ${Protobuf_INCLUDE_DIR}/get_table_name_service.proto
                                                ${Protobuf_INCLUDE_DIR}/delete_table_service.proto
                                                ${Protobuf_INCLUDE_DIR}/create_cartesian_product_service.proto
                                                ${Protobuf_INCLUDE_DIR}/columns_service.proto
                                                ${Protobuf_INCLUDE_DIR}/get_columns_count_service.proto
                                                ${Protobuf_INCLUDE_DIR}/get_rows_count_service.proto
                                                ${Protobuf_INCLUDE_DIR}/rename_table_service.proto
                                                ${Protobuf_INCLUDE_DIR}/rows_service.proto
                                                ${Protobuf_INCLUDE_DIR}/get_all_rows_service.proto)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../core core)

add_executable(gRpcDatabaseServer
               Services/GetDatabaseNameService.cpp        Services/GetDatabaseNameService.h
               Services/GetTableCountService.cpp          Services/GetTableCountService.h
               Services/CreateTableService.cpp            Services/CreateTableService.h
               Services/GetAllTablesIdService.cpp         Services/GetAllTablesIdService.h
               Services/GetTableNameService.cpp           Services/GetTableNameService.h
               Services/DeleteTableService.cpp            Services/DeleteTableService.h
               Services/CreateCartesianProductService.cpp Services/CreateCartesianProductService.h
               Services/ColumnsService.cpp                Services/ColumnsService.h
               Services/GetColumnsCountService.cpp        Services/GetColumnsCountService.h
               Services/GetRowsCountService.cpp           Services/GetRowsCountService.h
               Services/RenameTableService.cpp            Services/RenameTableService.h
               Services/RowsService.cpp                   Services/RowsService.h
               Services/GetAllRowsService.cpp             Services/GetAllRowsService.h
               ServicesHolder.cpp ServicesHolder.h
               ${PROTO_SRCS} ${PROTO_HDRS}
               ${GRPC_SRCS}  ${GRPC_HDRS}
               main.cpp)

target_link_libraries(gRpcDatabaseServer
        core
        ${_REFLECTION}
        ${_GRPC_GRPCPP}
        ${_PROTOBUF_LIBPROTOBUF}
        ${Boost_LIBRARIES})

target_include_directories(gRpcDatabaseServer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../core/include)
