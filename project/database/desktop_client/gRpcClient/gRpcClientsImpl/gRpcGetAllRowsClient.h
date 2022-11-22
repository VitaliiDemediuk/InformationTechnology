#ifndef GRPCGETALLROWSCLIENT_H
#define GRPCGETALLROWSCLIENT_H

// Core
#include "VirtualTable.h"

// gRpc
#include "get_all_rows_service.grpc.pb.h"

namespace db_grpc_client
{

class AllRowsGetter
{
public:
    explicit AllRowsGetter(std::shared_ptr<grpc::Channel> channel);

    std::vector<size_t> get(core::TableId tableId) const;

private:
    std::unique_ptr<::AllRowsGetter::Stub> fStub;
};

};

#endif // GRPCGETALLROWSCLIENT_H
