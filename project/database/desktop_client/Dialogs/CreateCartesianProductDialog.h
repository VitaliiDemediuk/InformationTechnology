#ifndef CREATECARTESIANPRODUCTDIALOG_H
#define CREATECARTESIANPRODUCTDIALOG_H

// Qt
#include <QDialog>

// STL
#include <set>

// Core
#include "VirtualTable.h"

namespace Ui {
class CreateCartesianProductDialog;
}

namespace desktop
{

class CreateCartesianProductDialog: public QDialog
{
    Q_OBJECT
    using Super = QDialog;

public:
    explicit CreateCartesianProductDialog(QWidget *parent = nullptr);
    ~CreateCartesianProductDialog();

    using Super::exec;

    int exec(const std::vector<std::pair<core::TableId, std::wstring>>& tableList,
             core::TableId& firstTableId, core::TableId& secondTableId);

private:
    std::unique_ptr<Ui::CreateCartesianProductDialog> ui;
};

} // desktop

#endif // CREATECARTESIANPRODUCTDIALOG_H
