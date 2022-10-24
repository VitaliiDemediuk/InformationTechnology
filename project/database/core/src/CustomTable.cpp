#include "CustomTable.h"

// boost
#include <boost/throw_exception.hpp>

#define CHECK_COLUMN_INDEX(idx, column_arr) \
    if (idx >= column_arr.size()) { \
        BOOST_THROW_EXCEPTION(std::logic_error("Invalid column index!")); \
    }

core::CustomTable::CustomTable(TableId id, std::wstring name)
    : fId{id}, fName{std::move(name)} {}

core::TableId core::CustomTable::id() const
{
    return fId;
}

const std::wstring& core::CustomTable::name() const
{
    return fName;
}

void core::CustomTable::changeName(std::wstring name)
{
    fName = std::move(name);
}

const core::VirtualColumnInfo& core::CustomTable::column(size_t idx) const
{
    CHECK_COLUMN_INDEX(idx, fColumns)
    return *fColumns[idx];
}

size_t core::CustomTable::columnCount() const
{
    return fColumns.size();
}

void core::CustomTable::createColumn(std::unique_ptr<VirtualColumnInfo> info)
{
    fColumns.push_back(std::move(info));
}

void core::CustomTable::deleteColumn(size_t idx)
{
    CHECK_COLUMN_INDEX(idx, fColumns)
    fColumns.erase(fColumns.begin() + idx);
}

void core::CustomTable::editColumnName(size_t idx, std::wstring name)
{
    CHECK_COLUMN_INDEX(idx, fColumns)
    fColumns[idx]->changeName(std::move(name));
}

const core::Row& core::CustomTable::row(size_t key) const
{
    const auto it = fTable.find(key);
    if (it == fTable.end()) {
        BOOST_THROW_EXCEPTION(std::logic_error("Invalid row key!"));
    }
    return it->second;
}

size_t core::CustomTable::rowCount() const
{
    return fTable.size();
}

size_t core::CustomTable::addRow(Row&& data)
{
    static size_t lastKey = 1;
    fTable[lastKey++] = std::move(data);
}

void core::CustomTable::deleteRow(size_t key)
{
    const auto it = fTable.find(key);
    if (it == fTable.end()) {
        BOOST_THROW_EXCEPTION(std::logic_error("Invalid row key!"));
    }
    fTable.erase(it);
}

void core::CustomTable::editRow(size_t key, const std::function<void(Row&)>& worker)
{
    auto it = fTable.find(key);
    if (it == fTable.end()) {
        BOOST_THROW_EXCEPTION(std::logic_error("Invalid row key!"));
    }
    worker(it->second);
}

core::FactoryType core::CustomTableFactory::type() const
{
    return FactoryType::Custom;
}

std::unique_ptr<core::VirtualTable> core::CustomTableFactory::createTable(TableId id, std::wstring name) const
{
    return std::make_unique<CustomTable>(id, std::move(name));
}