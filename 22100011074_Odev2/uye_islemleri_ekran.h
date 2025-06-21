// Sude Özsoy - 22100011074
#ifndef UYE_ISLEMLERI_EKRAN_H
#define UYE_ISLEMLERI_EKRAN_H

#include <QDialog>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QSqlError>
#include <QItemSelection>

namespace Ui {
class uye_islemleri_ekran;
}

class uye_islemleri_ekran : public QDialog
{
    Q_OBJECT

public:
    explicit uye_islemleri_ekran(QWidget *parent = nullptr);
    ~uye_islemleri_ekran();

private slots:
    void on_tableView_selectionChanged(const QItemSelection &selected);

    void on_pushButton_clicked();

    void on_pushButton_guncelle_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::uye_islemleri_ekran *ui;
    QSqlDatabase database= QSqlDatabase::addDatabase("QSQLITE");
    QSqlQuery *sorgu;
    QSqlQueryModel *model1,*model2;
    void show_table();
};

#endif // UYE_ISLEMLERI_EKRAN_H
