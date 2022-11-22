cmake_minimum_required(VERSION 3.5)

project(database VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../core core)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(Protobuf_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../proto)

set(CMAKE_PREFIX_PATH "/home/vitalii/Qt/6.3.0/gcc_64")

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

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

set(PROJECT_SOURCES
        ${PROTO_SRCS} ${PROTO_HDRS}
        ${GRPC_SRCS}  ${GRPC_HDRS}
        main.cpp
        MainWindow.cpp                              MainWindow.h                           MainWindow.ui
        DesktopDatabaseClient.cpp                   DesktopDatabaseClient.h
        Dialogs/NewDbDialog.cpp                     Dialogs/NewDbDialog.h                  Dialogs/NewDbDialog.ui
        Dialogs/TableNameDialog.cpp                 Dialogs/TableNameDialog.h              Dialogs/TableNameDialog.ui
        Dialogs/ColumnInfoDialog.cpp                Dialogs/ColumnInfoDialog.h             Dialogs/ColumnInfoDialog.ui
        Dialogs/SaveDatabaseDialog.cpp              Dialogs/SaveDatabaseDialog.h           Dialogs/SaveDatabaseDialog.ui
        Dialogs/LoadFromMongoDbDialog.cpp           Dialogs/LoadFromMongoDbDialog.h        Dialogs/LoadFromMongoDbDialog.ui
        Dialogs/CreateCartesianProductDialog.cpp    Dialogs/CreateCartesianProductDialog.h Dialogs/CreateCartesianProductDialog.ui
        Dialogs/ConnectTogRpcServerDialog.cpp       Dialogs/ConnectTogRpcServerDialog.h    Dialogs/ConnectTogRpcServerDialog.ui
        Models/TableListModel.cpp                   Models/TableListModel.h
        Models/DbTableModel.cpp                     Models/DbTableModel.h
        Delegates/DbTableDelegate.cpp               Delegates/DbTableDelegate.h
                                                    Models/ResetableModel.h
        gRpcClient/gRpcDatabase.cpp                 gRpcClient/gRpcDatabase.h
        gRpcClient/gRpcTable.cpp                    gRpcClient/gRpcTable.h
        gRpcClient/gRpcClientsImpl/gRpcGetDatabaseNameClient.cpp        gRpcClient/gRpcClientsImpl/gRpcGetDatabaseNameClient.h
        gRpcClient/gRpcClientsImpl/gRpcGetTableCountClient.cpp          gRpcClient/gRpcClientsImpl/gRpcGetTableCountClient.h
        gRpcClient/gRpcClientsImpl/gRpcCreateTableClient.cpp            gRpcClient/gRpcClientsImpl/gRpcCreateTableClient.h
        gRpcClient/gRpcClientsImpl/gRpcGetAllTablesIdClient.cpp         gRpcClient/gRpcClientsImpl/gRpcGetAllTablesIdClient.h
        gRpcClient/gRpcClientsImpl/gRpcGetTableNameClient.cpp           gRpcClient/gRpcClientsImpl/gRpcGetTableNameClient.h
        gRpcClient/gRpcClientsImpl/gRpcDeleteTableClient.cpp            gRpcClient/gRpcClientsImpl/gRpcDeleteTableClient.h
        gRpcClient/gRpcClientsImpl/gRpcCreateCartesianProductClient.cpp gRpcClient/gRpcClientsImpl/gRpcCreateCartesianProductClient.h
        gRpcClient/gRpcClientsImpl/gRpcColumnsClient.cpp                gRpcClient/gRpcClientsImpl/gRpcColumnsClient.h
        gRpcClient/gRpcClientsImpl/gRpcGetColumnsCountClient.cpp        gRpcClient/gRpcClientsImpl/gRpcGetColumnsCountClient.h
        gRpcClient/gRpcClientsImpl/gRpcGetRowsCountClient.cpp           gRpcClient/gRpcClientsImpl/gRpcGetRowsCountClient.h
        gRpcClient/gRpcClientsImpl/gRpcRenameTableClient.cpp            gRpcClient/gRpcClientsImpl/gRpcRenameTableClient.h
        gRpcClient/gRpcClientsImpl/gRpcRowsClient.cpp                   gRpcClient/gRpcClientsImpl/gRpcRowsClient.h
        gRpcClient/gRpcClientsImpl/gRpcGetAllRowsClient.cpp             gRpcClient/gRpcClientsImpl/gRpcGetAllRowsClient.h
)

qt_add_executable(DatabaseDesktopClient
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_link_libraries(DatabaseDesktopClient PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                                                    core
                                                    ${_REFLECTION}
                                                    ${_GRPC_GRPCPP}
                                                    ${_PROTOBUF_LIBPROTOBUF})

target_include_directories(DatabaseDesktopClient PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../core/include)

target_include_directories(DatabaseDesktopClient PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Dialogs)
target_include_directories(DatabaseDesktopClient PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Models)
target_include_directories(DatabaseDesktopClient PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Delegates)
target_include_directories(DatabaseDesktopClient PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/gRpcClient)
target_include_directories(DatabaseDesktopClient PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/gRpcClient/gRpcClientsImpl)

set_target_properties(DatabaseDesktopClient PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(DatabaseDesktopClient)
endif()
