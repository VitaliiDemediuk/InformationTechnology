#include "DeleteTableService.h"

// Core
#include "VirtualDatabase.h"

// STL
#include <iostream>

db_grpc_server::service::DeleteTable::DeleteTable(core::VirtualDatabase& db)
    : fDb{db} {}

grpc::Status db_grpc_server::service::DeleteTable::deleteTable(
    grpc::ServerContext* context, const TableId* request, Empty* response)
{
    const auto id = request->id();
    std::cout << "Request. Delete table by id: " << id << std::endl;
    fDb.deleteTable(id);
    return grpc::Status::OK;
}