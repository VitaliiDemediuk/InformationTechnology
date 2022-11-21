#include "GetDatabaseNameService.h"

#include <iostream>

grpc::Status db_grpc_server::service::GetDatabaseName::get(
    grpc::ServerContext* context, const Empty* request, DatabaseNameReply* response)
{
    std::cout << "Request. Get database name." << std::endl;
    response->set_name("db");
    return grpc::Status::OK;
}