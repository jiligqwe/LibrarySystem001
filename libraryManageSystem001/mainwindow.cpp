#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"logindialog.h"
class CenterAlignDelegate : public QStyledItemDelegate
{
public:
    explicit CenterAlignDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}
protected:
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const override
    {
        QStyledItemDelegate::initStyleOption(option, index);
        // 设置文字水平+垂直完全居中
        option->displayAlignment = Qt::AlignCenter;
    }
};
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer=new QTimer(this);//创建定时器
    int s=1000;
    timer->start(s);//1秒触发1次
    //绑定定时器到更新时间的函数
    connect(timer,&QTimer::timeout,this,&MainWindow::updateCurrentTime);
    ui->tableView_bookInformation->verticalHeader()->setVisible(false);
//设置个人中心信息只读
    ui->lineEdit_username->setReadOnly(true);
    ui->lineEdit_realname->setReadOnly(true);
    ui->lineEdit_phonenumber->setReadOnly(true);
    //加载书籍类型选择
    QMap<int,QString> categorymap=dbHelper::getALLCategory();
    ui->comboBox_category->clear();
    int zero=0;
    ui->comboBox_category->addItem("全部",zero);
    for (auto it =categorymap.begin();it!=categorymap.end();++it){
        ui->comboBox_category->addItem(it.value(),it.key());
    }
    //绑定点击某类型信号和函数
    connect(ui->comboBox_category,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&MainWindow::onCategoryFilterChanged);
dbHelper::createBooksTable();
initBookTableModel();
dbHelper::createBorrowTable();
initBorrowTableModel();
initUserTableModel();
qDebug()<<"现在"<<currentUser.userid;
//加载主页
updateCurrentTime();
refreshHomeData();
//确保先呈现主页
int zhuye=4;
ui->stackedWidget->setCurrentIndex(zhuye);
}
MainWindow::~MainWindow()
{
    delete ui;
}
//图书搜索
void MainWindow::on_pushButton_search_clicked()
{
QString keyword=ui->lineEdit_search->text().trimmed();
if (keyword.isEmpty()){
    bookmodel->setFilter("");//清空所有筛选条件
    bookmodel->select();}
else {
    keyword=keyword.replace("'","''");//转义关键词中的单引号
    QString filter=QString(
                "bookname LIKE '%%%1%'"
                "OR author LIKE '%%%1%'"
                "OR ISBN LIKE '%%%1%'").arg(keyword);
    bookmodel->setFilter(filter);
    //刷新表格显示搜索结果
    bookmodel->select();
}
}
//图书添加
void MainWindow::on_pushButton_bookAdd_clicked()
{int isAdmin=1;
    if (currentUser.role !=isAdmin) {
           QMessageBox::warning(this, "无权限", "您没有增加图书的管理员权限");
           return;
       }
addBookDialog a;
//调用图书入库的窗口实现操作
if (a.exec()==QDialog::Accepted){
    bookmodel->select();//更新数据
}
}
//图书修改
void MainWindow::on_pushButton_bookChange_clicked(){
    //防止普通用户修改数据
    int isAdmin=1;
    if (currentUser.role !=isAdmin) {
           QMessageBox::warning(this, "无权限", "您没有删除图书的管理员权限");
           return;
       }
QModelIndexList indexes= ui->tableView_bookInformation->selectionModel()->selectedRows();
    if (indexes.isEmpty()){
        QMessageBox::warning(this,"提示","请先选择要修改的图书");
        return;
    }
    //获取行索引
    int row=indexes.first().row();
    //获取书ID
    int bookidColumn=0;
    int bookid=bookmodel->index(row,bookidColumn).data().toInt();
    editBookDialog editDialog(bookid,this);
    if (editDialog.exec()==QDialog::Accepted){
        bookmodel->select();//更新数据
    }
    }
//图书删除
void MainWindow::on_pushButton_bookDelete_clicked()
{
    int isAdmin=1;
    if (currentUser.role !=isAdmin) {
           QMessageBox::warning(this, "无权限", "您没有删除图书的管理员权限");
           return;
       }
    QModelIndexList indexes= ui->tableView_bookInformation->selectionModel()->selectedRows();
        if (indexes.isEmpty()){
            QMessageBox::warning(this,"提示","请先选择要删除的图书");
            return;
        }
        int count=indexes.count();

        QMessageBox::StandardButton res=QMessageBox::question(this,"确认删除",QString("你选中了%1种图书，确定要全部删除吗？此操作不可恢复！").arg(count),
                                                              QMessageBox::Yes|QMessageBox::No);

        if (res!=QMessageBox::Yes){
            return;
        }
        if(count==1){
        //获取行索引
        int row=indexes.first().row();
        //获取书ID
        int bookid=bookmodel->index(row,0).data().toInt();
        QString bookname=bookmodel->index(row,2).data().toString();
        if (dbHelper::deleteBookById(bookid)){
         bookmodel->select();
            QMessageBox::information(this,"完成",QString("成功删除 %5").arg(bookname));

        }
        }
     else {
       //收集所有要删除的bookid
        QList<int> bookIds;
        for (auto index:indexes){
            int row=index.row();
            int bookidColumn=1;
            int bookid=bookmodel->index(row,bookidColumn).data().toInt();
            bookIds.append(bookid);
        }
        int successCount=dbHelper::deleteBooksByIds(bookIds);

        if (successCount==count){
            bookmodel->select();
            QMessageBox::information(this,"完成",QString("成功删除 %1 本图书").arg(successCount));

        }
        else {
            bookmodel->select();
            QMessageBox::warning(this,"失败","删除失败");
        }
        }
}
//初始化表格第一列序号
void MainWindow::updateSerialNumbers()
{
    QAbstractItemModel *model=ui->tableView_bookInformation->model();
    for (int i=0;i<model->rowCount();i++){
        int zero=0;
        bookmodel->setData(model->index(i,zero),i+1);

    }
}
//在主界面加载图书信息
void MainWindow::initBookTableModel()
{
    //使用关系表模型，支持外键关联
    bookmodel=new QSqlRelationalTableModel(this,QSqlDatabase::database());
bookmodel->setTable("books_info");
    bookmodel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //设置外键关联(category_id的列索引位6)
    bookmodel->setRelation(6,QSqlRelation("category","category_id","category_name"));
   bool selectok= bookmodel->select();//加载所有数据
   if (!selectok){
       qDebug()<<"刷新失败"<<bookmodel->lastError().text();
   }
   //绑定模型到表格   绑定到ui的tableview
    ui->tableView_bookInformation->setModel(bookmodel);
    int bookidColumn=0;
    ui->tableView_bookInformation->setColumnHidden(bookidColumn,true);
    ui->tableView_bookInformation->verticalHeader()->setVisible(true);
    //调整行号列宽度
    int width=100;
    ui->tableView_bookInformation->verticalHeader()->setDefaultSectionSize(width);
    //行号居中
    ui->tableView_bookInformation->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    //所有单元格居中
    ui->tableView_bookInformation->setItemDelegate(new CenterAlignDelegate(this));
ui->tableView_bookInformation->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//所以列均分表宽度
   //设置表头
int idc=0;
int isc=1;
int boc=2;
int auc=3;
int puc=4;
int stc=5;
int tyc=6;
     bookmodel->setHeaderData(idc,Qt::Horizontal,"ID");
    bookmodel->setHeaderData(isc,Qt::Horizontal,"IBSN");
     bookmodel->setHeaderData(boc,Qt::Horizontal,"书名");
      bookmodel->setHeaderData(auc,Qt::Horizontal,"作者");
       bookmodel->setHeaderData(puc,Qt::Horizontal,"出版社");
        bookmodel->setHeaderData(stc,Qt::Horizontal,"库存");
        bookmodel->setHeaderData(tyc,Qt::Horizontal,"书籍类型");
//      ui->tableView_bookInformation->setColumnHidden(1,true);
        //点击表格整行选中
        ui->tableView_bookInformation->setSelectionBehavior(QAbstractItemView::SelectRows);
//设置表格不可编辑
ui->tableView_bookInformation->setEditTriggers(QAbstractItemView::NoEditTriggers);
}
//设置用户权限
void MainWindow::initPermission()
{
}
//初始化借阅表格
void MainWindow::initBorrowTableModel()
{
    ui->tableView_borrow->setSelectionBehavior(QAbstractItemView::SelectRows);//整行选择
    ui->tableView_borrow->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    ui->tableView_borrow->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);//表头垂直居中
ui->tableView_borrow->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//所以列均分表宽度
ui->tableView_borrow->setStyleSheet("QTableView::item { text-align: center; }");//所有单元格文字垂直居中
borrowmodel=new QSqlQueryModel(this);

ui->tableView_borrow->setModel(borrowmodel);
refreshBorrowTable();
}
//更新表格
void MainWindow::refreshBorrowTable()
{int isAdmin=1;
    QString keyword = ui->lineEdit_borrowSearch->text().trimmed();
        int statusFilter = ui->comboBox_borrowStatus->currentIndex() - 1;
        int queryUserId = (currentUser.role == isAdmin) ? -1 : currentUser.userid;
       borrowmodel = dbHelper::getBorrowList(keyword, statusFilter, queryUserId);
        ui->tableView_borrow->setModel(borrowmodel);
        ui->tableView_borrow->setColumnHidden(0, true); // 隐藏借阅ID列
}
//接收用户身份
void MainWindow::setCurrentUser(const User user)
{
    this->currentUser=user;
    ui->lineEdit_username->setText(currentUser.username);
    ui->lineEdit_realname->setText(currentUser.realname);
    ui->lineEdit_phonenumber->setText(currentUser.phone);
    qDebug()<<"user现在1的id:"<<user.userid;
    refreshBorrowTable();
    refreshHomeData();
}
//按借书按钮
void MainWindow::on_pushButton_borrow_clicked()
{
    qDebug()<<"当前2用户id:"<<currentUser.userid;
    int userid=currentUser.userid;
    bool ok;
    //输入书名
    QString bookname = QInputDialog::getText(this,"借书登记","请输入书名：",QLineEdit::Normal,"",&ok);
    if (!ok||bookname.trimmed().isEmpty()){
        return;//用户取消或输入为空，直接退出
    }
    //根据书名获取书id
    int bookid=dbHelper::getBookIDByName(bookname);
    if (bookid==-1){
        QMessageBox::warning(this,"失败","未找到该图书，请检查该书名是否正确");
        return;
    }
    //输入借阅天数
    int days=QInputDialog::getInt(this,"借书登记","请输入借阅天数；",30,1,90,1,&ok);
       if (dbHelper::addBorrow(userid,bookid,days)){
           initBookTableModel();
           refreshBorrowTable();
           QMessageBox::information(this,"成功","借书登记完成");
           refreshHomeData();
       }
       else {
           QMessageBox::warning(this,"失败","借书失败，请检查图书库存是否充足");
       }
}
//刷新界面
void MainWindow::on_pushButton_borrowrefresh_clicked()
{
    refreshBorrowTable();
}
//还书
void MainWindow::on_pushButton_return_clicked()
{

    if (currentUser.role !=1) {
           QMessageBox::warning(this, "无权限", "您没有还书的管理员权限");
           return;
       }
QModelIndexList indexes=ui->tableView_borrow->selectionModel()->selectedRows();
if (indexes.isEmpty()){
    QMessageBox::warning(this,"提示","请先选择一条借阅记录");
    return;
}
QModelIndex index=indexes.first();
int row=index.row();
//获取借阅信息
int borrowId=borrowmodel->data(borrowmodel->index(row,0)).toInt();
QString bookname=borrowmodel->data(borrowmodel->index(row,1)).toString();
int bookId=dbHelper::getBookIDByName(bookname);
qDebug()<<"本书id为："<<bookId;
QString dueday=borrowmodel->data(borrowmodel->index(row,4)).toString();
QString status=borrowmodel->data(borrowmodel->index(row,6)).toString();//借阅状态
//已归还判断
if (status=="已归还"){
    QMessageBox::warning(this,"提示","该书已经归还！");
    return;
}
//逾期判断
bool isOverDue=false;
QDate dueDate=QDate::fromString(dueday,"yyyy-MM-dd");
QDate today=QDate::currentDate();
if (today>dueDate){
    isOverDue=true;
}
if (isOverDue){
    QMessageBox::StandardButton reply;
    reply =QMessageBox::warning(this,"逾期提醒",QString("《%1》已逾期,是否确认归还？").arg(bookname),
                                QMessageBox::Yes|QMessageBox::No);
    if (reply!=QMessageBox::Yes){
        return;
    }
}
//还书并更新表格模型
if (dbHelper::returnBook(borrowId,bookId)){
    refreshBorrowTable();
    initBookTableModel();
    QMessageBox::information(this,"成功",QString("《%1》已成功归还").arg(bookname));
}
else {
    QMessageBox::critical(this,"错误","还书失败，请检查日志");
}
}
void MainWindow::on_comboBox_borrowStatus_currentIndexChanged()
{
    refreshBorrowTable();
}
void MainWindow::on_pushButton_borrowsearch_clicked()
{
    refreshBorrowTable();
}
void MainWindow::on_lineEdit_borrowSearch_returnPressed()
{
    refreshBorrowTable();
}
//主页设计
void MainWindow::updateCurrentTime()
{//格式化时间
    QString time=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->label_time->setText("北京时间："+time);
}
//更新主页数据
void MainWindow::refreshHomeData()
{
    int borredBookNumber=0;
//    int stock=0;

    QString role=(currentUser.role==1)?"管理员":"普通用户";
    ui->label_welcome->setText(QString("欢迎您：%1 | 当前角色：%2").arg(currentUser.username).arg(role));
    QSqlQuery query;
    //总图书数量 库存加在借
    bool queryOk = query.exec("SELECT SUM(stock) FROM books_info");
    int stock = 0; // 用局部变量，每次刷新重新初始化，杜绝旧值干扰
    if (queryOk && query.next()){
        stock = query.value(0).toInt();
        qDebug() << "查询到的库存总和：" << stock; // 打印验证查询结果
    } else {
        qDebug() << "库存查询失败：" << query.lastError().text();
    }

    qDebug() << "在借图书数量：" << borredBookNumber; // 打印在借数验证
    int booksnumber = stock + borredBookNumber;
    qDebug() << "最终图书总数：" << booksnumber;

    ui->label_booksNumber->setText("总图书数量:" + QString::number(booksnumber));
//注册用户数
    query.exec("SELECT COUNT(*) FROM user_info");
    if (query.next()){
        ui->label_usersNumber->setText("总用户数:"+query.value(0).toString());
    }
    //热门图书
    QString hotBook="🔥 热门图书 TOP5：\n";
    QString sql=R"(
                SELECT bk.bookname,COUNT(br.borrow_id) AS cnt
            FROM books_info bk
            LEFT JOIN borrow_info br ON bk.bookid = br.book_id
            GROUP BY bk.bookid,bk.bookname
            ORDER BY cnt DESC
            LIMIT 5
                )";
    if (query.exec(sql)){
        int idx=1;
        while (query.next()){
            QString name=query.value(0).toString();
            int times=query.value(1).toInt();
            qDebug()<<"现在的hotBook:"<<hotBook;
            hotBook +=QString("%1. %2 (借阅%3次)\n").arg(idx).arg(name).arg(times);
            idx++;
    }
}
    else {
    }
ui->label_hotbook->setText(hotBook);
}
//实现用户管理界面
//在用户管理界面加载用户信息
void MainWindow::initUserTableModel()
{
 usermodel=new QSqlQueryModel(this);
 QString sql=R"(
             SELECT
             userid,
             username,
             realname,
         phone,
             CASE role
             WHEN 1 THEN '管理员'
             ELSE '普通用户'
             END AS role_text
         FROM user_info
             )";
 usermodel->setQuery(sql);
    //绑定模型到表格   绑定到ui的tableview
    ui->tableView_user->setModel(usermodel);
    ui->tableView_user->setColumnHidden(0,true);
    ui->tableView_user->verticalHeader()->setVisible(true);
    //调整行号列宽度
    ui->tableView_user->verticalHeader()->setDefaultSectionSize(100);
    //行号居中
    ui->tableView_user->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    //所有单元格居中
    ui->tableView_user->setItemDelegate(new CenterAlignDelegate(this));
ui->tableView_user->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);//所以列均分表宽度
    //设置表头
int idc=0;
int usc=1;
int nac=2;
int phc=3;
int roc=4;
     usermodel->setHeaderData(idc,Qt::Horizontal,"ID");
    usermodel->setHeaderData(usc,Qt::Horizontal,"用户名");
     usermodel->setHeaderData(nac,Qt::Horizontal,"姓名");
      usermodel->setHeaderData(phc,Qt::Horizontal,"联系电话");
      usermodel->setHeaderData(roc,Qt::Horizontal,"用户权限");
//      ui->tableView_bookInformation->setColumnHidden(1,true);
        //点击表格整行选中
        ui->tableView_user->setSelectionBehavior(QAbstractItemView::SelectRows);
//设置表格不可编辑
ui->tableView_user->setEditTriggers(QAbstractItemView::NoEditTriggers);
}
//按选择的类型更新表格
void  MainWindow::onCategoryFilterChanged()
{//获取选择的分类ID
    int selectedId=ui->comboBox_category->currentData().toInt();
    if (selectedId==0){
        bookmodel->setFilter("");
    }
else {
        qDebug()<<"现在的分类id是"<<selectedId;
        bookmodel->setFilter(QString("books_info.category_id= %1").arg(selectedId));
    }
    bookmodel->select();
    if (bookmodel->lastError().isValid()) {
           qDebug() << "筛选查询错误：" << bookmodel->lastError().text();
       }
}
//实现页面间跳转
void MainWindow::on_pushButton_boooks_manage_clicked()
{int bookManagePage=0;
     ui->stackedWidget->setCurrentIndex(bookManagePage);
}
void MainWindow::on_pushButton_books_borrow_manage_clicked()
{
    refreshBorrowTable();//跳转至借阅页面前先刷新表格
     ui->stackedWidget->setCurrentIndex(1);
}
void MainWindow::on_pushButton_users_manage_clicked()
{int isAdmin=1;
    if (currentUser.role !=isAdmin) {
           QMessageBox::warning(this, "无权限", "您没有进入用户管理页面的管理员权限");
           return;
       }
    initUserTableModel();
    int USERPAGE=3;
     ui->stackedWidget->setCurrentIndex(USERPAGE);
}
void MainWindow::on_pushButton_zhuye_to_personalcenter_clicked()
{
    int personPage=2;
     ui->stackedWidget->setCurrentIndex(personPage);
}
void MainWindow::on_pushButton_usermanagetozhuye_clicked()
{
    refreshHomeData();
    int HOMEPAGE=4;
     ui->stackedWidget->setCurrentIndex(HOMEPAGE);
}
void MainWindow::on_pushButton_personnal_center_to_zhuye_clicked()
{
    refreshHomeData();
     int HOMEPAGE=4;
     ui->stackedWidget->setCurrentIndex(HOMEPAGE);
}
void MainWindow::on_pushButton_rentManageTozhuye_clicked()
{
    refreshHomeData();
     int HOMEPAGE=4;
    ui->stackedWidget->setCurrentIndex(HOMEPAGE);
}

void MainWindow::on_pushButton_books_manage_to_zhuye_clicked()
{
    refreshHomeData();
     int HOMEPAGE=4;
    ui->stackedWidget->setCurrentIndex(HOMEPAGE);
}
//实现点击菜单切换页面
void MainWindow::on_action_bookManage_triggered()
{int bookManagePage=0;
    ui->stackedWidget->setCurrentIndex(bookManagePage);
}
//借阅界面
void MainWindow::on_action_rentManage_triggered()
{int RENTPAGE=1;
    ui->stackedWidget->setCurrentIndex(RENTPAGE);
    refreshBorrowTable();
}
//个人中心
void MainWindow::on_action_userManage_triggered()
{int personPage=2;
    ui->stackedWidget->setCurrentIndex(personPage);
}
//用户管理
void MainWindow::on_action_personalCenter_triggered()
{int isAdmin=1;
    if (currentUser.role !=isAdmin) {
           QMessageBox::warning(this, "无权限", "您没有进入用户管理页面的管理员权限");
           return;
       }
    initUserTableModel();
    int USERPAGE=3;
    ui->stackedWidget->setCurrentIndex(USERPAGE);
}
//主页
void MainWindow::on_action_zhuye_triggered()
{
     refreshHomeData();
      int HOMEPAGE=4;
    ui->stackedWidget->setCurrentIndex(HOMEPAGE);
}
void MainWindow::on_pushButton_register_new_user_clicked()
{
    RegisterDialog r;
    r.exec();
    initUserTableModel();
}
//删除用户
void MainWindow::on_pushButton_delete_user_clicked()
{
    int isAdmin=1;
    if (currentUser.role !=isAdmin) {
           QMessageBox::warning(this, "无权限", "您没有删除用户的管理员权限");
           return;
       }

    QModelIndexList selectedRows=ui->tableView_user->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()){
        QMessageBox::warning(this,"提示","请先选择要删除的用户");
        return;
    }
   int ret=QMessageBox::question(this,"确认删除","确认要删除选中的用户吗？此操作不可恢复",
                                 QMessageBox::Yes | QMessageBox::No);
   if (ret != QMessageBox::Yes){
       return;
   }
   QSqlQuery query;
   bool hasError=false;
   QString errorMsg;
   for (auto &index:selectedRows){
       int userId=usermodel->data(index.model()->index(index.row(),0)).toInt();
      //禁止删除当前在登陆的管理员账号
       if (userId==currentUser.userid){
           QMessageBox::warning(this,"提示","不能删除当前登录的账号");
           hasError=true;
           continue;
       }
       //查询该用户是否有借阅记录
       query.prepare("SELECT COUNT(*) FROM borrow_info WHERE user_id = ?");
       query.addBindValue(userId);
       if (!query.exec()||!query.next()){
       QMessageBox::critical(this,"错误","查询借阅记录失败了");
       hasError=true;
       errorMsg+=":删除ID为"+QString::number(userId)+"的用户失败";
       continue;
       }
       int Zero=0;
           int borrowCount=query.value(Zero).toInt();
           if (borrowCount>0){
               QMessageBox::warning(this,"提示",QString("ID为%1的用户存在借阅记录，禁止删除！").arg(userId));
               hasError=true;
               continue;
           }
      //执行删除SQL
           query.prepare("DELETE FROM user_info WHERE userid = ?");
           query.addBindValue(userId);
           if (!query.exec()){
               hasError=true;
               errorMsg+=":删除ID为"+QString::number(userId)+"的用户失败";
           }
   }
    if (!hasError){
        QMessageBox::information(this,"成功","用户删除成功！");

initUserTableModel();
    }
    else {
        QMessageBox::critical(this,"失败","删除失败"+errorMsg);
initUserTableModel();
    }
}
void MainWindow::on_pushButton_search_user_clicked()
{
QString key=ui->lineEdit_search_user->text().trimmed();
dbHelper::queryUserList(usermodel,key);
ui->tableView_user->setModel(usermodel);//刷新表格
if (usermodel->lastError().isValid()){
    QMessageBox::critical(this,"查询失败",usermodel->lastError().text());
}
int Zero=0;
if (usermodel->rowCount()==Zero){
    QMessageBox::information(this,"提示","未匹配到相关用户");
}
}
void MainWindow::on_pushButton_ok_clicked()
{
    QString username=ui->lineEdit_username->text().trimmed();
    QString realname=ui->lineEdit_realname->text().trimmed();
    QString phone=ui->lineEdit_phonenumber->text().trimmed();
    if (username.isEmpty()){
        QMessageBox::warning(this,"提示","用户名不能为空");
        return;
    }
    if (username!=currentUser.username) {
        //修改用户名后校验用户名唯一性
    if (dbHelper::isUserNameExisted(username)) {
        QMessageBox::warning(this,"提示","该用户名已存在，请更换");
        return;
    }
    }
    int num=11;
    int Zero=0;
    if (phone.length()!=num&&phone.length()!=Zero){
        QMessageBox::warning(this,"提示","请输入标准的11位电话号码");
        return;
    }
    if (dbHelper::updateUserInfo(currentUser.userid,username,realname,phone)){
        currentUser.username=username;
        currentUser.realname=realname;
        currentUser.phone=phone;
        //再次设置个人中心信息只读
            ui->lineEdit_username->setReadOnly(true);
            ui->lineEdit_realname->setReadOnly(true);
            ui->lineEdit_phonenumber->setReadOnly(true);

        QMessageBox::information(this,"成功","个人信息修改成功");
    }
    else {
        QMessageBox::critical(this,"失败","请重试");
    }
}
//改密码
void MainWindow::on_pushButton_passwordChange_clicked()
{
    bool ok;
    //校验旧密码
    QString oldpassword=QInputDialog::getText(this,"修改密码","请输入原密码",
                                              QLineEdit::Password,"",&ok);
    if (!ok){
        return;//点击取消直接退出
        }
if (!dbHelper::isOldPassword(currentUser.userid,oldpassword)){
    QMessageBox::warning(this,"验证失败","原密码输入错误，请重试");
    return;
}
QString newPassword=QInputDialog::getText(this,"修改密码","请输入新密码",QLineEdit::Password,"",&ok);
if (!ok){
    return;
}
    if (newPassword.isEmpty()){
    QMessageBox::warning(this,"提示","新密码不能为空");
    return;
}
QString confirmPassword=QInputDialog::getText(this,"修改密码","请确认新密码",QLineEdit::Password,"",&ok);
if(!ok){
    return;
}
if (confirmPassword!=newPassword){
QMessageBox::warning(this,"提示","两次输入的密码不一致");
return;
}
//限定密码长度
if (!(newPassword.length()>5&&newPassword.length()<13)){
    QMessageBox::warning(this,"提示","请输入6-12位的密码");

}
if (dbHelper::updateUserPassword(currentUser.userid,newPassword)){
    currentUser.password=newPassword;//更新本地密码
    QMessageBox::information(this,"成功","密码修改成功");
}
else {
    QMessageBox::critical(this,"失败","密码修改失败，请重试");
}
}
//编辑用户信息
void MainWindow::on_pushButton_edit_clicked()
{
    ui->lineEdit_username->setReadOnly(false);
    ui->lineEdit_realname->setReadOnly(false);
    ui->lineEdit_phonenumber->setReadOnly(false);
}
void MainWindow::on_pushButton_exit_clicked()
{
    int ret=QMessageBox::question(this,"退出登录","确认要退出当前账号吗？",
                                 QMessageBox::Yes | QMessageBox::No);
    if (ret!=QMessageBox::Yes){
         return;
    }
    //清空缓存信息
currentUser.userid=0;
currentUser.username.clear();
currentUser.role=0;
currentUser.phone.clear();
currentUser.realname.clear();
currentUser.password.clear();
//隐藏主窗口
this->hide();
//跳出登录对话框
LoginDialog lo;
if (lo.exec()==QDialog::Accepted){
    //确保先呈现主页
    ui->stackedWidget->setCurrentIndex(4);
    this->show();
    //重新加载账号信息
    User user=lo.getLoggedUser();
    this->setCurrentUser(user);
}
else {
    //登录页取消，直接退出程序
    QApplication::quit();
}
}
void MainWindow::on_lineEdit_search_returnPressed()
{
    initBookTableModel();
}
