#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_place.h"
#include <QApplication>
#include <QHostAddress>
#include "dialog_login.h"
#include <QDebug>
#include <QKeySequence>
#include <QPixmap>
MainWindow::MainWindow(QTcpSocket*& _tcpSocket,QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //Ui::Place ui_place;
    m_tcpSocket = _tcpSocket;//接受套接字
    this->setWindowTitle("客户端");
    isFirst = true;
    //——————————————————添加地点——————————————————
    Place* canteen = new Place(0,"食堂",m_tcpSocket,this);
    Place* dorm = new Place(1,"宿舍",m_tcpSocket,this);
    Place* library = new Place(2,"图书馆",m_tcpSocket,this);
    Place* basketball = new Place(3,"篮球场",m_tcpSocket,this);

    m_placeArray.push_back(canteen);
    m_placeArray.push_back(dorm);
    m_placeArray.push_back(library);
    m_placeArray.push_back(basketball);//创建四个地点对象并放入数组中


    ui->tabWidget->setCurrentIndex(0);
    QLabel * label_ConnetionStatus = new QLabel(this);
    ui->statusbar->addWidget(label_ConnetionStatus);
    label_ConnetionStatus->setText("连接状态：");
    QLabel * label_IsConnected = new QLabel(this);//创建一个Label表示连接状态
    ui->statusbar->addWidget(label_IsConnected);
    QPixmap  pixmax_check = QPixmap(":/Icons/Cancel.png");
    pixmax_check = pixmax_check.scaled(QSize(30,30));
    label_IsConnected->setPixmap(pixmax_check);
    connect(ui->tabWidget, &QTabWidget::tabBarClicked, this, [=](int index){
            if(index == 1)//个人信息界面被点击
            {
                if(isFirst == true)//首次点击
                {
                    isFirst = false;
                    //初始化请求： "6" + 六位账号
                   // qDebug() << "userAccount = " << userAccount;
                    m_tcpSocket->write("6" + userAccount.toUtf8());

                }
            }
        });
    //————————————————————读取来自服务器的信息————————————————————
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, [=](){
        int prefix = m_tcpSocket->read(1).toInt();
        qDebug() << "prefix = " << prefix;

        switch (prefix)
        {
        //普通用户的消息
        case 1:
        {
            QByteArray placeNum = m_tcpSocket->read(1);//读取地点编号
            int int_placeNum = placeNum.toInt();
            qDebug() << "int_placeNum = " << int_placeNum;
            QByteArray data = m_tcpSocket->readAll();
            qDebug() << "client received message = " << data;
            m_placeArray[int_placeNum]->getUi()->plainTextEdit_History->appendPlainText(data);//加入文本
            //    ui->plainTextEdit_History->appendPlainText(data);
        }
            break;
            //官方服务器的消息
        case 2:
        {
            QByteArray placeNum = m_tcpSocket->read(1);//读取地点编号
            int int_placeNum = placeNum.toInt();
            QByteArray data = m_tcpSocket->readAll();
            qDebug() << "client received message = " << data;
            if (placeNum == "a")
            {
                for (Place* place :m_placeArray)
                {
                    place->getUi()->plainTextEdit_History->appendPlainText("官方服务器: " + data);//加入文本
                }
            }
            else
            {
                if(int_placeNum >= m_placeArray.size())
                {
                    //越界不发送
                }
                else
                {
                    m_placeArray[int_placeNum]->getUi()->plainTextEdit_History->appendPlainText("官方服务器: " + data);//加入文本
                }
            }
        }
            break;
            //地点初始化
        case 3:
        {
            //获取地点标号
            int placeNum = m_tcpSocket->read(1).toInt();
            Ui::Place* ui_place = m_placeArray[placeNum]->getUi();
            //获取并设置昵称
            int sizeName = m_tcpSocket->read(2).toInt();
            QString Name = m_tcpSocket->read(sizeName);
            m_placeArray[placeNum]->setName(Name);
            ui_place->lineEdit_Name->setText(Name);
            //获取并设置地址
            int sizeAddress = m_tcpSocket->read(3).toInt();
            QString Address = m_tcpSocket->read(sizeAddress);
            m_placeArray[placeNum]->setAddress(Address);
            ui_place->lineEdit_Address->setText(Address);
            //获取并设置简介
            int sizeIntroduction = m_tcpSocket->read(3).toInt();
            QString Introduction = m_tcpSocket->read(sizeIntroduction);
            m_placeArray[placeNum]->setIntroduction(Introduction);
            ui_place->plainTextEdit_Introduction->setPlainText(Introduction);
            //获取并设置照片

            QByteArray byteArray_Photo = m_tcpSocket->readAll();
            QPixmap Photo;
            qDebug() << "here Photo";
            Photo.loadFromData(byteArray_Photo);
            m_placeArray[placeNum]->setPhoto(Photo);
            ui_place->label_Photo->setPixmap(Photo.scaled(ui_place->label_Photo->size().width(), ui_place->label_Photo->size().height(), Qt::IgnoreAspectRatio));
        }
            break;
        case 4:
        {
            int placeNum = m_tcpSocket->read(1).toInt();
            QByteArray data = m_tcpSocket->readAll();
            m_placeArray[placeNum]->getUi()->plainTextEdit_OldRecord->appendPlainText(data + "\n");

        }
            break;
            //个人信息初始化
        case 6://格式： "6" + 两位昵称大小 + 昵称 + 一位性别大小 + 性别 + 3位签名大小 + 签名 + 照片
        {
            int sizeName = m_tcpSocket->read(2).toInt();
            qDebug() << "sizeName = " << sizeName;
            userName = QString(m_tcpSocket->read(sizeName));
            qDebug() << "_userName = " << userName;
            int sizeGender = m_tcpSocket->read(1).toInt();
            userGender =QString( m_tcpSocket->read(sizeGender));
            qDebug() << "userGender = " << userGender;
            int sizeSignature = m_tcpSocket->read(3).toInt();
            userSignature =QString( m_tcpSocket->read(sizeSignature));
            qDebug() << "userSignature = " << userSignature;
            userPhoto = m_tcpSocket->readAll();
            ui->lineEdit_Name->setText(userName);
            ui->lineEdit_Gender->setText(userGender);
            ui->plainTextEdit_Signature->appendPlainText(userSignature);
            QPixmap Photo;
            Photo.loadFromData(userPhoto);
            ui->label_Photo->setPixmap(Photo.scaled(ui->label_Photo->size().width(), ui->label_Photo->size().height(), Qt::IgnoreAspectRatio));
        }
            break;
            //错误情况
        default:
        {
            QByteArray bin = m_tcpSocket->readAll();//垃圾桶
            qDebug() << "Error Prefix From Server";
        }
            break;
        }
    });

    //——————————————————————断开连接后更改连接图标————————————————————
    connect(m_tcpSocket, &QTcpSocket::disconnected, this, [=](){
        m_tcpSocket->close();//关闭套接字
        label_IsConnected->setPixmap(QPixmap(":/Icons/Cancel.png").scaled(30,30));//重新设置状态
    });
    //————————————————————————连接后更改图标————————————————————————————
    connect(m_tcpSocket, &QTcpSocket::connected, this, [=](){
        label_IsConnected->setPixmap(QPixmap(":/Icons/Check.png").scaled(30,30));//重新设置状态
    });
}

MainWindow::~MainWindow()
{
    m_tcpSocket->close();
}


void MainWindow::slot_showMainWindow()
{

    this->show();
}

void MainWindow::slot_sendAccountAndName(QString _userAccount, QString _userName)
{
    userAccount = _userAccount;//接收账号，用于之后的各种查询
    userName = _userName;//接受昵称,便于显示
    for (Place* place: m_placeArray)
    {
        place->setUserAccount(userAccount);
        place->setUserName(userName);
    }
}



void MainWindow::on_pushButton_Canteen_clicked()
{
    m_placeArray[0]->getUi()->tabWidget->setCurrentIndex(0);
    m_placeArray[0]->show();

}

void MainWindow::on_pushButton_Dorm_clicked()
{
    m_placeArray[1]->getUi()->tabWidget->setCurrentIndex(0);
    m_placeArray[1]->show();
}


void MainWindow::on_pushButton_Library_clicked()
{
    m_placeArray[2]->getUi()->tabWidget->setCurrentIndex(0);
    m_placeArray[2]->show();
}

void MainWindow::on_pushButton_Basketball_clicked()
{
    m_placeArray[3]->getUi()->tabWidget->setCurrentIndex(0);
    m_placeArray[3]->show();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    event->accept();
    QApplication::exit();
}
