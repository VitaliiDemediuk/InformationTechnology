#ifndef GRPCGETDATABASENAMECLIENT_H
#define GRPCGETDATABASENAMECLIENT_H

// gRpc
#include "database_getname_service.grpc.pb.h"

namespace db_grpc_client
{

class DatabaseNameGetter
{
public:
    explicit DatabaseNameGetter(std::shared_ptr<grpc::Channel> channel);

    std::wstring getName() const;

private:
    std::unique_ptr<::DatabaseNameGetter::Stub> fStub;
};

};

#endif // GRPCGETDATABASENAMECLIENT_H
