#ifndef COLUMNUTILS_H
#define COLUMNUTILS_H

#include <vector>
#include <variant>
#include <filesystem>

namespace core
{

enum class DataType
{
    INTEGER,
    REAL,
    CHAR,
    STRING,
    TEXT_FILE,
    INTERVAL_INTEGER
};

template <typename T>
struct IntervalType { using Type = T; };

template <DataType type>
struct ColumnTypeTrait {};

template<> struct ColumnTypeTrait<DataType::INTEGER>          { using Type = long long int; };
template<> struct ColumnTypeTrait<DataType::REAL>             { using Type = long double; };
template<> struct ColumnTypeTrait<DataType::CHAR>             { using Type = wchar_t; };
template<> struct ColumnTypeTrait<DataType::STRING>           { using Type = std::wstring; };
template<> struct ColumnTypeTrait<DataType::TEXT_FILE>        { using Type = std::filesystem::path; };
template<> struct ColumnTypeTrait<DataType::INTERVAL_INTEGER> { using Type = IntervalType<ColumnTypeTrait<DataType::INTEGER>::Type>; };

template<DataType type>
using column_t = typename ColumnTypeTrait<type>::Type;

using ColumnData = std::variant<column_t<DataType::INTEGER>,
                                column_t<DataType::REAL>,
                                column_t<DataType::CHAR>,
                                column_t<DataType::STRING>,
                                column_t<DataType::TEXT_FILE>,
                                column_t<DataType::INTERVAL_INTEGER>>;

using Row = std::vector<ColumnData>;

class VirtualColumnInfo {
public:
    explicit VirtualColumnInfo(std::wstring name);

    const std::wstring& name() const noexcept;
    void changeName(std::wstring name);

    virtual DataType dateType() const = 0;
private:
    std::wstring fName;
};

} // core

namespace detail
{
    constexpr bool isIntervalType(core::DataType type) noexcept
    {
        return type == core::DataType::INTERVAL_INTEGER;
    }
} // detail

namespace core
{

template <DataType type> requires (!detail::isIntervalType(type))
class ColumnInfo : VirtualColumnInfo
{
    using Super = VirtualColumnInfo;
public:
    using Super::Super;

    DataType dateType() const final { return type; }
};

template <typename T>
class IntervalColumnInfo : public VirtualColumnInfo
{
    using Super = VirtualColumnInfo;
public:
    explicit IntervalColumnInfo(T fLowerLimit, T upperLimit, std::wstring name)
        : Super{std::move(name)}, fLowerLimit{fLowerLimit}, fUpperLimit{upperLimit} {}

private:
    const T fLowerLimit;
    const T fUpperLimit;
};

using IntervalIntColumnInfo = IntervalColumnInfo<column_t<DataType::INTERVAL_INTEGER>::Type>;

} // core

#endif //COLUMNUTILS_H
