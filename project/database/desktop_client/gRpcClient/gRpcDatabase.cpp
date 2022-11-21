#include "gRpcDatabase.h"

// STL
#include <atomic>

// Core
#include "AbstractTableFactory.h"

// boost
#include <boost/throw_exception.hpp>

db_grpc_client::Database::Database(const std::string& ip, uint16_t port)
    : target{ip + ":" + std::to_string(port)}, fTableFactory{} {}

db_grpc_client::Database::Database::~Database() = default;

/// @todo implement!
const std::wstring& db_grpc_client::Database::name() const
{
    static const std::wstring empty;
    return empty;
}

/// @todo implement!
bool db_grpc_client::Database::changeName(std::wstring name)
{
    return true;
}

/// @todo implement!
void db_grpc_client::Database::saveDatabase(const core::save_load::Information&)
{
    throw std::logic_error("core::Database::saveDatabase not implemented!");
}

auto db_grpc_client::Database::lastSaveInfo() const -> const core::save_load::Information&
{
    static const core::save_load::Information info{core::save_load::Remote{}};
    return info;
}

/// @todo implement!
void db_grpc_client::Database::deleteDatabase() {}

/// @todo implement!
core::VirtualTable& db_grpc_client::Database::table(core::TableId id)
{}

/// @todo implement!
const core::VirtualTable& db_grpc_client::Database::table(core::TableId id) const
{}

/// @todo implement!
size_t db_grpc_client::Database::tableCount() const
{
    return 0;
}

/// @todo implement!
void db_grpc_client::Database::forAllTable(const std::function<void(const core::VirtualTable&)>& worker) const
{}

/// @todo implement!
core::VirtualTable& db_grpc_client::Database::createTable(std::wstring name)
{}

/// @todo implement!
void db_grpc_client::Database::deleteTable(core::TableId id)
{}

/// @todo implement!
void db_grpc_client::Database::createCartesianProduct(core::TableId firstId, core::TableId secondId)
{}

bool db_grpc_client::Database::validateTableName(const std::wstring& name) const
{
    return !name.empty();
}

bool db_grpc_client::Database::validateColumnName(const std::wstring& name) const
{
    return !name.empty();
}
