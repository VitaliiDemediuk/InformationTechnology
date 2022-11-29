#include "ColumnUtils.h"

// STL
#include <array>
#include <algorithm>

constexpr auto dataTypeNameArray = std::array<std::wstring_view, static_cast<size_t>(core::DataType::NN)> {
    L"Integer", L"Real", L"Char", L"String",
    L"Text file", L"Interval integer"
};

std::wstring_view core::dataTypeName(DataType t)
{
    return dataTypeNameArray[static_cast<size_t>(t)];
}

std::optional<core::DataType> core::dataTypeFromString(std::wstring_view str)
{
    const auto it = std::find(dataTypeNameArray.begin(), dataTypeNameArray.end(), str);
    if (it != dataTypeNameArray.end()) {
        return static_cast<core::DataType>(std::distance(dataTypeNameArray.begin(), it));
    }
    return std::nullopt;
}

core::VirtualColumnInfo::VirtualColumnInfo(std::wstring name)
    : fName(std::move(name))
{
    if (fName.empty()) {
        throw std::logic_error("core::VirtualColumnInfo::VirtualColumnInfo, column name is empty. "
                               "Surely programing bug!");
    }
}


const std::wstring &core::VirtualColumnInfo::name() const noexcept
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

core::DataType core::ColumnInfo::dataType() const
{
    return fType;
}

auto core::ColumnInfo::clone() const -> std::unique_ptr<VirtualColumnInfo>
{
    return std::make_unique<This>(dataType(), name());
}

bool core::ColumnInfo::isEditable() const
{
    return true;
}