#ifndef GRPCGETDATABASENAMECLIENT_H
#define GRPCGETDATABASENAMECLIENT_H

// Core
#include <VirtualTable.h>

// gRpc
#include "get_table_name_service.grpc.pb.h"

namespace db_grpc_client
{

class TableNameGetter
{
public:
    explicit TableNameGetter(std::shared_ptr<grpc::Channel> channel);

    std::wstring getName(core::TableId tableId) const;

private:
    std::unique_ptr<::TableNameGetter::Stub> fStub;
};

};

#endif // GRPCGETDATABASENAMECLIENT_H
