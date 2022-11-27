#include "ColumnsService.h"

// Core
#include "StringUtils.h"
#include "VirtualDatabase.h"

// STL
#include <iostream>

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

db_grpc_server::service::Columns::Columns(core::VirtualDatabase& db)
    : fDb{db} {}

grpc::Status db_grpc_server::service::Columns::get(
    grpc::ServerContext* context,const ::ColumnId* request, ::ColumnInfo* response)
{
    const auto tableId = request->tableid().id();
    const auto columnIdx = request->idx();
    std::cout << "Request. Get column info for table" << tableId << " column idx " << columnIdx << std::endl;
    const auto& columnInfo = fDb.table(tableId).column(columnIdx);
    *response = coreColumnToProtoColumn(columnInfo);
    return grpc::Status::OK;
}

grpc::Status db_grpc_server::service::Columns::create(
    grpc::ServerContext* context, const ::CreateColumnRequest* request, ::Empty* response)
{
    const auto tableId = request->tableid().id();
    std::cout << "Request. Create column in table" << tableId << std::endl;
    auto coreColumn = protoColumnToCoreColumn(request->info());
    fDb.table(tableId).createColumn(std::move(coreColumn));
    return grpc::Status::OK;
}

grpc::Status db_grpc_server::service::Columns::edit(
    grpc::ServerContext* context, const ::ColumnEditRequest* request, ::Empty* response)
{
    const auto tableId = request->columnid().tableid().id();
    const auto columnIdx = request->columnid().idx();
    std::cout << "Request. Edit column " << columnIdx << " in table" << tableId << std::endl;
    auto coreColumn = protoColumnToCoreColumn(request->info());
    fDb.table(tableId).editColumnName(columnIdx, coreColumn->name());
    return grpc::Status::OK;
}

grpc::Status db_grpc_server::service::Columns::deleteColumn(
    grpc::ServerContext* context, const ::ColumnId* request, ::Empty* response)
{
    const auto tableId = request->tableid().id();
    const auto columnIdx = request->idx();
    std::cout << "Request. Delete column " << columnIdx << " in table" << tableId << std::endl;
    fDb.table(tableId).deleteColumn(columnIdx);
    return grpc::Status::OK;
}