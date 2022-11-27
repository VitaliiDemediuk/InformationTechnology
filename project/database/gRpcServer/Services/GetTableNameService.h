#ifndef GETTABLENAMESERVICE_H
#define GETTABLENAMESERVICE_H

#include <get_table_name_service.grpc.pb.h>

namespace core {
    class VirtualDatabase;
}

namespace db_grpc_server::service
{

class GetTableName final : public ::TableNameGetter::Service
{
public:
    explicit GetTableName(core::VirtualDatabase& db);

    grpc::Status get(grpc::ServerContext* context,
                     const TableId* request,
                     TableName* response) final;

private:
    core::VirtualDatabase& fDb;
};

} // db_server::service

#endif //GETTABLENAMESERVICE_H
