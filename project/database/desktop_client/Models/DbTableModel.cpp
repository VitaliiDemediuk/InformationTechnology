#include "DbTableModel.h"

// Qt
#include <QBrush>
#include <QStyle>
#include <QApplication>

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
        [&res] (const core::column_t<core::DataType::REAL>& value) { res = QString::number(value).replace('.', ','); },
        [&res] (const core::column_t<core::DataType::CHAR>& value) { res = QChar{value}; },
        [&res] (const core::column_t<core::DataType::STRING>& value) { res = QString::fromStdWString(value); },
        [&res] (const core::column_t<core::DataType::TEXT_FILE>& value) { res = QString::fromStdWString(value.name); },
        [&res] (const core::column_t<core::DataType::INTERVAL_INTEGER>& value) { res = value.data; },
        [] (auto&& value) { throw std::logic_error("core::CellData to QVariant cast not implemented!"); }
    }, cellData);
    return res;
}

} // detail

size_t desktop::DbTableModel::rowId(int rowIdx) const
{
    return cells.at(rowIdx).id;
}

int desktop::DbTableModel::rowCount(const QModelIndex &parent) const
{
    return cells.size();
}

int desktop::DbTableModel::columnCount(const QModelIndex &parent) const
{
    return columnsInfo.size();
}

QVariant desktop::DbTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            return columnsInfo.at(section).name;
        }
    }
    return Super::headerData(section, orientation, role);
}

QVariant desktop::DbTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant{};
    }

    const auto rowIdx = index.row();
    const auto columnIdx = index.column();
    const auto& cell = cells.at(rowIdx).row.at(columnIdx);

    switch (role) {
        case Qt::DisplayRole: {
            if (cell.isNull()) {
                return "(empty)";
            }
            return cell;
        }
        case Qt::ForegroundRole: {
            if (cell.isNull()) {
                return QBrush(QColor(192,192,192, 200)); // Gray
            }
        } break;
        case Qt::EditRole: {
            return cell;
        }
        case Qt::DecorationRole: {
            if (columnsInfo.at(columnIdx).needFileIcon) {
                return QApplication::style()->standardIcon(QStyle::SP_FileIcon);
            } break;
        }
        default: {}
    }

    return QVariant{};
}

Qt::ItemFlags desktop::DbTableModel::flags(const QModelIndex &index) const
{
    const auto superFlags = Super::flags(index);

    if (!index.isValid()) {
        return superFlags;
    }

    if (columnsInfo.at(index.column()).isEditable) {
        return superFlags | Qt::ItemIsEditable;
    }
    return superFlags;
}

void desktop::DbTableModel::reset(const core::VirtualTable* table)
{
    beginResetModel();
    columnsInfo.clear();
    cells.clear();
    if (table) {
        const auto count = table->columnCount();
        columnsInfo.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            const auto& column = table->column(i);
            columnsInfo.emplace_back(ColumnInfo{.isEditable = column.isEditable(),
                                                .needFileIcon = column.dataType() == core::DataType::TEXT_FILE,
                                                .name = QString::fromStdWString(column.name())});
        }

        cells.reserve(table->rowCount());
        table->forAllRow([this] (size_t id, const core::Row& row) {
            auto& modelRow = cells.emplace_back(id).row;
            modelRow.reserve(row.size());
            for (const auto& cell : row) {
                modelRow.emplace_back(toQVariant(cell));
            }
        });
    }
    endResetModel();
}
