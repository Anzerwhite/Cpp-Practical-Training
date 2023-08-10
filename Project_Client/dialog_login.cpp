#include "dialog_login.h"
#include "ui_dialog_login.h"
#include <QtSql>
#include <QDebug>
#include <QMessageBox>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>
#include <QKeyEvent>
//#include <QFileBox>
Dialog_login::Dialog_login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Dialog_login)
{

    //将连接数据库的操作交给服务器
    //    DB = QSqlDatabase::addDatabase("QSQLITE");//添加SQLITE数据库驱动
    //    DB.setDatabaseName("C:\\Users\\13680\\Desktop\\databases_sqlite\\base_user.db");//设置数据库名称(通过路径名设置数据库)
    //    DB.open();//打开数据库

    ui->setupUi(this);
    this->setWindowTitle("登录");
    m_tcpSocket = new QTcpSocket(this);//初始化套接字

    ui->lineEdit_IP->setText("127.0.0.1");
    ui->lineEdit_Port->setText("8899");//设置初始端口和IP
    ui->lineEdit_Account->setFocus();
    connect(ui->btn_Login, &QPushButton::clicked, this, [=](){
        if(m_tcpSocket->state() != QAbstractSocket::ConnectedState)
        {
            m_tcpSocket->connectToHost(QHostAddress(ui->lineEdit_IP->text()),ui->lineEdit_Port->text().toUShort());//连接服务器
        }
            m_tcpSocket->write("0" + ui->lineEdit_Account->text().toUtf8() + ui->lineEdit_Password->text().toUtf8());//六位账号 + 任意位密码
    });
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &Dialog_login::slot_ReadyRead);
    connect(ui->btn_Exit, &QPushButton::clicked, this, &QWidget::close);
}

Dialog_login::~Dialog_login()
{
    delete ui;
}

QTcpSocket *&Dialog_login::getM_tcpSocket()
{
    return m_tcpSocket;
}

void Dialog_login::slot_ReadyRead()
{
    QByteArray prefix = m_tcpSocket->read(1);
    if (prefix == "0")//登录请求相关信息
    {
        QByteArray isCorrect = m_tcpSocket->read(5);
        if (isCorrect == "TRUTH")
        {
            QByteArray Name = m_tcpSocket->readAll();//接受服务器传递过来的昵称
            emit sendAccountAndName(ui->lineEdit_Account->text(), Name);//发送账号与昵称

            emit showMainWindow();
            disconnect(m_tcpSocket, &QTcpSocket::readyRead, this, &Dialog_login::slot_ReadyRead);

            this->destroy();
        }
        else if (isCorrect == "FALSE")
        {
            QMessageBox::information(this, "提示","用户名或密码错误！",QMessageBox::Ok, QMessageBox::Ok );
        }
    }
}

void Dialog_login::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
    {
        emit ui->btn_Login->clicked();
    }
    else if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down)
    {
        if(ui->lineEdit_Account->hasFocus() == true)
        {
            ui->lineEdit_Password->setFocus();
        }
        else if(ui->lineEdit_Password->hasFocus() == true)
        {
            ui->lineEdit_Account->setFocus();
        }
    }
}


