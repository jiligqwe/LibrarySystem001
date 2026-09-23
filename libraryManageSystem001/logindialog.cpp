#include "logindialog.h"
#include "ui_logindialog.h"
LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
}
LoginDialog::~LoginDialog()
{
    delete ui;
}
User LoginDialog::getLoggedUser()
{
    return loggedUser;
}
void LoginDialog::on_pushButton_login_clicked()
{
    QString username=ui->lineEdit_userName->text().trimmed();
    QString passport=ui->lineEdit_password->text().trimmed();
    User user=dbHelper::infoMatched(username,passport);
    bool ok=user.isValid;
    if (ok){
     loggedUser=user;
        qDebug()<<"user现在的id:"<<user.userid;
        QMessageBox::information(this,"成功","登录成功!");
        checkOverdueBooks(user.userid);
        this->accept();//关闭登录窗口，返回QDialog::Accepted
    }
    else {
        QMessageBox::warning(this,"错误","用户名或密码错误！");
    }
}
void LoginDialog::on_pushButton_register_clicked()
{
     RegisterDialog r;
     r.exec();
}
void LoginDialog::checkOverdueBooks(int userId)
{
    QString sql=R"(
            SELECT b.bookname,julianday('now')-julianday(bi.due_date) AS overdue_days
            FROM borrow_info bi
            LEFT JOIN books_info b ON bi.book_id =b.bookid
            WHERE bi.user_id = :userId
            AND bi.status = 0
            AND bi.due_date < date('now')
             )";
    QSqlQuery query;
    query.prepare(sql);
    query.bindValue(":userId",userId);
    if (!query.exec()){
            qDebug()<<"查找逾期图书失败"<<query.lastError().text();
}
    //把所有逾期书目拼起来
    QString tipText=" 你有以下图书已逾期，请尽快归还：\n\n";
    bool hasOverdue=false;
    while (query.next()){
        hasOverdue=true;
        QString bookname=query.value("bookname").toString();//取出书名
        int days=query.value("overdue_days").toInt();//逾期天数
        tipText +=QString("《%1》 逾期 %2 天\n").arg(bookname).arg(days);
    }
    if (hasOverdue){
        QMessageBox::warning(this,"借阅逾期提醒",tipText);
    }
}
