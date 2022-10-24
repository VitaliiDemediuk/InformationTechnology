#ifndef VIRTUALDATABASE_H
#define VIRTUALDATABASE_H


// Core headers
#include "VirtualTable.h"

namespace core
{

using SaveInformation = std::variant<std::filesystem::path>;

class VirtualDatabase // interface
{
public:

    // Database operations
    virtual void saveDatabase(const SaveInformation& saveInfo) = 0;
    virtual void deleteDatabase() = 0;

    // Table operations
    virtual VirtualTable& table(TableId id) = 0;
    virtual const VirtualTable& table(TableId id) const = 0;

    virtual VirtualTable& createTable(std::wstring name) = 0;
    virtual void deleteTable(TableId id) = 0;
    virtual VirtualTable& productTables(TableId firstId, TableId secondId) = 0;

    virtual ~VirtualDatabase() = default;
};

} // core

#endif //VIRTUALDATABASE_H
