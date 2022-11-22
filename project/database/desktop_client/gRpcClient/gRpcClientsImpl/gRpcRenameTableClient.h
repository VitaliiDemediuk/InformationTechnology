#ifndef GRPCRENAMETABLECLIENT_H
#define GRPCRENAMETABLECLIENT_H

// Core
#include <VirtualTable.h>

// gRpc
#include "rename_table_service.grpc.pb.h"

namespace db_grpc_client
{

class RenameTable
{
public:
    explicit RenameTable(std::shared_ptr<grpc::Channel> channel);

    void rename(core::TableId tableId, const std::wstring& name) const;

private:
    std::unique_ptr<::RenameTableService::Stub> fStub;
};

};

#endif // GRPCRENAMETABLECLIENT_H
