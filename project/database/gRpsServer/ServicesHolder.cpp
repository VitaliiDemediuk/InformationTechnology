#include "ServicesHolder.h"

// STL
#include <vector>

// gRpc
#include <grpcpp/server_builder.h>

// Services
#include "Services/GetDatabaseNameService.h"
#include "Services/GetTableCountService.h"
#include "Services/CreateTableService.h"
#include "Services/GetAllTablesIdService.h"
#include "Services/GetTableNameService.h"
#include "Services/DeleteTableService.h"
#include "Services/CreateCartesianProductService.h"
#include "Services/ColumnsService.h"
#include "Services/GetRowsCountService.h"
#include "Services/GetColumnsCountService.h"
#include "Services/RenameTableService.h"
#include "Services/RowsService.h"
#include "Services/GetAllRowsService.h"


struct db_grpc_server::ServicesHolder::Services
{
    std::vector<std::unique_ptr<grpc::Service>> list;
};

db_grpc_server::ServicesHolder::ServicesHolder(core::VirtualDatabase& db)
    : fServices(new ServicesHolder::Services())
{
    fServices->list.emplace_back(std::make_unique<service::GetDatabaseName>(db));
    fServices->list.emplace_back(std::make_unique<service::GetTableCount>(db));
    fServices->list.emplace_back(std::make_unique<service::TableCreator>(db));
    fServices->list.emplace_back(std::make_unique<service::AllTablesIdGetter>(db));
    fServices->list.emplace_back(std::make_unique<service::GetTableName>(db));
    fServices->list.emplace_back(std::make_unique<service::DeleteTable>(db));
    fServices->list.emplace_back(std::make_unique<service::CreateCartesianProduct>(db));
    fServices->list.emplace_back(std::make_unique<service::Columns>(db));
    fServices->list.emplace_back(std::make_unique<service::GetRowsCount>(db));
    fServices->list.emplace_back(std::make_unique<service::GetColumnsCount>(db));
    fServices->list.emplace_back(std::make_unique<service::RenameTable>(db));
    fServices->list.emplace_back(std::make_unique<service::Rows>(db));
    fServices->list.emplace_back(std::make_unique<service::AllRowsGetter>(db));
}

db_grpc_server::ServicesHolder::~ServicesHolder() = default;

void db_grpc_server::ServicesHolder::registerAll(grpc::ServerBuilder& builder)
{
    for (auto& service : fServices->list) {
        builder.RegisterService(service.get());
    }
}