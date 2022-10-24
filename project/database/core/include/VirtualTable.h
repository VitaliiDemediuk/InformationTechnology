#ifndef VIRTUALTABLE_H
#define VIRTUALTABLE_H

// Local
#include "ColumnUtils.h"

// STL
#include <functional>

namespace core
{

using TableId = std::size_t;

class VirtualTable // interface
{
public:
    virtual TableId id() const = 0;
    virtual const std::wstring& name() const = 0;
    virtual void changeName(std::wstring name) = 0;

    // Column info
    virtual const VirtualColumnInfo& column(size_t idx) const = 0;
    virtual size_t columnCount() const = 0;

    // Column operations
    virtual void createColumn(std::unique_ptr<VirtualColumnInfo> info) = 0;
    virtual void deleteColumn(size_t idx) = 0;
    virtual void editColumnName(size_t idx, std::wstring name) = 0;

    // Row info
    virtual const Row& row(size_t idx) const = 0;
    virtual size_t rowCount() const = 0;

    // Row operations
    virtual size_t addRow(Row&& data) = 0;
    virtual void deleteRow(size_t idx) = 0;
    virtual void editRow(size_t idx, const std::function<void(Row&)>& worker) = 0;


    virtual ~VirtualTable() = default;
};

} // core

#endif //VIRTUALTABLE_H
