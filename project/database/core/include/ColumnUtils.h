#ifndef COLUMNUTILS_H
#define COLUMNUTILS_H

// STL
#include <vector>
#include <variant>
#include <string>
#include <memory>
#include <optional>

namespace core
{

enum class DataType
{
    INTEGER = 0,
    REAL,
    CHAR,
    STRING,
    TEXT_FILE,
    INTERVAL_INTEGER,
    NN
};

std::wstring_view dataTypeName(DataType t);
std::optional<DataType> dataTypeFromString(std::wstring_view str);

template <typename T>
struct IntervalType {
    using Type = T;
    Type data{};

    bool operator==(const IntervalType&) const = default;
};

struct File
{
    std::wstring name;
    std::string data;

    bool operator==(const File&) const = default;
};

template <DataType type>
struct ColumnTypeTrait {};

template<> struct ColumnTypeTrait<DataType::INTEGER>          { using Type = long long int; };
template<> struct ColumnTypeTrait<DataType::REAL>             { using Type = double; };
template<> struct ColumnTypeTrait<DataType::CHAR>             { using Type = wchar_t; };
template<> struct ColumnTypeTrait<DataType::STRING>           { using Type = std::wstring; };
template<> struct ColumnTypeTrait<DataType::TEXT_FILE>        { using Type = File; };
template<> struct ColumnTypeTrait<DataType::INTERVAL_INTEGER> { using Type = IntervalType<ColumnTypeTrait<DataType::INTEGER>::Type>; };

template<DataType type>
using column_t = typename ColumnTypeTrait<type>::Type;

using CellData = std::variant<std::monostate,
                              column_t<DataType::INTEGER>,
                              column_t<DataType::REAL>,
                              column_t<DataType::CHAR>,
                              column_t<DataType::STRING>,
                              column_t<DataType::TEXT_FILE>,
                              column_t<DataType::INTERVAL_INTEGER>>;

using Row = std::vector<CellData>;

class VirtualColumnInfo {
public:
    explicit VirtualColumnInfo(std::wstring name);

    const std::wstring& name() const noexcept;
    void changeName(std::wstring name);

    virtual DataType dataType() const = 0;
    virtual std::unique_ptr<VirtualColumnInfo> clone() const = 0;
    // can edit row data
    virtual bool isEditable() const = 0;
private:
    std::wstring fName;
};

constexpr bool isIntervalType(core::DataType type) noexcept
{
    return type == core::DataType::INTERVAL_INTEGER;
}

class ColumnInfo: public VirtualColumnInfo
{
    using Super = VirtualColumnInfo;
    using This  = ColumnInfo;
public:
    explicit ColumnInfo(DataType type, std::wstring name);

    DataType dataType() const final;
    std::unique_ptr<VirtualColumnInfo> clone() const final;
    bool isEditable() const final;
private:
    const DataType fType;
};

template <DataType DataT> requires (isIntervalType(DataT))
class IntervalColumnInfo : public VirtualColumnInfo
{
    using Super  = VirtualColumnInfo;
    using This   = IntervalColumnInfo<DataT>;
public:
    using ValueT = typename column_t<DataT>::Type;

    explicit IntervalColumnInfo(ValueT fLowerLimit, ValueT upperLimit, std::wstring name)
        : Super{std::move(name)}, fLowerLimit{fLowerLimit}, fUpperLimit{upperLimit} {}

    DataType dataType() const final { return DataT; }

    std::unique_ptr<VirtualColumnInfo> clone() const final
    {
        return std::make_unique<This>(lowerLimit(), upperLimit(), name());
    }
    bool isEditable() const final { return true; }

    ValueT lowerLimit() const { return fLowerLimit; }
    ValueT upperLimit() const { return fUpperLimit; }
private:
    const ValueT fLowerLimit;
    const ValueT fUpperLimit;
};

using IntervalIntColumnInfo = IntervalColumnInfo<DataType::INTERVAL_INTEGER>;

} // core

#endif //COLUMNUTILS_H
