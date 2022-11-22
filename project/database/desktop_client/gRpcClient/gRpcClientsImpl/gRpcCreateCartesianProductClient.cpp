#include "gRpcCreateCartesianProductClient.h"

// gRpc
#include <grpcpp/grpcpp.h>

db_grpc_client::CartesianProductCreator::CartesianProductCreator(std::shared_ptr<grpc::Channel> channel)
    : fStub(::CartesianProductCreator::NewStub(channel)) {}


void db_grpc_client::CartesianProductCreator::create(core::TableId firstId, core::TableId secondId) const
{
    auto requestFirstId = new ::TableId;
    requestFirstId->set_id(firstId);

    auto requestSecondId = new ::TableId;
    requestSecondId->set_id(secondId);

    ::CartesianProductReguest request;
    request.set_allocated_firstid(requestFirstId);
    request.set_allocated_secondid(requestSecondId);

    ::Empty reply;
    grpc::ClientContext context;

    const auto status = fStub->create(&context, request, &reply);

    if (!status.ok()) {
        throw std::runtime_error("db_grpc_client::CartesianProductCreator. Status is not ok!");
    }
}
