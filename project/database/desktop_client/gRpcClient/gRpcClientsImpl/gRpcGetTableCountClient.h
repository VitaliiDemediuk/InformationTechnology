#ifndef GRPCGETTABLECOUNTCLIENT_H
#define GRPCGETTABLECOUNTCLIENT_H

// gRpc
#include "table_count_service.grpc.pb.h"

namespace db_grpc_client
{

class TableCountGetter
{
public:
    explicit TableCountGetter(std::shared_ptr<grpc::Channel> channel);

    size_t getCount() const;

private:
    std::unique_ptr<::TableCountGetter::Stub> fStub;
};

};

#endif // GRPCGETTABLECOUNTCLIENT_H
