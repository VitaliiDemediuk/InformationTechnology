#ifndef ROWSSERVICE_H
#define ROWSSERVICE_H

#include <rows_service.grpc.pb.h>

namespace core {
    class VirtualDatabase;
}

namespace db_grpc_server::service
{

class Rows final : public ::RowsService::Service
{
public:
    explicit Rows(core::VirtualDatabase& db);

    grpc::Status get(grpc::ServerContext* context, const RowId* request, ::Row* response) final;
    grpc::Status create(grpc::ServerContext* context, const TableId* request, RowCreateResponse* response) final;
    grpc::Status editCell(grpc::ServerContext* context, const CellEditRequest* request, Empty* response) final;
    grpc::Status deleteRow(grpc::ServerContext* context, const ::RowId* request, Empty* response) final;

private:
    core::VirtualDatabase& fDb;
};

} // db_server::service

#endif // ROWSSERVICE_H
