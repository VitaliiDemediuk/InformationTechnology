#include "gRpcTable.h"

// gRpc
#include <grpcpp/grpcpp.h>

// gRpc Clients
#include "gRpcGetTableNameClient.h"
#include "gRpcColumnsClient.h"
#include "gRpcGetColumnsCountClient.h"
#include "gRpcGetRowsCountClient.h"
#include "gRpcGetColumnsCountClient.h"
#include "gRpcRenameTableClient.h"

struct db_grpc_client::Table::Cache
{
    std::wstring tableName;
    std::unique_ptr<core::VirtualColumnInfo> lastColumn;
};

db_grpc_client::Table::Table(core::TableId id, std::string target)
    : fId{id}, fTarget{std::move(target)}, fCache(new Cache) {}

db_grpc_client::Table::~Table() = default;

core::TableId db_grpc_client::Table::id() const
{
    return fId;
}

const std::wstring& db_grpc_client::Table::name() const
{
    db_grpc_client::TableNameGetter getter(
         grpc::CreateChannel(fTarget, grpc::InsecureChannelCredentials()));
    fCache->tableName = getter.getName(id());

    return fCache->tableName;
}

void db_grpc_client::Table::changeName(std::wstring name)
{
    db_grpc_client::RenameTable renameTableClient(
         grpc::CreateChannel(fTarget, grpc::InsecureChannelCredentials()));
    renameTableClient.rename(id(), name);
}

const core::VirtualColumnInfo& db_grpc_client::Table::column(size_t idx) const
{
    db_grpc_client::Columns columnClient(
         grpc::CreateChannel(fTarget, grpc::InsecureChannelCredentials()));
    fCache->lastColumn = columnClient.get(id(), idx);
    return *fCache->lastColumn;
}

size_t db_grpc_client::Table::columnCount() const
{
    db_grpc_client::ColumnsCountGetter getter(
         grpc::CreateChannel(fTarget, grpc::InsecureChannelCredentials()));
    return getter.getCount(id());
}

void db_grpc_client::Table::createColumn(std::unique_ptr<core::VirtualColumnInfo> info)
{
    db_grpc_client::Columns columnClient(
         grpc::CreateChannel(fTarget, grpc::InsecureChannelCredentials()));
    columnClient.create(id(), *info);
}

void db_grpc_client::Table::deleteColumn(size_t idx)
{
    db_grpc_client::Columns columnClient(
         grpc::CreateChannel(fTarget, grpc::InsecureChannelCredentials()));
    columnClient.deleteColumn(id(), idx);
}

void db_grpc_client::Table::editColumnName(size_t idx, std::wstring name)
{
    auto columnInfo = column(idx).clone();
    columnInfo->changeName(std::move(name));
    db_grpc_client::Columns columnClient(
         grpc::CreateChannel(fTarget, grpc::InsecureChannelCredentials()));
    columnClient.edit(id(), idx, *columnInfo);
}

/// @todo implement!
const core::Row& db_grpc_client::Table::row(size_t id) const
{}

size_t db_grpc_client::Table::rowCount() const
{
    db_grpc_client::RowsCountGetter getter(
         grpc::CreateChannel(fTarget, grpc::InsecureChannelCredentials()));
    std::cout << getter.getCount(id()) << std::endl;
    return getter.getCount(id());
}

/// @todo implement!
size_t db_grpc_client::Table::createRow()
{}

/// @todo implement!GetColumnsCount
void db_grpc_client::Table::deleteRow(size_t idx)
{}

/// @todo implement!
void db_grpc_client::Table::setNewValue(size_t rowIdx, size_t columnIdx, core::CellData data)
{}

/// @todo implement!
void db_grpc_client::Table::forAllRow(std::function<void(size_t id, const core::Row&)> worker) const
{}
