#include "Commands.h"

// Local
#include "VirtualDatabase.h"

/////////////// CreateNewTable /////////////////////////////////////////////////////////////////////////////////////////

core::command::CreateNewTable::CreateNewTable(std::wstring name)
    : fName{std::move(name)} {}

void core::command::CreateNewTable::exec(VirtualDatabase& db)
{
    db.createTable(std::move(fName));
}

/////////////// RenameTable ////////////////////////////////////////////////////////////////////////////////////////////

core::command::RenameTable::RenameTable(core::TableId id, std::wstring name)
    : fId{id}, fName{std::move(name)} {}

void core::command::RenameTable::exec(VirtualDatabase& db)
{
    auto& table = db.table(fId);
    table.changeName(fName);
}

/////////////// DeleteTable ////////////////////////////////////////////////////////////////////////////////////////////

core::command::DeleteTable::DeleteTable(core::TableId id)
    : fId{id} {}

void core::command::DeleteTable::exec(VirtualDatabase& db)
{
    db.deleteTable(fId);
}

/////////////// AddColumn //////////////////////////////////////////////////////////////////////////////////////////////

core::command::AddColumn::AddColumn(core::TableId id, std::unique_ptr<core::VirtualColumnInfo> columnInfo)
    : fId{id}, fColumnInfo{std::move(columnInfo)} {}

void core::command::AddColumn::exec(VirtualDatabase& db)
{
    db.table(fId).createColumn(std::move(fColumnInfo));
}
