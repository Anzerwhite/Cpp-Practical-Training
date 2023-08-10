#ifndef DIALOG_LOGIN_H
#define DIALOG_LOGIN_H

#include <QWidget>
#include <QSqlDatabase>
#include<QTcpSocket>
namespace Ui {
class Dialog_login;
}

class Dialog_login : public QWidget
{
    Q_OBJECT

public:
    explicit Dialog_login(QWidget *parent = nullptr);
    ~Dialog_login();
    QTcpSocket*& getM_tcpSocket();
    void keyPressEvent(QKeyEvent *event) override;

signals:
    void showMainWindow();//显示主界面
    void sendAccountAndName(QString _userAccount, QString _userName);//发送账号给客户端
public slots:
    void slot_ReadyRead();

private:
    Ui::Dialog_login *ui;
    QTcpSocket * m_tcpSocket;
    //QSqlDatabase DB;

};

#endif // DIALOG_LOGIN_H
