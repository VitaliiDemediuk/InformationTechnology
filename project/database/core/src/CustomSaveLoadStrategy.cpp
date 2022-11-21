#include "CustomSaveLoadStrategy.h"

// STL
#include <fstream>

// Local
#include "Database.h"
#include "CustomTable.h"

core::save_load::CustomFileStrategy::CustomFileStrategy(CustomFileInfo saveLoadInfo)
    : fSaveLoadInfo{std::move(saveLoadInfo)} {}

namespace
{

template<typename T> requires std::is_trivial_v<T>
void writeTrivial(std::ofstream& stream, const T& value)
{
    stream.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

template <typename char_type>
void writeString(std::ofstream& stream, const std::basic_string<char_type>& str)

{
    const auto size = str.size();
    writeTrivial(stream, size);
    stream.write(reinterpret_cast<const char*>(str.data()), size * sizeof(char_type));
}

void writeColumn(std::ofstream& stream, const core::VirtualColumnInfo& columnInfo)
{
    const auto dataType = columnInfo.dataType();
    writeTrivial(stream, dataType);
    writeString(stream, columnInfo.name());

    if (core::isIntervalType(dataType)) {
        if (dataType == core::DataType::INTERVAL_INTEGER) {
            const auto intervalIntColInfo = static_cast<const core::IntervalIntColumnInfo&>(columnInfo);
            writeTrivial(stream, intervalIntColInfo.lowerLimit());
            writeTrivial(stream, intervalIntColInfo.upperLimit());
        } else {
            throw std::logic_error("Not implemented writing for this interval type to custom file.");
        }
    }
}

void writeColumns(std::ofstream& stream, const core::VirtualTable& table)
{
    const size_t count = table.columnCount() - 1;
    writeTrivial(stream, count);
    for (size_t i = 0; i < count; ++i) {
        writeColumn(stream, table.column(i + 1));
    }
}

// helper type for the visitor #4
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

void writeRow(std::ofstream& stream, const core::Row& row)
{
    const auto visitor = overloaded {
        [&stream] (const core::column_t<core::DataType::INTEGER>& value) { writeTrivial(stream, value); },
        [&stream] (const core::column_t<core::DataType::REAL>& value) { writeTrivial(stream, value); },
        [&stream] (const core::column_t<core::DataType::CHAR>& value) { writeTrivial(stream, value); },
        [&stream] (const core::column_t<core::DataType::STRING>& value) { writeString(stream, value); },
        [&stream] (const core::column_t<core::DataType::TEXT_FILE>& value) {
            writeString(stream, value.name);
            writeString(stream, value.data);
        },
        [&stream] (const core::column_t<core::DataType::INTERVAL_INTEGER>& value) { writeTrivial(stream, value.data); },
        [] (auto&& value) { throw std::logic_error("Not implemented writing data of this type to custom file."); }
    };

    for (const auto& cell : row) {
        const bool isEmpty = std::holds_alternative<std::monostate>(cell);
        writeTrivial(stream, isEmpty);
        if (isEmpty) {
            continue;
        }

        std::visit(visitor, cell);
    }
}

void writeRows(std::ofstream& stream, const core::VirtualTable& table)
{
    const size_t count = table.rowCount();
    writeTrivial(stream, count);
    table.forAllRow([&stream](size_t, const core::Row& row) {
        writeRow(stream, row);
    });
}

void writeTable(std::ofstream& stream, const core::VirtualTable& table)
{
    writeString(stream, table.name());
    writeTrivial(stream, table.id());
    writeColumns(stream, table);
    writeRows(stream, table);
}

} // anon namespace

void core::save_load::CustomFileStrategy::save(const VirtualDatabase& db) const
{
    std::ofstream stream(fSaveLoadInfo.filePath, std::ios::binary);
    if (!stream.is_open()) {
        throw std::runtime_error("Saving database to custom format file. Invalid path!");
    }

    const size_t tableCount = db.tableCount();
    writeTrivial(stream, tableCount);
    db.forAllTable([&stream](const core::VirtualTable& table) {
        writeTable(stream, table);
    });
}

namespace
{

template<typename T>
requires std::is_trivial_v<T>
T readTrivial(std::ifstream &stream)
{
    T value{};
    stream.read(reinterpret_cast<char *>(&value), sizeof(T));
    return value;
}

template<typename char_type>
auto readString(std::ifstream &stream)
{
    std::basic_string<char_type> str;
    const auto size = readTrivial<decltype(str.size())>(stream);
    str.resize(size);
    stream.read(reinterpret_cast<char *>(str.data()), size * sizeof(char_type));
    return str;
}

std::unique_ptr<core::VirtualColumnInfo> readColumn(std::ifstream &stream)
{
    const auto dataType = readTrivial<core::DataType>(stream);
    auto name = readString<wchar_t>(stream);
    std::unique_ptr<core::VirtualColumnInfo> columnInfo;
    if (core::isIntervalType(dataType)) {
        if (dataType == core::DataType::INTERVAL_INTEGER) {
            const auto lowerLimit = readTrivial<core::IntervalIntColumnInfo::ValueT>(stream);
            const auto upperLimit = readTrivial<core::IntervalIntColumnInfo::ValueT>(stream);
            columnInfo = std::make_unique<core::IntervalIntColumnInfo>(lowerLimit, upperLimit, std::move(name));
        }
        else {
            throw std::logic_error("Not implemented writing for this interval type to custom file.");
        }
    }
    else {
        columnInfo = std::make_unique<core::ColumnInfo>(dataType, std::move(name));
    }
    return columnInfo;
}

void readColumns(std::ifstream &stream, core::VirtualTable &table)
{
    const auto count = readTrivial<size_t>(stream);
    for (size_t i = 0; i < count; ++i) {
        auto columnInfo = readColumn(stream);
        table.createColumn(std::move(columnInfo));
    }
}

core::Row readRow(std::ifstream& stream, const core::VirtualTable& table) {
    const auto count = table.columnCount();
    core::Row row;
    row.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        if (readTrivial<bool>(stream)) { // is empty
            row.push_back(std::monostate{});
            continue;
        }

        switch (table.column(i).dataType()) {
        case core::DataType::INTEGER: {
            row.push_back(readTrivial<core::column_t<core::DataType::INTEGER>>(stream));
            break;
        }
        case core::DataType::REAL: {
            row.push_back(readTrivial<core::column_t<core::DataType::REAL>>(stream));
            break;
        }
        case core::DataType::CHAR: {
            row.push_back(readTrivial<core::column_t<core::DataType::CHAR>>(stream));
            break;
        }
        case core::DataType::STRING: {
            using CharT = core::column_t<core::DataType::STRING>::traits_type::char_type;
            row.push_back(readString<CharT>(stream));
            break;
        }
        case core::DataType::TEXT_FILE: {
            core::column_t<core::DataType::TEXT_FILE> file;
            file.name = readString<decltype(file.name)::traits_type::char_type>(stream);
            file.data = readString<decltype(file.data)::traits_type::char_type>(stream);
            row.push_back(std::move(file));
            break;
        }
        case core::DataType::INTERVAL_INTEGER: {
            core::column_t<core::DataType::INTERVAL_INTEGER> value;
            value.data = readTrivial<decltype(value.data)>(stream);
            row.push_back(value);
            break;
        }
        case core::DataType::NN: {
            throw std::logic_error("Invalid data type during database reading,");
        }
        }
    }
    return row;
}

} // anon namespace

void core::save_load::CustomFileStrategy::readRows(std::ifstream& stream, core::CustomTable& table) const
{
    const auto rowCount = readTrivial<size_t>(stream);
    for (size_t i = 0; i < rowCount; ++i) {
        auto row = readRow(stream, table);
        const auto rowId = std::get<core::column_t<core::DataType::INTEGER>>(row[0]);
        table.fTable[rowId] = std::move(row);
    }

    if (const auto& mapTable = table.fTable; !mapTable.empty()) {
        table.lastId = std::prev(table.fTable.end())->first + 1;
    }
}

void core::save_load::CustomFileStrategy::readTable(std::ifstream& stream, core::Database& db) const
{
    auto tableName = readString<wchar_t>(stream);
    (void)readTrivial<core::TableId>(stream);
    const auto tableId = db.createTable(tableName);
    auto& table = db.table(tableId);
    if (auto* customTable = dynamic_cast<core::CustomTable*>(&table)) {
        readColumns(stream, *customTable);
        readRows(stream, *customTable);
    } else {
        throw std::logic_error("Custom format db loading. Not implemented!");
    }
}

std::unique_ptr<core::VirtualDatabase> core::save_load::CustomFileStrategy::load() const
{
    std::ifstream stream(fSaveLoadInfo.filePath, std::ios::binary);
    if (!stream.is_open()) {
        throw std::runtime_error("Saving database to custom format file. Invalid path!");
    }

    auto db = std::make_unique<core::Database>(fSaveLoadInfo.filePath.stem().wstring(),
                                               std::make_unique<core::CustomTableFactory>());

    const auto tableCount = readTrivial<size_t>(stream);
    for (size_t i = 0; i < tableCount; ++i) {
        readTable(stream, *db);
    }

    db->fLastSaveInfo = fSaveLoadInfo;

    return db;
}
