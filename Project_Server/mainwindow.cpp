#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSqlQuery>
#include <QDebug>
#include <QThread>

#include <QFileDialog>
#include <QMessageBox>
#include <QShortcut>
#include <QThreadPool>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    DB = QSqlDatabase::addDatabase("QSQLITE");//添加SQLITE数据库驱动
//    DB.setDatabaseName("C:\\Users\\13680\\Desktop\\databases_sqlite\\base_user.db");//设置数据库名称(通过路径名设置数据库)
//    DB.open();//打开数据库

    DB = QSqlDatabase::addDatabase("QSQLITE");//添加SQLITE数据库驱动
    //唤出一个文件框,将选择的文件路径返回
     QString aFile;
    do
    {
     aFile = QFileDialog::getOpenFileName(this, "选择数据库文件", "", "SQLite数据库文件(*.db)");
    }
    while(aFile.isEmpty() == true);//未选择文件
    DB.setDatabaseName(aFile);//设置数据库名称(通过路径名设置数据库)
    if(DB.open() == false)//打开失败
    {//唤出一个提示框
        QMessageBox::warning(this, "Error", "打开数据库失败, 错误信息:\n" + DB.lastError().text(),QMessageBox::Ok, QMessageBox::Ok);
    }

    QThreadPool::globalInstance()->setMaxThreadCount(10);//初始化线程池

    //qDebug() << query.record().value("Account");
    QShortcut * shortcut_btnCheck = new QShortcut(QKeySequence(Qt::Key_S + Qt::CTRL),this);//设置快捷键ctrl + s发送信息
    shortcut_btnCheck->setContext(Qt::WindowShortcut);
    connect(shortcut_btnCheck,&QShortcut::activated, ui->pushButton_Check, &QPushButton::click);//绑定快捷键与按钮

    ui->lineEdit_Port->setText("8899");
    this->setWindowTitle("服务器");
    ui->plainTextEdit_History->setReadOnly(true);
    ui->pushButton_Check->setEnabled(false);
    ui->plainTextEdit_History->setReadOnly(true);
    ui->pushButton_Check->setDefault(true);
    QLabel * label_ConnetionStatus = new QLabel(this);
    ui->statusbar->addWidget(label_ConnetionStatus);
    label_ConnetionStatus->setText("连接状态：");
    QLabel * label_IsConnected = new QLabel(this);//创建一个Label表示连接状态
    ui->statusbar->addWidget(label_IsConnected);
    QPixmap  pixmax_check = QPixmap(":/Icons/Cancel.png");
    pixmax_check = pixmax_check.scaled(QSize(30,30));
    label_IsConnected->setPixmap(pixmax_check);

    ptrServer = new QTcpServer(this);//初始化
    emit ui->pushButton_Start->click();
    //新连接请求时    Q1:未实现多线程   Q2:实现线程的退出
    connect(ptrServer, &QTcpServer::newConnection, this, [=](){

        QTcpSocket* tcpSocket = ptrServer->nextPendingConnection();
        TaskServer* taskServer = new TaskServer(ptrServer,tcpSocket);

        //----------------------数组是否必要？？----必要-----
        m_tcpSocketArray.push_back(tcpSocket);
        m_taskServerArray.push_back(taskServer);
        //----------------------------------------------------------
        //-----------信号连接区--------------
        connect(taskServer, &TaskServer::quitWorking, this, [=, &tcpSocket](){
            tcpSocket = nullptr;
            slot_erase();
            qDebug() << "erase";

        });
       // connect(taskServer, &TaskServer::quitWorking, this,  &MainWindow::slot_erase);
        connect(taskServer, &TaskServer::newMessage, this, [=](QByteArray placeNum, QByteArray userName,QByteArray message){

            QSqlQuery query;
            query.prepare("SELECT Name FROM place WHERE PlaceNum = :PlaceNum");
            int int_placeNum = placeNum.toInt();
            query.bindValue(":PlaceNum", int_placeNum);
            query.exec();
            query.next();
            QString placeName = query.record().value("Name").toString();
            qDebug() << "Namefound = " << placeName;
            int Year = QDateTime::currentDateTime().date().year();
            QString year = QString::number(Year);//年
            int Month = QDateTime::currentDateTime().date().month();
            QString month = QString::number(Month);//月
            int Day = QDateTime::currentDateTime().date().day();
            QString day = QString::number(Day);//日
            QTime Time = QDateTime::currentDateTime().time();
            QString time = Time.toString();//时间
            switch (int_placeNum)
            {
            case 0:
                qDebug() << "canteen";
                query.prepare("INSERT INTO message_canteen VALUES (:Time, :Name, :Message)");
                break;
            case 1:
                qDebug() << "dorm";
                query.prepare("INSERT INTO message_dorm VALUES (:Time, :Name, :Message)");
                break;
            case 2:
                qDebug() << "library";
                query.prepare("INSERT INTO message_library VALUES (:Time, :Name, :Message)");
                break;
            case 3:
                qDebug() << "basketball";
                query.prepare("INSERT INTO message_basketball VALUES (:Time, :Name, :Message)");
                break;
            default:
                qDebug() << "Error placeNum";
                break;
            }
            QString Name = userName;
            QString Message = message;
            QDateTime dateTime(QDate(Year,Month, Day), Time);
            query.bindValue(":Time",dateTime);
            query.bindValue(":Name", Name);
            query.bindValue(":Message", Message);//绑定对象
            qDebug() << query.exec();
            //将信息发送给客户端
            for(QTcpSocket* tcpSocket: m_tcpSocketArray)
            {
                if(tcpSocket->state() == QTcpSocket::UnconnectedState)
                {
                    tcpSocket = nullptr;
                }
                else
                {
                    //服务器发送信息格式 "1" + 一位地点标号 + 用户昵称 + ": " + 信息
                    tcpSocket->write("1" + placeNum + year.toUtf8() + "/" + month.toUtf8() + "/"+ day.toUtf8() + " " +  time.toUtf8() + " " + userName + ": " + message);//收发消息前缀为1
                }
            }
            ui->plainTextEdit_History->appendPlainText(year+ "/"+ month + "/"+ day + " " + time + " " + placeName + "->" + userName + ": " + message);
        });
        //---------------------------------
        label_IsConnected->setPixmap(QPixmap(":/Icons/Check.png").scaled(30,30));//重新设置状态
        qDebug() << "m_tcpSocketArraySize = " << m_tcpSocketArray.size() << "m_taskServerArraySize = " << m_taskServerArray.size();
        QThreadPool::globalInstance()->start(taskServer);
    });

}

MainWindow::~MainWindow()
{
    qDebug() << "destructor";
    ptrServer->close();
    ptrServer->deleteLater();
    delete ui;
    for (QTcpSocket*& tcpSocket: m_tcpSocketArray)
    {
        tcpSocket->close();
        if (tcpSocket != nullptr)
        {
            delete tcpSocket;
            tcpSocket = nullptr;
        }
    }
    //QApplication::exit();
    emit timeToExit();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "jiJIi";
    event->accept();
    qDebug() << "exit";
}


void MainWindow::on_pushButton_Start_clicked()
{
    unsigned short port = ui->lineEdit_Port->text().toUShort();//获取端口号
    ptrServer->listen(QHostAddress::Any,port);//设置监听
    ui->pushButton_Start->setEnabled(false);//设置连接按钮不可用
    ui->pushButton_Check->setEnabled(true);//设置发送信息按钮可用
}

void MainWindow::on_pushButton_Check_clicked()
{
    QString message = ui->plainTextEdit_Edit->toPlainText();
    if(message.isEmpty() == false)
    {
        QByteArray modifiedMessage = "2" + message.toUtf8();
        qDebug() << "modifiedMessage = " << modifiedMessage;
        for(QTcpSocket*& tcpSocket: m_tcpSocketArray)
        {
            if(tcpSocket->state() == QTcpSocket::UnconnectedState)
            {
                tcpSocket = nullptr;
            }
            else
            {
                tcpSocket->write(modifiedMessage);//官方服务器发消息前缀为2 , 打前缀a表示所有地点
            }
        }
        //ui->plainTextEdit_History->appendPlainText(year+ "/"+ month + "/"+ day + " " + time + " " + placeName + "->" + userName + ": " + message);
        QDateTime dateTime = QDateTime::currentDateTime();
        QString year = QString::number(dateTime.date().year());
        QString month = QString::number(dateTime.date().month());
        QString day = QString::number(dateTime.date().day());
        QString time = dateTime.time().toString();
        ui->plainTextEdit_History->appendPlainText(year+ "/"+ month + "/"+ day + " " + time + " " + "官方服务器: " + message);
        ui->plainTextEdit_Edit->clear();
    }
}

void MainWindow::slot_erase()
{
    qDebug() << "slot_erase";
    for (auto iter = m_tcpSocketArray.begin(); iter != m_tcpSocketArray.end(); )
    {
        qDebug() << *iter;
        if( *iter == nullptr)
            iter = m_tcpSocketArray.erase(iter);//当删除时erase函数自动指向下一个位置，就不需要进行++
        else
            iter++;    //当没有进行删除的时候，迭代器++
    }
}




