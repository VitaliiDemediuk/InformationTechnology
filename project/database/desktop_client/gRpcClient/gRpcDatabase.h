#ifndef GRPCDATABASE_H
#define GRPCDATABASE_H

// STL
#include <map>

// Local
#include "VirtualDatabase.h"
#include "SaveLoadUtils.h"

namespace core {
    class AbstractTableFactory;
} // core

namespace db_grpc_client
{

class Database: public core::VirtualDatabase
{
    struct Cache;
public:

    explicit Database(const std::string& ip, uint16_t port);
    ~Database();

    // Database operations
    const std::wstring& name() const final;
    bool changeName(std::wstring name) final;
    void saveDatabase(const core::save_load::Information& info) final;
    const core::save_load::Information& lastSaveInfo() const final;
    void deleteDatabase() final;

    // Table operations
    core::VirtualTable& table(core::TableId id) final;
    const core::VirtualTable& table(core::TableId id) const final;
    size_t tableCount() const final;
    void forAllTable(const std::function<void(const core::VirtualTable&)>& worker) const final;

    core::TableId createTable(std::wstring name) final;
    void deleteTable(core::TableId id) final;
    void createCartesianProduct(core::TableId firstId, core::TableId secondId) final;

    // VirtualValidator
    bool validateTableName(const std::wstring& name) const final;
    bool validateColumnName(const std::wstring& name) const final;

private:

    const std::string fTarget;
    std::unique_ptr<core::AbstractTableFactory> fTableFactory;
    mutable std::unique_ptr<Database::Cache> fCache;
};

} // db_grpc_client

#endif //GRPCDATABASE_H
