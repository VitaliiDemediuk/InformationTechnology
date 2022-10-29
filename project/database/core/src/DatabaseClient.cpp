#include "DatabaseClient.h"

// Local
#include "VirtualDatabase.h"

// boost
#include <boost/throw_exception.hpp>

const std::wstring& core::DatabaseClient::databaseName() const
{
    if (!haveDatabase()) {
        BOOST_THROW_EXCEPTION(std::logic_error("There is no database!"));
    }
    return fDb->name();
}

bool core::DatabaseClient::haveDatabase() const
{
    return fDb != nullptr;
}

void core::DatabaseClient::setNewDatabase(std::unique_ptr<VirtualDatabase> db)
{
    fDb = std::move(db);
}

void core::DatabaseClient::exec(std::unique_ptr<AbstractCommand> cmd)
{
    if (!haveDatabase()) {
        BOOST_THROW_EXCEPTION(std::logic_error("There is no database!"));
    }
    cmd->exec(*fDb);
}

bool core::DatabaseClient::validateDatabaseName(const std::wstring& name) const
{
    return !name.empty();
}

bool core::DatabaseClient::validateTableName(const std::wstring& name) const
{
    if (!haveDatabase()) {
        return false;
    }
    return fDb->validateTableName(name);
}