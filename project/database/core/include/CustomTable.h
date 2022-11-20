#ifndef CUSTOMTABLE_H
#define CUSTOMTABLE_H

// STL
#include <map>
#include <atomic>

// Local
#include "VirtualTable.h"
#include "AbstractTableFactory.h"
#include "CustomSaveLoadStrategy.h"
#include "MongoDbSaveLoadStrategy.h"
#include "SQLiteSaveLoadStrategy.h"

namespace core
{

namespace save_load
{
    class CustomFileStrategy;
}

class CustomTable: public VirtualTable
{
    using Super = VirtualTable;
public:
    explicit CustomTable(TableId id, std::wstring name);

    TableId id() const final;
    const std::wstring& name() const final;
    void changeName(std::wstring name) final;

    // Column info
    const VirtualColumnInfo& column(size_t idx) const final;
    size_t columnCount() const final;

    // Column operations
    void createColumn(std::unique_ptr<VirtualColumnInfo> info) final;
    void deleteColumn(size_t idx) final;
    void editColumnName(size_t idx, std::wstring name) final;

    // Row info
    const Row& row(size_t idx) const final;
    size_t rowCount() const final;

    // Row operations
    size_t createRow() final;
    void deleteRow(size_t id) final;
    void setNewValue(size_t rowId, size_t columnIdx, CellData data) final;
    void forAllRow(std::function<void(size_t id, const Row&)> worker) const final;

private:
    friend class save_load::CustomFileStrategy;
    friend class save_load::MongoDbStrategy;
    friend class save_load::SQLiteStrategy;

    std::atomic<size_t> lastId = 1;
    const TableId fId;
    std::wstring fName;
    std::vector<std::unique_ptr<VirtualColumnInfo>> fColumns;
    std::map<size_t, Row> fTable;
};

class CustomTableFactory: public AbstractTableFactory
{
public:
    FactoryType type() const final;

    std::unique_ptr<VirtualTable> createTable(TableId id, std::wstring name) const final;
};

} // core

#endif //CUSTOMTABLE_H
