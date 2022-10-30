#include "Commands.h"

// Local
#include "VirtualDatabase.h"

/////////////// CreateNewTable /////////////////////////////////////////////////////////////////////////////////////////

core::command::CreateNewTable::CreateNewTable(std::wstring name)
    : fName{std::move(name)} {}

void core::command::CreateNewTable::exec(VirtualDatabase& db) const
{
    db.createTable(std::move(fName));
}