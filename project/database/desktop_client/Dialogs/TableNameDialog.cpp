#include "TableNameDialog.h"
#include "ui_TableNameDialog.h"

// Qt
#include <QPushButton>

// Core
#include <VirtualValidators.h>

desktop::TableNameDialog::TableNameDialog(const core::VirtualTableNameValidator& validator, QWidget* parent) :
    QDialog{parent},
    fValidator{validator},
    ui{new Ui::TableNameDialog}
{
    ui->setupUi(this);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    connect(ui->lineEdNewName, &QLineEdit::textEdited, this, [this] (const QString& text) {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(fValidator.validateTableName(text.trimmed().toStdWString()));
    });

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

desktop::TableNameDialog::~TableNameDialog() = default;

int desktop::TableNameDialog::exec(std::wstring& name)
{
    ui->lineEdNewName->setText(QString::fromStdWString(name));
    const int res = Super::exec();
    if (res != 0) {
        name = ui->lineEdNewName->text().trimmed().toStdWString();
    }
    return res;
}
