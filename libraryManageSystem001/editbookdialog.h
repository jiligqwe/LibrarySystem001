#ifndef EDITBOOKDIALOG_H
#define EDITBOOKDIALOG_H
#include"dbhelper.h"
#include <QDialog>
namespace Ui {
class editBookDialog;
}
class editBookDialog : public QDialog
{
    Q_OBJECT
public:
    explicit editBookDialog(int bookid,QWidget *parent = nullptr);
    ~editBookDialog();

private slots:
    void on_pushButton_ok_clicked();

    void on_pushButton_cancel_clicked();

private:
    Ui::editBookDialog *ui;
    int id;
};
#endif // EDITBOOKDIALOG_H
