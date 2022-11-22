#include "gRpcCreateTableClient.h"

// Core
#include "StringUtils.h"

// gRpc
#include <grpcpp/grpcpp.h>

db_grpc_client::TableCreator::TableCreator(std::shared_ptr<grpc::Channel> channel)
    : fStub(::TableCreator::NewStub(channel)) {}


core::TableId db_grpc_client::TableCreator::createTable(const std::wstring& tableName) const
{
    ::TableName request;
    request.set_name(core::utils::wstringToUtf8(tableName));

    ::TableId reply;
    grpc::ClientContext context;

    const auto status = fStub->create(&context, request, &reply);

    if (!status.ok()) {
        throw std::runtime_error("db_grpc_client::TableCreator. Status is not ok!");
    }

    return reply.id();
}
