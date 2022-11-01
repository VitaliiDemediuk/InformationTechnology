#ifndef RESETABLEMODEL_H
#define RESETABLEMODEL_H

namespace core
{
class VirtualDatabase;
class VirtualTable;
}

namespace desktop
{

class DbResetableModel
{
public:
    virtual void reset(const core::VirtualDatabase* db) = 0;
};

class TableResetableModel
{
public:
    virtual void reset(const core::VirtualTable* table) = 0;
};


} // desktop

#endif //RESETABLEMODEL_H
