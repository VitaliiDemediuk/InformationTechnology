#include "gRpcGetTableCountClient.h"

// Core
#include "StringUtils.h"

// gRpc
#include <grpcpp/grpcpp.h>

db_grpc_client::TableCountGetter::TableCountGetter(std::shared_ptr<::grpc::Channel> channel)
    : fStub(::TablesCountGetter::NewStub(channel)) {}

size_t db_grpc_client::TableCountGetter::getCount() const
{
    ::Empty request;

    ::TablesCount reply;
    grpc::ClientContext context;

    const auto status = fStub->get(&context, request, &reply);

    if (!status.ok()) {
        throw std::runtime_error("db_grpc_client::TableCountGetter. Status is not ok!");
    }

    return reply.count();
}
