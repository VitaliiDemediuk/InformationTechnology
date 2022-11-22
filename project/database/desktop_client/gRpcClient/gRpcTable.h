#ifndef GRPCTABLE_H
#define GRPCTABLE_H

// Core
#include "VirtualTable.h"

namespace db_grpc_client
{

class Table : public core::VirtualTable
{
    struct Cache;
public:
    explicit Table(core::TableId id, const std::string target);
    ~Table(); // for PIPL

    core::TableId id() const final;
    const std::wstring& name() const final;
    void changeName(std::wstring name) final;

    // Column info
    const core::VirtualColumnInfo& column(size_t idx) const final;
    size_t columnCount() const final;

    // Column operations
    void createColumn(std::unique_ptr<core::VirtualColumnInfo> info) final;
    void deleteColumn(size_t idx) final;
    void editColumnName(size_t idx, std::wstring name) final;

    // Row info
    const core::Row& row(size_t id) const final;
    size_t rowCount() const final;

    // Row operations
    size_t createRow() final;
    void deleteRow(size_t idx) final;
    void setNewValue(size_t rowIdx, size_t columnIdx, core::CellData data) final;
    void forAllRow(std::function<void(size_t id, const core::Row&)> worker) const final;

private:
    const core::TableId fId;
    const std::string fTarget;
    mutable std::unique_ptr<Table::Cache> fCache;
};

} // db_grpc_client

#endif // GRPCTABLE_H
