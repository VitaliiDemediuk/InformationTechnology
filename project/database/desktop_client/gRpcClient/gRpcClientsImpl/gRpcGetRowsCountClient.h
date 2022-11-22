#ifndef GRPCGETROWSCOUNTCLIENT_H
#define GRPCGETROWSCOUNTCLIENT_H

// Core
#include "VirtualTable.h"

// gRpc
#include "get_rows_count_service.grpc.pb.h"

namespace db_grpc_client
{

class RowsCountGetter
{
public:
    explicit RowsCountGetter(std::shared_ptr<grpc::Channel> channel);

    size_t getCount(core::TableId tableId) const;

private:
    std::unique_ptr<::RowsCountGetter::Stub> fStub;
};

};

#endif // GRPCGETROWSCOUNTCLIENT_H
