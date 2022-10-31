#include "ColumnUtils.h"

std::wstring core::dataTypeName(DataType t)
{
    switch (t) {
    case DataType::INTEGER: return L"Integer";
    case DataType::REAL: return L"Real";
    case DataType::CHAR: return L"Char";
    case DataType::STRING: return L"String";
    case DataType::TEXT_FILE: return L"Text file";
    case DataType::INTERVAL_INTEGER: return L"Interval integer";
    case DataType::NN: throw std::logic_error("Invalid data type!");
    }
}

core::VirtualColumnInfo::VirtualColumnInfo(std::wstring name)
    : fName(std::move(name))
{
    if (fName.empty()) {
        throw std::logic_error("core::VirtualColumnInfo::VirtualColumnInfo, column name is empty. "
                               "Surely programing bug!");
    }
}


const std::wstring& core::VirtualColumnInfo::name() const noexcept
{
    return fName;
}

void core::VirtualColumnInfo::changeName(std::wstring name)
{
    fName = std::move(name);
}

core::ColumnInfo::ColumnInfo(DataType type, std::wstring name)
    : Super{std::move(name)}, fType{type}
{
    if (isIntervalType(type)) {
        throw std::logic_error("Type cannot be interval!");
    }
}

core::DataType core::ColumnInfo::dateType() const
{
    return fType;
}

auto core::ColumnInfo::clone() const -> std::unique_ptr<VirtualColumnInfo>
{
    return std::make_unique<This>(dateType(), name());
}
