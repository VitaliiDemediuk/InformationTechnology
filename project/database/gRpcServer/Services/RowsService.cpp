#include "RowsService.h"

// Core
#include "StringUtils.h"
#include "VirtualDatabase.h"

// STL
#include <iostream>

namespace {

// helper type for the visitor #4
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

auto coreCellToProtoCell(const core::CellData& cellData) -> ::Cell
{
    ::Cell protoCell;

    std::visit(overloaded {
        [&protoCell] (const std::monostate&) {
            protoCell.set_allocated_empty(new ::Empty);
        },
        [&protoCell] (const core::column_t<core::DataType::INTEGER>& value) {
            protoCell.set_integer(value);
        },
        [&protoCell] (const core::column_t<core::DataType::REAL>& value) {
            protoCell.set_real(value);
        },
        [&protoCell] (const core::column_t<core::DataType::CHAR>& value) {
            protoCell.set_text(core::utils::wstringToUtf8(std::wstring(1, value)));
        },
        [&protoCell] (const core::column_t<core::DataType::STRING>& value) {
            protoCell.set_text(core::utils::wstringToUtf8(value));
        },
        [&protoCell] (const core::column_t<core::DataType::TEXT_FILE>& value) {
            auto file = new ::File;
            file->set_name(core::utils::wstringToUtf8(value.name));
            file->set_allocated_data(new std::string(value.data));
            protoCell.set_allocated_file(file);
        },
        [&protoCell] (const core::column_t<core::DataType::INTERVAL_INTEGER>& value) {
            protoCell.set_integer(value.data);
        },
        [] (auto&& value) { throw std::logic_error("Not implemented writing data of this type to custom file."); }
    }, cellData);

    return protoCell;
}

auto protoCellToCoreCell(const ::Cell& protoCell, core::DataType type) -> core::CellData
{
    core::CellData data;

    if (protoCell.has_empty()) {
        data = std::monostate{};
    } else {
        switch (type) {
        case core::DataType::INTEGER: {
            data = static_cast<core::column_t<core::DataType::INTEGER>>(protoCell.integer());
            break;
        }
        case core::DataType::REAL: {
            data = static_cast<core::column_t<core::DataType::REAL>>(protoCell.real());
            break;
        }
        case core::DataType::CHAR: {
            data = core::utils::utf8ToWstring(protoCell.text())[0];
            break;
        }
        case core::DataType::STRING: {
            data = core::utils::utf8ToWstring(protoCell.text());
            break;
        }
        case core::DataType::TEXT_FILE: {
            const auto& protoFile = protoCell.file();
            core::File file;
            file.name = core::utils::utf8ToWstring(protoFile.name());
            file.data = protoFile.data();
            data = std::move(file);
            break;
        }
        case core::DataType::INTERVAL_INTEGER: {
            core::column_t<core::DataType::INTERVAL_INTEGER> cell;
            cell.data = protoCell.integer();
            data = cell;
            break;
        }
        case core::DataType::NN: {
            throw std::logic_error("MongoDb. Invalid data type during database reading");
        }
        }
    }

    return data;
}

auto coreRowToProtoRow(const core::Row& row) -> ::Row
{
    ::Row protoRow;

    for (const auto& cell : row) {
        *protoRow.add_cells() = coreCellToProtoCell(cell);
    }

    return protoRow;
}

auto protoRowToCoreRow(const ::Row& protoRow, const core::VirtualTable& table) -> core::Row
{
    core::Row coreRow;
    coreRow.reserve(protoRow.cells_size());

    for (size_t i = 0; i < table.columnCount(); ++i) {
        coreRow.push_back(protoCellToCoreCell(protoRow.cells(i), table.column(i).dataType()));
    }

    return coreRow;
}

} // namespace

db_grpc_server::service::Rows::Rows(core::VirtualDatabase& db)
    : fDb{db} {}

grpc::Status db_grpc_server::service::Rows::get(
    grpc::ServerContext* context, const ::RowId* request, Row* response)
{
    const auto tableId = request->tableid().id();
    const auto rowId = request->id();
    std::cout << "Request. Get Row " << rowId << " for table" << tableId<< std::endl;
    const auto& coreRow = fDb.table(tableId).row(rowId);
    *response = coreRowToProtoRow(coreRow);
    return grpc::Status::OK;
}

grpc::Status db_grpc_server::service::Rows::create(
    grpc::ServerContext* context, const TableId* request, RowCreateResponse* response)
{
    const auto tableId = request->id();
    std::cout << "Request. Create row in table" << tableId << std::endl;
    const auto rowId = fDb.table(tableId).createRow();
    response->set_rowid(rowId);
    return grpc::Status::OK;
}

grpc::Status db_grpc_server::service::Rows::editCell(
    grpc::ServerContext* context, const CellEditRequest* request, Empty* response)
{
    const auto tableId = request->cellid().tableid().id();
    const auto columnIdx = request->cellid().columnidx();
    const auto rowId = request->cellid().rowid();
    std::cout << "Request. Edit cell (" << columnIdx << " , " << rowId << ") " << "in table" << tableId << std::endl;
    core::CellData data = protoCellToCoreCell(request->cell(), fDb.table(tableId).column(columnIdx).dataType());
    fDb.table(tableId).setNewValue(rowId, columnIdx, std::move(data));
    return grpc::Status::OK;
}

grpc::Status db_grpc_server::service::Rows::deleteRow(
    grpc::ServerContext* context, const ::RowId* request, ::Empty* response)
{
    const auto tableId = request->tableid().id();
    const auto rowId = request->id();
    std::cout << "Request. Delete row " << rowId << " in table" << tableId<< std::endl;
    fDb.table(tableId).deleteRow(rowId);
    return grpc::Status::OK;
}