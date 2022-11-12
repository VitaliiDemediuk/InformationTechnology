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

using Information = std::variant<std::monostate,
                                 CustomFileInfo,
                                 MongoDbInfo>;

} // core

#endif //SAVELOADUTILS_H