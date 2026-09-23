#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"dbhelper.h"
#include<QSqlTableModel>
#include<QSqlError>
#include"addbookdialog.h"
#include"editbookdialog.h"
#include"registerdialog.h"
#include<QStyledItemDelegate>
#include<QStyleOptionViewItem>
#include<QSqlQueryModel>
#include<QInputDialog>
#include<QDate>
#include<QTimer>
#include<QDateTime>
#include<QSqlRecord>
#include<QMap>
#include<QSqlRelationalTableModel>
#include<QSqlRelationalDelegate>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setCurrentUser(const User user);
private slots:
    //实现点击菜单跳转页面
    void on_action_bookManage_triggered();//0
     void on_action_rentManage_triggered();//1
      void on_action_userManage_triggered();//2
       void on_action_personalCenter_triggered();//3
        void on_action_zhuye_triggered();//4
        void updateSerialNumbers();
        void on_pushButton_search_clicked();
        void on_pushButton_bookAdd_clicked();
        void on_pushButton_bookChange_clicked();
        void on_pushButton_bookDelete_clicked();
        void on_pushButton_borrow_clicked();
        void on_pushButton_borrowrefresh_clicked();
        void on_pushButton_return_clicked();
        void on_comboBox_borrowStatus_currentIndexChanged();
        void on_pushButton_borrowsearch_clicked();
        void on_lineEdit_borrowSearch_returnPressed();
        //动态刷新主页时间
        void updateCurrentTime();
        void on_pushButton_boooks_manage_clicked();
        void on_pushButton_books_borrow_manage_clicked();
        void on_pushButton_users_manage_clicked();
        void on_pushButton_zhuye_to_personalcenter_clicked();
        void on_pushButton_usermanagetozhuye_clicked();
        void on_pushButton_personnal_center_to_zhuye_clicked();
        void on_pushButton_rentManageTozhuye_clicked();
        void on_pushButton_books_manage_to_zhuye_clicked();
        void on_pushButton_register_new_user_clicked();
        void on_pushButton_delete_user_clicked();
       void on_pushButton_search_user_clicked();
        void on_pushButton_ok_clicked();
        void on_pushButton_passwordChange_clicked();
        void on_pushButton_edit_clicked();
        void on_pushButton_exit_clicked();
        void on_lineEdit_search_returnPressed();
 void onCategoryFilterChanged();
private:
        User currentUser;
        QTimer *timer;
        //三个表的模型
        QSqlQueryModel *borrowmodel;
       QSqlRelationalTableModel *bookmodel;
       QSqlQueryModel *usermodel;
        void refreshHomeData();//更新主页数据
        void initBorrowTableModel();//加载借阅表
        void refreshBorrowTable();//更新借阅表
    Ui::MainWindow *ui;
 void initBookTableModel();//加载图书信息
 void initPermission();//初始化界面权限，普通用户限制权限
 void initUserTableModel();
};
#endif // MAINWINDOW_H
