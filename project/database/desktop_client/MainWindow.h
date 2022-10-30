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

private:
    // Methods
    void reenable();
    void refresh();

    // Fields
    std::unique_ptr<Ui::MainWindow> ui;
    std::unique_ptr<struct MainWindowData> d;
};

#endif // MAINWINDOW_H
