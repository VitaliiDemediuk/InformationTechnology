#ifndef GETROWSCOUNTSERVICE_H
#define GETROWSCOUNTSERVICE_H

#include <get_rows_count_service.grpc.pb.h>

namespace core {
    class VirtualDatabase;
}

namespace db_grpc_server::service
{

class GetRowsCount final : public ::RowsCountGetter::Service
{
public:
    explicit GetRowsCount(core::VirtualDatabase& db);

    grpc::Status get(grpc::ServerContext* context,
                     const TableId* request,
                     RowsCount* response) final;

private:
    core::VirtualDatabase& fDb;
};

} // db_server::service

#endif //GETROWSCOUNTSERVICE_H
