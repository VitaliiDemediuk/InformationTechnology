#include "gRpcRenameTableClient.h"

// Core
#include "StringUtils.h"

// gRpc
#include <grpcpp/grpcpp.h>

db_grpc_client::RenameTable::RenameTable(std::shared_ptr<::grpc::Channel> channel)
    : fStub(::RenameTableService::NewStub(channel)) {}

void db_grpc_client::RenameTable::rename(core::TableId tableId, const std::wstring& name) const
{
    ::RenameTableRequest request;
    auto* protoTabelId = new ::TableId;
    protoTabelId->set_id(tableId);
    request.set_allocated_tableid(protoTabelId);
    request.set_allocated_name(new std::string(core::utils::wstringToUtf8(name)));

    ::Empty reply;
    grpc::ClientContext context;

    const auto status = fStub->get(&context, request, &reply);

    if (!status.ok()) {
        throw std::runtime_error("db_grpc_client::RenameTable. Status is not ok!");
    }
}
