#include "MainWindow.h"
#include "./ui_mainwindow.h"

desktop::MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Window size
    setFixedSize(QSize(1400, 800));

    // Stretch factors
    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 3);


}

desktop::MainWindow::~MainWindow() = default;

