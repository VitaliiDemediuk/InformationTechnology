#define BOOST_TEST_MODULE core_unit_tests

// Boost
#include <boost/test/included/unit_test.hpp>

// STD
#include <unordered_map>

// Core
#include "CustomTable.h"
#include "Database.h"

BOOST_AUTO_TEST_CASE( CreateEmptyDatabase )
{
    const std::wstring databaseName = L"database name";
    auto database = core::Database{databaseName, std::make_unique<core::CustomTableFactory>()};
    BOOST_CHECK(database.name() == databaseName);
    BOOST_CHECK_EQUAL(database.tableCount(), 0);
}

BOOST_AUTO_TEST_CASE( RenameDatabase )
{
    auto database = core::Database{L"database name", std::make_unique<core::CustomTableFactory>()};
    const std::wstring newDatabaseName = L"New database name";
    database.changeName(newDatabaseName);
    BOOST_CHECK(database.name() == newDatabaseName);
}

BOOST_AUTO_TEST_CASE( CreateTable )
{
    auto database = core::Database{L"database name", std::make_unique<core::CustomTableFactory>()};

    const size_t tableCount = 10;

    std::unordered_map<core::TableId, std::wstring> tableNames;
    for (size_t i = 0; i < tableCount; ++i) {
        const auto name = L"table" + std::to_wstring(1);
        tableNames[database.createTable(name)] = name;;
    }

    BOOST_CHECK_EQUAL(tableNames.size(), tableCount);
    BOOST_CHECK_EQUAL(database.tableCount(), tableCount);

    for (const auto& [id, name] : tableNames) {
        BOOST_CHECK_NO_THROW(database.table(id));
        const auto& table = database.table(id);
        BOOST_CHECK_EQUAL(table.id(), id);
        BOOST_CHECK(table.name() == name);
        BOOST_CHECK_NE(dynamic_cast<const core::CustomTable*>(&table), nullptr);
    }
}

BOOST_AUTO_TEST_CASE( DeleteTable )
{
    auto database = core::Database{L"database name", std::make_unique<core::CustomTableFactory>()};

    const size_t tableCount = 10;

    std::unordered_map<core::TableId, std::wstring> tableNames;
    for (size_t i = 0; i < tableCount; ++i) {
        const auto name = L"table" + std::to_wstring(1);
        tableNames[database.createTable(name)] = name;;
    }

    std::unordered_map<core::TableId, std::wstring> noDeletedTableNames;
    size_t k = 0;
    for (const auto& [id, name] : tableNames) {
        if (k++ % 2 == 0) {
            database.deleteTable(id);
        } else {
            noDeletedTableNames[id] = name;
        }
    }

    BOOST_CHECK_EQUAL(noDeletedTableNames.size(), database.tableCount());

    for (const auto& [id, name] : noDeletedTableNames) {
        BOOST_CHECK_NO_THROW(database.table(id));
        const auto& table = database.table(id);
        BOOST_CHECK_EQUAL(table.id(), id);
        BOOST_CHECK(table.name() == name);
    }
}

namespace {

auto simpleCreateCartesianProduct(const core::VirtualTable& firstTable, const core::VirtualTable& secondTable) -> std::unique_ptr<core::VirtualTable>
{
    auto newTablePtr = std::make_unique<core::CustomTable>(0, firstTable.name() + L" * " + secondTable.name());
    auto& newTable = *newTablePtr;

    for (size_t i = 1; i < secondTable.columnCount(); ++i) {
        auto columnInfo = secondTable.column(i).clone();
        columnInfo->changeName(secondTable.name() + L"_" + columnInfo->name());
        newTable.createColumn(std::move(columnInfo));
    }

    for (size_t i = 1; i < firstTable.columnCount(); ++i) {
        auto columnInfo = firstTable.column(i).clone();
        columnInfo->changeName(firstTable.name() + L"_" + columnInfo->name());
        newTable.createColumn(std::move(columnInfo));
    }


    firstTable.forAllRow([&newTable, &secondTable] (size_t, const core::Row& firstTableRow) {
        secondTable.forAllRow([&newTable, &firstTableRow] (size_t, const core::Row& secondTableRow) {
            const auto rowId = newTable.createRow();

            for (size_t i = 1; i < firstTableRow.size(); ++i) {
                newTable.setNewValue(rowId, i, firstTableRow[i]);
            }
            const auto nFirstTableRowCells = firstTableRow.size();
            for (size_t i = 1; i < secondTableRow.size(); ++i) {
                newTable.setNewValue(rowId, nFirstTableRowCells + i - 1,secondTableRow[i]);
            }
        });
    });

    return newTablePtr;
}

} // anon namespace

BOOST_AUTO_TEST_CASE( CreateCartesianProduct )
{
    auto database = core::Database{L"database name", std::make_unique<core::CustomTableFactory>()};

    // First table
    const core::TableId firstTableId  = database.createTable(L"table1");
    auto& firstTable = database.table(firstTableId);
    firstTable.createColumn(std::make_unique<core::ColumnInfo>(core::DataType::INTEGER, L"column(INTEGER)"));
    firstTable.createColumn(std::make_unique<core::ColumnInfo>(core::DataType::REAL, L"column(REAL)"));

    firstTable.createRow(); firstTable.createRow();

    firstTable.setNewValue(1, 1, core::column_t<core::DataType::INTEGER>(123));
    firstTable.setNewValue(2, 2, core::column_t<core::DataType::REAL>(123.123));

    // Second table
    const core::TableId secondTableId = database.createTable(L"table2");
    auto& secondTable = database.table(secondTableId);
    secondTable.createColumn(std::make_unique<core::ColumnInfo>(core::DataType::STRING, L"column(STRING)"));
    secondTable.createColumn(std::make_unique<core::ColumnInfo>(core::DataType::CHAR, L"column(CHAR)"));
    secondTable.createColumn(std::make_unique<core::IntervalIntColumnInfo>(0, 1000, L"column(INTERVAL)"));

    secondTable.createRow(); secondTable.createRow(); secondTable.createRow();

    secondTable.setNewValue(3, 1, core::column_t<core::DataType::STRING>(L"something string"));
    secondTable.setNewValue(2, 2, core::column_t<core::DataType::CHAR>(std::wstring(L"c").at(0)));
    secondTable.setNewValue(1, 3, core::column_t<core::DataType::INTERVAL_INTEGER>{.data=500});

    // Cartesian product
    database.createCartesianProduct(firstTableId, secondTableId);
    const core::TableId cartesianProductId = 23;
    const auto& cartesianProduct = database.table(cartesianProductId);

    BOOST_CHECK_EQUAL(firstTable.columnCount() + secondTable.columnCount(), cartesianProduct.columnCount() + 1);
    BOOST_CHECK_EQUAL(firstTable.rowCount() * secondTable.rowCount(), cartesianProduct.rowCount());
    BOOST_CHECK(firstTable.name() + L" * " + secondTable.name() == cartesianProduct.name());

    const auto tableForTest = simpleCreateCartesianProduct(firstTable, secondTable);
    for (size_t i = 1; i <= cartesianProduct.rowCount(); ++i) {
        for (size_t j = 0; j < cartesianProduct.columnCount(); ++j) {
            BOOST_CHECK(cartesianProduct.row(i)[j] == tableForTest->row(i)[j]);
        }
    }
}
