#include "gRpcGetAllRowsClient.h"

// gRpc
#include <grpcpp/grpcpp.h>

db_grpc_client::AllRowsGetter::AllRowsGetter(std::shared_ptr<::grpc::Channel> channel)
    : fStub(::AllRowsGetter::NewStub(channel)) {}

std::vector<core::TableId> db_grpc_client::AllRowsGetter::get(core::TableId tableId) const
{
    ::TableId request;
    request.set_id(tableId);

    ::AllRowsId reply;
    grpc::ClientContext context;

    const auto status = fStub->get(&context, request, &reply);

    if (!status.ok()) {
        throw std::runtime_error("db_grpc_client::AllRowsGetter. Status is not ok!");
    }

    std::vector<size_t> res;

    const auto& rowsId = reply.rowid();

    res.reserve(rowsId.size());
    for (const auto& rowId : rowsId) {
        res.push_back(rowId);
    }

    return res;
}
