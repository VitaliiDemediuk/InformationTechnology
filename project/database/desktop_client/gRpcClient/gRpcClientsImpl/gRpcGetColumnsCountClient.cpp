#include "gRpcGetColumnsCountClient.h"

// Core
#include "StringUtils.h"

// gRpc
#include <grpcpp/grpcpp.h>

db_grpc_client::ColumnsCountGetter::ColumnsCountGetter(std::shared_ptr<::grpc::Channel> channel)
    : fStub(::ColumnsCountGetter::NewStub(channel)) {}

size_t db_grpc_client::ColumnsCountGetter::getCount(core::TableId tableId) const
{
    ::TableId request;
    request.set_id(tableId);

    ::ColumnsCount reply;
    grpc::ClientContext context;

    const auto status = fStub->get(&context, request, &reply);

    if (!status.ok()) {
        throw std::runtime_error("db_grpc_client::ColumnsCountGetter. Status is not ok!");
    }

    return reply.count();
}
