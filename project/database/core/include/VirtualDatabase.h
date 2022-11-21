#ifndef VIRTUALDATABASE_H
#define VIRTUALDATABASE_H

// Core headers
#include "VirtualTable.h"
#include "VirtualValidators.h"
#include "SaveLoadUtils.h"

// STL
#include <filesystem>

namespace core
{

class VirtualDatabase: public VirtualTableNameValidator,
                       public VirtualColumnNameValidator
{
public:

    // Database operations
    virtual const std::wstring& name() const = 0;
    virtual bool changeName(std::wstring name) = 0;
    virtual void saveDatabase(const save_load::Information& saveInfo) = 0;
    virtual const save_load::Information& lastSaveInfo() const = 0;
    virtual void deleteDatabase() = 0;

    // Table operations
    virtual VirtualTable& table(TableId id) = 0;
    virtual const VirtualTable& table(TableId id) const = 0;
    virtual size_t tableCount() const = 0;
    virtual void forAllTable(const std::function<void(const VirtualTable&)>& worker) const = 0;

    virtual core::TableId createTable(std::wstring name) = 0;
    virtual void deleteTable(TableId id) = 0;
    virtual void createCartesianProduct(TableId firstId, TableId secondId) = 0;

    virtual ~VirtualDatabase() = default;
};

} // core

#endif //VIRTUALDATABASE_H
