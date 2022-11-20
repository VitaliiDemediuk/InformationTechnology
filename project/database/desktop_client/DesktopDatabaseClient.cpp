#include "DesktopDatabaseClient.h"

// boost
#include <boost/throw_exception.hpp>

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

void desktop::DatabaseClient::forAllTables(const std::function<void(const core::VirtualTable&)>& func) const
{
    if (!hasDatabase()) {
        BOOST_THROW_EXCEPTION(std::logic_error("There is no database!"));
    }
    fDb->forAllTable(func);
}