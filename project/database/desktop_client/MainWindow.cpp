#include "MainWindow.h"
#include "./ui_MainWindow.h"

// Dialogs
#include "NewDbDialog.h"
#include "TableNameDialog.h"
#include "ColumnInfoDialog.h"
#include "DbTableModel.h"

// Core
#include "CustomTable.h"
#include "Database.h"
#include "Commands.h"

struct MainWindowData
{
    explicit MainWindowData(QWidget* parent)
        : acRenameTable{"Rename", parent},
          acDeleteTable{"Delete", parent}
    {}

    desktop::DatabaseClient dbClient;
    desktop::TableListModel tableListModel;
    desktop::DbTableModel dbTableModel;
    QAction acRenameTable;
    QAction acDeleteTable;
    int lastTableListIdx = -1;
    std::optional<core::TableId> currentTableId;
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

    connect(&d->acRenameTable, &QAction::triggered, this, &MainWindow::renameTable);
    connect(&d->acDeleteTable, &QAction::triggered, this, &MainWindow::deleteTable);

    ui->tableListView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableListView, &QListView::customContextMenuRequested, this, &MainWindow::showTableListContextMenu);
    connect(ui->tableListView, &QListView::activated, this, &MainWindow::refreshCurrentTableId);
    connect(ui->tableListView, &QListView::entered, this, &MainWindow::refreshCurrentTableId);
    connect(ui->tableListView, &QListView::pressed, this, &MainWindow::refreshCurrentTableId);
    connect(ui->tableListView, &QListView::clicked, this, &MainWindow::refreshCurrentTableId);
    connect(&d->tableListModel, &QAbstractListModel::modelReset, this, [this] () {
        if (ui->tableListView->currentIndex().row() == -1) {
            d->currentTableId.reset();
            refreshTable();
            reenable();
        }
    });

    connect(ui->addColumnBtn, &QPushButton::clicked, this, &MainWindow::addColumn);
    connect(ui->addRowBtn,    &QPushButton::clicked, this, &MainWindow::addRow);

    // Set models
    ui->tableListView->setModel(&d->tableListModel);
    ui->tableView->setModel(&d->dbTableModel);
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
        const auto index = d->tableListModel.index(d->tableListModel.rowCount() - 1, 0);
        ui->tableListView->setCurrentIndex(index);
        refreshCurrentTableId(index);
    }
}

void MainWindow::renameTable()
{
    const auto idx = d->lastTableListIdx;
    const auto dbName = d->tableListModel.data(d->tableListModel.index(idx, 0)).toString().toStdWString();

    desktop::TableNameDialog dialog(d->dbClient, this);
    if (std::wstring newDbName = dbName; dialog.exec(newDbName) && newDbName != dbName) {
        auto cmd = std::make_unique<core::command::RenameTable>(d->tableListModel.tableId(idx), std::move(newDbName));
        d->dbClient.exec(std::move(cmd));
        refresh();
    }
}

void MainWindow::deleteTable()
{
    if (d->lastTableListIdx >= 0) {
        auto cmd = std::make_unique<core::command::DeleteTable>(d->tableListModel.tableId(d->lastTableListIdx));
        d->dbClient.exec(std::move(cmd));
        refresh();
        if (const auto rowCount = d->tableListModel.rowCount(); rowCount > 0) {
            const auto index = d->tableListModel.index(std::min(rowCount - 1, d->lastTableListIdx), 0);
            ui->tableListView->setCurrentIndex(index);
            refreshCurrentTableId(index);
        }
    }
}

void MainWindow::showTableListContextMenu(const QPoint& p)
{
    QMenu tableListContextMenu("Table list context menu", this);

    tableListContextMenu.addAction(&d->acRenameTable);
    tableListContextMenu.addAction(&d->acDeleteTable);

    d->lastTableListIdx = ui->tableListView->indexAt(p).row();
    tableListContextMenu.exec(ui->tableListView->mapToGlobal(p));
}

void MainWindow::refreshCurrentTableId(const QModelIndex& index)
{
    const auto id = d->tableListModel.tableId(index.row());
    if (!d->currentTableId || d->currentTableId != id) {
        d->currentTableId = id;
        refreshTable();
        reenable();
    }
}

void MainWindow::addColumn()
{
    if (!d->currentTableId.has_value()) {
        return;
    }

    desktop::ColumnInfoDialog dialog(d->dbClient, this);
    if (std::unique_ptr<core::VirtualColumnInfo> newColumnInfo; dialog.exec(newColumnInfo)) {
        auto cmd = std::make_unique<core::command::AddColumn>(d->currentTableId.value(), std::move(newColumnInfo));
        d->dbClient.exec(std::move(cmd));
        refreshTable();
        reenable();
    }
}

void MainWindow::addRow()
{
    auto cmd = std::make_unique<core::command::AddRow>(d->currentTableId.value());
    d->dbClient.exec(std::move(cmd));
    refreshTable();
    reenable();
}

/////////////// Private ////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::reenable()
{
    ui->createNewTableBtn->setEnabled(d->dbClient.hasDatabase());
    ui->tableListView->setEnabled(d->dbClient.hasDatabase());
    ui->mainWidget->setEnabled(d->dbClient.hasDatabase() && d->currentTableId.has_value());

    if (d->currentTableId) {
        const auto* table = d->dbClient.table(d->currentTableId.value());
        ui->addRowBtn->setEnabled(table && table->columnCount() > 1);
    }
}

void MainWindow::refreshTable()
{
    d->dbClient.resetModel(d->currentTableId, d->dbTableModel);
}

void MainWindow::refresh()
{
    QString defaultTitle = "Database";
    if (d->dbClient.hasDatabase()) {
        defaultTitle.append(" (%1)");
        defaultTitle = defaultTitle.arg(QString::fromStdWString(d->dbClient.databaseName()));
    }
    setWindowTitle(defaultTitle);

    // reset model
    d->dbClient.resetModel(d->tableListModel);

    refreshTable();

    reenable();
}
