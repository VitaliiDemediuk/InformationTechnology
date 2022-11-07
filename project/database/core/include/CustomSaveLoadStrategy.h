#ifndef CUSTOMSAVELOADSTRATEGY_H
#define CUSTOMSAVELOADSTRATEGY_H

// STL
#include <filesystem>

// Local
#include "AbstractSaveLoadStrategy.h"

namespace core
{

class VirtualTable;
class CustomTable;

class CustomSaveLoadStrategy: public AbstractSaveLoadStrategy
{
public:
    explicit CustomSaveLoadStrategy(std::filesystem::path filePath);

    // AbstractSaveLoadStrategy
    void save(const VirtualDatabase& db) const final;
    std::unique_ptr<VirtualDatabase> load() const final;
private:
    void readRows(std::ifstream& stream, CustomTable& table) const;
    std::unique_ptr<VirtualTable> readTable(std::ifstream& stream) const;

    const std::filesystem::path fFilePath;
};

} // core

#endif //CUSTOMSAVELOADSTRATEGY_H
