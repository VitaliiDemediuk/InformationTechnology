#include "DbTableModel.h"

int desktop::DbTableModel::rowCount(const QModelIndex &parent) const
{
    return 0;
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
    return {};
}

void desktop::DbTableModel::reset(const core::VirtualTable* table)
{
    beginResetModel();
    columnNames.clear();
    if (table) {
        const auto count = table->columnCount();
        columnNames.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            const auto& column = table->column(i);
            columnNames.push_back(QString::fromStdWString(column.name()));
        }
    }
    endResetModel();
}