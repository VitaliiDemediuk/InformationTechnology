#ifndef RESETABLEMODEL_H
#define RESETABLEMODEL_H

namespace core
{
class VirtualDatabase;
}

namespace desktop
{

class ResetableModel
{
public:
    virtual void reset(const core::VirtualDatabase* db) = 0;
};

} // desktop

#endif //RESETABLEMODEL_H
