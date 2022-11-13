#include "MongoDbSaveLoadStrategy.h"

// Local
#include "StringUtils.h"
#include "VirtualDatabase.h"

// mongo
#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
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
            context = context << value;
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

auto core::save_load::MongoDbStrategy::load() const -> std::unique_ptr<VirtualDatabase>
{

}