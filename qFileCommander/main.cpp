#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("qFileCommander");
    QCoreApplication::setOrganizationDomain("qFileCommander.com");
    QCoreApplication::setApplicationName("qFileCommander");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
