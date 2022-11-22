#include "gRpcRowsClient.h"

// Core
#include "StringUtils.h"

// gRpc
#include <grpcpp/grpcpp.h>

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

db_grpc_client::Rows::Rows(std::shared_ptr<::grpc::Channel> channel)
    : fStub(::RowsService::NewStub(channel)) {}

core::Row db_grpc_client::Rows::get(const core::VirtualTable& table, size_t rowId) const
{
    ::RowId request;
    request.set_id(rowId);
    auto* protoTableId = new ::TableId;
    protoTableId->set_id(table.id());
    request.set_allocated_tableid(protoTableId);

    ::Row reply;
    grpc::ClientContext context;

    const auto status = fStub->get(&context, request, &reply);

    if (!status.ok()) {
        throw std::runtime_error("db_grpc_client::Rows::get. Status is not ok!");
    }

    return protoRowToCoreRow(reply, table);
}

size_t db_grpc_client::Rows::create(core::TableId tableId) const
{
    ::TableId request;
    request.set_id(tableId);

    ::RowCreateResponse reply;
    grpc::ClientContext context;

    const auto status = fStub->create(&context, request, &reply);

    if (!status.ok()) {
        throw std::runtime_error("db_grpc_client::Rows::create. Status is not ok!");
    }
    return reply.rowid();
}

void db_grpc_client::Rows::editCell(core::TableId tableId, size_t colIdx, size_t rowId, const core::CellData& cellData) const
{
    auto* cellId = new ::CellId;
    cellId->set_rowid(rowId);
    cellId->set_columnidx(colIdx);
    auto* protoTableId = new ::TableId;
    protoTableId->set_id(tableId);
    cellId->set_allocated_tableid(protoTableId);

    ::CellEditRequest request;
    request.set_allocated_cellid(cellId);
    request.set_allocated_cell(new ::Cell(coreCellToProtoCell(cellData)));

    ::Empty reply;
    grpc::ClientContext context;

    const auto status = fStub->editCell(&context, request, &reply);

    if (!status.ok()) {
        throw std::runtime_error("db_grpc_client::Rows::editCell. Status is not ok!");
    }
}

void db_grpc_client::Rows::deleteRow(core::TableId tableId, size_t rowId) const
{
    ::RowId request;
    request.set_id(rowId);
    auto* protoTableId = new ::TableId;
    protoTableId->set_id(tableId);
    request.set_allocated_tableid(protoTableId);

    ::Empty reply;
    grpc::ClientContext context;

    const auto status = fStub->deleteRow(&context, request, &reply);

    if (!status.ok()) {
        throw std::runtime_error("db_grpc_client::Rows::deleteRow. Status is not ok!");
    }
}
