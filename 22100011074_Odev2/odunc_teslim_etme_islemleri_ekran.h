// Sude Özsoy - 22100011074
#ifndef ODUNC_TESLIM_ETME_ISLEMLERI_EKRAN_H
#define ODUNC_TESLIM_ETME_ISLEMLERI_EKRAN_H

#include <QDialog>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QSqlError>
#include <QItemSelection>
#include <QDate>

namespace Ui {
class odunc_teslim_etme_islemleri_ekran;
}

class odunc_teslim_etme_islemleri_ekran : public QDialog
{
    Q_OBJECT

public:
    explicit odunc_teslim_etme_islemleri_ekran(QWidget *parent = nullptr);
    ~odunc_teslim_etme_islemleri_ekran();

private slots:
    void on_tableView_selectionChanged(const QItemSelection &selected);

    void on_pushButton_clicked();

private:
    Ui::odunc_teslim_etme_islemleri_ekran *ui;
    QSqlDatabase database= QSqlDatabase::addDatabase("QSQLITE");
    QSqlQuery *sorgu;
    QSqlQueryModel *model1,*model2,*model3;
    void show_table1(),show_table2();
    int borcHesapla(QString,QString);
};

#endif // ODUNC_TESLIM_ETME_ISLEMLERI_EKRAN_H
