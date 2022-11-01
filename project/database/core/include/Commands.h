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
    explicit DeleteTable(core::TableId id);

    void exec(VirtualDatabase& db) final;
private:
    const core::TableId fId;
};

class AddColumn: public AbstractCommand
{
public:
    explicit AddColumn(core::TableId id, std::unique_ptr<core::VirtualColumnInfo> columnInfo);

    void exec(VirtualDatabase& db) final;
private:
    const core::TableId fId;
    std::unique_ptr<core::VirtualColumnInfo> fColumnInfo;
};

class AddRow: public AbstractCommand
{
public:
    explicit AddRow(core::TableId id);

    void exec(VirtualDatabase& db) final;
private:
    const core::TableId fId;
};

} // core::command

#endif //COMMANDS_H
