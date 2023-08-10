#include "mainwindow.h"
#include "dialog_login.h"
#include "place.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Dialog_login d;//登录界面
    MainWindow w(d.getM_tcpSocket());
    QObject::connect(&d, &Dialog_login::showMainWindow, &w, &MainWindow::slot_showMainWindow);//连接信号
    QObject::connect(&d, &Dialog_login::sendAccountAndName, &w, &MainWindow::slot_sendAccountAndName);//连接账号的发送与接受

    d.show();


    return a.exec();
}
