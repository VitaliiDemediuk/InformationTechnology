#include "GetTableNameService.h"

// Core
#include "StringUtils.h"
#include "VirtualDatabase.h"

// STL
#include <iostream>

db_grpc_server::service::GetTableName::GetTableName(core::VirtualDatabase& db)
    : fDb{db} {}

grpc::Status db_grpc_server::service::GetTableName::get(
    grpc::ServerContext* context, const TableId* request, TableName* response)
{
    const auto id = request->id();
    std::cout << "Request. Get table name by id: " << id << std::endl;
    response->set_name(core::utils::wstringToUtf8(fDb.table(id).name()));
    return grpc::Status::OK;
}