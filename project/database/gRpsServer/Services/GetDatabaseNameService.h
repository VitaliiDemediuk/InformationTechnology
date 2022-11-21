#ifndef GETDATABASENAMESERVICE_H
#define GETDATABASENAMESERVICE_H

#include <database_name.grpc.pb.h>

namespace db_grpc_server::service
{

class GetDatabaseName final : public ::DatabaseNameGetter::Service
{
public:
    grpc::Status get(grpc::ServerContext* context,
                     const Empty* request,
                     DatabaseNameReply* response) final;
};

} // db_server::service

#endif //GETDATABASENAMESERVICE_H
