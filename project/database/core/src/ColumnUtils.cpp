#include "ColumnUtils.h"

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