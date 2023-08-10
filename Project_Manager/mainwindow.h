#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include"dialog_update.h"
#include <QMainWindow>
#include<QtSql>
#include<QSqlQuery>
#include<QSqlQueryModel>
#include<QSqlError>
#include<QDataWidgetMapper>
#include<QSqlDatabase>
#include<QMessageBox>
#include<QFileDialog>
#include<QDebug>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QSqlDatabase DB;//数据库
    QSqlQueryModel * qryModel;//数据模型
    QItemSelectionModel * theSelection;//选择模型
    QDataWidgetMapper * dataMapper;//数据界面的映射，连接数据与小组件，实现同步更新
    QString aFile;//打开的数据库路径

    void openTable();//打开数据表
    void refreshTableView();//移动记录时刷新tableView(界面组件)的当前行

    void updateRecord(int recNo);//更新记录
signals:

private slots:
    void currentRowChanged(const QModelIndex & current, const QModelIndex & previous);//当行变化时更新找照片等信息显示
    void actionOpenDatabase_triggered();//打开数据库按钮被触发
    void actionToFirstRecord_triggered();//到首行
    void actionToLastRecord_triggered();//到尾行
    void actionToNextRecord_triggered();//到下一行
    void actionToPreviousRecord_triggered();//到上一行
    void actionEditRecord_triggered();//编辑按钮被点击的槽函数
    void tableView_doubleClicked(const QModelIndex & index);
    void actionInsertRecord_triggered();//插入记录
    void actionDeleteRecord_triggered();//删除记录
    void actionSortByAsc_triggered();//升序
    void actionSortByDesc_triggered();//降序
    void btnSearchByAccount_triggered();//查找用户名

public slots:
    void showMainWindow_login();//登录显示界面
};
#endif // MAINWINDOW_H
