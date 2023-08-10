#include "login_base.h"
#include "ui_login_base.h"

#include <QKeyEvent>

Login_base::Login_base(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login_base)
{
    ui->setupUi(this);
    this->setWindowTitle("登录");
    ui->lineEdit_Account->setFocus();
    connect(ui->btn_Login, &QPushButton::clicked, this, [=](){
        if (ui->lineEdit_Account->text() == "114514" && ui->lineEdit_Password->text() == "123456")
        {
            this->hide();
            emit showMainWindow();//发送信号
        }
        else
        {
            QMessageBox::information(this, "提示","账号不存在或密码错误！", QMessageBox::Ok,QMessageBox::Ok);
        }
    });
    connect(ui->btn_Exit, &QPushButton::clicked, this, &QDialog::close);

}

Login_base::~Login_base()
{
    delete ui;
}

void Login_base::keyPressEvent(QKeyEvent *event)
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


