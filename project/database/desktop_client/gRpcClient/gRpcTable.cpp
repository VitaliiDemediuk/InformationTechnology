#include "gRpcTable.h"

db_grpc_client::Table::Table(core::TableId id)
    : fId{id} {}

core::TableId db_grpc_client::Table::id() const
{
    return fId;
}

/// @todo implement!
const std::wstring& db_grpc_client::Table::name() const
{}

/// @todo implement!
void db_grpc_client::Table::changeName(std::wstring name)
{}

/// @todo implement!
const core::VirtualColumnInfo& db_grpc_client::Table::column(size_t idx) const
{}

/// @todo implement!
size_t db_grpc_client::Table::columnCount() const
{
    return 0;
}

/// @todo implement!
void db_grpc_client::Table::createColumn(std::unique_ptr<core::VirtualColumnInfo> info)
{}

/// @todo implement!
void db_grpc_client::Table::deleteColumn(size_t idx)
{}

/// @todo implement!
void db_grpc_client::Table::editColumnName(size_t idx, std::wstring name)
{}

/// @todo implement!
const core::Row& db_grpc_client::Table::row(size_t id) const
{}

/// @todo implement!
size_t db_grpc_client::Table::rowCount() const
{}

/// @todo implement!
size_t db_grpc_client::Table::createRow()
{}

/// @todo implement!
void db_grpc_client::Table::deleteRow(size_t idx)
{}

/// @todo implement!
void db_grpc_client::Table::setNewValue(size_t rowIdx, size_t columnIdx, core::CellData data)
{}

/// @todo implement!
void db_grpc_client::Table::forAllRow(std::function<void(size_t id, const core::Row&)> worker) const
{}
