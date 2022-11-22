#ifndef GRPCCOLUMNCLIENT_H
#define GRPCCOLUMNCLIENT_H

// Core
#include <VirtualTable.h>

// gRpc
#include "columns_service.grpc.pb.h"

namespace db_grpc_client
{

class Columns
{
public:
    explicit Columns(std::shared_ptr<grpc::Channel> channel);

    std::unique_ptr<core::VirtualColumnInfo> get(core::TableId tableId, size_t colIdx) const;
    void create(core::TableId tableId, const core::VirtualColumnInfo& colInfo) const;
    void edit(core::TableId tableId, size_t colIdx, const core::VirtualColumnInfo& colInfo) const;
    void deleteColumn(core::TableId tableId, size_t colIdx) const;

private:
    std::unique_ptr<::ColumnsService::Stub> fStub;
};

};

#endif // GRPCCOLUMNCLIENT_H
