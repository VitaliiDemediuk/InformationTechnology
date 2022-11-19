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

namespace sql_type
{
    constexpr std::wstring_view integer = L"INTEGER";
    constexpr std::wstring_view real = L"REAL";
    constexpr std::wstring_view text = L"TEXT";
    constexpr std::wstring_view blob = L"BLOB";
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

    std::wstring getCreateColumnLine(const core::VirtualColumnInfo& column)
    {
        using T = core::DataType;
        switch (column.dataType()) {
            case T::INTEGER: {
                return (boost::wformat(LR"("%1%" %2%)") % column.name() % sql_type::integer).str();
            }
            case T::REAL: {
                return (boost::wformat(LR"("%1%" %2%)") % column.name() % sql_type::real).str();
            }
            case T::CHAR: {
                return (boost::wformat(LR"("%1%" %2% CHECK(LENGTH("%1%") <= 1))") % column.name() % sql_type::text).str();
            }
            case T::STRING: {
                return (boost::wformat(LR"("%1%" %2%)") % column.name() % sql_type::text).str();
            }
            case T::TEXT_FILE: {
                return (boost::wformat(LR"("%1%" %2%)") % column.name() % sql_type::blob).str();
            }
            case T::INTERVAL_INTEGER: {
                const auto limitColumn = static_cast<const core::IntervalIntColumnInfo&>(column);
                return (boost::wformat(LR"("%1%" %2% CHECK("%1%" BETWEEN %3% AND %4%))")
                        % column.name()
                        % sql_type::blob
                        % limitColumn.lowerLimit()
                        % limitColumn.upperLimit()).str();
            }
            case T::NN: {
                throw std::logic_error("SQLite. Invalid data type during database creating");
            }
        }
        throw std::logic_error("SQLite. Not implemented.");
    }

    void createTable(SQLite::Database& sqlDb, const core::VirtualTable& table)
    {
        std::wstring query = L"CREATE TABLE table_name (";
        const size_t nColumn = table.columnCount();
        for (size_t i = 0; i < nColumn; ++i) {
            query.append(getCreateColumnLine(table.column(i)));
            if (i < nColumn - 1) {
                query.append(L", ");
            }
        }
        query.append(L");");

        sqlDb.exec(core::utils::wstringToUtf8(query));
    }

    void writeTable(SQLite::Database& sqlDb, const core::VirtualTable& table)
    {
        createTable(sqlDb, table);
    }

} // anon namespace



void core::save_load::SQLiteStrategy::save(const VirtualDatabase &db) const
{
    SQLite::Database sqlDb(fSQLiteInfo.filePath, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);
    SQLite::Transaction transaction(sqlDb);

    dropAllTables(sqlDb);

    db.forAllTable([&sqlDb] (const core::VirtualTable& table) {
        writeTable(sqlDb, table);
    });

    transaction.commit();
}

std::unique_ptr<core::VirtualDatabase> core::save_load::SQLiteStrategy::load() const
{
    return {};
}
