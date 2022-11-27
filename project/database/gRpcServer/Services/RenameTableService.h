#ifndef RENAMETABLESERVICE_H
#define RENAMETABLESERVICE_H

#include <rename_table_service.grpc.pb.h>

namespace core {
    class VirtualDatabase;
}

namespace db_grpc_server::service
{

class RenameTable final : public ::RenameTableService::Service
{
public:
    explicit RenameTable(core::VirtualDatabase& db);

    grpc::Status get(grpc::ServerContext* context,
                     const RenameTableRequest* request,
                     Empty* response) final;

private:
    core::VirtualDatabase& fDb;
};

} // db_server::service

#endif //RENAMETABLESERVICE_H
