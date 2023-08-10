#include "dialog_update.h"
#include "ui_dialog_update.h"

Dialog_update::Dialog_update(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_update)
{
    //mRecord = QSqlRecord();
    ui->setupUi(this);
    connect(ui->btn_SetPhoto, &QPushButton::clicked, this, &Dialog_update::btnSetPhoto_clicked);
    connect(ui->btn_ClearPhoto, &QPushButton::clicked, this, &Dialog_update::btnClearPhoto_clicked);
    connect(ui->btn_Check, &QPushButton::clicked, this, &QDialog::accept);
    connect(ui->btn_Cancel, &QPushButton::clicked, this, &QDialog::reject);

    ui->btn_Check->setDefault(true);
}

Dialog_update::~Dialog_update()
{
    delete ui;
}

void Dialog_update::setUpdateRecord(QSqlRecord &recData)
{
    mRecord = recData;//表面上是要具体的记录，实际上要存储的记录的表格式
    //更新组件显示值为记录值
    ui->spinBox_ID->setValue(recData.value("ID").toInt());
    ui->lineEdit_Account->setText(recData.value("Account").toString());
    ui->lineEdit_Password->setText(recData.value("Password").toString());
    ui->lineEdit_Name->setText(recData.value("Name").toString());
    ui->comboBox_Gender->setCurrentText(recData.value("Gender").toString());
    ui->textEdit_Signature->setText(recData.value("Signature").toString());
    QVariant value_Photo = recData.value("Photo");
    if(value_Photo.isValid() == false)
    {
        ui->label_Photo->clear();
    }
    else
    {
        //设置图片
        QByteArray data = value_Photo.toByteArray();
        QPixmap picture;
        picture.loadFromData(data);
        //适应大小
        ui->label_Photo->setPixmap(picture.scaled(ui->label_Photo->size().width(),ui->label_Photo->size().height(), Qt::IgnoreAspectRatio));

    }

}

void Dialog_update::setInsertRecord(QSqlRecord &recData)
{
    mRecord = recData;//保存recData到内部变量
    ui->spinBox_ID->setEnabled(true);//设置可编辑
    setWindowTitle("插入新记录");//设置窗口名称
    ui->spinBox_ID->setValue(recData.value("ID").toInt());

}

QSqlRecord Dialog_update::getRecordData()
{
    mRecord.setValue("ID", ui->spinBox_ID->value());
    mRecord.setValue("Account", ui->lineEdit_Account->text());
    mRecord.setValue("Password", ui->lineEdit_Password->text());
    mRecord.setValue("Name", ui->lineEdit_Name->text());
    mRecord.setValue("Gender", ui->comboBox_Gender->currentText());
    mRecord.setValue("Signature", ui->textEdit_Signature->toPlainText());
    return mRecord;
}

void Dialog_update::btnClearPhoto_clicked()
{
    ui->label_Photo->clear();
    mRecord.setNull("Photo");
}

void Dialog_update::btnSetPhoto_clicked()
{
    QString aFile = QFileDialog::getOpenFileName(this, "选择图片","", "图片(*.jpg *.png)");
    if (aFile.isEmpty())//未选择图片
    {
        return;
    }
    QByteArray data;
    QFile * file = new QFile(aFile);
    file->open(QIODevice::ReadOnly);
    data = file->readAll();
    file->close();
    mRecord.setValue("Photo", data);
    QPixmap picture;
    picture.loadFromData(data);
    ui->label_Photo->setPixmap(picture.scaled(ui->label_Photo->size().width(),ui->label_Photo->size().height(), Qt::IgnoreAspectRatio));
}
