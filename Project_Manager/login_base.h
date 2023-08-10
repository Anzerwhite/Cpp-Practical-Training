#ifndef LOGIN_BASE_H
#define LOGIN_BASE_H

#include <QDialog>
#include"mainwindow.h"
#include<QMessageBox>
namespace Ui {
class Login_base;
}

class Login_base : public QDialog
{
    Q_OBJECT

public:
    explicit Login_base(QWidget *parent = nullptr);
    ~Login_base();
    void keyPressEvent(QKeyEvent *event) override;
signals:
    void showMainWindow();//显示主界面
private:
    Ui::Login_base *ui;
    //MainWindow *mainWindow;//维护主界面的指针
};

#endif // LOGIN_BASE_H
