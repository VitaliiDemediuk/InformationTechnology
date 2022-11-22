#ifndef GRPCDELETETABLECLIENT_H
#define GRPCDELETETABLECLIENT_H

// Core
#include "VirtualTable.h"

// gRpc
#include "delete_table_service.grpc.pb.h"

namespace db_grpc_client
{

class TableDeleter
{
public:
    explicit TableDeleter(std::shared_ptr<grpc::Channel> channel);

    void deleteTable(core::TableId tableId) const;

private:
    std::unique_ptr<::TableDeleter::Stub> fStub;
};

};

#endif // GRPCDELETETABLECLIENT_H
