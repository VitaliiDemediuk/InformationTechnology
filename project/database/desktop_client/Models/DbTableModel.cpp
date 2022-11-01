#include "DbTableModel.h"

namespace {

// helper type for the visitor #4
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

QVariant toQVariant(const core::CellData& cellData)
{
    QVariant res;
    std::visit(overloaded {
        [&res] (const std::monostate& value) { res = QVariant{}; },
        [&res] (const core::column_t<core::DataType::INTEGER>& value) { res = value; },
        [&res] (const core::column_t<core::DataType::REAL>& value) { res = value; },
        [&res] (const core::column_t<core::DataType::CHAR>& value) { res = QChar{value}; },
        [&res] (const core::column_t<core::DataType::STRING>& value) { res = QString::fromStdWString(value); },
        [&res] (const core::column_t<core::DataType::TEXT_FILE>& value) { res = QString::fromStdWString(value.wstring()); },
        [&res] (const core::column_t<core::DataType::INTERVAL_INTEGER>& value) { res = value.data; },
        [] (auto&& value) { throw std::logic_error("core::CellData to QVariant cast not implemented!"); }
    }, cellData);
    return res;
}

}

int desktop::DbTableModel::rowCount(const QModelIndex &parent) const
{
    return cells.size();
}

int desktop::DbTableModel::columnCount(const QModelIndex &parent) const
{
    return columnNames.size();
}

QVariant desktop::DbTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            return columnNames.at(section);
        }
    }
    return Super::headerData(section, orientation, role);
}

QVariant desktop::DbTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        if (index.isValid()) {
            const auto rowIdx = index.row();
            const auto columnIdx = index.column();
            const auto& cell = cells.at(rowIdx).at(columnIdx);
            if (cell.isNull()) {
                return "(empty)";
            }
            return cell;
        }
    }
    return QVariant{};
}

void desktop::DbTableModel::reset(const core::VirtualTable* table)
{
    beginResetModel();
    columnNames.clear();
    cells.clear();
    if (table) {
        const auto count = table->columnCount();
        columnNames.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            const auto& column = table->column(i);
            columnNames.push_back(QString::fromStdWString(column.name()));
        }

        cells.reserve(table->rowCount());
        table->forAllRow([this] (const core::Row& row) {
            auto& modelRow = cells.emplace_back();
            modelRow.reserve(row.size());
            for (const auto& cell : row) {
                modelRow.push_back(toQVariant(cell));
            }
        });
    }
    endResetModel();
}