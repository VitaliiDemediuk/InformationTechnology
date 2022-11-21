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

    // helper type for the visitor #4
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    // explicit deduction guide (not needed as of C++20)
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    void writeRow(SQLite::Database& sqlDb, const std::wstring& tableName, const core::Row& row)
    {
        std::wstring query = (boost::wformat(LR"(INSERT INTO "%1%" VALUES ()") % tableName).str();
        for (size_t i = 0; i < row.size() - 1; ++i) {
            query.append(L"?, ");
        }
        query.append(L"?);");

        SQLite::Statement st(sqlDb, core::utils::wstringToUtf8(query));
        for (int i = 1; i <= row.size(); ++i) {
            std::visit(overloaded {
                       [i, &st] (const std::monostate& value) { st.bind(i); },
                       [i, &st] (const core::column_t<core::DataType::INTEGER>& value) { st.bind(i, static_cast<int64_t>(value)); },
                       [i, &st] (const core::column_t<core::DataType::REAL>& value) { st.bind(i, value); },
                       [i, &st] (const core::column_t<core::DataType::CHAR>& value) { st.bind(i, core::utils::wstringToUtf8(std::wstring(1, value))); },
                       [i, &st] (const core::column_t<core::DataType::STRING>& value) { st.bind(i, core::utils::wstringToUtf8(value)); },
                       [i, &st] (const core::column_t<core::DataType::TEXT_FILE>& value) {
                            const std::string nameUtf8 = core::utils::wstringToUtf8(value.name);
                            const size_t blobDataSize = sizeof(size_t) + nameUtf8.size() + value.data.size();
                            std::unique_ptr<char[]> blobData(new char[blobDataSize]);
                            char* ptr = blobData.get();

                            // write filename size
                            *reinterpret_cast<size_t*>(ptr) = nameUtf8.size();
                            ptr += sizeof(size_t);

                            // write filename
                            std::copy(nameUtf8.begin(), nameUtf8.end(), ptr);
                            ptr += nameUtf8.size();

                            // write data
                           std::copy(value.data.begin(), value.data.end(), ptr);

                           st.bind(i, reinterpret_cast<void*>(blobData.get()), static_cast<int>(blobDataSize));
                       },
                       [i, &st] (const core::column_t<core::DataType::INTERVAL_INTEGER>& value) { st.bind(i, static_cast<int32_t>(value.data)); },
                       [] (auto&& value) { throw std::logic_error("Not implemented writing data of this type to custom file."); }},
                       row[i-1]);
        }

        st.exec();
    }

    void writeTable(SQLite::Database& sqlDb, const core::VirtualTable& table)
    {
        createTable(sqlDb, table);
        table.forAllRow([&sqlDb, &tableName = table.name()] (size_t, const core::Row& row) {
            writeRow(sqlDb, tableName, row);
        });
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

core::CellData readCell(SQLite::Statement& st, core::DataType type, size_t i)
{
    auto sqlCell = st.getColumn(i);
    if (sqlCell.isNull()) {
        return std::monostate();
    }

    switch (type) {
    case core::DataType::INTEGER: {
        return static_cast<core::column_t<core::DataType::INTEGER>>(sqlCell.getInt64());
    }
    case core::DataType::REAL: {
        return static_cast<core::column_t<core::DataType::REAL>>(sqlCell.getDouble());
    }
    case core::DataType::CHAR: {
        const auto str = core::utils::utf8ToWstring(sqlCell.getString());
        return str.at(0);
    }
    case core::DataType::STRING: {
        return core::utils::utf8ToWstring(sqlCell.getString());
    }
    case core::DataType::TEXT_FILE: {
        core::File file;

        auto* blobData = reinterpret_cast<const char*>(sqlCell.getBlob());
        auto size = sqlCell.getBytes();

        // read filename size
        const auto filenameSize = *reinterpret_cast<const size_t*>(blobData);
        blobData += sizeof(size_t);
        size -= sizeof(size_t);

        // read filename
        file.name = core::utils::utf8ToWstring(std::string_view(blobData, filenameSize));
        blobData += filenameSize;
        size -= static_cast<int>(filenameSize);

        // read file data
        file.data = std::string(blobData, size);

        return file;
    }
    case core::DataType::INTERVAL_INTEGER: {
        core::column_t<core::DataType::INTERVAL_INTEGER> interval;
        interval.data = static_cast<core::column_t<core::DataType::INTEGER>>(sqlCell.getInt64());
        return interval;
    }
    case core::DataType::NN: {
        throw std::logic_error("SQLite. Invalid data type during database reading,");
    }
    }
    return std::monostate{};
}

} // anon namespace

void core::save_load::SQLiteStrategy::readRows(SQLite::Database& sqlDb, core::CustomTable& table) const
{
    const auto selectAllQuery = (boost::wformat(L"SELECT * FROM %1%;") % table.name()).str();

    SQLite::Statement statement(sqlDb, core::utils::wstringToUtf8(selectAllQuery));

    while (statement.executeStep()) {
        core::Row row(table.columnCount());

        for (size_t i = 0; i < table.columnCount(); ++i) {
            row[i] = readCell(statement, table.column(i).dataType(), i);
        }

        const auto rowId = std::get<core::column_t<core::DataType::INTEGER>>(row[0]);
        table.fTable[rowId] = std::move(row);
    }
}

void core::save_load::SQLiteStrategy::getTableFromSQLiteDb(SQLite::Database& sqlDb, core::VirtualTable& table) const
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

    if (auto* customTable = dynamic_cast<core::CustomTable*>(&table)) {
        readRows(sqlDb, *customTable);
    } else {
        throw std::logic_error("SQLite db loading. Not implemented!");
    }
}

void core::save_load::SQLiteStrategy::getTablesFromSQLiteDb(SQLite::Database& sqlDb, core::Database& db) const
{
    const auto tableNames = getTableNames(sqlDb);
    for (const auto& tableName : tableNames) {
        const auto tableId = db.createTable(tableName);
        auto& table = db.table(tableId);
        getTableFromSQLiteDb(sqlDb, table);
    }
}

std::unique_ptr<core::VirtualDatabase> core::save_load::SQLiteStrategy::load() const
{
    SQLite::Database sqlDb(fSQLiteInfo.filePath, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);
    auto db = std::make_unique<core::Database>(fSQLiteInfo.filePath.stem().wstring(),
                                               std::make_unique<core::CustomTableFactory>());
    getTablesFromSQLiteDb(sqlDb, *db);
    return db;
}
