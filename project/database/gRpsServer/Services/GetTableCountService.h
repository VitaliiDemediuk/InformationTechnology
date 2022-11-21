#ifndef GETTABLECOUNTSERVICE_H
#define GETTABLECOUNTSERVICE_H

#include <table_count_service.grpc.pb.h>

namespace core {
    class VirtualDatabase;
}

namespace db_grpc_server::service
{

class GetTableCount final : public ::TableCountGetter::Service
{
public:
    explicit GetTableCount(core::VirtualDatabase& db);

    grpc::Status get(grpc::ServerContext* context,
                     const Empty* request,
                     TableCount* response) final;

private:
    core::VirtualDatabase& fDb;
};

} // db_server::service

#endif //GETTABLECOUNTSERVICE_H
