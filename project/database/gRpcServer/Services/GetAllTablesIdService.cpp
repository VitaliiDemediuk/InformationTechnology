#include "GetAllTablesIdService.h"

// Core
#include "StringUtils.h"
#include "VirtualDatabase.h"

// STL
#include <iostream>

db_grpc_server::service::AllTablesIdGetter::AllTablesIdGetter(core::VirtualDatabase& db)
    : fDb{db} {}

grpc::Status db_grpc_server::service::AllTablesIdGetter::get(
    grpc::ServerContext* context, const Empty* request, AllTablesId* response)
{
    std::cout << "Request. Get all tables id." << std::endl;
    fDb.forAllTable([response] (const core::VirtualTable& table) {
        response->add_tablesid()->set_id(table.id());
    });
    return grpc::Status::OK;
}