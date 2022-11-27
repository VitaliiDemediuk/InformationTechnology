#ifndef GETCOLUMNSCOUNTSERVICE_H
#define GETCOLUMNSCOUNTSERVICE_H

#include <get_columns_count_service.grpc.pb.h>

namespace core {
class VirtualDatabase;
}

namespace db_grpc_server::service
{

class GetColumnsCount final : public ::ColumnsCountGetter::Service
{
public:
    explicit GetColumnsCount(core::VirtualDatabase& db);

    grpc::Status get(grpc::ServerContext* context,
                     const TableId* request,
                     ColumnsCount* response) final;

private:
    core::VirtualDatabase& fDb;
};

} // db_server::service

#endif //GETCOLUMNSCOUNTSERVICE_H
