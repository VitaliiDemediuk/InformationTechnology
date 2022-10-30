#ifndef COMMANDS_H
#define COMMANDS_H

// Local
#include "AbstractCommand.h"

namespace core::command
{

class CreateNewTable: public AbstractCommand
{
public:
    explicit CreateNewTable(std::wstring name);

    void exec(VirtualDatabase& db) const final;

private:
    std::wstring fName;
};

} // core::command

#endif //COMMANDS_H
