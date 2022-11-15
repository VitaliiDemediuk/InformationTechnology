#include "SQLiteSaveLoadStrategy.h"

#include <iostream>

// Boost
#include <boost/format.hpp>

// Local
#include "StringUtils.h"
#include "VirtualDatabase.h"

// SQLite
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>

core::save_load::SQLiteStrategy::SQLiteStrategy(SQLiteInfo sqliteInfo)
    : fSQLiteInfo(std::move(sqliteInfo)) {}

namespace sql_query
{
    constexpr const char* GET_ALL_TABLE_NAMES = "SELECT name FROM sqlite_schema "
                                                "WHERE type = 'table' AND name NOT LIKE 'sqlite_%' "
                                                "ORDER BY 1;";

    constexpr const wchar_t* DROP_TABLE = L"DROP TABLE %1%";
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

    void dropTable(SQLite::Database& sqlDb, std::wstring tableName)
    {
        const std::wstring query = (boost::wformat(sql_query::DROP_TABLE) % tableName).str();
        sqlDb.exec(core::utils::wstringToUtf8(query));
    }

    void dropAllTables(SQLite::Database& sqlDb)
    {
        const auto tableNames = getTableNames(sqlDb);
        for (auto& tableName : tableNames) {
            dropTable(sqlDb, tableName);
        }
    }
}

void core::save_load::SQLiteStrategy::save(const VirtualDatabase &db) const
{
    SQLite::Database sqlDb(fSQLiteInfo.filePath, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);\
    dropAllTables(sqlDb);
    SQLite::Transaction transaction(sqlDb);
    transaction.commit();


}

std::unique_ptr<core::VirtualDatabase> core::save_load::SQLiteStrategy::load() const
{
    return {};
}
