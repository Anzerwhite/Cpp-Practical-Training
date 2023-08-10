#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QToolButton>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);//文本在图标下方
    ui->toolBar->addSeparator();//添加分割线
    this->setWindowTitle("用户信息管理系统");
    ui->toolBar->addAction(ui->actionOpenDatabase);//设置打开数据库的按钮
    connect(ui->actionOpenDatabase, &QAction::triggered, this, &MainWindow::actionOpenDatabase_triggered);//连接OpenDatabase按钮和打开数据库操作
    //工具栏
    ui->toolBar->addAction(ui->actionToFirstRecord);
    ui->toolBar->addAction(ui->actionToLastRecord);
    ui->toolBar->addAction(ui->actionToNextRecord);
    ui->toolBar->addAction(ui->actionToPreviousRecord);//添加移动记录的操作
    ui->toolBar->addAction(ui->actionEditRecord);
    ui->toolBar->addAction(ui->actionInsertRecord);
    ui->toolBar->addAction(ui->actionDeleteRecord);
    ui->toolBar->addAction(ui->actionSortByAsc);
    ui->toolBar->addAction(ui->actionSortByDesc);
    ui->toolBar->addAction(ui->actionSelectAll);
    //初始化使能状态
    ui->actionEditRecord->setEnabled(false);
    ui->actionToFirstRecord->setEnabled(false);
    ui->actionToLastRecord->setEnabled(false);
    ui->actionToNextRecord->setEnabled(false);
    ui->actionToPreviousRecord->setEnabled(false);
    ui->actionInsertRecord->setEnabled(false);
    ui->actionDeleteRecord->setEnabled(false);
    ui->actionSortByAsc->setEnabled(false);
    ui->actionSortByDesc->setEnabled(false);
    ui->toolButton_SearchByAccount->setEnabled(false);
    ui->actionSelectAll->setEnabled(false);
    //信号连接
    connect(ui->actionToFirstRecord, &QAction::triggered, this, &MainWindow::actionToFirstRecord_triggered);
    connect(ui->actionToLastRecord, &QAction::triggered, this, &MainWindow::actionToLastRecord_triggered);
    connect(ui->actionToNextRecord, &QAction::triggered, this, &MainWindow::actionToNextRecord_triggered);
    connect(ui->actionToPreviousRecord, &QAction::triggered, this, &MainWindow::actionToPreviousRecord_triggered);
    connect(ui->actionEditRecord, &QAction::triggered, this, &MainWindow::actionEditRecord_triggered);
    connect(ui->tableView, &QTableView::doubleClicked, this, &MainWindow::tableView_doubleClicked);
    connect(ui->actionInsertRecord, &QAction::triggered, this, &MainWindow::actionInsertRecord_triggered);
    connect(ui->actionDeleteRecord, &QAction::triggered, this, &MainWindow::actionDeleteRecord_triggered);
    connect(ui->actionSortByAsc, &QAction::triggered, this, &MainWindow::actionSortByAsc_triggered);
    connect(ui->actionSortByDesc, &QAction::triggered, this, &MainWindow::actionSortByDesc_triggered);
    connect(ui->toolButton_SearchByAccount, &QToolButton::clicked, this, &MainWindow::btnSearchByAccount_triggered);
    connect(ui->actionSelectAll, &QAction::triggered, this, [=](){
        qryModel->setQuery("SELECT ID, Account, Password, Name, Gender FROM user ORDER BY ID ASC");
    });
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openTable()
{
    qryModel = new QSqlQueryModel(this);
    qryModel->setQuery("SELECT ID, Account, Password, Name, Gender FROM user ORDER BY ID ASC");//查询所有数据
    if(qryModel->lastError().isValid())//判断是否有错（语法错误等）
    {//弹出一个错误框，默认按键为OK
        QMessageBox::critical(this, "Error", "数据表查询错误，错误信息：\n" + qryModel->lastError().text(), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    else
    {//正常打开
        qryModel->setHeaderData(0,Qt::Horizontal, "ID");//设置表头,若不设置,则会默认以表名设置
        qryModel->setHeaderData(1,Qt::Horizontal, "账号");
        qryModel->setHeaderData(2,Qt::Horizontal, "密码");
        qryModel->setHeaderData(3,Qt::Horizontal, "昵称");
        qryModel->setHeaderData(4,Qt::Horizontal, "性别");

        theSelection = new QItemSelectionModel(qryModel);//设置选择模型(用来控制单选,多选等制约)

        connect(theSelection, &QItemSelectionModel::currentRowChanged, this, &MainWindow::currentRowChanged);//行变换则执行对应槽函数(头像信息的显示)
        ui->tableView->setModel(qryModel);
        ui->tableView->setSelectionModel(theSelection);//将tableView组件与数据模型和选择模型相关联
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);//设置为整行选择
        //ui->tableView->setSelectionBehavior(QAbstractItemView::SelectItems);//或设置为单格选择

        dataMapper = new QDataWidgetMapper();
        dataMapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);//设置自动提交(焦点改变自动更新)  ???ManualSubmit可能更好
        dataMapper->setModel(qryModel);//将DataMapper和数据模型连接起来,用于组件的映射
        //添加映射(以列为单位)
        dataMapper->addMapping(ui->spinBox_ID, 0);
        dataMapper->addMapping(ui->lineEdit_Account, 1);
        dataMapper->addMapping(ui->lineEdit_Password, 2);
        dataMapper->addMapping(ui->lineEdit_Name, 3);
        dataMapper->addMapping(ui->comboBox_Gender, 4);

        dataMapper->toFirst();//定位到第一条记录,即将索引置零
        //打开数据库后对应按键失效或生效
        ui->actionOpenDatabase->setEnabled(false);
        ui->actionEditRecord->setEnabled(true);
        ui->actionToFirstRecord->setEnabled(true);
        ui->actionToLastRecord->setEnabled(true);
        ui->actionToNextRecord->setEnabled(true);
        ui->actionToPreviousRecord->setEnabled(true);
        ui->actionInsertRecord->setEnabled(true);
        ui->actionDeleteRecord->setEnabled(true);
        ui->actionSortByAsc->setEnabled(true);
        ui->actionSortByDesc->setEnabled(true);
        ui->toolButton_SearchByAccount->setEnabled(true);
        ui->actionSelectAll->setEnabled(true);
    }
}



void MainWindow::actionOpenDatabase_triggered()
{
    //唤出一个文件框,将选择的文件路径返回
    aFile = QFileDialog::getOpenFileName(this, "选择数据库文件", "C:\\Users\\13680\\Desktop\\databases_sqlite", "SQLite数据库文件(*.db)");
    if(aFile.isEmpty() == true)//未选择文件
    {
        return;
    }
    DB = QSqlDatabase::addDatabase("QSQLITE");//添加SQLITE数据库驱动
    DB.setDatabaseName(aFile);//设置数据库名称(通过路径名设置数据库)
    if(DB.open() == false)//打开失败
    {//唤出一个提示框
        QMessageBox::warning(this, "Error", "打开数据库失败, 错误信息:\n" + DB.lastError().text(),QMessageBox::Ok, QMessageBox::Ok);
    }
    openTable();//打开数据表
}

void MainWindow::actionToFirstRecord_triggered()
{
    dataMapper->toFirst();
    refreshTableView();//更新视图
}

void MainWindow::actionToLastRecord_triggered()
{
    dataMapper->toLast();
    refreshTableView();
}

void MainWindow::actionToNextRecord_triggered()
{
    dataMapper->toNext();
    refreshTableView();
}

void MainWindow::actionToPreviousRecord_triggered()
{
    dataMapper->toPrevious();
    refreshTableView();
}

void MainWindow::actionEditRecord_triggered()
{
    int curRecordRow = theSelection->currentIndex().row();
    updateRecord(curRecordRow);//更新记录
}

void MainWindow::tableView_doubleClicked(const QModelIndex &index)
{
    int curRecordRow = index.row();
    updateRecord(curRecordRow);//更新记录
}

void MainWindow::actionInsertRecord_triggered()
{
    QSqlQuery query;
    query.exec("SELECT * FROM user WHERE ID = -1");//只查询字段信息
    QSqlRecord curRec = query.record();//记录查询的空记录
    curRec.setValue("ID",qryModel->rowCount());//
    Dialog_update * dialog_update = new Dialog_update(this);
    Qt::WindowFlags flags = dialog_update->windowFlags();
    dialog_update->setWindowFlags(flags | Qt::MSWindowsFixedSizeDialogHint);
    dialog_update->setInsertRecord(curRec);
    int ret = dialog_update->exec();//显示对话框
    if (ret == QDialog::Accepted)//OK键被按下
    {
        QSqlRecord recData = dialog_update->getRecordData();//获取编辑框内的数据
        query.prepare("INSERT INTO user (ID, Account, Password, Name, Gender, Signature, Photo) VALUES (:ID, :Account, :Password, :Name, :Gender, :Signature, :Photo)");
        query.bindValue(":ID", recData.value("ID"));
        query.bindValue(":Account", recData.value("Account"));
        query.bindValue(":Password", recData.value("Password"));
        query.bindValue(":Name", recData.value("Name"));
        query.bindValue(":Gender", recData.value("Gender"));
        query.bindValue(":Signature", recData.value("Signature"));
        query.bindValue(":Photo", recData.value("Photo"));
        bool isExecuted = query.exec();//执行INSERT语句

        if(isExecuted == false)
        {
            QMessageBox::critical(this, "Error", "插入记录错误：\n" + query.lastError().text(), QMessageBox::Ok, QMessageBox::Ok);
        }
        else
        {
           // qDebug() << "222";
            qryModel->query().exec();
        }
        qryModel->setQuery("SELECT ID, Account, Password, Name, Gender FROM user ORDER BY ID ASC");
    }
    query.finish();
    delete dialog_update;
}

void MainWindow::actionDeleteRecord_triggered()
{
    int curRecRow = theSelection->currentIndex().row();//获取当前索引
    QSqlRecord curRec = qryModel->record(curRecRow);//获取当前索引的记录
    if (curRec.isEmpty() == true)//空记录则返回
    {
        return;
    }

    int ID = curRec.value("ID").toInt();//获取ID
    QSqlQuery query;
    query.prepare("DELETE FROM user WHERE ID = :ID");
    query.bindValue(":ID", ID);
    if (query.exec() == false)//执行失败
    {
        QMessageBox::critical(this, "Error", "删除记录失败：\n" + query.lastError().text(), QMessageBox::Ok, QMessageBox::Ok);
    }
    else
    {
        qryModel->query().exec();
    }
    qryModel->setQuery("SELECT ID, Account, Password, Name, Gender FROM user ORDER BY ID ASC");
    //refresh();
}

void MainWindow::actionSortByAsc_triggered()
{
    qryModel->setQuery("SELECT ID, Account, Password, Name, Gender FROM user ORDER BY ID ASC");
}

void MainWindow::actionSortByDesc_triggered()
{
    qryModel->setQuery("SELECT ID, Account, Password, Name, Gender FROM user ORDER BY ID DESC");
}

void MainWindow::btnSearchByAccount_triggered()
{
      QSqlQuery query;
      query.prepare("SELECT ID, Account, Password, Name, Gender FROM user WHERE Account = :Account");
      query.bindValue(":Account",ui->lineEdit_SearchByAccount->text());
      query.exec();
      QSqlRecord currentRecord = query.record();
      if (currentRecord.isEmpty() == true)//记录为空
      {
          QMessageBox::information(this, "Information", "查无此账号!", QMessageBox::Ok,QMessageBox::Ok);
      }
      else
      {
          qryModel->setQuery(query);
         // qryModel->index(qryModel->rowCount()
          //ui->tableView->setCurrentIndex();
      }

}

void MainWindow::showMainWindow_login()
{
    this->show();
}

void MainWindow::currentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    if(current.isValid() == false)//当前索引为无效的
    {
        ui->label_Photo->clear();//清空图片
    }

    dataMapper->setCurrentModelIndex(current);//设置当前行号(从0开始)
    //实现移动记录的操作
    bool first = (current.row() == 0);//记录是否为首行
    bool last = (current.row() == qryModel->rowCount() - 1);//记录是否为末行
    //更新使能状态
    ui->actionToFirstRecord->setEnabled(!first);
    ui->actionToLastRecord->setEnabled(!last);
    ui->actionToNextRecord->setEnabled(!last);
    ui->actionToPreviousRecord->setEnabled(!first);

    int curRecRow = theSelection->currentIndex().row();//获取当前函数
    QSqlRecord currentRecord = qryModel->record(curRecRow);//获取当前行的记录信息
    int userID = currentRecord.value("ID").toInt();
    QSqlQuery query;
    query.prepare("SELECT ID, Signature, Photo FROM user WHERE ID = :ID");
    query.bindValue(":ID", userID);
    query.exec();//查找ID为userID的签名和照片
    query.first();//定位到第一条语句

    QVariant value_photo = query.value("Photo");//显示图片
    if(value_photo.isValid() == false)//无效数据(为空)
    {
        ui->label_Photo->clear();//清除照片
    }
    else
    {
        QByteArray data_photo = value_photo.toByteArray();// 转化为二进制的类型
        QPixmap picture;
        picture.loadFromData(data_photo);//载入图片
        ui->label_Photo->setPixmap(picture.scaled(ui->label_Photo->size().width(), ui->label_Photo->size().height(), Qt::IgnoreAspectRatio));//适应label的尺寸
    }
    QVariant value_signature = query.value("Signature");//显示签名
    ui->textEdit_Signature->setText(value_signature.toString());
    query.finish();
}
void MainWindow::refreshTableView()
{
    int index = dataMapper->currentIndex();//获取当前行索引
    QModelIndex curIndex = qryModel->index(index, 0);//返回当前行和列的索引
    theSelection->clearSelection();//清空选择项
    theSelection->setCurrentIndex(curIndex, QItemSelectionModel::Select);//设置当前索引,并选用精确选择模式
}

void MainWindow::updateRecord(int recNo)
{

    QSqlRecord curRec = qryModel->record(recNo);
    int ID = curRec.value("ID").toInt();
    QSqlQuery query;//查询对应ID的所有字段
    query.prepare("SELECT * FROM user WHERE ID = :ID");
    query.bindValue(":ID", ID);
    query.exec();//执行
    query.first();//定位到首记录
    if(query.isValid()== false)//若为空记录
    {
        return;
    }

    curRec = query.record();//返回当前查询的记录
    Dialog_update * dialog_update = new Dialog_update(this);
    Qt::WindowFlags flags = dialog_update->windowFlags();//返回一个Qt::Dialog
    dialog_update->setWindowFlags(flags | Qt::MSWindowsFixedSizeDialogHint);
    dialog_update->setUpdateRecord(curRec);//调用更新函数更新数据
    int ret = dialog_update->exec();//以模态方式显示对话框
    if (ret == QDialog::Accepted)//Ok键被按下
    {
        QSqlRecord recData = dialog_update->getRecordData();

        //查询ID对应记录
        query.prepare("UPDATE user SET Account=:Account, Password=:Password, Name=:Name, Gender=:Gender, Signature=:Signature, Photo=:Photo WHERE ID=:ID");
        query.bindValue(":Password", recData.value("Password"));
        query.bindValue(":Account", recData.value("Account"));

        query.bindValue(":Name", recData.value("Name"));
        query.bindValue(":Gender", recData.value("Gender"));
        query.bindValue(":Signature", recData.value("Signature"));
        query.bindValue(":Photo", recData.value("Photo"));
        query.bindValue(":ID", ID);

        if(query.exec() == false)//执行失败弹出错误窗口
        {
            QMessageBox::critical(this, "Error", "记录更新错误:\n" + query.lastError().text(), QMessageBox::Ok, QMessageBox::Ok);
        }
        else
        {
            //qDebug() << "111";
            qryModel->query().exec();//重新执行SQL语句查询
        }
        query.finish();
        //openTable();
        delete dialog_update;//释放内存
    }
}


