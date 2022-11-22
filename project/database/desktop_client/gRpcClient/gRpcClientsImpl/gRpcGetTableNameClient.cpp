#include "gRpcGetTableNameClient.h"

// Core
#include "StringUtils.h"

// gRpc
#include <grpcpp/grpcpp.h>

db_grpc_client::TableNameGetter::TableNameGetter(std::shared_ptr<::grpc::Channel> channel)
    : fStub(::TableNameGetter::NewStub(channel)) {}

std::wstring db_grpc_client::TableNameGetter::getName(core::TableId tableId) const
{
    ::TableId request;
    request.set_id(tableId);

    ::TableName reply;
    grpc::ClientContext context;

    const auto status = fStub->get(&context, request, &reply);

    if (!status.ok()) {
        throw std::runtime_error("db_grpc_client::TableNameGetter. Status is not ok!");
    }

    return core::utils::utf8ToWstring(reply.name());
}
