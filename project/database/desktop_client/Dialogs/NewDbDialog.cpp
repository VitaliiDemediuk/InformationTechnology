#include "NewDbDialog.h"
#include "ui_NewDbDialog.h"

// Qt
#include <QPushButton>

desktop::NewDbDialog::NewDbDialog(const core::VirtualDatabaseNameValidator& validator, QWidget *parent) :
    QDialog{parent},
    fValidator{validator},
    ui{new Ui::NewDbDialog}
{
    ui->setupUi(this);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    connect(ui->lineEdNewName, &QLineEdit::textEdited, this, [this] (const QString& text) {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(fValidator.validateDatabaseName(text.trimmed().toStdWString()));
    });

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

desktop::NewDbDialog::~NewDbDialog() = default;

int desktop::NewDbDialog::exec(std::wstring& name)
{
    const int res = Super::exec();
    if (res != 0) {
        name = ui->lineEdNewName->text().trimmed().toStdWString();
    }
    return res;
}
