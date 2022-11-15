#include "DatabaseClient.h"

// Local
#include "VirtualDatabase.h"
#include "SaveLoadUtils.h"

// boost
#include <boost/throw_exception.hpp>

// Local
#include <Commands.h>

const std::wstring& core::DatabaseClient::databaseName() const
{
    if (!hasDatabase()) {
        BOOST_THROW_EXCEPTION(std::logic_error("There is no database!"));
    }
    return fDb->name();
}

bool core::DatabaseClient::hasDatabase() const
{
    return fDb != nullptr;
}

void core::DatabaseClient::setNewDatabase(std::unique_ptr<VirtualDatabase> db)
{
    fDb = std::move(db);
    fHaveChanges = false;
}

bool core::DatabaseClient::hasChanges() const
{
    return fHaveChanges;
}

const core::VirtualTable* core::DatabaseClient::table(core::TableId id) const
{
    if (!hasDatabase()) {
        return nullptr;
    }
    return &fDb->table(id);
}

const core::save_load::Information& core::DatabaseClient::lastSaveInfo() const
{
    if (!hasDatabase()) {
        static const core::save_load::Information emptyInfo{};
        return emptyInfo;
    }
    return fDb->lastSaveInfo();
}

void core::DatabaseClient::exec(std::unique_ptr<AbstractCommand> cmd)
{
    if (!hasDatabase()) {
        BOOST_THROW_EXCEPTION(std::logic_error("There is no database!"));
    }
    cmd->exec(*fDb);
    if (dynamic_cast<command::SaveDatabase*>(cmd.get())) {
        fHaveChanges = false;
    } else {
        fHaveChanges = true;
    }

}

bool core::DatabaseClient::validateDatabaseName(const std::wstring& name) const
{
    return !name.empty();
}

bool core::DatabaseClient::validateTableName(const std::wstring& name) const
{
    if (!hasDatabase()) {
        return false;
    }
    return fDb->validateTableName(name);
}

bool core::DatabaseClient::validateColumnName(const std::wstring &name) const
{
    if (!hasDatabase()) {
        return false;
    }
    return fDb->validateColumnName(name);
}
