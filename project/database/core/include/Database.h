#ifndef DATABASE_H
#define DATABASE_H

// STL
#include <map>

// Local
#include "AbstractTableFactory.h"
#include "VirtualDatabase.h"
#include "CustomSaveLoadStrategy.h"
#include "MongoDbSaveLoadStrategy.h"

namespace core
{

namespace save_load
{
    class CustomFileStrategy;
}

class Database: public VirtualDatabase
{
public:

    explicit Database(std::wstring name, std::unique_ptr<const AbstractTableFactory> factory);

    // Database operations
    const std::wstring& name() const final;
    bool changeName(std::wstring name) final;
    void saveDatabase(const save_load::Information& saveInfo) final;
    const save_load::Information& lastSaveInfo() const final;
    void deleteDatabase() final;

    // Table operations
    VirtualTable& table(TableId id) final;
    const VirtualTable& table(TableId id) const final;
    size_t tableCount() const final;
    void forAllTable(const std::function<void(const VirtualTable&)>& worker) const final;

    core::TableId createTable(std::wstring name) final;
    void deleteTable(TableId id) final;
    void createCartesianProduct(TableId firstId, TableId secondId) final;

    // VirtualValidator
    bool validateTableName(const std::wstring& name) const final;
    bool validateColumnName(const std::wstring& name) const final;

private:
    friend class save_load::CustomFileStrategy;

    std::wstring fName;
    const std::unique_ptr<const AbstractTableFactory> fTableFactory;
    std::map<TableId, std::unique_ptr<VirtualTable>> fTables;
    save_load::Information fLastSaveInfo;
};

} // core

#endif //DATABASE_H
