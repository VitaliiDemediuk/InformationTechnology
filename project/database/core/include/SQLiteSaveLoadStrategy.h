#ifndef SQLITESAVELOADSTRATEGY_H
#define SQLITESAVELOADSTRATEGY_H

// Local
#include "AbstractSaveLoadStrategy.h"
#include "SaveLoadUtils.h"

namespace SQLite{
    class Database;
}

namespace core
{

class Database;
class VirtualTable;
class CustomTable;

namespace save_load
{

class SQLiteStrategy: public AbstractStrategy
{
public:
    explicit SQLiteStrategy(SQLiteInfo sqliteInfo);

    // AbstractStrategy
    void save(const VirtualDatabase &db) const final;
    std::unique_ptr<VirtualDatabase> load() const final;
private:
    void getTablesFromSQLiteDb(SQLite::Database &sqlDb, core::Database &db) const;
    void getTableFromSQLiteDb(SQLite::Database &sqlDb, core::VirtualTable &table) const;
    void readRows(SQLite::Database &sqlDb, core::CustomTable &table) const;

    SQLiteInfo fSQLiteInfo;
};

} // save_load

} // core

#endif //SQLITESAVELOADSTRATEGY_H
