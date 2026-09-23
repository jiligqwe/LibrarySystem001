#ifndef ADDBOOKDIALOG_H
#define ADDBOOKDIALOG_H
#include <QDialog>
#include"dbhelper.h"
namespace Ui {
class addBookDialog;
}
class addBookDialog : public QDialog
{
    Q_OBJECT
public:
    explicit addBookDialog(QWidget *parent = nullptr);
    ~addBookDialog();
private slots:
    void on_pushButton_ok_clicked();

    void on_pushButton_cancel_clicked();
private:
    Ui::addBookDialog *ui;
};
#endif // ADDBOOKDIALOG_H
