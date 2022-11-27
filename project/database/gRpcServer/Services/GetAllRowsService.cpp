#include "GetAllRowsService.h"

// Core
#include "VirtualDatabase.h"

// STL
#include <iostream>

db_grpc_server::service::AllRowsGetter::AllRowsGetter(core::VirtualDatabase& db)
    : fDb{db} {}

grpc::Status db_grpc_server::service::AllRowsGetter::get(
    grpc::ServerContext* context, const TableId* request, AllRowsId* response)
{
    const auto tableId = request->id();
    std::cout << "Request. Get all rows id of table " << tableId << std::endl;
    fDb.table(tableId).forAllRow([response] (size_t rowId, const core::Row&) {
        response->add_rowid(rowId);
    });
    return grpc::Status::OK;
}