#ifndef PLACE_H
#define PLACE_H

#include <QMainWindow>
#include <QApplication>
#include <QHostAddress>
#include "dialog_login.h"
#include <QDebug>
#include <QKeySequence>

namespace Ui {
class Place;
}

class Place : public QMainWindow
{
    Q_OBJECT

public:
    explicit Place(int _placeNum, QString _Name, QTcpSocket*& _tcpSocket, QWidget *parent = nullptr);
    ~Place();

    Ui::Place * getUi(){return ui;}
    void setUserAccount(QString _userAccount){ m_userAccount = _userAccount;}
    void setUserName(QString _userName){ m_userName = _userName;}
    void setName(QString _Name){m_Name = _Name;}
    void setAddress(QString _Address){ m_Address = _Address;}
    void setIntroduction(QString _Introduction){ m_Introduction = _Introduction;}
    void setPhoto(QPixmap _Photo){m_Photo = _Photo;}
private slots:
    void on_pushButton_Send_clicked();
private:
    Ui::Place *ui;
    QTcpSocket* m_tcpSocket;//套接字
    QString m_userAccount;//用户账号
    QString m_userName;//用户昵称
    int m_placeNum;//地点的标号
    QString m_Name;//地点的名称
    QString m_Address;//地点的地址
    QString m_Introduction;//地点简介
    QPixmap m_Photo;//照片
    bool isFirst;//初始化条件
};

#endif // PLACE_H
