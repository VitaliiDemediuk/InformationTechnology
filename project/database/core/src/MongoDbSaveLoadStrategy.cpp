#include "MongoDbSaveLoadStrategy.h"

// Local
#include "StringUtils.h"
#include "Database.h"
#include "CustomTable.h"

// mongo
#include <bsoncxx/json.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <bsoncxx/builder/stream/document.hpp>

core::save_load::MongoDbStrategy::MongoDbStrategy(MongoDbInfo mongoDbInfo)
    : fMongoDbInfo{std::move(mongoDbInfo)}
{
    static mongocxx::instance inst{};
}

namespace
{

constexpr const char* tablesInfoCollectionName = "TablesInfo";

void writeTablesInfo(mongocxx::database& mongoDb, const core::VirtualDatabase& db)
{
    mongocxx::collection coll = mongoDb[tablesInfoCollectionName];

    db.forAllTable([&coll, &db] (const core::VirtualTable& table) {
        auto builder = bsoncxx::builder::stream::document{};

        auto arrayContext = builder
            << "table_id" << static_cast<int64_t>(table.id())
            << "table_name" << core::utils::wstringToUtf8(table.name())
            << "columns" << bsoncxx::builder::stream::open_array;

        for (size_t i = 0; i < table.columnCount(); ++i) {
            const auto& columnInfo = table.column(i);
            auto context = arrayContext << bsoncxx::builder::stream::open_document
                         << "column_name" << core::utils::wstringToUtf8(columnInfo.name())
                         << "type" << core::utils::wstringToUtf8(core::dataTypeName(columnInfo.dataType()));
            if (core::isIntervalType(columnInfo.dataType())) {
                if (auto* intervalColInfo = dynamic_cast<const core::IntervalIntColumnInfo*>(&columnInfo)) {
                    context = context << "lowerLimit" << static_cast<int64_t>(intervalColInfo->lowerLimit())
                                      << "upperLimit" << static_cast<int64_t>(intervalColInfo->upperLimit());
                }
            }

            context << bsoncxx::builder::stream::close_document;
        }

        const auto document = arrayContext
            << bsoncxx::builder::stream::close_array
            << bsoncxx::builder::stream::finalize;

        coll.insert_one(document.view());
    });

}

// helper type for the visitor #4
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

void writeRow(mongocxx::collection& coll, const core::Row& row)
{
    auto builder = bsoncxx::builder::stream::document{};
    const auto rowId = std::get<core::column_t<core::DataType::INTEGER>>(row[0]);
    auto context = builder << "id" << static_cast<int64_t>(rowId)
                           << "row" << bsoncxx::builder::stream::open_array;

    const auto visitor = overloaded {
        [&context] (const std::monostate& value) {
            static constexpr bsoncxx::types::b_null null;
            context = context << null;
        },
        [&context] (const core::column_t<core::DataType::INTEGER>& value) {
            context = context << static_cast<int64_t>(value);
        },
        [&context] (const core::column_t<core::DataType::REAL>& value) {
            context = context << value;
        },
        [&context] (const core::column_t<core::DataType::CHAR>& value) {
            context = context << core::utils::wstringToUtf8(std::wstring(1, value));
        },
        [&context] (const core::column_t<core::DataType::STRING>& value) {
            context = context << core::utils::wstringToUtf8(value);
        },
        [&context] (const core::column_t<core::DataType::TEXT_FILE>& value) {
            context = context << bsoncxx::builder::stream::open_document
                              << "file_name" << core::utils::wstringToUtf8(value.name)
                              << "data" << value.data
                              << bsoncxx::builder::stream::close_document;
        },
        [&context] (const core::column_t<core::DataType::INTERVAL_INTEGER>& value) {
            context = context << static_cast<int64_t>(value.data);
        },
        [] (auto&& value) { throw std::logic_error("Not implemented writing data of this type to custom file."); }
    };

    for (size_t i = 1; i < row.size(); ++i) {
        std::visit(visitor, row[i]);
    }

    context << bsoncxx::builder::stream::close_array;

    coll.insert_one(builder.view());
}

void writeTable(mongocxx::database& mongoDb, const core::VirtualTable& table)
{
    mongocxx::collection coll = mongoDb[core::utils::wstringToUtf8(table.name())];
    table.forAllRow([&coll] (size_t, const core::Row& row) {
        writeRow(coll, row);
    });
}

void writeTables(mongocxx::database& mongoDb, const core::VirtualDatabase& db)
{
    db.forAllTable([&mongoDb] (const core::VirtualTable& table) {
        writeTable(mongoDb, table);
    });
}

} // anon namespace

void core::save_load::MongoDbStrategy::save(const VirtualDatabase& db) const
{

    const auto& [name, ip, port] = fMongoDbInfo;
    mongocxx::client conn{
        mongocxx::uri{
            "mongodb://" + ip + ":" + std::to_string(port)
        }
    };
    mongocxx::database mongoDb = conn[core::utils::wstringToUtf8(name)];
    mongoDb.drop();

    writeTablesInfo(mongoDb, db);
    writeTables(mongoDb, db);
}

namespace {

using CustomTableRefs = std::vector<std::reference_wrapper<core::CustomTable>>;

auto readTableInfo(mongocxx::database& mongoDb, core::Database& db) -> CustomTableRefs
{
    CustomTableRefs tableRefs;
    if (mongoDb.has_collection(tablesInfoCollectionName)) {
        auto coll = mongoDb[tablesInfoCollectionName];
        mongocxx::cursor cursor = coll.find({});

        for (auto doc : cursor) {
            auto tableNameUtf8 = doc.find("table_name")->get_utf8().value.to_string();
            const auto tableId = db.createTable(core::utils::utf8ToWstring(tableNameUtf8));
            auto& table = db.table(tableId);
            tableRefs.push_back(dynamic_cast<core::CustomTable&>(table));

            auto columnsInfo = doc.find("columns")->get_array().value;
            for (auto it = std::next(columnsInfo.begin()); it != columnsInfo.end(); ++it) {

                const auto& colInfo = it->get_document().value;
                std::unique_ptr<core::VirtualColumnInfo> coreColumnInfo;
                auto columnName = core::utils::utf8ToWstring(colInfo.find("column_name")->get_utf8().value.to_string());
                const auto typeName = core::utils::utf8ToWstring(colInfo.find("type")->get_utf8().value.to_string());
                const auto type = core::dataTypeFromString(typeName).value();
                if (core::isIntervalType(type)) {
                    if (type == core::DataType::INTERVAL_INTEGER) {
                        const auto lowerLimit = colInfo.find("lowerLimit")->get_int64().value;
                        const auto upperLimit = colInfo.find("upperLimit")->get_int64().value;
                        coreColumnInfo = std::make_unique<core::IntervalIntColumnInfo>(lowerLimit, upperLimit, std::move(columnName));
                    }  else {
                        throw std::logic_error("readTableInfo. Not implemented!");
                    }
                } else {
                    coreColumnInfo = std::make_unique<core::ColumnInfo>(type, std::move(columnName));
                }

                table.createColumn(std::move(coreColumnInfo));
            }
        }
    }
    return tableRefs;
}

} // anon namespace

void core::save_load::MongoDbStrategy::readRows(mongocxx::database& mongoDb, CustomTable& table) const
{
    const auto tableName = core::utils::wstringToUtf8(table.name());
    if (mongoDb.has_collection(tableName)) {
        auto coll = mongoDb[tableName];
        mongocxx::cursor cursor = coll.find({});

        for (auto doc : cursor) {
            const auto count = table.columnCount();
            core::Row row;
            row.reserve(count);

            row.emplace_back(doc.find("id")->get_int64());

            const auto rowMongoDb = doc.find("row")->get_array().value;
            for (size_t i = 0; i < count - 1 ; ++i) {
                const auto cell = rowMongoDb[i];

                if (cell.get_value().type() == bsoncxx::type::k_null) {
                    row.emplace_back(std::monostate{});
                    continue;
                }

                switch (table.column(i+1).dataType()) {
                    case core::DataType::INTEGER: {
                        row.push_back(cell.get_int64());
                        break;
                    }
                    case core::DataType::REAL: {
                        row.push_back(cell.get_double());
                        break;
                    }
                    case core::DataType::CHAR: {
                        row.push_back(core::utils::utf8ToWstring(cell.get_utf8().value.to_string())[0]);
                        break;
                    }
                    case core::DataType::STRING: {
                        row.push_back(core::utils::utf8ToWstring(cell.get_utf8().value.to_string()));
                        break;
                    }
                    case core::DataType::TEXT_FILE: {
                        const auto cellDoc = cell.get_document().value;
                        core::column_t<core::DataType::TEXT_FILE> file;
                        file.name = core::utils::utf8ToWstring(cellDoc.find("file_name")->get_utf8().value.to_string());
                        file.data = cellDoc.find("data")->get_utf8().value.to_string();
                        row.push_back(std::move(file));
                        break;
                    }
                    case core::DataType::INTERVAL_INTEGER: {
                        row.push_back(cell.get_int64());
                        break;
                    }
                    case core::DataType::NN: {
                        throw std::logic_error("MongoDb. Invalid data type during database reading");
                    }
                }
            }

            const auto rowId = std::get<core::column_t<core::DataType::INTEGER>>(row[0]);
            table.fTable[rowId] = std::move(row);
        }
    }
}

auto core::save_load::MongoDbStrategy::load() const -> std::unique_ptr<VirtualDatabase>
{
    const auto& [name, ip, port] = fMongoDbInfo;
    mongocxx::client conn{
        mongocxx::uri{
            "mongodb://" + ip + ":" + std::to_string(port)
        }
    };

    mongocxx::database mongoDb = conn[core::utils::wstringToUtf8(name)];

    auto db = std::make_unique<core::Database>(name, std::make_unique<core::CustomTableFactory>());

    auto tableRefs = readTableInfo(mongoDb, *db);
    for (auto& table : tableRefs) {
        readRows(mongoDb, table);
    }

    return db;
}
