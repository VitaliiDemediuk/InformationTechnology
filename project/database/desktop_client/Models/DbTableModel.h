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

    size_t rowId(int rowIdx) const;

    // QAbstractTableModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const final;
    int columnCount(const QModelIndex &parent = QModelIndex()) const final;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const final;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const final;
    Qt::ItemFlags flags(const QModelIndex &index) const final;

    // TableResetableModel
    void reset(const core::VirtualTable* table) final;

private:
    core::TableId tableId;

    struct ColumnInfo {
        const bool isEditable;
        const bool needFileIcon;
        const QString name;
    };

    std::vector<ColumnInfo> columnsInfo;

    struct ModelRowData {
        explicit ModelRowData(size_t id) : id(id) {}

        const size_t id;
        std::vector<QVariant> row;
    };
    std::vector<ModelRowData> cells;
};

} // desktop

#endif //DBTABLEMODEL_H
