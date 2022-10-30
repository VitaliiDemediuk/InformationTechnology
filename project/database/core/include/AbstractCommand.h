#ifndef ABSTRACTCOMMAND_H
#define ABSTRACTCOMMAND_H

#include <memory>

namespace core
{

class VirtualDatabase;

class AbstractCommand
{
public:
    virtual void exec(VirtualDatabase& db) = 0;

    virtual ~AbstractCommand() = default;
};

class AbstractExecutor{
public:
    virtual void exec(std::unique_ptr<AbstractCommand> cmd) = 0;

    virtual ~AbstractExecutor() = default;
};

} // core

#endif //ABSTRACTCOMMAND_H
