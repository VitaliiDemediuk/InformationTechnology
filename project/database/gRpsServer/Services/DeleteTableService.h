#ifndef DELETETABLESERVICE_H
#define DELETETABLESERVICE_H

#include <delete_table_service.grpc.pb.h>

namespace core {
    class VirtualDatabase;
}

namespace db_grpc_server::service
{

class DeleteTable final : public ::TableDeleter::Service
{
public:
    explicit DeleteTable(core::VirtualDatabase& db);

    grpc::Status deleteTable(grpc::ServerContext* context,
                             const TableId* request,
                             Empty* response) final;

private:
    core::VirtualDatabase& fDb;
};

} // db_server::service

#endif //DELETETABLESERVICE_H
