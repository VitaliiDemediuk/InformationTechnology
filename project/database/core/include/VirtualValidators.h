#ifndef VIRTUALVALIDATORS_H
#define VIRTUALVALIDATORS_H

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

} // core

#endif //VIRTUALVALIDATORS_H
