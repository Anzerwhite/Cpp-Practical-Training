#include "mainwindow.h"
#include "dialog_update.h"
#include"login_base.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    Login_base l;
    QObject::connect(&l,&Login_base::showMainWindow, &w, &MainWindow::showMainWindow_login);
    l.show();
    return a.exec();
}
