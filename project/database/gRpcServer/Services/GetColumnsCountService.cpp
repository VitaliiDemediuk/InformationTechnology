#include "GetColumnsCountService.h"

// Core
#include "VirtualDatabase.h"

// STL
#include <iostream>

db_grpc_server::service::GetColumnsCount::GetColumnsCount(core::VirtualDatabase& db)
    : fDb{db} {}

grpc::Status db_grpc_server::service::GetColumnsCount::get(
    grpc::ServerContext* context, const TableId* request, ColumnsCount* response)
{
    const auto tableId = request->id();
    std::cout << "Request. Get rows count for table " << tableId << std::endl;
    response->set_count(fDb.table(tableId).columnCount());
    return grpc::Status::OK;
}