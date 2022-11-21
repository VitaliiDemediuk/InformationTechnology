#ifndef SAVELOADUTILS_H
#define SAVELOADUTILS_H

// STL
#include <cstdint>
#include <variant>
#include <filesystem>

namespace core::save_load
{

struct CustomFileInfo
{
    std::filesystem::path filePath;
};

struct MongoDbInfo
{
    std::wstring dbName;
    std::string ipAddress;
    uint16_t port;
};

struct SQLiteInfo
{
    std::filesystem::path filePath;
};

struct Remote {};

using Information = std::variant<std::monostate,
                                 CustomFileInfo,
                                 MongoDbInfo,
                                 SQLiteInfo,
                                 Remote>;

} // core::save_load

#endif //SAVELOADUTILS_H
