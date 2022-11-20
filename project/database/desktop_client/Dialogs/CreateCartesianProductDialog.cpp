#include "CreateCartesianProductDialog.h"
#include "ui_CreateCartesianProductDialog.h"

// Qt
#include <QPushButton>

desktop::CreateCartesianProductDialog::CreateCartesianProductDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateCartesianProductDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &CreateCartesianProductDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &CreateCartesianProductDialog::reject);
}

desktop::CreateCartesianProductDialog::~CreateCartesianProductDialog() = default;

namespace
{

void buildCombo(const std::vector<std::pair<core::TableId, std::wstring>>& tableList, QComboBox* combo)
{
    combo->clear();
    for (const auto& [_, tableName] : tableList) {
        combo->addItem(QString::fromStdWString(tableName));
    }
}

} // namespace

int desktop::CreateCartesianProductDialog::exec(const std::vector<std::pair<core::TableId, std::wstring>>& tableList,
                                                core::TableId& firstTableId, core::TableId& secondTableId)
{
    buildCombo(tableList, ui->firstTableCombo);
    buildCombo(tableList, ui->secondTableCombo);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!tableList.empty());

    if (const int res = Super::exec()) {
        firstTableId  = tableList[ui->firstTableCombo->currentIndex()].first;
        secondTableId = tableList[ui->secondTableCombo->currentIndex()].first;
        return res;
    }

    return 0;
}
