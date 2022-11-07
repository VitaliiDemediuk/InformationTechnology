#ifndef DATABASE_H
#define DATABASE_H

// STL
#include <map>

// Local
#include "VirtualDatabase.h"
#include "CustomSaveLoadStrategy.h"

namespace core
{

class AbstractTableFactory;

class Database: public VirtualDatabase
{
public:

    explicit Database(std::wstring name, std::unique_ptr<const AbstractTableFactory> factory);

    // Database operations
    const std::wstring& name() const final;
    bool changeName(std::wstring name) final;
    void saveDatabase(const SaveInformation& saveInfo) final;
    const SaveInformation& lastSaveInfo() const final;
    void deleteDatabase() final;

    // Table operations
    VirtualTable& table(TableId id) final;
    const VirtualTable& table(TableId id) const final;
    size_t tableCount() const final;
    void forAllTable(std::function<void(const VirtualTable&)> worker) const final;

    VirtualTable& createTable(std::wstring name) final;
    void deleteTable(TableId id) final;
    VirtualTable& productTables(TableId firstId, TableId secondId) final;

    // VirtualValidator
    bool validateTableName(const std::wstring& name) const final;
    bool validateColumnName(const std::wstring& name) const final;

private:
    friend class CustomSaveLoadStrategy;

    std::wstring fName;
    const std::unique_ptr<const AbstractTableFactory> fTableFactory;
    std::map<TableId, std::unique_ptr<VirtualTable>> fTables;
    SaveInformation fLastSaveInfo;
};

} // core

#endif //DATABASE_H
