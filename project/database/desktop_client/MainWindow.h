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
    ~MainWindow();

private slots:
    void createNewDatabase();
    void createNewTable();

private:
    // Methods
    void reenable();
    void refresh();

    // Fields
    desktop::DatabaseClient dbClient;
    std::unique_ptr<Ui::MainWindow> ui;
    desktop::TableListModel tableListModel;
};

#endif // MAINWINDOW_H
