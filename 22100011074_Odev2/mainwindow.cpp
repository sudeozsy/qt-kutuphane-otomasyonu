// Sude Özsoy - 22100011074
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    iconlarıayarla();

    database.setDatabaseName("veritabani.db");
    if(!database.open())
    {
        ui->statusbar->showMessage("Veri Tabanına Bağlanılamadı!");
    }
    else
        ui->statusbar->showMessage("Veri Tabanına Bağlanıldı!");
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::iconlarıayarla()  //giriş ekranındaki iconları ayarlamak için
{
    ui->uye_islemleri->setIcon(QIcon(":/images/user.jpg"));
    ui->uye_islemleri->setIconSize(QSize(120,120));

    ui->kitap_islemleri->setIcon(QIcon(":/images/book.jpg"));
    ui->kitap_islemleri->setIconSize(QSize(120,120));

    ui->odunc_alma->setIcon(QIcon(":/images/odunc_1.jpg"));
    ui->odunc_alma->setIconSize(QSize(120,120));

    ui->odunc_teslim->setIcon(QIcon(":/images/odunc_2.jpg"));
    ui->odunc_teslim->setIconSize(QSize(120,120));
}

void MainWindow::on_uye_islemleri_clicked()
{
    u= new uye_islemleri_ekran(this);
    u->exec();
}


void MainWindow::on_kitap_islemleri_clicked()
{
    k= new kitap_islemleri_ekran(this);
    k->exec();
}


void MainWindow::on_odunc_alma_clicked()
{
    oa= new odunc_alma_islemleri_ekran(this);
    oa->exec();
}


void MainWindow::on_odunc_teslim_clicked()
{
    ov= new odunc_teslim_etme_islemleri_ekran(this);
    ov->exec();
}

