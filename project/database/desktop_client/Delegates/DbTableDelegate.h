#ifndef DBTABLEDELEGATE_H
#define DBTABLEDELEGATE_H

// Qt
#include <QItemDelegate>

// Core
#include "VirtualTable.h"

namespace desktop
{

class DbCellEditor {
public:
    virtual void editCell(size_t rowIdx, size_t columnIdx, core::CellData data) = 0;

    virtual ~DbCellEditor() = default;
};

class DbTableDelegate : public QItemDelegate
{
    using Super = QItemDelegate;
public:
    explicit DbTableDelegate(DbCellEditor& cellEditor, QObject* parent = nullptr);

    void setTable(const core::VirtualTable* table);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const final;
    void setModelData(QWidget* editor,  QAbstractItemModel* model, const QModelIndex& index) const;

private:
    DbCellEditor& fCellEditor;
    const core::VirtualTable* fCoreTable = nullptr;
    std::unique_ptr<QIntValidator> intIntervalValidator;
};

} // desktop

#endif //DBTABLEDELEGATE_H
