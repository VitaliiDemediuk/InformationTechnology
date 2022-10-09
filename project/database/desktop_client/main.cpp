#include "mainwindow.h"

#include <QApplication>

#include "library.h"

int main(int argc, char *argv[])
{
    hello();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
