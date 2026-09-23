#ifndef DBHELPER_H
#define DBHELPER_H
#include<QSqlDatabase>//数据库类
#include<QSqlQuery>//执行数据库语句的
#include<QDebug>//调试输出工具
#include<QMessageBox>
#include<QSqlError>
#include<QSqlQueryModel>
#include<QMap>
#include<QCoreApplication>
#include<QDir>
struct User{
    int userid;
    QString username;
    QString password;
    int role;
bool isValid=false;
QString realname;
QString phone;
};
struct Book{
    int bookid;
    QString IBSN;
    QString bookname;
    QString author;
    QString publisher;
    int stock;
    int category_id;
};
class dbHelper
{
public:
    dbHelper();
   static void createTable();//创建用户表
   //更改用户基本信息
   static bool updateUserInfo(int userId,const QString &username,const QString &realname,const QString phone);
   //更改用户密码
   static bool updateUserPassword(int userid,const QString password);
   //验证密码是否正确
   static bool isOldPassword(int userid,const QString &oldPassword );
   static void createBooksTable();//创建图书表
   //获取全部分类列表
   static QMap<int,QString> getALLCategory();
   static bool insertInfo(User &p);//插入用户信息
   static bool insertBookInfo(Book &b);//插入书籍
   static bool updateBookInfoById(Book &b,int bookid);
   static int deleteBooksByIds(const QList<int>& boookIds);//删除多本
   static bool deleteBookById(int bookid );//删除单本
    void selectInfo();//查询信息
    static bool initDatabase();
    static User infoMatched( QString userName, QString password);//信息匹配
    static bool isUserNameExisted(QString &username);//判断用户名是否已经存在
    static void createBorrowTable();//创建借阅表
    static bool addBorrow(int userid,int bookid,int BorrowDays=30);//借书
    static bool returnBook(int borrowId);//还书
    static int getBookIDByName(QString& bookname);
static void queryUserList(QSqlQueryModel* model,QString& searchKey);//查询用户
static QSqlQueryModel* getBorrowList(QString keyword,int statusFilter,int userId);
static bool returnBook(int borrowId,int bookId);
private:
   static QSqlDatabase db;
};
#endif // DBHELPER_H
