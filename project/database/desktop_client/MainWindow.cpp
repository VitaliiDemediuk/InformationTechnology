#include "MainWindow.h"
#include "./ui_MainWindow.h"

// Dialogs
#include "NewDbDialog.h"
#include "TableNameDialog.h"

// Core
#include "CustomTable.h"
#include "Database.h"
#include "Commands.h"

#include <iostream>

struct MainWindowData
{
    explicit MainWindowData(QWidget* parent)
        : acRenameTable{"Rename", parent},
          acDeleteTable{"Delete", parent}
    {}

    desktop::DatabaseClient dbClient;
    desktop::TableListModel tableListModel;
    QAction acRenameTable;
    QAction acDeleteTable;
    int lastTableListIdx = -1;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      d(new MainWindowData(this))
{
    ui->setupUi(this);

    // Window size
    setFixedSize(QSize(1400, 800));

    // Stretch factors
    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 3);

    // Enable
    reenable();

    ui->tableListView->setContextMenuPolicy(Qt::CustomContextMenu);

    // Signal-slot connections
    connect(ui->acNewDatabase, &QAction::triggered, this, &MainWindow::createNewDatabase);
    connect(ui->createNewTableBtn, &QPushButton::clicked, this, &MainWindow::createNewTable);

    connect(&d->acRenameTable, &QAction::triggered, this, [this] () {
        const auto idx = d->lastTableListIdx;
        const auto dbName = d->tableListModel.data(d->tableListModel.index(idx, 0)).toString().toStdWString();

        desktop::TableNameDialog dialog(d->dbClient, this);
        if (std::wstring newDbName = dbName; dialog.exec(newDbName) && newDbName != dbName) {
            auto cmd = std::make_unique<core::command::RenameTable>(d->tableListModel.tableId(idx), std::move(newDbName));
            d->dbClient.exec(std::move(cmd));
            refresh();
        }


    });
    connect(&d->acDeleteTable, &QAction::triggered, this, [this] () {
        if (d->lastTableListIdx >= 0) {
            auto cmd = std::make_unique<core::command::DeleteTable>(d->tableListModel.tableId(d->lastTableListIdx));
            d->dbClient.exec(std::move(cmd));
            refresh();
        }
    });

    ui->tableListView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableListView, &QListView::customContextMenuRequested, this,[this] (const QPoint& p) {
        QMenu tableListContextMenu("Table list context menu", this);

        tableListContextMenu.addAction(&d->acRenameTable);
        tableListContextMenu.addAction(&d->acDeleteTable);

        d->lastTableListIdx = ui->tableListView->indexAt(p).row();
        tableListContextMenu.exec(ui->tableListView->mapToGlobal(p));
    });

    // Set models
    ui->tableListView->setModel(&d->tableListModel);
}

MainWindow::~MainWindow() = default;

/////////////// Private Slots //////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::createNewDatabase()
{
    desktop::NewDbDialog dialog(d->dbClient, this);
    if (std::wstring dbName; dialog.exec(dbName)) {
        auto db = std::make_unique<core::Database>(std::move(dbName), std::make_unique<core::CustomTableFactory>());
        d->dbClient.setNewDatabase(std::move(db));
        refresh();
    }
}

void MainWindow::createNewTable()
{
    desktop::TableNameDialog dialog(d->dbClient, this);
    if (std::wstring dbName; dialog.exec(dbName)) {
        auto cmd = std::make_unique<core::command::CreateNewTable>(std::move(dbName));
        d->dbClient.exec(std::move(cmd));
        refresh();
    }
}

/////////////// Private ////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::reenable()
{
    ui->createNewTableBtn->setEnabled(d->dbClient.haveDatabase());
    ui->tableListView->setEnabled(d->dbClient.haveDatabase());
    ui->mainWidget->setEnabled(d->dbClient.haveDatabase());
}

void MainWindow::refresh()
{
    QString defaultTitle = "Database";
    if (d->dbClient.haveDatabase()) {
        defaultTitle.append(" (%1)");
        defaultTitle = defaultTitle.arg(QString::fromStdWString(d->dbClient.databaseName()));
    }
    setWindowTitle(defaultTitle);

    // reset model
    d->dbClient.resetModel(d->tableListModel);

    reenable();
}