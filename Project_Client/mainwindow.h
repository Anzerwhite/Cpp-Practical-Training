#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTcpSocket>
#include <QShortcut>
#include "place.h"
#include <vector>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QTcpSocket*& _tcpSocket, QWidget *parent = nullptr);
    ~MainWindow();
    void closeEvent(QCloseEvent* event);
public slots:
    void slot_showMainWindow();//显示客户端主界面
    void slot_sendAccountAndName(QString _userAccount, QString _userName);//接收账号
private slots:


    void on_pushButton_Canteen_clicked();

    void on_pushButton_Dorm_clicked();

    void on_pushButton_Library_clicked();

    void on_pushButton_Basketball_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket* m_tcpSocket;//套接字
    QString userAccount;//用户账号
    QString userName;//用户昵称
    QString userGender;//用户性别
    QString userSignature;//用户签名
    QByteArray userPhoto;//用户照片
    QVector<Place*> m_placeArray;//存放地点的数组
    bool isFirst;
};
#endif // MAINWINDOW_H
