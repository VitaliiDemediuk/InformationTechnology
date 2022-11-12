#ifndef ABSTRACTSAVELOADSTRATEGY_H
#define ABSTRACTSAVELOADSTRATEGY_H

#include <memory>

namespace core
{

class VirtualDatabase;

namespace save_load
{

class AbstractStrategy
{
public:
    virtual void save(const VirtualDatabase& db) const = 0;
    virtual std::unique_ptr<VirtualDatabase> load() const = 0;

    virtual ~AbstractStrategy() = default;

};

} // save_load

} // core

#endif //ABSTRACTSAVELOADSTRATEGY_H
