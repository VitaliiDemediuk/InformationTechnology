#include "GetTableCountService.h"

// Core
#include "VirtualDatabase.h"

// STL
#include <iostream>

db_grpc_server::service::GetTableCount::GetTableCount(core::VirtualDatabase& db)
    : fDb{db} {}

grpc::Status db_grpc_server::service::GetTableCount::get(
    grpc::ServerContext* context, const Empty* request, TablesCount* response)
{
    std::cout << "Request. Get table count." << std::endl;
    response->set_count(fDb.tableCount());
    return grpc::Status::OK;
}