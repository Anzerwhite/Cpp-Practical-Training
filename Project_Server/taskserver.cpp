#include "taskserver.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QtSql>
#include <QDebug>
#include <QObject>
#include <QApplication>
#include <QEventLoop>
#include <QSqlRecord>

TaskServer::TaskServer(QTcpServer*&  _tcpServer,QTcpSocket*& _tcpSocket, QObject *parent)
    : QObject(parent), QRunnable(),m_tcpServer(_tcpServer),m_tcpSocket(_tcpSocket)
{
//    m_tcpServer = _tcpServer;//传递服务器
//    m_tcpSocket = _tcpSocket;//传递套接字
    //----------------保持疑问------------
    //    _tcpServer->deleteLater();
    //    _tcpSocket->deleteLater();//释放内存
    //----------------------------------
    setAutoDelete(true);//自动析构
}

TaskServer::~TaskServer()
{

}

void TaskServer::run()//工作函数
{
    QEventLoop loop;
    //———————————————————接受并解析来自客户端的信息——————————————————
    //形式： 一位前缀（0 表示登录 1 表示消息 3 表示初始化请求 4 表示查找请求） + 六位账号 + 一位地点标号 (+ 昵称长度 + 昵称 + 数据)
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, [=](){
        QByteArray prefix = m_tcpSocket->read(1);
        int num_prefix = prefix.toInt();

        switch (num_prefix)
        {
        //前缀为0则为登录请求   "0" + 六位账号 + 任意位密码
        case 0:
        {
            QByteArray account = m_tcpSocket->read(6);
            QByteArray password = m_tcpSocket->readAll();//读取账号密码
            QSqlQuery query;
            query.prepare("SELECT Account,Password FROM user WHERE Account = :Account AND Password = :Password");
            query.bindValue(":Account",QString(account));
            query.bindValue(":Password", QString(password));
            query.exec();//查询记录

            query.next();//移动索引

            QSqlRecord record =query.record();
            QString _account = record.value("Account").toString();
            QString _password = record.value("Password").toString();
            qDebug() << account << _account;
            qDebug() << password << _password;
            if (_account == account && _password == password && account.isEmpty() == false && password.isEmpty() == false)
            {
                query.prepare("SELECT Name FROM user WHERE Account = :Account");
                query.bindValue(":Account", QString(account));
                query.exec();
                query.next();
                QString Name = query.record().value("Name").toString();
                qDebug() << "query name = " << Name;
                m_tcpSocket->write("0TRUTH" + Name.toUtf8());//返回TRUE 和名称
            }
            else
            {
                m_tcpSocket->write("0FALSE");//返回FALSE
            }
            break;
        }
            //前缀为1则为信息  "1" + 六位账号 + 一位地点标号 + 昵称长度 + 昵称 + 数据
        case 1:
        {
            QByteArray userAccount = m_tcpSocket->read(6);//读取账号
            QByteArray placeNum = m_tcpSocket->read(1);//读取地点标号

            QByteArray size_userName = m_tcpSocket->read(2);//读取昵称长度
            int sizeNum_userName = size_userName.toInt();
            //测试
            qDebug() << "QByteArray size_userName = " << size_userName;
            qDebug() << "size of Name = " << sizeNum_userName;
            QByteArray userName = m_tcpSocket->read(sizeNum_userName);//读取昵称
            qDebug() << "userName = " << userName;
            QByteArray message = m_tcpSocket->readAll();//读取信息
            //----------------------
            qDebug() << QThread::currentThread() << message;//测试  结果：同个线程？
            //----------------------

            emit newMessage(placeNum, userName, message);
            break;
        }
            //前缀为3则为初始化请求   "3" + 地点标号
        case 3:
        {
            QByteArray byteArray_placeNum = m_tcpSocket->read(1);//接收地点标号
            int placeNum = byteArray_placeNum.toInt();
            QSqlQuery query;

            query.prepare("SELECT * FROM place WHERE placeNum = :placeNum");
            query.bindValue(":placeNum", placeNum);
            query.exec();//查询地点信息
            query.next();
            QSqlRecord record = query.record();//获取记录

            QByteArray Name = record.value("Name").toByteArray();
            QString sizeName = QString::number(Name.size());//昵称的长度,两位数
            if(sizeName.size() < 2)
            {
                sizeName = "0" + sizeName;
            }

            QByteArray Address = record.value("Address").toByteArray();
            QString sizeAddress = QString::number(Address.size());//地址的长度,三位数
            if(sizeAddress.size() <= 2)
            {
                sizeAddress = "0" + sizeAddress;
            }

            QByteArray Introduction = record.value("Introduction").toByteArray();
            QString sizeIntroduction = QString::number(Introduction.size());//简介的长度,三位数
            if(sizeIntroduction.size() <= 2)
            {
                sizeIntroduction = "0" + sizeIntroduction;
            }
            QByteArray Photo = record.value("Photo").toByteArray();

            //发送信息格式： "3" + 一位地点标号 + 2位昵称长度 + 昵称 + 三位地址长度 + 地址 + 三位简介长度 + 简介 + 照片
            m_tcpSocket->write("3" + byteArray_placeNum + sizeName.toUtf8() +  Name + sizeAddress.toUtf8() + Address + sizeIntroduction.toUtf8() + Introduction + Photo);

            break;
        }
            //前缀为4 查找信息 "4" + 地点标号
        case 4:
        {
            int placeNum = m_tcpSocket->read(1).toInt();//读取地点标号
            QString str_placeNum = QString::number(placeNum);
            QSqlQuery query;
            switch (placeNum)
            {
            case 0://canteen
                query.exec("SELECT * FROM message_canteen");
                break;
            case 1://dorm
                query.exec("SELECT * FROM message_dorm");
                break;
            case 2://library
                query.exec("SELECT * FROM message_library");
                break;
            case 3://basketball
                query.exec("SELECT * FROM message_basketball");
                break;
            default:
                qDebug() << "Wrong placeNum in prefix 4";
                break;
            }
            QSqlRecord record;
            m_tcpSocket->write("4" + str_placeNum.toUtf8());
            while (query.next() == true)
            {qDebug() << "slot_quitWorking";
                record = query.record();//Time Name Message
                QDateTime time = record.value("Time").toDateTime();
                QString Year = QString::number(time.date().year());
                QString Month = QString::number(time.date().month());
                QString Day = QString::number(time.date().day());
                QString Time = time.time().toString();
                QString Name = record.value("Name").toString();
                QString Message = record.value("Message").toString();
                m_tcpSocket->write(Year.toUtf8() + "/" + Month.toUtf8() + "/" + Day.toUtf8() + " " + Time.toUtf8() + " " + Name.toUtf8() + ": " + Message.toUtf8() + "\n");
            }
            break;
        }
        case 5:
        {
            int placeNum = m_tcpSocket->read(1).toInt();//读取地点标号
            QByteArray message = m_tcpSocket->readAll();//读取信息
            QString str_message = QString(message);
            QString str_placeNum = QString::number(placeNum);
            QSqlQuery query;
            switch (placeNum)
            {
            case 0://canteen
                query.prepare("SELECT * FROM message_canteen WHERE Message = :Message");
                break;
            case 1://dorm
                query.prepare("SELECT * FROM message_dorm WHERE Message = :Message");
                break;
            case 2://library
                query.prepare("SELECT * FROM message_library WHERE Message = :Message");
                break;
            case 3://basketball
                query.prepare("SELECT * FROM message_basketball WHERE Message = :Message");
                break;
            default:
                qDebug() << "Wrong placeNum in prefix 4";
                break;
            }
            query.bindValue(":Message", str_message);
            query.exec();
            QSqlRecord record;
            m_tcpSocket->write("4" + str_placeNum.toUtf8());
            while (query.next() == true)
            {
                record = query.record();//Time Name Message
                QDateTime time = record.value("Time").toDateTime();
                QString Year = QString::number(time.date().year());
                QString Month = QString::number(time.date().month());
                QString Day = QString::number(time.date().day());
                QString Time = time.time().toString();
                QString Name = record.value("Name").toString();
                QString Message = record.value("Message").toString();
                m_tcpSocket->write(Year.toUtf8() + "/" + Month.toUtf8() + "/" + Day.toUtf8() + " " + Time.toUtf8() + " " + Name.toUtf8() + ": " + Message.toUtf8() + "\n");
            }
        }
            break;
        case 6:
        {
            QString Account = QString(m_tcpSocket->readAll());
            qDebug() << "Account in Server :" << Account;
            QSqlQuery query;
            query.prepare("SELECT * FROM user WHERE Account = :Account");
            query.bindValue(":Account", Account);
            query.exec();
            query.next();
            QSqlRecord record = query.record();
            //昵称和昵称大小
            QString Name = record.value("Name").toString();
            qDebug() << "Name = " << Name;
            QString sizeName = QString::number(Name.toUtf8().size());
            while (sizeName.size() < 2)
            {
                sizeName = "0" + sizeName;
            }
             qDebug() << "sizeName = " << sizeName;
            //性别和性别大小
            QString Gender = record.value("Gender").toString();
            QString sizeGender = QString::number(Gender.toUtf8().size());
             qDebug() << "Gender = " << Gender;
            //签名和签名大小
            QString Signature = record.value("Signature").toString();
            QString sizeSignature = QString::number(Signature.toUtf8().size());;
            while (sizeSignature.size() < 3)
            {
                sizeSignature = "0" + sizeSignature;
            }
            qDebug() << "Signature = " << Signature;
            QByteArray Photo = record.value("Photo").toByteArray();
            //格式： "6" + 两位昵称大小 + 昵称 + 一位性别大小 + 性别 + 3位签名大小 + 签名 + 照片
            m_tcpSocket->write("6" + sizeName.toUtf8() + Name.toUtf8() + sizeGender.toUtf8() + Gender.toUtf8() + sizeSignature.toUtf8() + Signature.toUtf8() + Photo);
        }
            break;
        default:
            QByteArray bin = m_tcpSocket->readAll();//垃圾桶
            qDebug() << "Error Prefix From Client";
            break;
        }
    });//connect 里放connect   ->    使调用正确的实例化对象

    connect(m_tcpSocket, &QTcpSocket::disconnected, this, [=](){
        m_tcpSocket->close();//关闭套接字
        m_tcpSocket = nullptr;
        qDebug() << "disconnected: " << m_tcpSocket;
        //m_tcpSocket->deleteLater();//释放内存
        emit quitWorking();//发送信号
    });
    connect(this, &TaskServer::quitWorking, &loop, &QEventLoop::quit);
    loop.exec();

}

