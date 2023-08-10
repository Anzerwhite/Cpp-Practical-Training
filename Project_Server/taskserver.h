#ifndef TASKSERVER_H
#define TASKSERVER_H

#include <QObject>
#include <QRunnable>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDateTime>
class TaskServer : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit TaskServer(QTcpServer*&  _tcpServer, QTcpSocket*& _tcpSocket,QObject *parent = nullptr);
    ~TaskServer();
    void run() override;
signals:
    void quitWorking();
    void newMessage(QByteArray placeNum, QByteArray userName, QByteArray data);
    void unconnect();
private:
    QTcpServer * m_tcpServer;//公用一个服务器
    QTcpSocket * m_tcpSocket;//每个线程都有一个套接字,用来与客户端连接

};

#endif // TASKSERVER_H
