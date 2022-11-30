#ifndef COMMANDS_H
#define COMMANDS_H

// Local
#include "AbstractCommand.h"
#include "VirtualTable.h"
#include "VirtualDatabase.h"

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

class RenameColumn: public AbstractCommand
{
public:
    explicit RenameColumn(core::TableId id, size_t columnIdx, std::wstring newName);

    void exec(VirtualDatabase& db) final;

private:
    const core::TableId fId;
    const size_t fColumnIdx;
    std::wstring fNewName;
};

class DeleteColumn: public AbstractCommand
{
public:
    explicit DeleteColumn(core::TableId id, size_t columnIdx);

    void exec(VirtualDatabase& db) final;

private:
    const core::TableId fId;
    const size_t fColumnIdx;
};

class AddRow: public AbstractCommand
{
public:
    explicit AddRow(core::TableId id);

    void exec(VirtualDatabase& db) final;
private:
    const core::TableId fId;
};

class DeleteRow: public AbstractCommand
{
public:
    explicit DeleteRow(core::TableId tableId, size_t rowId);

    void exec(VirtualDatabase& db) final;
private:
    const core::TableId fTableId;
    const size_t fRowId;
};

class EditCell: public AbstractCommand
{
public:
    explicit EditCell(core::TableId tableId, size_t rowId, size_t columnIdx, CellData data);

    void exec(VirtualDatabase& db) final;
private:
    const core::TableId fTableId;
    const size_t fRowId;
    const size_t fColumnIdx;
    CellData data;
};

class CreateCartesianProduct: public AbstractCommand
{
public:
    explicit CreateCartesianProduct(TableId firstTableId, TableId secondTableId);

    void exec(VirtualDatabase& db) final;
private:
    const TableId fFirstTableId;
    const TableId fSecondTableId;
};

class SaveDatabase: public AbstractCommand
{
public:
    explicit SaveDatabase(core::save_load::Information saveInfo);

    void exec(VirtualDatabase& db) final;
private:
    core::save_load::Information fSaveInfo;
};

} // core::command

#endif //COMMANDS_H
