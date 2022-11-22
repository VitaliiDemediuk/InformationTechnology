#ifndef GRPCGETALLTABLESCLIENT_H
#define GRPCGETALLTABLESCLIENT_H

// Core
#include "VirtualTable.h"

// gRpc
#include "get_all_tables_service.grpc.pb.h"

namespace db_grpc_client
{

class AllTablesGetter
{
public:
    explicit AllTablesGetter(std::shared_ptr<grpc::Channel> channel);

    std::vector<core::TableId> get() const;

private:
    std::unique_ptr<::AllTablesIdGetter::Stub> fStub;
};

};

#endif // GRPCGETALLTABLESCLIENT_H
