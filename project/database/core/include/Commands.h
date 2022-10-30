#ifndef COMMANDS_H
#define COMMANDS_H

// Local
#include "AbstractCommand.h"
#include "VirtualTable.h"

namespace core::command
{

class CreateNewTable: public AbstractCommand
{
public:
    explicit CreateNewTable(std::wstring name);

    void exec(VirtualDatabase& db) final;
private:
    std::wstring fName;
};

class RenameTable: public AbstractCommand
{
public:
    explicit RenameTable(core::TableId id, std::wstring name);

    void exec(VirtualDatabase& db) final;
private:
    const core::TableId fId;
    std::wstring fName;
};

class DeleteTable: public AbstractCommand
{
public:
    explicit DeleteTable(core::TableId fId);

    void exec(VirtualDatabase& db) final;
private:
    const core::TableId fId;
};

} // core::command

#endif //COMMANDS_H
