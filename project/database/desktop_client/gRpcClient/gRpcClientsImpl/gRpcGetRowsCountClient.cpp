#include "gRpcGetRowsCountClient.h"

// Core
#include "StringUtils.h"

// gRpc
#include <grpcpp/grpcpp.h>

db_grpc_client::RowsCountGetter::RowsCountGetter(std::shared_ptr<::grpc::Channel> channel)
    : fStub(::RowsCountGetter::NewStub(channel)) {}

size_t db_grpc_client::RowsCountGetter::getCount(core::TableId tableId) const
{
    ::TableId request;
    request.set_id(tableId);

    ::RowsCount reply;
    grpc::ClientContext context;

    const auto status = fStub->get(&context, request, &reply);

    if (!status.ok()) {
        throw std::runtime_error("db_grpc_client::RowsCountGetter. Status is not ok!");
    }

    return reply.count();
}
