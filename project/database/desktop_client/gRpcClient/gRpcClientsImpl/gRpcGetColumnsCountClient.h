#ifndef GRPCGETCOLUMNSCOUNTCLIENT_H
#define GRPCGETCOLUMNSCOUNTCLIENT_H

// Core
#include "VirtualTable.h"

// gRpc
#include "get_columns_count_service.grpc.pb.h"

namespace db_grpc_client
{

class ColumnsCountGetter
{
public:
    explicit ColumnsCountGetter(std::shared_ptr<grpc::Channel> channel);

    size_t getCount(core::TableId tableId) const;

private:
    std::unique_ptr<::ColumnsCountGetter::Stub> fStub;
};

};

#endif // GRPCGETCOLUMNSCOUNTCLIENT_H
