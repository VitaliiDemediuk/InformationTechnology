#ifndef GRPCCREATETABLECLIENT_H
#define GRPCCREATETABLECLIENT_H

// Core
#include "VirtualTable.h"

// gRpc
#include "create_table_service.grpc.pb.h"

namespace db_grpc_client
{

class TableCreator
{
public:
    explicit TableCreator(std::shared_ptr<grpc::Channel> channel);

    core::TableId createTable(const std::wstring& tableName) const;

private:
    std::unique_ptr<::TableCreator::Stub> fStub;
};

};

#endif // GRPCCREATETABLECLIENT_H
