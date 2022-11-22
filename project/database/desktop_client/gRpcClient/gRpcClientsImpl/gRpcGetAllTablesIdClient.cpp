#include "gRpcGetAllTablesIdClient.h"

// Core
#include "StringUtils.h"

// gRpc
#include <grpcpp/grpcpp.h>

db_grpc_client::AllTablesGetter::AllTablesGetter(std::shared_ptr<::grpc::Channel> channel)
    : fStub(::AllTablesIdGetter::NewStub(channel)) {}

std::vector<core::TableId> db_grpc_client::AllTablesGetter::get() const
{
    ::Empty request;
    ::AllTablesId reply;
    grpc::ClientContext context;

    const auto status = fStub->get(&context, request, &reply);

    if (!status.ok()) {
        throw std::runtime_error("db_grpc_client::AllTablesGetter. Status is not ok!");
    }

    std::vector<core::TableId> res;

    const auto& tablesId = reply.tablesid();

    res.reserve(tablesId.size());
    for (const auto& tableId : tablesId) {
        res.push_back(tableId.id());
    }

    return res;
}
