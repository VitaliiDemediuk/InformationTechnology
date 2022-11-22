#include "gRpcDeleteTableClient.h"

// gRpc
#include <grpcpp/grpcpp.h>

db_grpc_client::TableDeleter::TableDeleter(std::shared_ptr<grpc::Channel> channel)
    : fStub(::TableDeleter::NewStub(channel)) {}


void db_grpc_client::TableDeleter::deleteTable(core::TableId tableId) const
{
    ::TableId request;
    request.set_id(tableId);

    ::Empty reply;
    grpc::ClientContext context;

    const auto status = fStub->deleteTable(&context, request, &reply);

    if (!status.ok()) {
        throw std::runtime_error("db_grpc_client::TableDeleter. Status is not ok!");
    }
}
