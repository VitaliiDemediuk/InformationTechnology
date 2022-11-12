#ifndef CUSTOMSAVELOADSTRATEGY_H
#define CUSTOMSAVELOADSTRATEGY_H

// STL
#include <filesystem>

// Local
#include "AbstractSaveLoadStrategy.h"
#include "SaveLoadUtils.h"

namespace core
{

class VirtualTable;
class CustomTable;

namespace save_load
{

class CustomFileStrategy: public AbstractStrategy
{
public:
    explicit CustomFileStrategy(CustomFileInfo saveLoadInfo);

    // AbstractSaveLoadStrategy
    void save(const VirtualDatabase& db) const final;
    std::unique_ptr<VirtualDatabase> load() const final;
private:
    void readRows(std::ifstream& stream, CustomTable& table) const;
    std::unique_ptr<VirtualTable> readTable(std::ifstream& stream) const;

    const CustomFileInfo fSaveLoadInfo;
};

} // save_load

} // core

#endif //CUSTOMSAVELOADSTRATEGY_H
