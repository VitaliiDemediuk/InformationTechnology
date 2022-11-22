#ifndef GRPCCREATECARTESIANPRODUCTCLIENT_H
#define GRPCCREATECARTESIANPRODUCTCLIENT_H

// Core
#include "VirtualTable.h"

// gRpc
#include "create_cartesian_product_service.grpc.pb.h"

namespace db_grpc_client
{

class CartesianProductCreator
{
public:
    explicit CartesianProductCreator(std::shared_ptr<grpc::Channel> channel);

    void create(core::TableId firstId, core::TableId secondId) const;

private:
    std::unique_ptr<::CartesianProductCreator::Stub> fStub;
};

};

#endif // GRPCCREATECARTESIANPRODUCTCLIENT_H
