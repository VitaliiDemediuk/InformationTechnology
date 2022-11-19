#include "ColumnInfoDialog.h"
#include "ui_ColumnInfoDialog.h"

// Qt
#include <QPushButton>

// Core
#include "VirtualValidators.h"
#include "ColumnUtils.h"

desktop::ColumnInfoDialog::ColumnInfoDialog(const core::VirtualColumnNameValidator& validator, QWidget* parent) :
    QDialog{parent},
    fValidator{validator},
    ui{new Ui::ColumnInfoDialog}
{
    ui->setupUi(this);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    for (int i = 0; i < static_cast<int>(core::DataType::NN); ++i) {
        auto typeName = dataTypeName(static_cast<core::DataType>(i));
        ui->typeComboBox->addItem(QString::fromStdWString({typeName.data(), typeName.length()}));
    }

    ui->lowerSpinBox->setVisible(false);
    ui->upperSpinBox->setVisible(false);

    connect(ui->typeComboBox, &QComboBox::currentIndexChanged, this, [this] (int index) {
        const bool isSpinBoxVisible = core::isIntervalType(static_cast<core::DataType>(index));
        ui->lowerSpinBox->setVisible(isSpinBoxVisible);
        ui->upperSpinBox->setVisible(isSpinBoxVisible);
    });

    connect(ui->lineEdNewName, &QLineEdit::textEdited, this, &ColumnInfoDialog::reenableButtonBox);
    connect(ui->typeComboBox, &QComboBox::currentIndexChanged, this, &ColumnInfoDialog::reenableButtonBox);
    connect(ui->lowerSpinBox, &QSpinBox::valueChanged, this, &ColumnInfoDialog::reenableButtonBox);
    connect(ui->upperSpinBox, &QSpinBox::valueChanged, this, &ColumnInfoDialog::reenableButtonBox);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void desktop::ColumnInfoDialog::reenableButtonBox()
{
    const int index = ui->typeComboBox->currentIndex();
    const auto text = ui->lineEdNewName->text();
    const bool isSpinBoxVisible = core::isIntervalType(static_cast<core::DataType>(index));
    const bool isValidInterval = ui->lowerSpinBox->value() < ui->upperSpinBox->value();
    const bool isValidValues = fValidator.validateColumnName(text.trimmed().toStdWString())
        && (index >= 0)
        && (!isSpinBoxVisible || isValidInterval);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isValidValues);
}

desktop::ColumnInfoDialog::~ColumnInfoDialog() = default;

void desktop::ColumnInfoDialog::reenable(bool isEnable)
{
    ui->typeComboBox->setEnabled(isEnable);
    ui->lowerSpinBox->setEnabled(isEnable);
    ui->upperSpinBox->setEnabled(isEnable);
}

int desktop::ColumnInfoDialog::exec(std::unique_ptr<core::VirtualColumnInfo>& columnInfo)
{
    reenable(true);
    ui->typeComboBox->setCurrentIndex(-1);

    const int res = Super::exec();
    if (!res) {
        return res;
    }

    const auto type = static_cast<core::DataType>(ui->typeComboBox->currentIndex());
    auto name = ui->lineEdNewName->text().trimmed().toStdWString();
    if (core::isIntervalType(type)) {
        columnInfo = std::make_unique<core::IntervalIntColumnInfo>(ui->lowerSpinBox->value(), ui->upperSpinBox->value(), std::move(name));
    } else {
        columnInfo = std::make_unique<core::ColumnInfo>(type, std::move(name));
    }
    return res;
}

int desktop::ColumnInfoDialog::exec(const core::VirtualColumnInfo& columnInfo, std::wstring& newName)
{
    reenable(false);
    ui->typeComboBox->setCurrentIndex(static_cast<int>(columnInfo.dataType()));

    newName = columnInfo.name();
    ui->lineEdNewName->setText(QString::fromStdWString(newName));

    if (const auto* info = dynamic_cast<const core::IntervalIntColumnInfo*>(&columnInfo)) {
        ui->lowerSpinBox->setValue(info->lowerLimit());
        ui->upperSpinBox->setValue(info->upperLimit());
    }

    if (const auto res = Super::exec()) {
        if (auto uiNewName = ui->lineEdNewName->text().toStdWString(); uiNewName != newName) {
            newName = std::move(uiNewName);
            return res;
        }
    }

    return 0;
}
