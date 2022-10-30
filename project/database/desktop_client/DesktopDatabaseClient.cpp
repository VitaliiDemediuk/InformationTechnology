#include "DesktopDatabaseClient.h"

void desktop::DatabaseClient::resetModel(desktop::ResetableModel& model) const
{
    model.reset(fDb.get());
}