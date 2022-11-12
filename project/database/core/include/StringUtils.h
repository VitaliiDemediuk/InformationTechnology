#ifndef STRINGUTILS_H
#define STRINGUTILS_H

// STL
#include <string>

namespace core::utils
{

std::wstring utf8ToWstring(const std::string_view& str);
std::string wstringToUtf8(const std::wstring_view& str);

} // core::utils


#endif //STRINGUTILS_H
