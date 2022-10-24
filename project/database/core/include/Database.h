#ifndef DATABASE_H
#define DATABASE_H

// STL
#include <unordered_map>

// Local
#include "VirtualDatabase.h"

namespace core
{

class AbstractTableFactory;

class Database: public VirtualDatabase
{
public:

    explicit Database(std::unique_ptr<const AbstractTableFactory> factory);

    // Database operations
    void saveDatabase(const SaveInformation& saveInfo) final;
    void deleteDatabase() final;

    // Table operations
    VirtualTable& table(TableId id) final;
    const VirtualTable& table(TableId id) const final;

    VirtualTable& createTable(std::wstring name) final;
    void deleteTable(TableId id) final;
    VirtualTable& productTables(TableId firstId, TableId secondId) final;

private:
    const std::unique_ptr<const AbstractTableFactory> fTableFactory;
    std::unordered_map<TableId, std::unique_ptr<VirtualTable>> fTables;
};

} // core

#endif //DATABASE_H
