#ifndef DESKTOPDATABASECLIENT_H
#define DESKTOPDATABASECLIENT_H

// STL
#include <optional>

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
    void resetModel(desktop::DbResetableModel& model) const;
    void resetModel(std::optional<core::TableId> tableId, desktop::TableResetableModel& model) const;
};

} // desktop

#endif //DESKTOPDATABASECLIENT_H
