#include "GetDatabaseNameService.h"

// Core
#include "StringUtils.h"
#include "VirtualDatabase.h"

// STL
#include <iostream>

db_grpc_server::service::GetDatabaseName::GetDatabaseName(core::VirtualDatabase& db)
    : fDb{db} {}

grpc::Status db_grpc_server::service::GetDatabaseName::get(
    grpc::ServerContext* context, const Empty* request, DatabaseName* response)
{
    std::cout << "Request. Get database name." << std::endl;
    response->set_name(core::utils::wstringToUtf8(fDb.name()));
    return grpc::Status::OK;
}