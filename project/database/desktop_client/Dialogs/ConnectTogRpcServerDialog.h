#ifndef CONNECTTOGRPCSERVERDIALOG_H
#define CONNECTTOGRPCSERVERDIALOG_H

// Qt
#include <QDialog>

// Core
#include "VirtualDatabase.h"

namespace core {
    class VirtualColumnNameValidator;
}

namespace Ui {
    class ConnectTogRpcServerDialog;
}

namespace desktop
{

class ConnectTogRpcServerDialog: public QDialog
{
    Q_OBJECT

    using Super = QDialog;
public:
    explicit ConnectTogRpcServerDialog(QWidget* parent = nullptr);
    ~ConnectTogRpcServerDialog() override;

    using Super::exec;
    int exec(std::string& ip, uint16_t& port);

private slots:
    void reenable();

private:
    std::unique_ptr<Ui::ConnectTogRpcServerDialog> ui;
};

} // desktop

#endif // CONNECTTOGRPCSERVERDIALOG_H
