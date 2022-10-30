#include "TableListModel.h"

// Core
#include "VirtualDatabase.h"

core::TableId desktop::TableListModel::tableId(size_t idx) const
{
    return fTableList.at(idx).id;
}

void desktop::TableListModel::reset(const core::VirtualDatabase* db)
{
    beginResetModel();
    fTableList.clear();
    if (db) {
        db->forAllTable([this] (const core::VirtualTable& table) {
            TableInfo info{.id = table.id(), .name = QString::fromStdWString(table.name())};
            fTableList.push_back(std::move(info));
        });
    }
    endResetModel();
}

int desktop::TableListModel::rowCount(const QModelIndex &paren) const
{
    return fTableList.size();
}

QVariant desktop::TableListModel::data(const QModelIndex &index, int role) const
{
    const auto row = index.row();
    if (role == Qt::DisplayRole) {
        return fTableList.at(row).name;
    }
    return {};
}
