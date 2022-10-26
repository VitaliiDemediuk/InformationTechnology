#include "MainWindow.h"

// Qt
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    desktop::MainWindow w;
    w.show();
    return a.exec();
}
