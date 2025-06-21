// Sude Özsoy - 22100011074
#ifndef KITAP_ISLEMLERI_EKRAN_H
#define KITAP_ISLEMLERI_EKRAN_H

#include <QDialog>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QSqlError>
#include <QItemSelection>

namespace Ui {
class kitap_islemleri_ekran;
}

class kitap_islemleri_ekran : public QDialog
{
    Q_OBJECT

public:
    explicit kitap_islemleri_ekran(QWidget *parent = nullptr);
    ~kitap_islemleri_ekran();

private slots:
    void on_tableView_selectionChanged(const QItemSelection &selected);

    void on_pushButton_yeni_clicked();

    void on_pushButton_guncelle_clicked();

    void on_pushButton_sil_clicked();

private:
    Ui::kitap_islemleri_ekran *ui;
    QSqlDatabase database= QSqlDatabase::addDatabase("QSQLITE");
    QSqlQuery *sorgu;
    QSqlQueryModel *model1,*model2,*model3;
    void show_table1(),show_table2(QString),show_table3(QString);
    void tablo2_3temizle();
};

#endif // KITAP_ISLEMLERI_EKRAN_H
