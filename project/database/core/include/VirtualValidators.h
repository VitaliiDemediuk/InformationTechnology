#ifndef VIRTUALVALIDATORS_H
#define VIRTUALVALIDATORS_H

#include <string>

namespace core
{

class VirtualTableNameValidator
{
public:
    virtual bool validateTableName(const std::wstring &name) const = 0;

    virtual ~VirtualTableNameValidator() = default;
};

class VirtualDatabaseNameValidator
{
public:
    virtual bool validateDatabaseName(const std::wstring &name) const = 0;

    virtual ~VirtualDatabaseNameValidator() = default;
};

class VirtualColumnNameValidator
{
public:
    virtual bool validateColumnName(const std::wstring &name) const = 0;

    virtual ~VirtualColumnNameValidator() = default;
};

} // core

#endif //VIRTUALVALIDATORS_H
