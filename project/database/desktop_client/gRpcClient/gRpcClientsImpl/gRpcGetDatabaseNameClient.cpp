#include "gRpcGetDatabaseNameClient.h"

// Core
#include "StringUtils.h"

// gRpc
#include <grpcpp/grpcpp.h>

db_grpc_client::DatabaseNameGetter::DatabaseNameGetter(std::shared_ptr<::grpc::Channel> channel)
    : fStub(::DatabaseNameGetter::NewStub(channel)) {}

std::wstring db_grpc_client::DatabaseNameGetter::getName() const
{
    ::Empty request;
    ::DatabaseName reply;
    grpc::ClientContext context;

    const auto status = fStub->get(&context, request, &reply);

    if (!status.ok()) {
        throw std::runtime_error("db_grpc_client::DatabaseNameGetter. Status is not ok!");
    }

    return core::utils::utf8ToWstring(reply.name());
}
