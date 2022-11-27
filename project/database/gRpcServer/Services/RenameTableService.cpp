#include "RenameTableService.h"

// Core
#include "StringUtils.h"
#include "VirtualDatabase.h"

// STL
#include <iostream>

db_grpc_server::service::RenameTable::RenameTable(core::VirtualDatabase& db)
    : fDb{db} {}

grpc::Status db_grpc_server::service::RenameTable::get(
    grpc::ServerContext* context, const RenameTableRequest* request, Empty* response)
{
    const auto tableId = request->tableid().id();
    auto newName = core::utils::utf8ToWstring(request->name());
    std::cout << "Request. Rename table" << tableId << std::endl;
    fDb.table(tableId).changeName(std::move(newName));
    return grpc::Status::OK;
}
