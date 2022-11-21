#ifndef DATABASECREATORSERVICE_H
#define DATABASECREATORSERVICE_H

#include <create_new_database.grpc.pb.h>

namespace db_grpc_server::service
{

class DatabaseCreator final : public ::DatabaseCreator::Service
{
public:
    grpc::Status create(grpc::ServerContext* context,
                        const CreateDatabaseRequest* request,
                        CreateDatabaseReply* response) final;
};

} // db_server::service

#endif //DATABASECREATORSERVICE_H
