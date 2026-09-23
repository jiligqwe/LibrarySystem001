#include "dbhelper.h"
QSqlDatabase dbHelper::db;//静态成员必须在类外实现初始化
dbHelper::dbHelper()
{
}
//加载并打开数据库
bool dbHelper::initDatabase()
{
    // 你当前用的exe同级路径写法
    QString dbPath = QCoreApplication::applicationDirPath() + "/library.db";
    qDebug() << "程序预期读取的数据库路径：" << dbPath;
    //加载数据库驱动，连接数据库
    db=QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
    qDebug() << "数据库路径：" << dbPath;
   // 打开数据库
   if (!db.open()){
       qDebug()<<"数据库打开失败";
       return false;
   }
   else {
       qDebug()<<"数据库打开成功";
   }
       return true;
}
//1.用户
//创建用户及管理员信息表
void dbHelper::createTable()
{
    QString sql="create table IF NOT EXISTS user_info("
            "userid INTEGER PRIMARY KEY AUTOINCREMENT,"
                 "username text,"

                  "password text not null,"
            "role Integer not null default 0"
            ");";
        QSqlQuery query;
        bool ok=query.exec(sql);
    bool ok1=    query.exec("ALTER TABLE user_info ADD COLUMN realname TEXT");
     bool ok2=     query.exec("ALTER TABLE user_info ADD COLUMN phone TEXT");
     if (!ok1||!ok2){
         qDebug()<<"新增失败";
     }
       if (ok){
            qDebug()<<"创建表成功";
        }
        else {
           qDebug()<<"创建表失败";
}
}
bool dbHelper::updateUserInfo(int userId, const QString &username, const QString &realname, const QString phone)
{
    QSqlQuery query;
    query.prepare("UPDATE user_info SET username = ?,realname = ?,phone = ? WHERE userid = ?");
    query.addBindValue(username);
    query.addBindValue(realname);
    query.addBindValue(phone);
    query.addBindValue(userId);
    return query.exec();
}
bool dbHelper::updateUserPassword(int userid, const QString password)
{
    QSqlQuery query;
     query.prepare("UPDATE user_info SET password = ? WHERE userid = ?");
     query.addBindValue(password);
     query.addBindValue(userid);
     return query.exec();
}
bool dbHelper::isOldPassword(int userid, const QString &oldPassword)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM user_info WHERE userid= ? AND password = ?");
    query.addBindValue(userid);
    query.addBindValue(oldPassword);
    if(query.exec()&&query.next())
{
        return query.value(0).toInt()==1;
    }
    return false;
}
//向数据库中插入账号信息
bool dbHelper::insertInfo(User &p)
{ if (!QSqlDatabase::database().isOpen()) {
        qDebug() << "数据库未打开";
        return false;
    }
    QString sql="INSERT INTO user_info(username,password,role) VALUES(:username,:password,:role)";
    QSqlQuery query;
query.prepare(sql);//准备不完整sql语句
//替换占位符
query.bindValue(":username",p.username);
query.bindValue(":password",p.password);
query.bindValue(":role",p.role);
bool ok=query.exec();
if (ok){
  qDebug()<<"插入成功";
  return true;
}
else {
    qDebug()<<"插入失败";
}
    return false;
}
//将用户输入的账号密码与数据库中数据的进行匹配，并返回用户账号
User dbHelper::infoMatched(QString username, QString password){
QSqlQuery query;
User user;
    QString sql="SELECT userid,password,role,realname,phone from user_info WHERE username=:username";
    if (!query.prepare(sql)){
        qDebug()<<"SQl prepqre失败"<<query.lastError().text();
    }
    query.bindValue(":username",username);
    bool ok=query.exec();
    int ZERO=0;
    int ONE=1;
    int TWO=2;
    if (ok){//运行成功
        if (query.next()){//用户名存在
            qDebug() << "第0列：" << query.value(ZERO).toInt();
                  qDebug() << "第1列：" << query.value(ONE).toString();
                  qDebug() << "第2列：" << query.value(TWO).toString();
        QString realPassword=query.value("password").toString();
        int role=query.value("role").toInt();
        QString realname=query.value("realname").toString();
        QString phone=query.value("phone").toString();
        if (realPassword==password){//密码正确
           user.userid=query.value("userid").toInt();
           user.username=username;
           user.password=password;
           user.role=role;
           user.realname=realname;
           user.phone=phone;
           qDebug()<<"第0列：" << query.value(ZERO).toInt();
           user.isValid=true;
           qDebug()<<"成功";
           return user;
        }
        }
        else {
            qDebug()<<"用户名不存在";
        }
            return user;
    }
    qDebug()<<"登录查询失败"<<query.lastError().text();
    return user;
}
//判断用户名在数据库中是否已经存在
bool dbHelper::isUserNameExisted(QString &username)
{
    QSqlQuery query;
    QString sql="SELECT 1 FROM user_info WHERE username= :username";
    query.prepare(sql);//准备不完整sql语句
    query.bindValue(":username",username);//替换占位符
    if (!query.exec()){
    qDebug()<<"查询用户名失败:"<<query.lastError().text();
    return false;
    }
    else {
        return query.next();
}
}
//2.图书
//创建图书信息表
void dbHelper::createBooksTable()
{
    QString sql="create table if not exists books_info("
            "bookid integer primary key autoincrement,"
            "ISBN text not null unique,"
            "bookname text not null,"
            "author text not null,"
            "publisher text,"
            "stock integer not null default 0"
            ");";
 QSqlQuery query;
 if (!query.exec(sql)){
    qDebug()<<"创建图书表失败:"<<query.lastError().text();
 }
 else {
 query.exec("ALTER TABLE books_info ADD COLUMN category_id INTEGER DEFAULT 0");//添加分类标准
     qDebug()<<"创建图书表成功";
 }
 //添加图书分类表
 QString categorySql="create table if not exists category("
         "category_id integer primary key autoincrement,"
         "category_name text not null unique"
         ");";
if (!query.exec(categorySql)){
    qDebug()<<"创建图书分类表失败";
}
else {
   bool ok1= query.exec("INSERT OR IGNORE INTO category (category_name) VALUES ('文学小说')");
   bool ok2=  query.exec("INSERT OR IGNORE INTO category (category_name) VALUES ('计算机技术')");
   bool ok3=  query.exec("INSERT OR IGNORE INTO category (category_name) VALUES ('历史人文')");
   bool ok4= query.exec("INSERT OR IGNORE INTO category (category_name) VALUES ('教育学习')");
    bool ok5= query.exec("INSERT OR IGNORE INTO category (category_name) VALUES ('科学技术')");
            if(ok1&&ok2&&ok3&&ok4&&ok5){
                qDebug()<<"图书分类表初始化完成";
            }else{
                qDebug()<<"图书分类表初始化失败";
            }
}
}
QMap<int, QString> dbHelper::getALLCategory()
{
    QMap<int,QString> categoryMap;
    QSqlQuery query;
    query.exec("SELECt category_id,category_name FROM category ORDER BY category_id");
    while (query.next()){
        int id=query.value("category_id").toInt();
        QString name=query.value("category_name").toString();
        categoryMap.insert(id,name);

    }
    return categoryMap;
}
//在界面加载用户表
void dbHelper::queryUserList(QSqlQueryModel *model, QString &searchKey)
{
    QString sql;
    QString key=searchKey.trimmed();
    if (key.isEmpty()){
       sql= R"(
                     SELECT
                     userid,
                     username,
                     password,
                     CASE role
                     WHEN 1 THEN '管理员'
                     ELSE '普通用户'
                     END AS role_text
                 FROM user_info
                     )";
    }
    else {
        sql=QString(R"(
                    SELECT
                     userid,
                    username,
                    password,
                    CASE role
                    WHEN 1 THEN '管理员'
                    ELSE '普通用户'
                    END AS role_text
                    FROM user_info
                    WHERE username LIKE '%%1%'
                    )").arg(key);
    }
    model->setQuery(sql);
}
//图书入库
bool dbHelper::insertBookInfo(Book &b)
{
    QString sql="INSERT INTO books_info(ISBN,bookname,author,publisher,stock,category_id) VALUES(:ISBN,:bookname,:author,:publisher,:stock,:id)";
    QSqlQuery query;
    query.prepare(sql);
    query.bindValue(":ISBN",b.IBSN);
    query.bindValue(":bookname",b.bookname);
    query.bindValue(":author",b.author);
    query.bindValue(":publisher",b.publisher);
    query.bindValue(":stock",b.stock);
    query.bindValue(":id",b.category_id);
    if (!query.exec()){
        qDebug()<<"图书入库失败"<<query.lastError().text();
        return false;
    }
    else {
        qDebug()<<"图书入库成功";
    }
    return true;
}
//更新图书信息
bool dbHelper::updateBookInfoById(Book &b, int bookid)
{
    QSqlQuery query;
    query.prepare("UPDATE books_info "
                  "SET ISBN=:isbn,bookname=:name,author=:auth,publisher=:pub,stock=:sto "
                  "WHERE bookid=:id");
    query.bindValue(":isbn",b.IBSN);
    query.bindValue(":name",b.bookname);
    query.bindValue(":auth",b.author);
    query.bindValue(":pub",b.publisher);
    query.bindValue(":sto",b.stock);
    query.bindValue(":id",bookid);
    if (query.exec()){
        return query.numRowsAffected()>0;
    }
    else {
        qDebug()<<"修改图书失败："<<query.lastError().text();
        return false;
    }
}
//删除多本
int dbHelper::deleteBooksByIds(const QList<int>& boookIds)
{
    if (boookIds.isEmpty()){
        return 0;
    }
        QSqlDatabase db=QSqlDatabase::database();
    if (!db.transaction()){
        qDebug()<<"无法开启事务"<<db.lastError().text();
        return 0;
    }
    QSqlQuery query;
    int successCount=0;
    for (int bookid:boookIds){
        query.prepare("DELETE FROM books_info WHERE bookid = :bid");
        query.bindValue(":bid",bookid);
        if(query.exec()&&query.numRowsAffected()>0){//确保数据真的被修改
            successCount++;
        }
        else {
            qDebug()<<"删除bookid="<<bookid<<"失败："<<query.lastError().text();
        }
        if (successCount==boookIds.size()){
            db.commit();
        }
        else {
            db.rollback();
            successCount=0;//视为全部失败
        }
    }
    return successCount;
}
//删除单本
bool dbHelper::deleteBookById(int bookid)
{QSqlQuery query;
    query.prepare("DELETE FROM books_info WHERE bookid= :bid");
    query.bindValue(":bid",bookid);
    if (query.exec()&&query.numRowsAffected()>0){
        return true;
    }
    else {
        qDebug()<<"删除失败："<<query.lastError().text();
        return false;
    }
}
//3.借阅
//创建借阅表
void dbHelper::createBorrowTable()
{
    QString sql="CREATE TABLE IF NOT EXISTS borrow_info ("
            "borrow_id INTEGER PRIMARY KEY AUTOINCREMENT,"
          "  user_id INTEGER NOT NULL,"
           " book_id TEXT NOT NULL,"
          "  borrow_date TEXT NOT NULL,"
          "  due_date TEXT NOT NULL,"
           " return_date TEXT,"
           " status INTEGER DEFAULT 0,"
            "FOREIGN KEY (user_id) REFERENCES user_info(userid),"
           " FOREIGN KEY (book_id) REFERENCES books_info(bookid)"
        ")";
    QSqlQuery query;
   if (query.exec(sql)){
    qDebug()<<"创建借阅表成功";
   }
   else {
       qDebug()<<"创建借阅表失败";
}
}
bool dbHelper::addBorrow(int userid, int bookid, int BorrowDays)
{
QSqlDatabase db=QSqlDatabase::database();
        db.transaction();// 开启事务
        // 1. 校验图书库存是否充足
        QSqlQuery checkStock;
        checkStock.prepare("SELECT stock FROM books_info WHERE bookid = :id");
        checkStock.bindValue(":id", bookid);
        if (!checkStock.exec() || !checkStock.next()) {
            db.rollback();//事务滚动回开始
            return false;
        }
        int stock = checkStock.value(0).toInt();
        if (stock <= 0) {
            db.rollback();
            return false;
        }
        // 2. 更新库存
        QSqlQuery updateStock;
        updateStock.prepare("UPDATE books_info SET stock = stock - 1 WHERE bookid = :id");
        updateStock.bindValue(":id", bookid);
        if (!updateStock.exec()) {
            db.rollback();
            return false;
        }

        // 3. 新增借阅记录
        QSqlQuery addRecord;
        addRecord.prepare(R"(
            INSERT INTO borrow_info (user_id, book_id, borrow_date, due_date, status)
            VALUES (:uid, :id, date('now'), date('now', :days || ' day'), 0)
        )");
        addRecord.bindValue(":uid", userid);
        addRecord.bindValue(":id", bookid);
        addRecord.bindValue(":days", BorrowDays);
        if (!addRecord.exec()) {
            db.rollback();
            return false;
        }
        db.commit();//提交事务
        return true;
}
int dbHelper::getBookIDByName(QString &bookname)
{
    QSqlQuery query;
    query.prepare("SELECT bookid FROM books_info WHERE bookname= :name LIMIT 1");
    query.bindValue(":name",bookname);
    if (query.exec()&&query.next()){
        int ZERO=0;
        return query.value(ZERO).toInt();
    }
    return -1;
}
//查询借阅列表并返回借阅表模型
QSqlQueryModel *dbHelper::getBorrowList(QString keyword, int statusFilter, int userId)
{QSqlQueryModel *model=new QSqlQueryModel();
   QString sql=R"(
                       SELECT
            b.borrow_id AS 借阅ID,
               bk.bookname AS 书名,
               u.username AS 借阅人,
                       b.borrow_date AS 借书日期,
                                   b.due_date AS 应还日期,
                                   IFNULL(b.return_date, '-') AS 还书日期,
                                   CASE
                                       WHEN b.status = 1 THEN '已归还'
                                       WHEN date('now') > b.due_date THEN '逾期'
                                       ELSE '未归还'
                                   END AS 借阅状态
                               FROM borrow_info b
                               LEFT JOIN user_info u ON b.user_id = u.userid
                               LEFT JOIN books_info bk ON b.book_id = bk.bookid
                               WHERE 1=1
                           )";
//非管理员只看自己的记录
if (userId!=-1){//管理员的id传入-1
    sql += QString(" AND b.user_id=%1").arg(userId);
}
int isReturned=1;
int isOverDue=2;
int isNotReturned=0;
//关键词筛选(借阅人/书名)
if (!keyword.isEmpty()){
    keyword=keyword.replace("'","''");
    sql += QString(" AND (u.username LIKE '%%%1%' OR bk.bookname LIKE '%%%1%')").arg(keyword);
}
//状态筛选
if (statusFilter==isNotReturned){//未归还
    sql+=" AND b.status != 1 AND date('now')<=b.due_date";
}
else if (statusFilter == isReturned) { // 已归还
        sql += " AND b.status = 1";
    } else if (statusFilter == isOverDue) { // 逾期
        sql += " AND b.status != 1 AND date('now') > b.due_date";
    }
    sql += " ORDER BY b.borrow_id DESC";
//    qDebug()<<"生成的sql语句"<<sql;
    model->setQuery(sql);
    if (model->lastError().isValid()) {
            qDebug() << "SQL执行错误：" << model->lastError().text();
        }
    return model;
}
bool dbHelper::returnBook(int borrowId, int bookId)
{
    QSqlDatabase db=QSqlDatabase::database();
    if (!db.transaction()){
        qDebug()<<"事务启动失败"<<db.lastError().text();
        return false;
    }
    QSqlQuery query;
    //更新借阅记录，设置还书日期
    QString updateBorrowSql=R"(
                            UPDATE borrow_info
            SET status = 1,return_date=date('now')
                            WHERE borrow_id = ?
                            )";
    query.prepare(updateBorrowSql);
    query.addBindValue(borrowId);
    if (!query.exec()){
        qDebug()<<"更新借阅记录失败："<<query.lastError().text();
        db.rollback();
        return false;
    }
    //更新图书库存，库存+1
    QString updateBookSql="UPDATE books_info SET stock = stock+1 WHERE bookid = ?";
    query.prepare(updateBookSql);
    query.addBindValue(bookId);
    if (!query.exec()){
        qDebug()<<"更新图书库存失败"<<query.lastError().text();
        db.rollback();
        return false;
    }
    //提交事务
    if (!db.commit()){
        qDebug()<<"事务提交失败："<<db.lastError().text();
        return false;
    }
    return true;
}
