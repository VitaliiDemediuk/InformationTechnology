#ifndef DATABASECLIENT_H
#define DATABASECLIENT_H

#include "AbstractCommand.h"

namespace core
{

class VirtualDatabase;

class DatabaseClient : public AbstractExecutor
{
public:

    bool haveDatabase() const;
    void setNewDatabase(std::unique_ptr<VirtualDatabase> db);

    // AbstractExecutor
    void exec(std::unique_ptr<AbstractCommand> cmd) final;

protected:
    std::unique_ptr<VirtualDatabase> fDb;
};

} // core

#endif //DATABASECLIENT_H
