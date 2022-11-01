#include "DesktopDatabaseClient.h"

void desktop::DatabaseClient::resetModel(desktop::DbResetableModel& model) const
{
    model.reset(fDb.get());
}

void desktop::DatabaseClient::resetModel(std::optional<core::TableId> tableId, desktop::TableResetableModel& model) const
{
    if (tableId) {
        model.reset(&fDb->table(tableId.value()));
    } else {
        model.reset(nullptr);
    }
}