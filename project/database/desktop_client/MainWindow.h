#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt
#include <QMainWindow>

// Local
#include "TableListModel.h"
#include "DesktopDatabaseClient.h"

namespace Ui { class MainWindow; }

class MainWindow: public QMainWindow
{
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void createNewDatabase();
    void createNewTable();
    void renameTable();
    void deleteTable();
    void showTableListContextMenu(const QPoint& p);
    void refreshCurrentTableId(const QModelIndex& index);
    void addColumn();

private:
    // Methods
    void reenable();
    void refreshTable();
    void refresh();

    // Fields
    std::unique_ptr<Ui::MainWindow> ui;
    std::unique_ptr<struct MainWindowData> d;
};

#endif // MAINWINDOW_H
