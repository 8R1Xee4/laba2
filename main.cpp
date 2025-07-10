#include <QApplication>
#include<iostream>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // 1) Create the QApplication instance
    QApplication app(argc, argv);

    // 2) Instantiate and show your MainWindow
    MainWindow w;
    w.show();

    // 3) Enter the Qt event loop
    return app.exec();
}