#include "addbookdialog.h"
#include "ui_addbookdialog.h"

addBookDialog::addBookDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addBookDialog)
{
    ui->setupUi(this);
    //加载书籍类型选择
    QMap<int,QString> categorymap=dbHelper::getALLCategory();
    ui->comboBox_category->clear();
    ui->comboBox_category->addItem("未分类",0);
    for (auto it =categorymap.begin();it!=categorymap.end();++it){
        ui->comboBox_category->addItem(it.value(),it.key());
    }
}
addBookDialog::~addBookDialog()
{
    delete ui;
}
void addBookDialog::on_pushButton_ok_clicked()
{Book b;
    b.IBSN=ui->lineEdit_IBSN->text().trimmed();
    b.bookname=ui->lineEdit_bookname->text().trimmed();
    b.author=ui->lineEdit_author->text().trimmed();
   b.publisher=ui->lineEdit_publisher->text().trimmed();
    b.stock=ui->lineEdit_stock->text().toInt();
    b.category_id=ui->comboBox_category->currentIndex();
    if (b.IBSN.isEmpty()){
        QMessageBox::warning(this,"提示","IBSN不能为空");
        return;
    }
    if (b.bookname.isEmpty()){
        QMessageBox::warning(this,"提示","书名不能为空");
        return;
    }
    if (b.author.isEmpty()){
        QMessageBox::warning(this,"提示","作者不能为空");
        return ;
    }
    if (b.stock==0){
        QMessageBox::warning(this,"提示","入库数量不能为0");
        return;
    }
if (b.category_id==0){
QMessageBox::warning(this,"提示","请选择图书类型");
return;
}
    if (!dbHelper::insertBookInfo(b)){
        QMessageBox::critical(this,"错误","入库失败");
    }
    else {
        QMessageBox::information(this,"入库成功","图书已添加");
    }
        accept();//关闭弹窗并成功返回
}
void addBookDialog::on_pushButton_cancel_clicked()
{
    reject();//关闭弹窗
}
