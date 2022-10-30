#ifndef DESKTOPDATABASECLIENT_H
#define DESKTOPDATABASECLIENT_H

// Core
#include "DatabaseClient.h"

// Models
#include "ResetableModel.h"

namespace desktop
{

class DatabaseClient: public core::DatabaseClient
{
    using Super = core::DatabaseClient;
public:
    void resetModel(desktop::ResetableModel& model) const;
};

} // desktop

#endif //DESKTOPDATABASECLIENT_H
