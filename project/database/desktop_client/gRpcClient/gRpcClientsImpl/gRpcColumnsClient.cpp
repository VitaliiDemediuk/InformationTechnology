#include "gRpcColumnsClient.h"

// Core
#include "StringUtils.h"

// gRpc
#include <grpcpp/grpcpp.h>

namespace {

auto coreColumnToProtoColumn(const core::VirtualColumnInfo& coreColumn) -> ::ColumnInfo
{
    ::ColumnInfo protoInfo;

    if (core::isIntervalType(coreColumn.dataType())) {
        if (const auto* intervalCoreColumn = dynamic_cast<const core::IntervalIntColumnInfo*>(&coreColumn)) {
            auto* intervalProtoColumn = new ::IntervalIntColumnInfo;
            intervalProtoColumn->set_type(static_cast<::ColumnDataType>(intervalCoreColumn->dataType()));
            intervalProtoColumn->set_allocated_name(new std::string(core::utils::wstringToUtf8(intervalCoreColumn->name())));
            intervalProtoColumn->set_lowerlimit(intervalCoreColumn->lowerLimit());
            intervalProtoColumn->set_upperlimit(intervalCoreColumn->upperLimit());
            protoInfo.set_allocated_intintervalinfo(intervalProtoColumn);
        } else {
            throw std::logic_error("Core column info to proto column info not implemented for this data type.");
        }
    } else {
        auto* protoColumn = new ::DefaultColumnInfo;
        protoColumn->set_type(static_cast<::ColumnDataType>(coreColumn.dataType()));
        protoColumn->set_allocated_name(new std::string(core::utils::wstringToUtf8(coreColumn.name())));
        protoInfo.set_allocated_defaultinfo(protoColumn);
    }

    return protoInfo;
}

auto protoColumnToCoreColumn(const ::ColumnInfo& protoColumn) -> std::unique_ptr<core::VirtualColumnInfo>
{
    std::unique_ptr<core::VirtualColumnInfo> coreColumn;

    if (protoColumn.has_intintervalinfo()) {
        const auto& intervalProtoColumn = protoColumn.intintervalinfo();
        const auto type = static_cast<core::DataType>(intervalProtoColumn.type());
        auto name = core::utils::utf8ToWstring(intervalProtoColumn.name());
        const auto lowerLimit = intervalProtoColumn.lowerlimit();
        const auto upperLimit = intervalProtoColumn.upperlimit();
        coreColumn = std::make_unique<core::IntervalIntColumnInfo>(lowerLimit, upperLimit, std::move(name));
    } else {
        const auto& defaultProtoColumn = protoColumn.defaultinfo();
        const auto type = static_cast<core::DataType>(defaultProtoColumn.type());
        auto name = core::utils::utf8ToWstring(defaultProtoColumn.name());
        coreColumn = std::make_unique<core::ColumnInfo>(type, std::move(name));
    }

    return coreColumn;
}

} // namespace

db_grpc_client::Columns::Columns(std::shared_ptr<::grpc::Channel> channel)
    : fStub(::ColumnsService::NewStub(channel)) {}

auto db_grpc_client::Columns::get(core::TableId tableId, size_t colIdx) const -> std::unique_ptr<core::VirtualColumnInfo>
{
    ::ColumnId request;
    request.set_idx(colIdx);
    auto* protoTableId = new ::TableId;
    protoTableId->set_id(tableId);
    request.set_allocated_tableid(protoTableId);

    ::ColumnInfo reply;
    grpc::ClientContext context;

    const auto status = fStub->get(&context, request, &reply);

    if (!status.ok()) {
        throw std::runtime_error("db_grpc_client::Columns::get. Status is not ok!");
    }

    return protoColumnToCoreColumn(reply);
}

void db_grpc_client::Columns::create(core::TableId tableId, const core::VirtualColumnInfo& colInfo) const
{
    ::CreateColumnRequest request;
    auto* protoTableId = new ::TableId;
    protoTableId->set_id(tableId);
    request.set_allocated_tableid(protoTableId);
    request.set_allocated_info(new ::ColumnInfo(coreColumnToProtoColumn(colInfo)));

    ::Empty reply;
    grpc::ClientContext context;

    const auto status = fStub->create(&context, request, &reply);

    if (!status.ok()) {
        throw std::runtime_error("ddb_grpc_client::Columns::create. Status is not ok!");
    }
}

void db_grpc_client::Columns::edit(core::TableId tableId, size_t colIdx, const core::VirtualColumnInfo& colInfo) const
{
    auto* columnId = new ::ColumnId;
    columnId->set_idx(colIdx);
    auto* protoTableId = new ::TableId;
    protoTableId->set_id(tableId);
    columnId->set_allocated_tableid(protoTableId);

    ::ColumnEditRequest request;
    request.set_allocated_columnid(columnId);
    request.set_allocated_info(new ::ColumnInfo(coreColumnToProtoColumn(colInfo)));

    ::Empty reply;
    grpc::ClientContext context;

    const auto status = fStub->edit(&context, request, &reply);

    if (!status.ok()) {
        throw std::runtime_error("ddb_grpc_client::Columns::edit. Status is not ok!");
    }
}

void db_grpc_client::Columns::deleteColumn(core::TableId tableId, size_t colIdx) const
{
    ::ColumnId request;
    request.set_idx(colIdx);
    auto* protoTableId = new ::TableId;
    protoTableId->set_id(tableId);
    request.set_allocated_tableid(protoTableId);

    ::Empty reply;
    grpc::ClientContext context;

    const auto status = fStub->deleteColumn(&context, request, &reply);

    if (!status.ok()) {
        throw std::runtime_error("ddb_grpc_client::Columns::create. Status is not ok!");
    }
}
