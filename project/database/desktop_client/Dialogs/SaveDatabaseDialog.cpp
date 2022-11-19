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
        const auto dialogPath = customFileInfo.has_value() ? QString::fromStdWString(customFileInfo->filePath.wstring())
                                                           : QString{};
        const auto fileName = QFileDialog::getSaveFileName(this, "Save database", dialogPath,
                                                                 "Database (*.cdb)");
        if (!fileName.isEmpty()) {
            customFileInfo.emplace(core::save_load::CustomFileInfo{.filePath = fileName.toStdWString()});
        }

        refresh();
    });

    connect(ui->selectSqliteDbBtn, &QPushButton::clicked, this, [this] () {
        const auto dialogPath = sqliteDbInfo.has_value() ? QString::fromStdWString(sqliteDbInfo->filePath.wstring())
                                                         : QString{};
        const auto fileName = QFileDialog::getSaveFileName(this, "Save SQLite database", dialogPath,
                                                                 "Database (*.db *.db3)");
        if (!fileName.isEmpty()) {
            sqliteDbInfo.emplace(core::save_load::SQLiteInfo{.filePath = fileName.toStdWString()});
        }

        refresh();
    });

    connect(ui->customFormatRb, &QRadioButton::toggled, this, &SaveDatabaseDialog::refresh);
    connect(ui->sqlitePageRb, &QRadioButton::toggled, this, &SaveDatabaseDialog::refresh);
    connect(ui->mongoDbPageRb, &QRadioButton::toggled, this, &SaveDatabaseDialog::refresh);

    connect(ui->mongoDbPageRb, &QRadioButton::toggled, this, &SaveDatabaseDialog::getMongoDbInfoFromUi);
    connect(ui->mongoDbIpEdit, &QLineEdit::textEdited, this, &SaveDatabaseDialog::getMongoDbInfoFromUi);
    connect(ui->mongoDbPortSpinBox, &QSpinBox::valueChanged, this, &SaveDatabaseDialog::getMongoDbInfoFromUi);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

}

desktop::SaveDatabaseDialog::~SaveDatabaseDialog() = default;

void desktop::SaveDatabaseDialog::refresh()
{
    bool haveValidSaveInfo = false;
    if (ui->customFormatRb->isChecked()) {
        ui->stackedWidget->setCurrentWidget(ui->customFormatPage);
        if (customFileInfo.has_value()) {
            const auto& info = *customFileInfo;
            haveValidSaveInfo = true;
            ui->customFilePathLb->setText(QString::fromStdWString(info.filePath.wstring()));
        } else {
            haveValidSaveInfo = false;
            ui->customFilePathLb->setText("No path");
        }
    } else if (ui->sqlitePageRb->isChecked()) {
        ui->stackedWidget->setCurrentWidget(ui->sqlitePage);
        if (sqliteDbInfo.has_value()) {
            const auto& info = *sqliteDbInfo;
            haveValidSaveInfo = true;
            ui->sqliteFileLb->setText(QString::fromStdWString(info.filePath.wstring()));
        } else {
            haveValidSaveInfo = false;
            ui->sqliteFileLb->setText("No database");
        }
    } else if (ui->mongoDbPageRb->isChecked()) {
        if (mongoDbInfo.has_value()) {
            haveValidSaveInfo = !mongoDbInfo->ipAddress.empty();
            ui->mongoDbIpEdit->setText(QString::fromStdString(mongoDbInfo->ipAddress));
            ui->mongoDbPortSpinBox->setValue(mongoDbInfo->port);
        } else {
            ui->mongoDbIpEdit->setText("localhost");
            ui->mongoDbPortSpinBox->setValue(27017);
            haveValidSaveInfo = true;
        }
        ui->stackedWidget->setCurrentWidget(ui->mongoDbPage);

    } else {
        haveValidSaveInfo = false;
    }
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(haveValidSaveInfo);
}

void desktop::SaveDatabaseDialog::getMongoDbInfoFromUi()
{
    auto uiIp = ui->mongoDbIpEdit->text().toStdString();
    auto uiPort = static_cast<uint16_t>(ui->mongoDbPortSpinBox->value());
    mongoDbInfo = core::save_load::MongoDbInfo{.ipAddress = std::move(uiIp),
                                               .port = uiPort};
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!mongoDbInfo->ipAddress.empty());
}

// helper type for the visitor #4
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

void desktop::SaveDatabaseDialog::setInfo(const core::save_load::Information& info)
{
    customFileInfo.reset();
    mongoDbInfo.reset();
    std::visit(overloaded {
        [this] (const std::monostate&) { ui->customFormatRb->setChecked(true); },
        [this] (const core::save_load::CustomFileInfo& info) {
            ui->customFormatRb->setChecked(true);
            customFileInfo.emplace(info);
        },
        [this] (const core::save_load::SQLiteInfo& info) {
            ui->sqlitePageRb->setChecked(true);
            sqliteDbInfo.emplace(info);
        },
        [this] (const core::save_load::MongoDbInfo& info) {
            ui->mongoDbPageRb->setChecked(true);
            mongoDbInfo.emplace(info);
        },
        [] (auto&& value) { throw std::logic_error("desktop::SaveDatabaseDialog::setInfo not implemented!"); }
    }, info);

    refresh();
}

core::save_load::Information desktop::SaveDatabaseDialog::getInfo()
{
    core::save_load::Information res;
    if (ui->customFormatRb->isChecked() && customFileInfo.has_value()) {
        res = std::move(*customFileInfo);
        customFileInfo.reset();
    } else if (ui->sqlitePageRb->isChecked() && sqliteDbInfo.has_value()) {
        res = std::move(*sqliteDbInfo);
        sqliteDbInfo.reset();
    } else if (ui->mongoDbPageRb->isChecked() && mongoDbInfo.has_value()) {
        res = std::move(*mongoDbInfo);
        mongoDbInfo.reset();
    }
    return res;
}

int desktop::SaveDatabaseDialog::exec(core::save_load::Information& saveInfo)
{
    setInfo(saveInfo);

    const int res = Super::exec();
    if (!res) {
        return res;
    }

    saveInfo = getInfo();
    return res;
}
