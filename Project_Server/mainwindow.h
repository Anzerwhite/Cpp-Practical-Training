#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include<QTcpServer>
#include <QTcpSocket>
#include <QtSql>
#include <vector>
#include "taskserver.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);
    void slot_erase();
signals:
    void timeToExit(int recode = 0);
private slots:
    void on_pushButton_Start_clicked();//开启监听

    void on_pushButton_Check_clicked();


private:
    Ui::MainWindow *ui;
    QSqlDatabase DB;//数据库
    QTcpServer * ptrServer;//服务器的指针
    //QTcpSocket* m_tcpSocket;//套接字()

    QVector<QTcpSocket*> m_tcpSocketArray;//服务器指针数组
    QVector<TaskServer*> m_taskServerArray;//套接字数组
};
#endif // MAINWINDOW_H
