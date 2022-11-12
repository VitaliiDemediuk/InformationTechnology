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
}

auto core::save_load::MongoDbStrategy::load() const -> std::unique_ptr<VirtualDatabase>
{

}