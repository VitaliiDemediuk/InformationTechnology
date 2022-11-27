#include "CreateCartesianProductService.h"

// Core
#include "VirtualDatabase.h"

// STL
#include <iostream>

db_grpc_server::service::CreateCartesianProduct::CreateCartesianProduct(core::VirtualDatabase& db)
    : fDb{db} {}

grpc::Status db_grpc_server::service::CreateCartesianProduct::create(
    grpc::ServerContext* context, const CartesianProductReguest* request, Empty* response)
{
    const auto firstId = request->firstid().id();
    const auto secondId = request->secondid().id();
    std::cout << "Request. Create cartesian product of " << firstId << " & " << secondId << std::endl;
    fDb.createCartesianProduct(firstId, secondId);
    return grpc::Status::OK;
}
