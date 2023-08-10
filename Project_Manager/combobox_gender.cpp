#include "combobox_gender.h"
#include "ui_combobox_gender.h"

ComboBox_Gender::ComboBox_Gender(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ComboBox_Gender)
{
    ui->setupUi(this);
//    ui->comboBox->addItem("男");
}

ComboBox_Gender::~ComboBox_Gender()
{
    delete ui;
}
