#ifndef DIALOG_UPDATE_H
#define DIALOG_UPDATE_H

#include <QDialog>
#include<QtSql>
#include<QPushButton>
#include<QFileDialog>
#include<QDebug>
namespace Ui {
class Dialog_update;
}

class Dialog_update : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_update(QWidget *parent = nullptr);
    ~Dialog_update();
    void setUpdateRecord(QSqlRecord &recData);//更新记录
    void setInsertRecord(QSqlRecord &recData);//插入数据
    QSqlRecord getRecordData();//获取录入的数据

private slots:
    void btnClearPhoto_clicked();//清空照片
    void btnSetPhoto_clicked();//设置照片
private:
    Ui::Dialog_update *ui;
    QSqlRecord mRecord;//保存一条记录的数据
};

#endif // DIALOG_UPDATE_H
