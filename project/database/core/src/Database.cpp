#include "Database.h"

// STL
#include <atomic>

// boost
#include <boost/throw_exception.hpp>
#include <MongoDbSaveLoadStrategy.h>
#include <SQLiteSaveLoadStrategy.h>

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

void core::Database::saveDatabase(const core::save_load::Information& saveInfo)
{
    fLastSaveInfo = saveInfo;
    const auto saveLoadStrategy = [&saveInfo] () -> std::unique_ptr<save_load::AbstractStrategy> {
        if (std::holds_alternative<core::save_load::CustomFileInfo>(saveInfo)) {
            return std::make_unique<save_load::CustomFileStrategy>(std::get<core::save_load::CustomFileInfo>(saveInfo));
        }
        if (std::holds_alternative<core::save_load::SQLiteInfo>(saveInfo)) {
            return std::make_unique<save_load::SQLiteStrategy>(std::get<core::save_load::SQLiteInfo>(saveInfo));
        }
        if (std::holds_alternative<core::save_load::MongoDbInfo>(saveInfo)) {
            return std::make_unique<save_load::MongoDbStrategy>(std::get<core::save_load::MongoDbInfo>(saveInfo));
        }
        return nullptr;
    }();
    saveLoadStrategy->save(*this);

    if (std::holds_alternative<core::save_load::CustomFileInfo>(saveInfo)) {
        auto newName = std::get<core::save_load::CustomFileInfo>(saveInfo).filePath.stem().wstring();
        changeName(std::move(newName));
    } else if (std::holds_alternative<core::save_load::SQLiteInfo>(saveInfo)) {
        auto newName = std::get<core::save_load::SQLiteInfo>(saveInfo).filePath.stem().wstring();
        changeName(std::move(newName));
    } else if (std::holds_alternative<core::save_load::MongoDbInfo>(saveInfo)) {
        auto newName = std::get<core::save_load::MongoDbInfo>(saveInfo).dbName;
        changeName(std::move(newName));
    }
}

const core::save_load::Information& core::Database::lastSaveInfo() const
{
    return fLastSaveInfo;
}

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

size_t core::Database::tableCount() const
{
    return fTables.size();
}

void core::Database::forAllTable(std::function<void(const VirtualTable&)> worker) const
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

core::VirtualTable& core::Database::createCartesianProduct(TableId firstId, TableId secondId)
{
    const auto& firstTable = table(firstId);
    const auto& secondTable = table(secondId);

    auto& newTable = createTable(firstTable.name() + L" * " + secondTable.name());

    const size_t newTableColumnsCount = firstTable.columnCount() + secondTable.columnCount() - 2;

    for (size_t i = 1; i < firstTable.columnCount(); ++i) {
        newTable.createColumn(firstTable.column(i).clone());
    }

    for (size_t i = 1; i < secondTable.columnCount(); ++i) {
        newTable.createColumn(secondTable.column(i).clone());
    }

    const auto rowId = newTable.createRow();

    firstTable.forAllRow([rowId, &newTable, &secondTable] (size_t, const Row& firstTableRow) {
        secondTable.forAllRow([rowId, &newTable, &firstTableRow] (size_t, const Row& secondTableRow) {
            for (size_t i = 1; i < firstTableRow.size(); ++i) {
                newTable.setNewValue(rowId, i, firstTableRow[i]);
            }
            const auto nFirstTableRowCells = firstTableRow.size();
            for (size_t i = 1; i < secondTableRow.size(); ++i) {
                newTable.setNewValue(rowId, nFirstTableRowCells + i - 1,secondTableRow[i]);
            }
        });
    });
}

bool core::Database::validateTableName(const std::wstring& name) const
{
    return !name.empty();
}

bool core::Database::validateColumnName(const std::wstring& name) const
{
    return !name.empty();
}
