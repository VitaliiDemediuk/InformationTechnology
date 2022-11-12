#ifndef MONGODBSAVELOADSTRATEGY_H
#define MONGODBSAVELOADSTRATEGY_H

// STL
#include <memory>

// Local
#include "AbstractSaveLoadStrategy.h"
#include "SaveLoadUtils.h"

namespace core::save_load
{

class MongoDbStrategy : public AbstractStrategy
{
public:
    explicit MongoDbStrategy(MongoDbInfo mongoDbInfo);

    void save(const VirtualDatabase& db) const final;
    std::unique_ptr<VirtualDatabase> load() const final;

private:
    MongoDbInfo fMongoDbInfo;
};

} // core

#endif //MONGODBSAVELOADSTRATEGY_H
