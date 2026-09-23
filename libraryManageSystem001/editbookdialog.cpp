#include "editbookdialog.h"
#include "ui_editbookdialog.h"
editBookDialog::editBookDialog(int bookid,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::editBookDialog),
    id(bookid)
{
    ui->setupUi(this);
    QSqlQuery query;
    query.prepare("SELECT ISBN,bookname,author,publisher,stock FROM books_info WHERE bookid= :bid");
    query.bindValue(":bid",id);
    if (query.exec()&&query.next()){
        ui->lineEdit_IBSN->setText(query.value("ISBN").toString());
        ui->lineEdit_bookName->setText(query.value("bookname").toString());
        ui->lineEdit_author->setText(query.value("author").toString());
        ui->lineEdit_publisher->setText(query.value("publisher").toString());
        ui->lineEdit_stock->setText(QString::number(query.value("stock").toInt()));
    }
    else {QMessageBox::warning(this,"错误","未查询到该图书信息");
    qDebug()<<"未查询到书籍"<<query.lastError().text();
}
}
editBookDialog::~editBookDialog()
{
    delete ui;
}
void editBookDialog::on_pushButton_ok_clicked()
{
Book b;
b.IBSN=ui->lineEdit_IBSN->text().trimmed();
b.stock=ui->lineEdit_stock->text().toInt();
b.author=ui->lineEdit_author->text().trimmed();
b.bookname=ui->lineEdit_bookName->text().trimmed();
b.publisher=ui->lineEdit_publisher->text().trimmed();
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
if (!dbHelper::updateBookInfoById(b,id)){
    QMessageBox::critical(this,"错误","修改失败");
}
else { QMessageBox::information(this,"修改成功","图书已修改");
accept();//关闭弹窗并返回
}
}
void editBookDialog::on_pushButton_cancel_clicked()
{
    reject();
}
