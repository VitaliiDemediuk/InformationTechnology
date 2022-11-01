#ifndef DBTABLEMODEL_H
#define DBTABLEMODEL_H

// Qt
#include <QAbstractTableModel>

// Local
#include "ResetableModel.h"

// Core
#include "VirtualTable.h"

namespace desktop
{

class DbTableModel: public QAbstractTableModel,
                    public TableResetableModel
{
    using Super = QAbstractTableModel;
public:
    using Super::Super;

    // QAbstractTableModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const final;
    int columnCount(const QModelIndex &parent = QModelIndex()) const final;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const final;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const final;

    // TableResetableModel
    void reset(const core::VirtualTable* table) final;

private:
    core::TableId tableId;
    std::vector<QString> columnNames;
};

} // desktop

#endif //DBTABLEMODEL_H
