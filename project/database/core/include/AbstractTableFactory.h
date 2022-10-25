#ifndef ABSTRACTTABLEFACTORY_H
#define ABSTRACTTABLEFACTORY_H

// Local
#include <VirtualTable.h>

namespace core
{

enum class FactoryType {
    Custom
};

class AbstractTableFactory
{
public:
    virtual FactoryType type() const = 0;

    virtual std::unique_ptr<VirtualTable> createTable(TableId id, std::wstring name) const = 0;

    virtual ~AbstractTableFactory() = default;
};

} // core

#endif //ABSTRACTTABLEFACTORY_H
