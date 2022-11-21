#include "DatabaseCreatorService.h"

#include <iostream>

grpc::Status db_grpc_server::service::DatabaseCreator::create(
    grpc::ServerContext*, const CreateDatabaseRequest* request, CreateDatabaseReply* response)
{
    std::cout << "Request. Create new database with name: " << request->dbname() << std::endl;
    response->set_wascreatedsuccessfully(true);
    return grpc::Status::OK;
}