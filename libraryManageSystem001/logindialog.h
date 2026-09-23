#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H
#include <QDialog>
#include<QDebug>
#include<QMessageBox>
#include<QSqlQuery>
#include"dbhelper.h"
#include"registerdialog.h"
#include"mainwindow.h"
namespace Ui {
class LoginDialog;
}
class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();
    User getLoggedUser();
private slots:
    void on_pushButton_login_clicked();
    void on_pushButton_register_clicked();
private:
    Ui::LoginDialog *ui;
 User loggedUser;
 void checkOverdueBooks(int userId);
};
#endif // LOGINDIALOG_H
