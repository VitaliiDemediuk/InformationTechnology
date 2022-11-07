#ifndef ABSTRACTSAVELOADSTRATEGY_H
#define ABSTRACTSAVELOADSTRATEGY_H

namespace core
{

class VirtualDatabase;

class AbstractSaveLoadStrategy
{
public:
    virtual void save(const VirtualDatabase& db) const = 0;
    virtual std::unique_ptr<VirtualDatabase> load() const = 0;
};

} // core

#endif //ABSTRACTSAVELOADSTRATEGY_H
