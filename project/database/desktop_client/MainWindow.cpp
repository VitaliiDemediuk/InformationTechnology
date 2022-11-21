#include "MainWindow.h"
#include "./ui_MainWindow.h"

// Qt
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include <MongoDbSaveLoadStrategy.h>
#include <SQLiteSaveLoadStrategy.h>

// gRpc
#include "gRpcDatabase.h"

// Dialogs
#include "NewDbDialog.h"
#include "TableNameDialog.h"
#include "ColumnInfoDialog.h"
#include "SaveDatabaseDialog.h"
#include "LoadFromMongoDbDialog.h"
#include "CreateCartesianProductDialog.h"
#include "ConnectTogRpcServerDialog.h"

// Models
#include "TableListModel.h"
#include "DbTableModel.h"

// Core
#include "CustomTable.h"
#include "Database.h"
#include "Commands.h"

struct MainWindowData
{
    explicit MainWindowData(MainWindow* parent)
        : acRenameTable{"Rename table", parent},
          acDeleteTable{"Delete table", parent},
          acRenameColumn{"Rename column", parent},
          acDeleteColumn{"Delete column", parent},
          acDeleteRow{"Delete row", parent},
          dbTableDelegate{*parent, parent}
    {}

    std::optional<core::TableId> currentTableId;

    desktop::DatabaseClient dbClient;
    desktop::TableListModel tableListModel;
    desktop::DbTableModel dbTableModel;
    desktop::DbTableDelegate dbTableDelegate;

    QAction acRenameTable;
    QAction acDeleteTable;
    QAction acRenameColumn;
    QAction acDeleteColumn;
    QAction acDeleteRow;

    struct CombosData {
        int tableListIdx = -1;
        int columnIdx = -1;
        int rowIdx = -1;
    } combosData;
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

    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // Signal-slot connections
    connect(ui->acNewDatabase, &QAction::triggered, this, &MainWindow::createNewDatabase);
    connect(ui->createNewTableBtn, &QPushButton::clicked, this, &MainWindow::createNewTable);

    connect(&d->acRenameTable, &QAction::triggered, this, &MainWindow::renameTable);
    connect(&d->acDeleteTable, &QAction::triggered, this, &MainWindow::deleteTable);
    connect(&d->acRenameColumn, &QAction::triggered, this, &MainWindow::renameColumn);
    connect(&d->acDeleteColumn, &QAction::triggered, this, &MainWindow::deleteColumn);
    connect(&d->acDeleteRow, &QAction::triggered, this, &MainWindow::deleteRow);

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

    ui->tableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView->horizontalHeader(), &QTableView::customContextMenuRequested, this, &MainWindow::showHorizontalHeaderContextMenu);
    ui->tableView->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView->verticalHeader(), &QTableView::customContextMenuRequested, this, &MainWindow::showVerticalHeaderContextMenu);

    connect(ui->acSave, &QAction::triggered, this, &MainWindow::save);
    connect(ui->acSaveAs, &QAction::triggered, this, &MainWindow::saveAs);

    connect(ui->acOpenFromCustomFormatFile, &QAction::triggered, this, &MainWindow::openFromCustomFormatFile);
    connect(ui->acLoadFromMongoDb, &QAction::triggered, this, &MainWindow::loadFromMongoDb);
    connect(ui->acOpenFromSQLite, &QAction::triggered, this, &MainWindow::openFromSQLiteFile);

    connect(ui->acCartesianProduct, &QAction::triggered, this, &MainWindow::createCartesianProduct);

    connect(ui->acConnectTogRpcServer, &QAction::triggered, this, &MainWindow::connectTogRpcServer);

    // Set models
    ui->tableListView->setModel(&d->tableListModel);
    ui->tableView->setModel(&d->dbTableModel);

    // Set delegates
    ui->tableView->setItemDelegate(&d->dbTableDelegate);
}

MainWindow::~MainWindow() = default;

void MainWindow::editCell(size_t rowIdx, size_t columnIdx, core::CellData data)
{
    if (!d->currentTableId) {
        return;
    }

    const auto rowId = d->dbTableModel.rowId(rowIdx);
    auto cmd = std::make_unique<core::command::EditCell>(*d->currentTableId, rowId, columnIdx, std::move(data));
    d->dbClient.exec(std::move(cmd));
    refreshTable();
}

/////////////// Private Slots //////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::createNewDatabase()
{
    if (proposeSavingsIfNecessary() == Continue::NO) {
        return;
    }

    desktop::NewDbDialog dialog(d->dbClient, this);
    if (std::wstring dbName; dialog.exec(dbName)) {
        core::Database ddb(std::move(dbName), std::make_unique<core::CustomTableFactory>());
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
    if (d->combosData.tableListIdx < 0) {
        return;
    }

    const auto idx = d->combosData.tableListIdx;
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
    if (d->combosData.tableListIdx >= 0) {
        auto cmd = std::make_unique<core::command::DeleteTable>(d->tableListModel.tableId(d->combosData.tableListIdx));
        d->dbClient.exec(std::move(cmd));
        refresh();
        if (const auto rowCount = d->tableListModel.rowCount(); rowCount > 0) {
            const auto index = d->tableListModel.index(std::min(rowCount - 1, d->combosData.tableListIdx), 0);
            ui->tableListView->setCurrentIndex(index);
            refreshCurrentTableId(index);
        }
    }
}

void MainWindow::renameColumn()
{
    if (d->combosData.columnIdx < 0 || !d->currentTableId) {
        return;
    }

    const auto tableId = d->currentTableId.value();
    const auto colIdx = d->combosData.columnIdx;

    const auto& columnInfo = d->dbClient.table(tableId)->column(colIdx);
    desktop::ColumnInfoDialog dialog{d->dbClient, this};
    if (std::wstring newName; dialog.exec(columnInfo, newName)) {
        auto cmd = std::make_unique<core::command::RenameColumn>(tableId, colIdx, std::move(newName));
        d->dbClient.exec(std::move(cmd));
        refreshTable();
        reenable();
    }
}

void MainWindow::deleteColumn()
{
    if (d->combosData.columnIdx >= 0 and d->currentTableId) {
        auto cmd = std::make_unique<core::command::DeleteColumn>(d->currentTableId.value(), d->combosData.columnIdx);
        d->dbClient.exec(std::move(cmd));
        refreshTable();
        reenable();
    }
}

void MainWindow::deleteRow()
{
    if (d->combosData.rowIdx >= 0 and d->currentTableId) {
        auto cmd = std::make_unique<core::command::DeleteRow>(d->currentTableId.value(),
                                                              d->dbTableModel.rowId(d->combosData.rowIdx));
        d->dbClient.exec(std::move(cmd));
        refreshTable();
        reenable();
    }
}

void MainWindow::showTableListContextMenu(const QPoint& p)
{
    QMenu tableListContextMenu("Table list context menu", this);

    tableListContextMenu.addAction(&d->acRenameTable);
    tableListContextMenu.addAction(&d->acDeleteTable);

    d->combosData.tableListIdx= ui->tableListView->indexAt(p).row();
    tableListContextMenu.exec(ui->tableListView->mapToGlobal(p));
}

void MainWindow::showHorizontalHeaderContextMenu(const QPoint& p)
{
    QMenu tableListContextMenu("Horizontal header context menu", this);

    const auto* headerView = ui->tableView->horizontalHeader();
    const auto colIdx = d->combosData.columnIdx = headerView->logicalIndexAt(p);

    d->acRenameColumn.setEnabled(colIdx > 0);
    d->acDeleteColumn.setEnabled(colIdx > 0);

    tableListContextMenu.addAction(&d->acRenameColumn);
    tableListContextMenu.addAction(&d->acDeleteColumn);

    tableListContextMenu.exec(headerView->mapToGlobal(p));
}

void MainWindow::showVerticalHeaderContextMenu(const QPoint& p)
{
    QMenu tableListContextMenu("Vertical header context menu", this);

    const auto* verticalView = ui->tableView->verticalHeader();
    const auto rowIdx = d->combosData.rowIdx = verticalView->logicalIndexAt(p);

    d->acRenameColumn.setEnabled(rowIdx > 0);

    tableListContextMenu.addAction(&d->acDeleteRow);

    tableListContextMenu.exec(verticalView->mapToGlobal(p));
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

bool MainWindow::save()
{
    const auto& info = d->dbClient.lastSaveInfo();
    if (std::holds_alternative<std::monostate>(info)) {
        return saveAs();
    } else {
        auto cmd = std::make_unique<core::command::SaveDatabase>(info);
        d->dbClient.exec(std::move(cmd));
        return true;
    }
}

bool MainWindow::saveAs()
{
    auto saveInfo = d->dbClient.lastSaveInfo();
    if (std::holds_alternative<std::monostate>(saveInfo)) {
        std::filesystem::path path{QDir::homePath().toStdWString()};
        path.append(d->dbClient.databaseName() + L".db");
        saveInfo = core::save_load::CustomFileInfo{.filePath = std::move(path)};
    }

    desktop::SaveDatabaseDialog dialog(this);
    if (dialog.exec(saveInfo)) {
        if (std::holds_alternative<core::save_load::MongoDbInfo>(saveInfo)) {
            std::get<core::save_load::MongoDbInfo>(saveInfo).dbName = d->dbClient.databaseName();
        }
        auto cmd = std::make_unique<core::command::SaveDatabase>(std::move(saveInfo));
        d->dbClient.exec(std::move(cmd));
        refresh();
        return true;
    }
    return false;
}

void MainWindow::openFromCustomFormatFile()
{
    if (proposeSavingsIfNecessary() == Continue::NO) {
        return;
    }

    auto filename = QFileDialog::getOpenFileName(this, "Open file", "","Database (*.cdb)");
    if (!filename.isEmpty()) {
        core::save_load::CustomFileStrategy saveLoadStrategy{{.filePath = filename.toStdWString()}};
        auto newDb = saveLoadStrategy.load();
        d->dbClient.setNewDatabase(std::move(newDb));
        refresh();
    }
}

void MainWindow::openFromSQLiteFile()
{
    if (proposeSavingsIfNecessary() == Continue::NO) {
        return;
    }

    auto filename = QFileDialog::getOpenFileName(this, "Open SQLite file", "","Database (*.db3)");
    if (!filename.isEmpty()) {
        core::save_load::SQLiteStrategy saveLoadStrategy{{.filePath = filename.toStdWString()}};
        auto newDb = saveLoadStrategy.load();
        d->dbClient.setNewDatabase(std::move(newDb));
        refresh();
    }
}

void MainWindow::loadFromMongoDb()
{
    if (proposeSavingsIfNecessary() == Continue::NO) {
        return;
    }

    desktop::LoadFromMongoDbDialog dialog(d->dbClient, this);
    core::save_load::MongoDbInfo saveInfo;

    if (dialog.exec(saveInfo)) {
        core::save_load::MongoDbStrategy saveLoadStrategy{std::move(saveInfo)};
        auto newDb = saveLoadStrategy.load();
        d->dbClient.setNewDatabase(std::move(newDb));
        refresh();
    }
}

void MainWindow::createCartesianProduct()
{
    std::vector<std::pair<size_t, std::wstring>> tableList;
    d->dbClient.forAllTables([&tableList] (const core::VirtualTable& table) {
        tableList.emplace_back(table.id(), table.name());
    });

    core::TableId firstTableId{};
    core::TableId secondTableId{};
    desktop::CreateCartesianProductDialog dialog(this);
    if (dialog.exec(tableList, firstTableId, secondTableId)) {
        auto cmd = std::make_unique<core::command::CreateCartesianProduct>(firstTableId, secondTableId);
        d->dbClient.exec(std::move(cmd));
        refresh();
    }
}

#include <iostream>

void MainWindow::connectTogRpcServer()
{
    if (proposeSavingsIfNecessary() == Continue::NO) {
        return;
    }

    desktop::ConnectTogRpcServerDialog dialog(this);

    std::string ip;
    uint16_t port{};
    if (dialog.exec(ip, port)) {
        auto database = std::make_unique<db_grpc_client::Database>(std::move(ip), port);
        d->dbClient.setNewDatabase(std::move(database));
        refresh();
    }
}

/////////////// Private ////////////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::reenable()
{
    ui->createNewTableBtn->setEnabled(d->dbClient.hasDatabase());
    ui->tableListView->setEnabled(d->dbClient.hasDatabase());
    ui->mainWidget->setEnabled(d->dbClient.hasDatabase() && d->currentTableId.has_value());
    ui->acSave->setEnabled(d->dbClient.hasDatabase());
    ui->acSaveAs->setEnabled(d->dbClient.hasDatabase());
    ui->acCartesianProduct->setEnabled(d->dbClient.hasDatabase());

    if (d->currentTableId) {
        const auto* table = d->dbClient.table(d->currentTableId.value());
        ui->addRowBtn->setEnabled(table && table->columnCount() > 1);
    }
}

void MainWindow::refreshTable()
{
    d->dbTableDelegate.setTable(d->currentTableId ? d->dbClient.table(*d->currentTableId) : nullptr);
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

MainWindow::Continue MainWindow::proposeSavingsIfNecessary()
{
    if (!d->dbClient.hasDatabase() || !d->dbClient.hasChanges()) {
        return Continue::YES;
    }

    const auto res = QMessageBox::question(this, "Save database?",
                                           "The database has been modified. Do you want to save your changes?",
                                           QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                           QMessageBox::Save);

    switch (res) {
    case QMessageBox::Save: {
        return save() ? Continue::YES : Continue::NO;
    }
    case QMessageBox::Discard: {
        return Continue::YES;
    }
    case QMessageBox::Cancel: {
        return Continue::NO;
    }
    default: {
        throw std::logic_error("Unexpected result from QMessageBox::question.");
    }
    }
}
