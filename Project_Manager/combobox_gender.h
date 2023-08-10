#ifndef COMBOBOX_GENDER_H
#define COMBOBOX_GENDER_H

#include <QWidget>

namespace Ui {
class ComboBox_Gender;
}

class ComboBox_Gender : public QWidget
{
    Q_OBJECT

public:
    explicit ComboBox_Gender(QWidget *parent = nullptr);
    ~ComboBox_Gender();

private:
    Ui::ComboBox_Gender *ui;
};

#endif // COMBOBOX_GENDER_H
