#ifndef CUSTOMTABLE_H
#define CUSTOMTABLE_H

// STL
#include <map>

// Local
#include "VirtualTable.h"

namespace core
{

class CustomTable: public VirtualTable
{
    using Super = VirtualTable;
public:
    explicit CustomTable(std::wstring name);

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
    size_t addRow(Row&& data) final;
    void deleteRow(size_t key) final;
    void editRow(size_t key, const std::function<void(Row&)>& worker) final;

private:
    const TableId fId;
    std::wstring fName;
    std::vector<std::unique_ptr<VirtualColumnInfo>> fColumns;
    std::map<size_t, Row> fTable;
};

} // core

#endif //CUSTOMTABLE_H
