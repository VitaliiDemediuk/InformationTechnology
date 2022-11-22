#ifndef GETALLTABLESIDSERVICE_H
#define GETALLTABLESIDSERVICE_H

#include <get_all_tables_service.grpc.pb.h>

namespace core {
    class VirtualDatabase;
}

namespace db_grpc_server::service
{

class AllTablesIdGetter final : public ::AllTablesIdGetter::Service
{
public:
    explicit AllTablesIdGetter(core::VirtualDatabase& db);

    grpc::Status get(grpc::ServerContext* context,
                     const Empty* request,
                     AllTablesId* response) final;

private:
    core::VirtualDatabase& fDb;
};

} // db_server::service

#endif //GETALLTABLESIDSERVICE_H
