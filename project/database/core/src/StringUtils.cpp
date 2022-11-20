#include "StringUtils.h"

// STL
#include <locale>
#include <codecvt>

static std::wstring_convert<std::codecvt_utf8<wchar_t>> strConvertor;

std::wstring core::utils::utf8ToWstring(const std::string_view& str)
{
    return strConvertor.from_bytes(str.data(), str.data() + str.size());
}

std::string core::utils::wstringToUtf8(const std::wstring_view& str)
{
    return strConvertor.to_bytes(str.data(), str.data() + str.size());
}