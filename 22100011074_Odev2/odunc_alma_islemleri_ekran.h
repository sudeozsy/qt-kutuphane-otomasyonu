// Sude Özsoy - 22100011074
#ifndef ODUNC_ALMA_ISLEMLERI_EKRAN_H
#define ODUNC_ALMA_ISLEMLERI_EKRAN_H

#include <QDialog>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QSqlError>
#include <QItemSelection>

namespace Ui {
class odunc_alma_islemleri_ekran;
}

class odunc_alma_islemleri_ekran : public QDialog
{
    Q_OBJECT

public:
    explicit odunc_alma_islemleri_ekran(QWidget *parent = nullptr);
    ~odunc_alma_islemleri_ekran();
    QSqlDatabase database= QSqlDatabase::addDatabase("QSQLITE");
    QSqlQuery *sorgu;
    QSqlQueryModel *model1,*model2,*model3;

private slots:
    void on_tableView_selectionChanged(const QItemSelection &selected);
    void on_tableView_2_selectionChanged(const QItemSelection &selected);

    void on_pushButton_clicked();

private:
    Ui::odunc_alma_islemleri_ekran *ui;
    void show_table1(),show_table2(),show_table3();
    bool stok_kontrol(QString);
};

#endif // ODUNC_ALMA_ISLEMLERI_EKRAN_H
