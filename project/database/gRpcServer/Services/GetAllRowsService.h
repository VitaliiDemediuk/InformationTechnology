#ifndef GETALLROWSSERVICE_H
#define GETALLROWSSERVICE_H

#include <get_all_rows_service.grpc.pb.h>

namespace core {
    class VirtualDatabase;
}

namespace db_grpc_server::service
{

class AllRowsGetter final : public ::AllRowsGetter::Service
{
public:
    explicit AllRowsGetter(core::VirtualDatabase& db);

    grpc::Status get(grpc::ServerContext* context,
                     const TableId* request,
                     AllRowsId* response) final;

private:
    core::VirtualDatabase& fDb;
};

} // db_server::service

#endif //GETALLROWSSERVICE_H
