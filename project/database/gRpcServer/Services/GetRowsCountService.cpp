#include "GetRowsCountService.h"

// Core
#include "VirtualDatabase.h"

// STL
#include <iostream>

db_grpc_server::service::GetRowsCount::GetRowsCount(core::VirtualDatabase& db)
    : fDb{db} {}

grpc::Status db_grpc_server::service::GetRowsCount::get(
    grpc::ServerContext* context, const TableId* request, RowsCount* response)
{
    const auto tableId = request->id();
    std::cout << "Request. Get columns count for table " << tableId << std::endl;
    response->set_count(fDb.table(tableId).rowCount());
    return grpc::Status::OK;
}