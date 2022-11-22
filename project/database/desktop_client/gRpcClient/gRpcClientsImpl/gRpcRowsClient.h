#ifndef GRPCROWCLIENT_H
#define GRPCROWCLIENT_H

// Core
#include <VirtualTable.h>

// gRpc
#include "rows_service.grpc.pb.h"

namespace db_grpc_client
{

class Rows
{
public:
    explicit Rows(std::shared_ptr<grpc::Channel> channel);

    core::Row get(const core::VirtualTable& table, size_t rowId) const;
    size_t create(core::TableId tableId) const;
    void editCell(core::TableId tableId, size_t colIdx, size_t rowId, const core::CellData& cellData) const;
    void deleteRow(core::TableId tableId, size_t rowId) const;

private:
    std::unique_ptr<::RowsService::Stub> fStub;
};

};

#endif // GRPCROWCLIENT_H
