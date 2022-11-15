#ifndef MONGODBSAVELOADSTRATEGY_H
#define MONGODBSAVELOADSTRATEGY_H

// STL
#include <memory>

// Local
#include "AbstractSaveLoadStrategy.h"
#include "SaveLoadUtils.h"

// MongoDb
//

namespace mongocxx {
inline namespace v_noabi
{
    class database;
}
}

namespace core
{

class VirtualTable;
class CustomTable;

namespace save_load
{

class MongoDbStrategy: public AbstractStrategy
{
public:
    explicit MongoDbStrategy(MongoDbInfo mongoDbInfo);

    // AbstractStrategy
    void save(const VirtualDatabase &db) const final;
    std::unique_ptr<VirtualDatabase> load() const final;

private:
    void readRows(mongocxx::database &mongoDb, CustomTable &table) const;

    MongoDbInfo fMongoDbInfo;
};

} // save_load

} // core

#endif //MONGODBSAVELOADSTRATEGY_H
