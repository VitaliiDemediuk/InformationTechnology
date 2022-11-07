#ifndef VIRTUALDATABASE_H
#define VIRTUALDATABASE_H

// Core headers
#include "VirtualTable.h"
#include "VirtualValidators.h"

// STL
#include <filesystem>

namespace core
{

using SaveInformation = std::variant<std::monostate,
                                     std::filesystem::path>;

class VirtualDatabase: public VirtualTableNameValidator,
                       public VirtualColumnNameValidator
{
public:

    // Database operations
    virtual const std::wstring& name() const = 0;
    virtual bool changeName(std::wstring name) = 0;
    virtual void saveDatabase(const SaveInformation& saveInfo) = 0;
    virtual const SaveInformation& lastSaveInfo() const = 0;
    virtual void deleteDatabase() = 0;

    // Table operations
    virtual VirtualTable& table(TableId id) = 0;
    virtual const VirtualTable& table(TableId id) const = 0;
    virtual size_t tableCount() const = 0;
    virtual void forAllTable(std::function<void(const VirtualTable&)> worker) const = 0;

    virtual VirtualTable& createTable(std::wstring name) = 0;
    virtual void deleteTable(TableId id) = 0;
    virtual VirtualTable& productTables(TableId firstId, TableId secondId) = 0;

    virtual ~VirtualDatabase() = default;
};

} // core

#endif //VIRTUALDATABASE_H
