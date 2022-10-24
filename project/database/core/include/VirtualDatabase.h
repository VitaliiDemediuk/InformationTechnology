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

    using TablePtr      = VirtualTable*;
    using ConstTablePtr = const VirtualTable*;
    // Table operations
    virtual TablePtr table(TableId id) = 0;
    virtual ConstTablePtr table(TableId id) const = 0;

    virtual TablePtr createTable() = 0;
    virtual void deleteTable(TableId id) = 0;
    virtual TablePtr productTables(TableId firstId, TableId secondId) = 0;

    virtual ~VirtualDatabase() = default;
};

} // core

#endif //VIRTUALDATABASE_H
