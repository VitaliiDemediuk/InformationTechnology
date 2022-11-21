#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt
#include <QMainWindow>

// Local
#include "DesktopDatabaseClient.h"
#include "DbTableDelegate.h"

namespace Ui { class MainWindow; }

class MainWindow: public QMainWindow,
                  public desktop::DbCellEditor
{
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    // desktop::DbCellEditor
    void editCell(size_t rowIdx, size_t columnIdx, core::CellData data) final;

private slots:
    void createNewDatabase();
    void createNewTable();
    void renameTable();
    void deleteTable();
    void renameColumn();
    void deleteColumn();
    void deleteRow();
    void showTableListContextMenu(const QPoint& p);
    void showHorizontalHeaderContextMenu(const QPoint& p);
    void showVerticalHeaderContextMenu(const QPoint& p);
    void refreshCurrentTableId(const QModelIndex& index);
    void addColumn();
    void addRow();
    bool save();
    bool saveAs();
    void openFromCustomFormatFile();
    void openFromSQLiteFile();
    void loadFromMongoDb();
    void createCartesianProduct();
    void connectTogRpcServer();

private:
    enum class Continue : bool { NO = false, YES = true};

    // Methods
    void reenable();
    void refreshTable();
    void refresh();
    Continue proposeSavingsIfNecessary();

    // Fields
    std::unique_ptr<Ui::MainWindow> ui;
    std::unique_ptr<struct MainWindowData> d;
};

#endif // MAINWINDOW_H
