#include "ServicesHolder.h"

// STL
#include <vector>

// gRpc
#include <grpcpp/server_builder.h>

// Services
#include "Services/GetDatabaseNameService.h"
#include "Services/GetTableCountService.h"

struct db_grpc_server::ServicesHolder::Services
{
    std::vector<std::unique_ptr<grpc::Service>> list;
};

db_grpc_server::ServicesHolder::ServicesHolder(core::VirtualDatabase& db)
    : fServices(new ServicesHolder::Services())
{
    fServices->list.emplace_back(std::make_unique<service::GetDatabaseName>(db));
    fServices->list.emplace_back(std::make_unique<service::GetTableCount>(db));
}

db_grpc_server::ServicesHolder::~ServicesHolder() = default;

void db_grpc_server::ServicesHolder::registerAll(grpc::ServerBuilder& builder)
{
    for (auto& service : fServices->list) {
        builder.RegisterService(service.get());
    }
}