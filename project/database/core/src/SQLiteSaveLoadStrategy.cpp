#include "SQLiteSaveLoadStrategy.h"

#include <iostream>

// STD
#include <regex>

// Boost
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

// Local
#include "Database.h"
#include "CustomTable.h"
#include "StringUtils.h"

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
                        % sql_type::integer
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
        std::wstring query = (boost::wformat(L"CREATE TABLE %1% (") % table.name()).str();
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

namespace
{

struct SQLiteColumnInfo
{
    std::wstring name;
    std::wstring type;
};

auto getTableColumnNames(SQLite::Database& sqlDb, const std::wstring& tableName) -> std::vector<SQLiteColumnInfo>
{
    const auto columnNamesQuery = (boost::wformat(LR"(PRAGMA table_info("%1%");)") % tableName).str();
    SQLite::Statement allColumnInfoQuery(sqlDb, core::utils::wstringToUtf8(columnNamesQuery));
    std::vector<SQLiteColumnInfo> res;
    while(allColumnInfoQuery.executeStep()) {
        constexpr static size_t nameIdx = 1;
        constexpr static size_t typeIdx = 2;
        res.push_back({.name = core::utils::utf8ToWstring(allColumnInfoQuery.getColumn(nameIdx).getString()),
                       .type = core::utils::utf8ToWstring(allColumnInfoQuery.getColumn(typeIdx).getString())});
    }
    return res;
}

std::wstring getSqlCreateTableQuery(SQLite::Database& sqlDb, const std::wstring& tableName)
{
    const auto queryStr = (boost::wformat(LR"(SELECT sql FROM sqlite_master
                                                 WHERE tbl_name = "%1%" AND type = "table")") % tableName ).str();
    SQLite::Statement query(sqlDb, core::utils::wstringToUtf8(queryStr));
    if (query.executeStep()) {
        return core::utils::utf8ToWstring(query.getColumn(0).getString());
    }
    return {};
}

auto getColumnInfo(const SQLiteColumnInfo& sqliteColumnInfo, const std::wstring& createTableQuery)
    -> std::unique_ptr<core::VirtualColumnInfo>
{
    auto tableName = sqliteColumnInfo.name;

    const auto createColumnLineWithCheck = [&tableName, &createTableQuery] () -> std::wstring {
        const std::wstring regexStr = (boost::wformat(LR"("%1%"[^,]*?CHECK.+?(,|\)\)))") % tableName).str();
        const std::wregex regex{regexStr};
        std::wsmatch sm;
        if (std::regex_search(createTableQuery, sm, regex)) {
            return sm.str();
        }
        return {};
    }();

    std::wcout << createColumnLineWithCheck << std::endl;

    std::unique_ptr<core::VirtualColumnInfo> colInfo;
    if (sqliteColumnInfo.type == sql_type::integer) {
        if (createColumnLineWithCheck.empty()) {
            colInfo = std::make_unique<core::ColumnInfo>(core::DataType::INTEGER, sqliteColumnInfo.name);
        } else {
            static const std::wregex numberRegex(LR"([+-]?\b[0-9]+\b)");
            auto bIt = std::wsregex_iterator(createColumnLineWithCheck.begin(), createColumnLineWithCheck.end(), numberRegex);
            const auto numbers = std::vector(bIt, std::wsregex_iterator());
            if (numbers.size() != 2) {
                throw std::logic_error("Invalid createColumnLineWithCheck. Expected two numbers");
            }

            const auto lowerLimit = boost::lexical_cast<int64_t>(numbers[0].str());
            const auto upperLimit = boost::lexical_cast<int64_t>(numbers[1].str());
            colInfo = std::make_unique<core::IntervalIntColumnInfo>(lowerLimit, upperLimit, sqliteColumnInfo.name);
        }
    } else if (sqliteColumnInfo.type == sql_type::real) {
        colInfo = std::make_unique<core::ColumnInfo>(core::DataType::REAL, sqliteColumnInfo.name);
    } else if (sqliteColumnInfo.type == sql_type::text) {
        if (createColumnLineWithCheck.empty()) {
            colInfo = std::make_unique<core::ColumnInfo>(core::DataType::STRING, sqliteColumnInfo.name);
        } else {
            colInfo = std::make_unique<core::ColumnInfo>(core::DataType::CHAR, sqliteColumnInfo.name);
        }
    } else if (sqliteColumnInfo.type == sql_type::blob) {
        colInfo = std::make_unique<core::ColumnInfo>(core::DataType::TEXT_FILE, sqliteColumnInfo.name);
    }
    return colInfo;
}

void getTableFromSQLiteDb(SQLite::Database& sqlDb, core::VirtualTable& table)
{
    const auto& tableName = table.name();
    const auto sqliteColumnsInfo = getTableColumnNames(sqlDb, tableName);
    const auto createTableQuery = [&sqlDb, &tableName] () {
        auto query = getSqlCreateTableQuery(sqlDb, tableName);
        boost::algorithm::trim(query);
        return query;
    } () ;

    for (size_t i = 1; i < sqliteColumnsInfo.size(); ++i) {
        table.createColumn(getColumnInfo(sqliteColumnsInfo[i], createTableQuery));
    }
}

void getTablesFromSQLiteDb(SQLite::Database& sqlDb, core::Database& db)
{
    const auto tableNames = getTableNames(sqlDb);
    for (const auto& tableName : tableNames) {
        auto& table = db.createTable(tableName);
        getTableFromSQLiteDb(sqlDb, table);
    }
}

} // anon namespace

std::unique_ptr<core::VirtualDatabase> core::save_load::SQLiteStrategy::load() const
{
    SQLite::Database sqlDb(fSQLiteInfo.filePath, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);
    auto db = std::make_unique<core::Database>(fSQLiteInfo.filePath.stem().wstring(),
                                               std::make_unique<core::CustomTableFactory>());
    getTablesFromSQLiteDb(sqlDb, *db);
    return db;
}
