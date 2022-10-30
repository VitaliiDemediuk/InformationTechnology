#include "MainWindow.h"
#include "./ui_MainWindow.h"

// Dialogs
#include "NewDbDialog.h"
#include "NewTableDialog.h"

// Core
#include "CustomTable.h"
#include "Database.h"
#include "Commands.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Window size
    setFixedSize(QSize(1400, 800));

    // Stretch factors
    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 3);

    // Enable
    reenable();

    // Signal-slot connections
    connect(ui->acNewDatabase, &QAction::triggered, this, &MainWindow::createNewDatabase);
    connect(ui->createNewTableBtn, &QPushButton::clicked, this, &MainWindow::createNewTable);

    // Set models
    ui->tableListView->setModel(&tableListModel);
}

MainWindow::~MainWindow() = default;

/////////////// Private Slots //////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::createNewDatabase()
{
    desktop::NewDbDialog dialog(dbClient, this);
    if (std::wstring dbName; dialog.exec(dbName)) {
        auto db = std::make_unique<core::Database>(std::move(dbName), std::make_unique<core::CustomTableFactory>());
        dbClient.setNewDatabase(std::move(db));
        refresh();
    }
}

void MainWindow::createNewTable()
{
    desktop::NewTableDialog dialog(dbClient, this);
    if (std::wstring dbName; dialog.exec(dbName)) {
        auto cmd = std::make_unique<core::command::CreateNewTable>(std::move(dbName));
        dbClient.exec(std::move(cmd));
        refresh();
    }
}

/////////////// Private ////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::reenable()
{
    ui->createNewTableBtn->setEnabled(dbClient.haveDatabase());
    ui->tableListView->setEnabled(dbClient.haveDatabase());
    ui->tableView->setEnabled(dbClient.haveDatabase());
}

void MainWindow::refresh()
{
    QString defaultTitle = "Database";
    if (dbClient.haveDatabase()) {
        defaultTitle.append(" (%1)");
        defaultTitle = defaultTitle.arg(QString::fromStdWString(dbClient.databaseName()));
    }
    setWindowTitle(defaultTitle);

    // reset model
    dbClient.resetModel(tableListModel);

    reenable();
}
