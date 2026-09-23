#include "registerdialog.h"
#include "ui_registerdialog.h"
RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
ui->label_key->setVisible(false);
ui->lineEdit_keynumber->setVisible(false);
    setWindowTitle("用户注册");
    //创建表
    dbHelper::createTable();
}
RegisterDialog::~RegisterDialog()
{
    delete ui;
}
void RegisterDialog::on_pushButton_reg_clicked()
{
    //获取用户名，密码，确认密码
QString username=ui->lineEdit_yonghuming->text().trimmed();
QString password=ui->lineEdit_mima->text().trimmed();
QString repassword=ui->lineEdit_mimaqueren->text().trimmed();
int role=ui->comboBox_role->currentIndex();
//非空验证
if (username.isEmpty()||password.isEmpty()||repassword.isEmpty()){
    QMessageBox::warning(this,"提示","用户名，密码不能为空!");
    return;
}
//两次密码输入一致验证
if (password!=repassword){
    QMessageBox::warning(this,"提示","两次密码不一致!");
return;
}
if (dbHelper::isUserNameExisted(username)){
QMessageBox::warning(this,"提示","该用户名已经被注册!");
    return;
}
//限定密码长度
if (!(password.length()>5&&password.length()<13)){
    QMessageBox::warning(this,"提示","请输入6-12位的密码");
return;
}
if (role==1){
    QString keynumber=ui->lineEdit_keynumber->text().trimmed();
    if (keynumber.isEmpty()){
        QMessageBox::warning(this,"提示","注册管理员请输入管理员密钥");
        return;
    }
  //设置管理员密钥为654321
    if (keynumber!="654321"){
        QMessageBox::warning(this,"提示","管理员密钥错误！");
        return;
    }
}
User user;
user.role=role;
user.password=password;
user.username=username;
//注册，信息进入数据库
if (dbHelper::insertInfo(user)){
    QMessageBox::information(this,"成功","新用户注册成功");

       this->close();
}
else{
    QMessageBox::critical(this,"错误",QString("注册失败：%1").arg(query.lastError().text()));
}
}
void RegisterDialog::on_pushButton_cancel_clicked()
{
    this->close();
}
void RegisterDialog::on_comboBox_role_currentIndexChanged(int index)
{
    bool isAdmin=(index==1);
    ui->label_key->setVisible(isAdmin);
    ui->lineEdit_keynumber->setVisible(isAdmin);
}
