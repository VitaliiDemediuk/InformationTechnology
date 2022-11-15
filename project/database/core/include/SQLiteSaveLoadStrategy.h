#ifndef SQLITESAVELOADSTRATEGY_H
#define SQLITESAVELOADSTRATEGY_H

// Local
#include "AbstractSaveLoadStrategy.h"
#include "SaveLoadUtils.h"

namespace core::save_load
{

class SQLiteStrategy : public AbstractStrategy
{
public:
    explicit SQLiteStrategy(SQLiteInfo sqliteInfo);

    // AbstractStrategy
    void save(const VirtualDatabase &db) const final;
    std::unique_ptr<VirtualDatabase> load() const final;
private:
    SQLiteInfo fSQLiteInfo;
};

} // core::save_load

#endif //SQLITESAVELOADSTRATEGY_H
