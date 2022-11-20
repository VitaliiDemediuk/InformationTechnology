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

/////////////// RenameColumn ///////////////////////////////////////////////////////////////////////////////////////////

core::command::RenameColumn::RenameColumn(core::TableId id, size_t columnIdx, std::wstring newName)
    : fId{id}, fColumnIdx{columnIdx}, fNewName{std::move(newName)} {}

void core::command::RenameColumn::exec(VirtualDatabase& db)
{
    db.table(fId).editColumnName(fColumnIdx, std::move(fNewName));
}

/////////////// DeleteColumn ///////////////////////////////////////////////////////////////////////////////////////////

core::command::DeleteColumn::DeleteColumn(core::TableId id, size_t columnIdx)
    : fId{id}, fColumnIdx{columnIdx} {}

void core::command::DeleteColumn::exec(VirtualDatabase& db)
{
    db.table(fId).deleteColumn(fColumnIdx);
}

/////////////// AddRow /////////////////////////////////////////////////////////////////////////////////////////////////

core::command::AddRow::AddRow(core::TableId id) : fId{id} {}

void core::command::AddRow::exec(VirtualDatabase& db)
{
    db.table(fId).createRow();
}

/////////////// AddRow /////////////////////////////////////////////////////////////////////////////////////////////////

core::command::DeleteRow::DeleteRow(core::TableId tableId, size_t rowId)
    : fTableId{tableId}, fRowId{rowId} {}

void core::command::DeleteRow::exec(VirtualDatabase& db)
{
    db.table(fTableId).deleteRow(fRowId);
}

/////////////// EditCell ///////////////////////////////////////////////////////////////////////////////////////////////

core::command::EditCell::EditCell(core::TableId tableId, size_t rowId, size_t columnIdx, CellData data)
    : fTableId{tableId}, fRowId{rowId}, fColumnIdx{columnIdx}, data{std::move(data)} {}

void core::command::EditCell::exec(VirtualDatabase& db)
{
    db.table(fTableId).setNewValue(fRowId, fColumnIdx, std::move(data));
}

/////////////// CreateCartesianProduct /////////////////////////////////////////////////////////////////////////////////

core::command::CreateCartesianProduct::CreateCartesianProduct(TableId firstTableId, TableId secondTableId)
 : fFirstTableId{firstTableId}, fSecondTableId{secondTableId} {}

void core::command::CreateCartesianProduct::exec(VirtualDatabase& db)
{
    db.createCartesianProduct(fFirstTableId, fSecondTableId);
}

/////////////// SaveDatabase ///////////////////////////////////////////////////////////////////////////////////////////

core::command::SaveDatabase::SaveDatabase(core::save_load::Information saveInfo)
    : fSaveInfo{std::move(saveInfo)} {}

void core::command::SaveDatabase::exec(VirtualDatabase& db)
{
    db.saveDatabase(fSaveInfo);
}