#include "ServicesHolder.h"

// STL
#include <vector>

// gRpc
#include <grpcpp/server_builder.h>

// Services
#include "Services/GetDatabaseNameService.h"

struct db_grpc_server::ServicesHolder::Services
{
    std::vector<std::unique_ptr<grpc::Service>> list;
};

db_grpc_server::ServicesHolder::ServicesHolder()
    : services(new ServicesHolder::Services())
{
    services->list.emplace_back(std::make_unique<service::GetDatabaseName>());
}

db_grpc_server::ServicesHolder::~ServicesHolder() = default;

void db_grpc_server::ServicesHolder::registerAll(grpc::ServerBuilder& builder)
{
    for (auto& service : services->list) {
        builder.RegisterService(service.get());
    }
}