#ifndef SERVICESHOLDER_H
#define SERVICESHOLDER_H

#include <memory>

namespace grpc {
    class ServerBuilder;
}

namespace db_grpc_server
{

class ServicesHolder
{
    struct Services;

public:
    explicit ServicesHolder();
    ~ServicesHolder(); // for PIMPL

    void registerAll(grpc::ServerBuilder& builder);

private:
    std::unique_ptr<Services> services;
};

} // grpc_server

#endif //SERVICESHOLDER_H
