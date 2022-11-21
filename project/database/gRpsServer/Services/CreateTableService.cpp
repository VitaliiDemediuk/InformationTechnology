#include "CreateTableService.h"

// Core
#include "StringUtils.h"
#include "VirtualDatabase.h"

// STL
#include <iostream>

db_grpc_server::service::TableCreator::TableCreator(core::VirtualDatabase& db)
    : fDb{db} {}

grpc::Status db_grpc_server::service::TableCreator::create(
    grpc::ServerContext* context, const TableName* request, TableId* response)
{
    const auto newName = core::utils::utf8ToWstring(request->name());
    std::wcout << L"Request. Create table with name: " << newName << std::endl;
    const auto id = fDb.createTable(std::move(newName));
    response->set_id(id);
    return grpc::Status::OK;
}