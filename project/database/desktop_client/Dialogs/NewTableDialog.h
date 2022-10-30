#ifndef NEWTABLEDIALOG_H
#define NEWTABLEDIALOG_H

// Qt
#include <QDialog>

namespace core {
    class VirtualTableNameValidator;
}

namespace Ui {
class NewTableDialog;
}

namespace desktop
{

class NewTableDialog: public QDialog
{
    Q_OBJECT

    using Super = QDialog;
public:
    explicit NewTableDialog(const core::VirtualTableNameValidator& validator, QWidget* parent = nullptr);
    ~NewTableDialog() override;

    using Super::exec;
    int exec(std::wstring& name);

private:
    std::unique_ptr<Ui::NewTableDialog> ui;
    const core::VirtualTableNameValidator& fValidator;
};

} // desktop

#endif // NEWTABLEDIALOG_H
