#ifndef NEWDBDIALOG_H
#define NEWDBDIALOG_H

// Qt
#include <QDialog>

namespace Ui {
class NewDbDialog;
}

namespace core {
    class VirtualDatabaseNameValidator;
}

namespace desktop
{

class NewDbDialog: public QDialog
{
    Q_OBJECT

    using Super = QDialog;
public:
    explicit NewDbDialog(const core::VirtualDatabaseNameValidator& validator, QWidget* parent = nullptr);
    ~NewDbDialog() override;

    using Super::exec;
    int exec(std::wstring& name);

private:
    std::unique_ptr<Ui::NewDbDialog> ui;
    const core::VirtualDatabaseNameValidator& fValidator;
};

} // desktop

#endif // NEWDBDIALOG_H
