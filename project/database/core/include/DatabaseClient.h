#ifndef DATABASECLIENT_H
#define DATABASECLIENT_H

// Local
#include "AbstractCommand.h"
#include "VirtualDatabase.h"
#include "VirtualValidators.h"

namespace core
{

class DatabaseClient : public AbstractExecutor,
                       public VirtualTableNameValidator,
                       public VirtualDatabaseNameValidator,
                       public VirtualColumnNameValidator
{
public:
    const std::wstring& databaseName() const;
    bool hasDatabase() const;
    void setNewDatabase(std::unique_ptr<VirtualDatabase> db);
    bool hasChanges() const;

    const core::VirtualTable* table(core::TableId id) const;
    const core::save_load::Information& lastSaveInfo() const;

    // AbstractExecutor
    void exec(std::unique_ptr<AbstractCommand> cmd) final;

    // VirtualValidator
    bool validateDatabaseName(const std::wstring& name) const final;
    bool validateTableName(const std::wstring& name) const final;
    bool validateColumnName(const std::wstring &name) const final;

protected:
    std::unique_ptr<VirtualDatabase> fDb;
    bool fHaveChanges = false;
};

} // core

#endif //DATABASECLIENT_H
