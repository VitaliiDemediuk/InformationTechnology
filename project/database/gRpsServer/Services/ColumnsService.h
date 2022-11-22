#ifndef COLUMNSSERVICE_H
#define COLUMNSSERVICE_H

#include <columns_service.grpc.pb.h>

namespace core {
    class VirtualDatabase;
}

namespace db_grpc_server::service
{

class Columns final : public ::ColumnsService::Service
{
public:
    explicit Columns(core::VirtualDatabase& db);

    grpc::Status get(grpc::ServerContext* context, const ::ColumnId* request, ::ColumnInfo* response) final;
    grpc::Status create(grpc::ServerContext* context, const ::CreateColumnRequest* request, ::Empty* response) final;
    grpc::Status edit(grpc::ServerContext* context, const ::ColumnEditRequest* request, ::Empty* response) final;
    grpc::Status deleteColumn(grpc::ServerContext* context, const ::ColumnId* request, ::Empty* response) final;

private:
    core::VirtualDatabase& fDb;
};

} // db_server::service

#endif // COLUMNSSERVICE_H
