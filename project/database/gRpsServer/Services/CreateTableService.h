#ifndef CREATETABLESERVICE_H
#define CREATETABLESERVICE_H

#include <create_table_service.grpc.pb.h>

namespace core {
    class VirtualDatabase;
}

namespace db_grpc_server::service
{

class TableCreator final : public ::TableCreator::Service
{
public:
    explicit TableCreator(core::VirtualDatabase& db);

    grpc::Status create(grpc::ServerContext* context,
                        const TableName* request,
                        TableId* response) final;

private:
    core::VirtualDatabase& fDb;
};

} // db_server::service

#endif //CREATETABLESERVICE_H
