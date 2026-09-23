#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H
#include <QDialog>
#include<QSqlQuery>//执行sql语句，操作数据库
#include<QMessageBox>//实现弹窗提示
#include"dbhelper.h"
#include<QDebug>
#include<QSqlError>
namespace Ui {
class RegisterDialog;
}
class RegisterDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();
private slots:
    void on_pushButton_reg_clicked();
    void on_pushButton_cancel_clicked();
    void on_comboBox_role_currentIndexChanged(int index);
private:
    Ui::RegisterDialog *ui;
    QSqlQuery query;
    QSqlDatabase db;
};
#endif // REGISTERDIALOG_H
