#ifndef GETDATABASENAMESERVICE_H
#define GETDATABASENAMESERVICE_H

#include <database_getname_service.grpc.pb.h>

namespace core {
    class VirtualDatabase;
}

namespace db_grpc_server::service
{

class GetDatabaseName final : public ::DatabaseNameGetter::Service
{
public:
    explicit GetDatabaseName(core::VirtualDatabase& db);

    grpc::Status get(grpc::ServerContext* context,
                     const Empty* request,
                     DatabaseName* response) final;

private:
    core::VirtualDatabase& fDb;
};

} // db_server::service

#endif //GETDATABASENAMESERVICE_H
