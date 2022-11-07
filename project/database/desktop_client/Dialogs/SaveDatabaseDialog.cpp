#include "SaveDatabaseDialog.h"
#include "ui_SaveDatabaseDialog.h"

// Qt
#include <QPushButton>
#include <QFileDialog>

// Core
#include "VirtualValidators.h"
#include "ColumnUtils.h"

desktop::SaveDatabaseDialog::SaveDatabaseDialog(QWidget* parent) :
    QDialog{parent},
    ui{new Ui::SaveDatabaseDialog}
{
    ui->setupUi(this);

    refresh();

    connect(ui->selectFolderBtn, &QPushButton::clicked, this, [this] () {
        const auto dialogPath = customFilePath.has_value() ? QString::fromStdWString(customFilePath->wstring())
                                                           : QString{};
        const auto fileName = QFileDialog::getSaveFileName(this, "Save database", dialogPath,
                                                                 "Database (*.db)");
        if (!fileName.isEmpty()) {
            customFilePath = fileName.toStdWString();
        }

        refresh();
    });

    connect(ui->customFormatRb, &QRadioButton::toggled, this, &SaveDatabaseDialog::refresh);
    connect(ui->sqlitePageRb, &QRadioButton::toggled, this, &SaveDatabaseDialog::refresh);
    connect(ui->mongoDbPageRb, &QRadioButton::toggled, this, &SaveDatabaseDialog::refresh);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

desktop::SaveDatabaseDialog::~SaveDatabaseDialog() = default;

void desktop::SaveDatabaseDialog::refresh()
{
    bool haveValidSaveInfo = false;
    if (ui->customFormatRb->isChecked()) {
        ui->stackedWidget->setCurrentWidget(ui->customFormatPage);
        if (customFilePath.has_value()) {
            const auto& path = *customFilePath;
            haveValidSaveInfo = true;
            ui->customFilePathLb->setText(QString::fromStdWString(path.wstring()));
        } else {
            haveValidSaveInfo = false;
            ui->customFilePathLb->setText("No path");
        }
    } else if (ui->sqlitePageRb->isChecked()) {
        ui->stackedWidget->setCurrentWidget(ui->sqlitePage);
        haveValidSaveInfo = false;
    } else if (ui->mongoDbPageRb->isChecked()) {
        ui->stackedWidget->setCurrentWidget(ui->mongoDbPage);
        haveValidSaveInfo = false;
    } else {
        haveValidSaveInfo = false;
    }
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(haveValidSaveInfo);
}

// helper type for the visitor #4
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

void desktop::SaveDatabaseDialog::setInfo(const core::SaveInformation& info)
{
    customFilePath.reset();
    std::visit(overloaded {
        [this] (const std::monostate&) { ui->customFormatRb->setChecked(true); },
        [this] (const std::filesystem::path& path) {
            ui->customFormatRb->setChecked(true);
            customFilePath.emplace(path);
        },
        [] (auto&& value) { throw std::logic_error("desktop::SaveDatabaseDialog::setInfo not implemented!"); }
    }, info);
    refresh();
}

core::SaveInformation desktop::SaveDatabaseDialog::getInfo()
{
    core::SaveInformation res;
    if (ui->customFormatRb->isChecked() and customFilePath.has_value()) {
        res = std::move(*customFilePath);
        customFilePath.reset();
    }
    return res;
}

int desktop::SaveDatabaseDialog::exec(core::SaveInformation& saveInfo)
{
    setInfo(saveInfo);

    const int res = Super::exec();
    if (!res) {
        return res;
    }

    saveInfo = getInfo();
    return res;
}
