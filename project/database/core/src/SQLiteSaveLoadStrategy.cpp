#include "SQLiteSaveLoadStrategy.h"

#include <iostream>

// Local
#include "StringUtils.h"
#include "VirtualDatabase.h"

// SQLite
#include <SQLiteCpp/Database.h>

core::save_load::SQLiteStrategy::SQLiteStrategy(SQLiteInfo sqliteInfo)
    : fSQLiteInfo(std::move(sqliteInfo)) {}

namespace sql_query
{
    constexpr const char* GET_ALL_TABLE_NAMES = "SELECT name FROM sqlite_schema "
                                                "WHERE type = 'table' AND name NOT LIKE 'sqlite_%' "
                                                "ORDER BY 1;";
}

namespace {
    std::vector<std::wstring> getTableNames(SQLite::Database& sqlDb)
    {
        SQLite::Statement allTableNameQuery(sqlDb, sql_query::GET_ALL_TABLE_NAMES);
        std::vector<std::wstring> res;
        while(allTableNameQuery.executeStep()) {
            res.push_back(core::utils::utf8ToWstring(allTableNameQuery.getColumn(0).getString()));
        }
        return res;
    }

    void dropAllTables(SQLite::Database& sqlDb)
    {
        for (const auto& name : getTableNames(sqlDb)) {
            std::wcout << name << std::endl;
        }
    }
}

void core::save_load::SQLiteStrategy::save(const VirtualDatabase &db) const
{
    SQLite::Database sqlDb(fSQLiteInfo.filePath, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);\
    dropAllTables(sqlDb);
}

std::unique_ptr<core::VirtualDatabase> core::save_load::SQLiteStrategy::load() const
{
    SQLite::Database sqlDb(fSQLiteInfo.filePath, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);\
    dropAllTables(sqlDb);
    return {};
}
