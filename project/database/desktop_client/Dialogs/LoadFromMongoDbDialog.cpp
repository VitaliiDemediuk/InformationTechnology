#include "LoadFromMongoDbDialog.h"
#include "ui_LoadFromMongoDbDialog.h"

// Qt
#include <QPushButton>
#include <QFileDialog>

// Core
#include "VirtualValidators.h"
#include "ColumnUtils.h"

desktop::LoadFromMongoDbDialog::LoadFromMongoDbDialog(const core::VirtualDatabaseNameValidator& validator, QWidget* parent) :
    QDialog{parent},
    fValidator{validator},
    ui{new Ui::LoadFromMongoDbDialog}
{
    ui->setupUi(this);

    reenable();

    connect(ui->dbNameEdit, &QLineEdit::textChanged, this, &LoadFromMongoDbDialog::reenable);
    connect(ui->dbIpEdit, &QLineEdit::textChanged, this, &LoadFromMongoDbDialog::reenable);
    connect(ui->dbPortSpinBox, &QSpinBox::textChanged, this, &LoadFromMongoDbDialog::reenable);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

}

desktop::LoadFromMongoDbDialog::~LoadFromMongoDbDialog() = default;

void desktop::LoadFromMongoDbDialog::reenable()
{
    const bool haveValidSaveInfo = fValidator.validateDatabaseName(ui->dbNameEdit->text().toStdWString())
                                   && !ui->dbIpEdit->text().isEmpty();
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(haveValidSaveInfo);
}

int desktop::LoadFromMongoDbDialog::exec(core::save_load::MongoDbInfo& saveInfo)
{
    const int res = Super::exec();
    if (!res) {
        return res;
    }

    saveInfo.dbName = ui->dbNameEdit->text().toStdWString();
    saveInfo.ipAddress = ui->dbIpEdit->text().toStdString();
    saveInfo.port = static_cast<uint16_t>(ui->dbPortSpinBox->value());

    return res;
}
