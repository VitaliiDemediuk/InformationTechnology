#ifndef SERVICESHOLDER_H
#define SERVICESHOLDER_H

// STL
#include <memory>

namespace core {
    class VirtualDatabase;
}

namespace grpc {
    class ServerBuilder;
}

namespace db_grpc_server
{

class ServicesHolder
{
    struct Services;

public:
    explicit ServicesHolder(core::VirtualDatabase& db);
    ~ServicesHolder(); // for PIMPL

    void registerAll(grpc::ServerBuilder& builder);

private:
    std::unique_ptr<Services> fServices;
};

} // grpc_server

#endif //SERVICESHOLDER_H
