#include "Database.h"

// Local
#include "AbstractTableFactory.h"

// STL
#include <atomic>

// boost
#include <boost/throw_exception.hpp>

core::Database::Database(std::wstring name, std::unique_ptr<const AbstractTableFactory> factory)
    : fName(std::move(name)), fTableFactory{std::move(factory)} {}

const std::wstring& core::Database::name() const
{
    return fName;
}

bool core::Database::changeName(std::wstring name)
{
    fName = std::move(name);
}

void core::Database::saveDatabase(const SaveInformation& saveInfo)
{}

void core::Database::deleteDatabase()
{}

core::VirtualTable& core::Database::table(TableId id)
{
    return const_cast<core::VirtualTable&>(static_cast<const Database*>(this)->table(id));
}

const core::VirtualTable& core::Database::table(TableId id) const
{
    const auto it = fTables.find(id);
    if (it == fTables.end()) {
        BOOST_THROW_EXCEPTION(std::logic_error("Invalid table id!"));
    }
    return *it->second;
}

auto core::Database::forAllTable(std::function<void(const VirtualTable&)> worker) const -> const VirtualTable&
{
    for (const auto& [_, table] : fTables) {
        worker(*table);
    }
}

core::VirtualTable& core::Database::createTable(std::wstring name)
{
    static std::atomic<core::TableId> lastTableId{1};
    const auto newId = lastTableId++;
    return *(fTables[newId] = fTableFactory->createTable(newId, std::move(name)));
}

void core::Database::deleteTable(TableId id)
{
    const auto it = fTables.find(id);
    if (it == fTables.end()) {
        BOOST_THROW_EXCEPTION(std::logic_error("Invalid table id!"));
    }
    fTables.erase(it);
}

core::VirtualTable& core::Database::productTables(TableId firstId, TableId secondId)
{

}

bool core::Database::validateTableName(const std::wstring& name) const
{
    return !name.empty();
}

bool core::Database::validateColumnName(const std::wstring& name) const
{
    return !name.empty();
}
