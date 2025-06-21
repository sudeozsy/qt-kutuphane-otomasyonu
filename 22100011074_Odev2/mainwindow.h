// Sude Özsoy - 22100011074
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "uye_islemleri_ekran.h"
#include "kitap_islemleri_ekran.h"
#include "odunc_alma_islemleri_ekran.h"
#include "odunc_teslim_etme_islemleri_ekran.h"

#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QSqlError>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_uye_islemleri_clicked();

    void on_kitap_islemleri_clicked();

    void on_odunc_alma_clicked();

    void on_odunc_teslim_clicked();

private:
    Ui::MainWindow *ui;
    void iconlarıayarla();
    uye_islemleri_ekran *u;
    kitap_islemleri_ekran *k;
    odunc_alma_islemleri_ekran *oa;
    odunc_teslim_etme_islemleri_ekran *ov;
    QSqlDatabase database= QSqlDatabase::addDatabase("QSQLITE");
    QSqlQuery *sorgu;
};
#endif // MAINWINDOW_H
