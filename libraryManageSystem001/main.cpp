#include "mainwindow.h"
#include"dbhelper.h"
#include"registerdialog.h"
#include <QApplication>
#include "logindialog.h"
int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    dbHelper::initDatabase();
    MainWindow w;
   LoginDialog l;
  if (l.exec()==QDialog::Accepted){
      User user=l.getLoggedUser();
      w.setCurrentUser(user);
        w.show();
        return a.exec();
    }
    return 0;
}
