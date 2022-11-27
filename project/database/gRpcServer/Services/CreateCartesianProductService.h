#ifndef CREATECARTESIANPRODUCTSERVICE_H
#define CREATECARTESIANPRODUCTSERVICE_H

#include <create_cartesian_product_service.grpc.pb.h>

namespace core {
    class VirtualDatabase;
}

namespace db_grpc_server::service
{

class CreateCartesianProduct final : public ::CartesianProductCreator::Service
{
public:
    explicit CreateCartesianProduct(core::VirtualDatabase& db);

    grpc::Status create(grpc::ServerContext* context,
                        const CartesianProductReguest* request,
                        Empty* response) final;

private:
    core::VirtualDatabase& fDb;
};

} // db_server::service

#endif //CREATECARTESIANPRODUCTSERVICE_H
