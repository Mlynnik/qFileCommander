#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("qFileCommander");
    QCoreApplication::setOrganizationDomain("qFileCommander.com");
    QCoreApplication::setApplicationName("qFileCommander 1.0");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
