#include "ConnectTogRpcServerDialog.h"
#include "ui_ConnectTogRpcServerDialog.h"

// Qt
#include <QPushButton>
#include <QFileDialog>

// Core
#include "VirtualValidators.h"
#include "ColumnUtils.h"

desktop::ConnectTogRpcServerDialog::ConnectTogRpcServerDialog(QWidget* parent) :
    QDialog{parent},
    ui{new Ui::ConnectTogRpcServerDialog}
{
    ui->setupUi(this);

    reenable();

    connect(ui->ipEdit, &QLineEdit::textChanged, this, &ConnectTogRpcServerDialog::reenable);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

}

desktop::ConnectTogRpcServerDialog::~ConnectTogRpcServerDialog() = default;

void desktop::ConnectTogRpcServerDialog::reenable()
{
    const bool haveValidSaveInfo = !ui->ipEdit->text().isEmpty();
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(haveValidSaveInfo);
}

int desktop::ConnectTogRpcServerDialog::exec(std::string& ip, uint16_t& port)
{
    const int res = Super::exec();
    if (!res) {
        return res;
    }

    ip = ui->ipEdit->text().toStdString();
    port = static_cast<uint16_t>(ui->portSpinBox->value());

    return res;
}
