#ifndef TABLENAMEDIALOG_H
#define TABLENAMEDIALOG_H

// Qt
#include <QDialog>

namespace core {
    class VirtualTableNameValidator;
}

namespace Ui {
    class TableNameDialog;
}

namespace desktop
{

class TableNameDialog: public QDialog
{
    Q_OBJECT

    using Super = QDialog;
public:
    explicit TableNameDialog(const core::VirtualTableNameValidator& validator, QWidget* parent = nullptr);
    ~TableNameDialog() override;

    using Super::exec;
    int exec(std::wstring& name);

private:
    std::unique_ptr<Ui::TableNameDialog> ui;
    const core::VirtualTableNameValidator& fValidator;
};

} // desktop

#endif // TABLENAMEDIALOG_H
