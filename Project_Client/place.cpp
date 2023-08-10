#include "place.h"
#include "ui_place.h"

#include <QShortcut>
Place::Place(int _placeNum, QString _Name, QTcpSocket*& _tcpSocket, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Place)
{
    ui->setupUi(this);
    m_placeNum = _placeNum;//获取地点标号
    m_tcpSocket = _tcpSocket;//获取套接字
    isFirst = true;//是否首次点击
    this->setWindowTitle(_Name);
    ui->plainTextEdit_History->setReadOnly(true);//消息记录只读不写
    ui->plainTextEdit_Edit->setFocus();//设置焦点
    ui->pushButton_Send->setDefault(true);//设置默认按钮
    ui->toolBar->addAction(ui->actionBack);//设置返回图标
    QShortcut * shortcut_btnCheck = new QShortcut(QKeySequence(Qt::Key_S + Qt::CTRL),this);//设置快捷键ctrl + s发送信息
    shortcut_btnCheck->setContext(Qt::WindowShortcut);

    connect(ui->pushButton_Search_All, &QPushButton::clicked, this, [=](){
        ui->plainTextEdit_OldRecord->clear();//清空
        //查找所有信息请求："4" + 一位地点标号
        m_tcpSocket->write("4" + QString::number(m_placeNum).toUtf8());
    });
    connect(ui->pushButton_Search,&QPushButton::clicked, this, [=](){
        ui->plainTextEdit_OldRecord->clear();//清空
        //查找具体信息请求："5" + 一位地点标号 + 具体信息
        QString message = ui->plainTextEdit_Search->toPlainText();
        m_tcpSocket->write("5" + QString::number(m_placeNum).toUtf8() + message.toUtf8());
    });
    connect(shortcut_btnCheck,&QShortcut::activated, ui->pushButton_Send, &QPushButton::click);//绑定快捷键与按钮
    connect(ui->actionBack, &QAction::triggered, this, &QWidget::close);
    connect(ui->tabWidget, &QTabWidget::tabBarClicked, this, [=](int index){
        if(index == 1)//地点信息界面被点击
        {
            if(isFirst == true)//首次点击
            {
                isFirst = false;
                //初始化请求： "3" + 地点标号
                QString string_placeNum = QString::number(m_placeNum);//地点标号
                m_tcpSocket->write("3" + string_placeNum.toUtf8());
            }
        }
    });
}

Place::~Place()
{
    delete ui;
}

//发送信息 形式： 一位前缀（0 表示登录 1 表示消息） + 六位账号 + 一位地点标号 + 昵称长度 + 昵称 + 数据
void Place::on_pushButton_Send_clicked()
{
    //——————————————————发送信息给服务器————————————————
    QString message = ui->plainTextEdit_Edit->toPlainText();
    QByteArray byteArray_userName = m_userName.toUtf8();
    if (message.isEmpty() == false)
    {
        QString sizeName = QString::number(byteArray_userName.size());//昵称的长度,限定在9个字符内

        if(sizeName.size() < 2)
        {
            sizeName = "0" + sizeName;
        }
        //潜在问题：昵称太大
        QString string_placeNum = QString::number(m_placeNum);//地点标号(目前从 0 到 9)
        qDebug() << "string_placeNum.toUtf8() = " << string_placeNum.toUtf8();
        m_tcpSocket->write("1" + m_userAccount.toUtf8() + string_placeNum.toUtf8() + sizeName.toUtf8() + m_userName.toUtf8() + message.toUtf8());
        //发送信息 形式： 一位前缀（0 表示登录 1 表示消息） + 六位账号 + 一位地点标号 + 昵称长度 + 昵称 + 数据
        ui->plainTextEdit_Edit->clear();
    }
}
