#ifndef TABLELISTMODEL_H
#define TABLELISTMODEL_H

// Local
#include "ResetableModel.h"

// Core
#include "VirtualTable.h"

// Qt
#include <QAbstractListModel>

namespace desktop
{

class TableListModel: public QAbstractListModel,
                      public DbResetableModel
{
public:
    core::TableId tableId(size_t idx) const;

    void reset(const core::VirtualDatabase* db) final;

    // QAbstractListModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const final;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const final;

private:
    struct TableInfo
    {
        core::TableId id;
        QString name;
    };

    std::vector<TableInfo> fTableList;
};

} // desktop

#endif //TABLELISTMODEL_H
