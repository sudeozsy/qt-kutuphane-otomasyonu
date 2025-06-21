/*// Sude Özsoy - 22100011074
#include "kitap_islemleri_ekran.h"
#include "ui_kitap_islemleri_ekran.h"

kitap_islemleri_ekran::kitap_islemleri_ekran(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::kitap_islemleri_ekran)
{
    ui->setupUi(this);
    database.setDatabaseName("veritabani.db");
    if (!database.open()) {
        QMessageBox::critical(this, "Veritabanı Hatası", "Veritabanı açılamadı!");
        return;
    }
    show_table1();

    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, //Tablodaki verilere tıkladıktan sonraki işlemler için connect
            this, &kitap_islemleri_ekran::on_tableView_selectionChanged);

}

kitap_islemleri_ekran::~kitap_islemleri_ekran()
{
    delete ui;
}

void kitap_islemleri_ekran::show_table1() //Tüm kitapları listeye yazdıran fonks
{
    model1 = new QSqlQueryModel(this);
    model1->setQuery("SELECT * FROM kitap", database);
    if (model1->lastError().isValid()) {
        QMessageBox::critical(this, "Veritabanı Hatası", model1->lastError().text());
        return;
    }
    ui->tableView->setModel(model1);
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &kitap_islemleri_ekran::on_tableView_selectionChanged);
}

void kitap_islemleri_ekran::show_table2(QString kitap_no)   //Seçilen kitabı odunç alanları gösteren listeyi yazdıran fonks
{
    model2 = new QSqlQueryModel(this);
    QSqlQuery sorgu(database);
    sorgu.prepare("SELECT * FROM odunc_alinan WHERE kitap_no = :kitap_no"); //Sadece seçilen kitabın bilgilerini göstermesi için sorgu
    sorgu.bindValue(":kitap_no", kitap_no);
    sorgu.exec();

    model2->setQuery(sorgu);
    if (model2->lastError().isValid()) {
        QMessageBox::critical(this, "Veritabanı Hatası", model2->lastError().text());
        return;
    }
    ui->tableView_2->setModel(model2);
}

void kitap_islemleri_ekran::show_table3(QString kitap_no)  //Seçilen kitabı teslim edenleri gösteren listeyi yazdıran fonks
{
    model3 = new QSqlQueryModel(this);
    QSqlQuery sorgu(database);
    sorgu.prepare("SELECT * FROM odunc_teslim_edilen WHERE kitap_no = :kitap_no"); //Sadece seçilen kitabın bilgilerini göstermesi için sorgu
    sorgu.bindValue(":kitap_no", kitap_no);
    sorgu.exec();

    model3->setQuery(sorgu);
    if (model3->lastError().isValid()) {
        QMessageBox::critical(this, "Veritabanı Hatası", model3->lastError().text());
        return;
    }
    ui->tableView_3->setModel(model3);
}

void kitap_islemleri_ekran::tablo2_3temizle()  //Yeni kitap eklendiğinde herhangi bir satır seçili değilken iki tabloyu temizler
{
    model2 = new QSqlQueryModel(this);
    model2->setQuery("SELECT NULL", database);
    ui->tableView_2->setModel(model2);

    model3 = new QSqlQueryModel(this);
    model3->setQuery("SELECT NULL", database);
    ui->tableView_3->setModel(model3);
}

void kitap_islemleri_ekran::on_tableView_selectionChanged(const QItemSelection &selected) //Tabloya tıklandığında
{
    if (selected.isEmpty()) {
        return;
    }

    QModelIndex index = selected.indexes().first();
    QString kitap_no = model1->data(model1->index(index.row(), 0)).toString();
    QString kitap_ad = model1->data(model1->index(index.row(), 1)).toString();
    QString kitap_sayisi  = model1->data(model1->index(index.row(), 2)).toString();

    ui->lineEdit_no->setText(kitap_no);
    ui->lineEdit_ad->setText(kitap_ad);
    ui->lineEdit_sayisi ->setText(kitap_sayisi );

    show_table2(kitap_no);
    show_table3(kitap_no);
}

void kitap_islemleri_ekran::on_pushButton_yeni_clicked() //Yeni kitap ekleme
{

    QString kitap_ad = ui->lineEdit_ad->text();
    QString kitap_sayisi = ui->lineEdit_sayisi ->text();

    if (kitap_ad.isEmpty() || kitap_sayisi .isEmpty()) {
        QMessageBox::warning(this, "Geçersiz Girdi", "Lütfen tüm bilgileri eksiksiz doldurun.");
        return;
    }

    QSqlQuery kontrol_sorgusu(database);
    kontrol_sorgusu.prepare("SELECT COUNT(*) FROM kitap WHERE kitap_ad = :kitap_ad");
    kontrol_sorgusu.bindValue(":kitap_ad", kitap_ad);

    if (kontrol_sorgusu.exec()) {
        kontrol_sorgusu.next();
        int sayac = kontrol_sorgusu.value(0).toInt();
        if (sayac > 0) {
            QMessageBox::warning(this, "Kayıt Mevcut", "Bu kitap zaten kayıtlı.");
            return;
        }
    } else {
        QMessageBox::critical(this, "Veritabanı Hatası", kontrol_sorgusu.lastError().text());
        return;
    }

    QSqlQuery sorgu(database);
    sorgu.prepare("INSERT INTO kitap (kitap_ad, kitap_sayisi ) VALUES (?, ?)");
    sorgu.addBindValue(kitap_ad);
    sorgu.addBindValue(kitap_sayisi);

    if (!sorgu.exec()) {
        QMessageBox::critical(this, "Veritabanı Hatası", sorgu.lastError().text());
    }
    else
    {
        QMessageBox::critical(this, "Kayıt!","Kayıt Oluşturuldu.");
    }

    int kitap_no = sorgu.lastInsertId().toInt();
    ui->lineEdit_no->setText(QString::number(kitap_no));

    show_table1();
    tablo2_3temizle();
}

void kitap_islemleri_ekran::on_pushButton_guncelle_clicked() //Kitap bilgilerini güncelleme
{
    int kitap_no = ui->lineEdit_no->text().toInt();
    QString kitap_ad = ui->lineEdit_ad->text();
    QString kitap_sayisi = ui->lineEdit_sayisi->text();

    if (kitap_ad.isEmpty() || kitap_sayisi.isEmpty() || kitap_no <= 0) {
        QMessageBox::warning(this, "Geçersiz Girdi", "Lütfen tüm bilgileri eksiksiz doldurun.");
        return;
    }

    QSqlQuery sorgu(database);
    sorgu.prepare("UPDATE kitap SET kitap_ad = :kitap_ad, kitap_sayisi = :kitap_sayisi WHERE kitap_no = :kitap_no");
    sorgu.bindValue(":kitap_ad", kitap_ad);         //Seçilen kitap no'suna göre bilgileri değiştirmek için kullanılan sorgu
    sorgu.bindValue(":kitap_sayisi", kitap_sayisi);
    sorgu.bindValue(":kitap_no", kitap_no);


    if (!sorgu.exec()) {
        QMessageBox::critical(this, "Veritabanı Hatası", sorgu.lastError().text());
        return;
    }
    else
    {
        QMessageBox::critical(this, "Güncelleme!","Güncelleme İşlemi Başarıyla Gerçekleşti.");
    }

    show_table1();
    tablo2_3temizle();
}

void kitap_islemleri_ekran::on_pushButton_sil_clicked()  //Kitap silme
{
    int kitap_no = ui->lineEdit_no->text().toInt();
    QString kitap_ad = ui->lineEdit_ad->text();
    int kitap_sayisi = ui->lineEdit_sayisi->text().toInt();

    if ( kitap_ad.isEmpty() || kitap_no <= 0) {
        QMessageBox::warning(this, "Geçersiz Girdi", "Lütfen tüm bilgileri eksiksiz doldurun.");
        return;
    }
    QSqlQuery sorgu(database);
    sorgu.prepare("SELECT COUNT(*) FROM odunc_alinan WHERE kitap_no = ?");  //Kitap no'suna göre o kitabı veritabanından siler.
    sorgu.addBindValue(kitap_no);

    if (!sorgu.exec()) {
        qDebug() << "Sorgu hatası:" << sorgu.lastError().text();
    }
    else
    {
        if (sorgu.next()) {
            int count = sorgu.value(0).toInt();
            if (count > 0) {
                QMessageBox::warning(this, "Geçersiz Girdi", "Bu kitap silinemez! Bu kitap bir üyeye ödünç verilmiştir.","OK");
                return;
            }
        }
    }
    sorgu.prepare("DELETE FROM kitap WHERE kitap_no = ?");
    sorgu.addBindValue(kitap_no);

    if (!sorgu.exec()) {
        QMessageBox::critical(this, "Veritabanı Hatası", sorgu.lastError().text());
        return;
    }
    QMessageBox::information(this, "Başarılı", "Girilen kitap numarasına göre kitap silme işlemi başarıyla gerçekleştirilmiştir.");
    show_table1();
}
// Sude Özsoy - 22100011074
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
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

// Sude Özsoy - 22100011074
#include "odunc_alma_islemleri_ekran.h"
#include "ui_odunc_alma_islemleri_ekran.h"

odunc_alma_islemleri_ekran::odunc_alma_islemleri_ekran(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::odunc_alma_islemleri_ekran)
{
    ui->setupUi(this);
    database.setDatabaseName("veritabani.db");
    if (!database.open()) {
        QMessageBox::critical(this, "Veritabanı Hatası", "Veritabanı açılamadı!");
        return;
    }
    show_table1();
    show_table2();
    show_table3();

    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,    //Tablodaki verilere tıkladıktan sonraki işlemler için connect
            this, &odunc_alma_islemleri_ekran::on_tableView_selectionChanged);

    connect(ui->tableView_2->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &odunc_alma_islemleri_ekran::on_tableView_2_selectionChanged);
}

odunc_alma_islemleri_ekran::~odunc_alma_islemleri_ekran()
{
    delete ui;
}

void odunc_alma_islemleri_ekran::show_table1()
{
    model1 = new QSqlQueryModel(this);
    model1->setQuery("SELECT * FROM üye", database);
    if (model1->lastError().isValid()) {
        QMessageBox::critical(this, "Veritabanı Hatası", model1->lastError().text());
        return;
    }
    ui->tableView->setModel(model1);
}

void odunc_alma_islemleri_ekran::show_table2()
{
    model2 = new QSqlQueryModel(this);
    model2->setQuery("SELECT * FROM kitap", database);
    if (model2->lastError().isValid()) {
        QMessageBox::critical(this, "Veritabanı Hatası", model1->lastError().text());
        return;
    }
    ui->tableView_2->setModel(model2);
}

void odunc_alma_islemleri_ekran::show_table3()
{
    model3 = new QSqlQueryModel(this);
    model3->setQuery("SELECT * FROM odunc_alinan", database);
    if (model3->lastError().isValid()) {
        QMessageBox::critical(this, "Veritabanı Hatası", model1->lastError().text());
        return;
    }
    ui->tableView_3->setModel(model3);
}

void odunc_alma_islemleri_ekran::on_tableView_selectionChanged(const QItemSelection &selected) {

    if (selected.isEmpty()) {
        return;
    }

    QModelIndex index = selected.indexes().first();
    QString uye_no = model1->data(model1->index(index.row(), 0)).toString();

    ui->lineEdit_uyeno->setText(uye_no);
}

void odunc_alma_islemleri_ekran::on_tableView_2_selectionChanged(const QItemSelection &selected) {

    if (selected.isEmpty()) {
        return;
    }

    QModelIndex index = selected.indexes().first();
    QString kitap_no = model2->data(model2->index(index.row(), 0)).toString();

    ui->lineEdit_kitapno->setText(kitap_no);
}

void odunc_alma_islemleri_ekran::on_pushButton_clicked()    //Kitap ödünç alma butonu
{
    QString uye_no = ui->lineEdit_uyeno->text();
    QString kitap_no = ui->lineEdit_kitapno ->text();
    QString alma_tarihi = ui->dateEdit->text();

    if (kitap_no.isEmpty() || uye_no.isEmpty() ) {
        QMessageBox::warning(this, "Geçersiz Girdi", "Lütfen tüm bilgileri eksiksiz doldurun.");
        return;
    }
    else if(!stok_kontrol(kitap_no))  //kitapların hepsi ödünc alınmışsa ve teslim edilmemişse stok uyarısı verir.
    {
        QMessageBox::warning(this, "Hata", "Ödünç alınmak istenen kitabın stoğu yok.");
    }
    else
    {
        QSqlQuery sorgu(database);
        sorgu.prepare("SELECT * FROM odunc_alinan WHERE uye_no = ? AND kitap_no = ?");
        sorgu.addBindValue(uye_no);
        sorgu.addBindValue(kitap_no);

        if (sorgu.exec() && sorgu.next()) {
            QMessageBox::warning(this, "Hata", "Bu üye bu kitabın bir tanesini ödünç almış,tekrar ödünç verilemez.");
        }
        else
        {
            sorgu.prepare("INSERT INTO odunc_alinan (uye_no, kitap_no, odunc_alma_tarihi) VALUES (?, ?, ?)");
            sorgu.addBindValue(uye_no);
            sorgu.addBindValue(kitap_no);
            sorgu.addBindValue(alma_tarihi);

            if (!sorgu.exec()) {
                QMessageBox::critical(this, "Veritabanı Hatası", sorgu.lastError().text());
            }
            else
            {
                QMessageBox::critical(this, "Kayıt!","Kayıt Oluşturuldu.");
            }
            show_table3();
        }
    }
}

bool odunc_alma_islemleri_ekran::stok_kontrol(QString kitap_no) //kitapno parametresine göre stok kontrol yapar.
{
    int stok=0,odunc_alinan=0;
    QSqlQuery sorgu(database);
    sorgu.prepare("SELECT kitap_sayisi FROM kitap WHERE kitap_no = ?");
    sorgu.addBindValue(kitap_no);

    if (sorgu.exec() && sorgu.next()) {
        stok = sorgu.value(0).toInt();  //Veritabanındaki kitap_sayisi bilgisini alir.
    }

    sorgu.prepare("SELECT COUNT(*) FROM odunc_alinan WHERE kitap_no = ?");
    sorgu.addBindValue(kitap_no);
    if (sorgu.exec() && sorgu.next()) {
        odunc_alinan = sorgu.value(0).toInt();  //veritabanındaki o kitabın kaç tane ödünç alınmış olduğunu hesaplar
    }
    if((stok - odunc_alinan) > 0)   //eğer tüm kitaplar ödünç alınmamışsa true döndürür, alınmışsa false
        return true;
    else
        return false;
}

// Sude Özsoy - 22100011074
#include "odunc_teslim_etme_islemleri_ekran.h"
#include "ui_odunc_teslim_etme_islemleri_ekran.h"

odunc_teslim_etme_islemleri_ekran::odunc_teslim_etme_islemleri_ekran(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::odunc_teslim_etme_islemleri_ekran)
{
    ui->setupUi(this);
    database.setDatabaseName("veritabani.db");
    if (!database.open()) {
        QMessageBox::critical(this, "Veritabanı Hatası", "Veritabanı açılamadı!");
        return;
    }
    show_table1();
    show_table2();
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &odunc_teslim_etme_islemleri_ekran::on_tableView_selectionChanged);
}

odunc_teslim_etme_islemleri_ekran::~odunc_teslim_etme_islemleri_ekran()
{
    delete ui;
}

void odunc_teslim_etme_islemleri_ekran::show_table1()
{
    model1 = new QSqlQueryModel(this);
    model1->setQuery("SELECT * FROM odunc_alinan", database);
    if (model1->lastError().isValid()) {
        QMessageBox::critical(this, "Veritabanı Hatası", model1->lastError().text());
        return;
    }
    ui->tableView->setModel(model1);
}

void odunc_teslim_etme_islemleri_ekran::show_table2()
{
    model2 = new QSqlQueryModel(this);
    model2->setQuery("SELECT * FROM odunc_teslim_edilen", database);
    if (model2->lastError().isValid()) {
        QMessageBox::critical(this, "Veritabanı Hatası", model1->lastError().text());
        return;
    }
    ui->tableView_2->setModel(model2);
}

int odunc_teslim_etme_islemleri_ekran::borcHesapla(QString alma_tarihi,QString teslim_tarihi) //parametredeki bilgileri alarak borcu hesaplar.
{
    int borc=0;
    QList alma_list = alma_tarihi.split(".");           //tarihlerin yazılma formatına göre(dateEdit '.' ile yazdırdığı için) splitle ayırma
    QList teslim_list = teslim_tarihi.split(".");

    int tarih1 = alma_list[0].toInt() + 30 * (alma_list[1].toInt() + 12 * alma_list[2].toInt());        //tarihlerin toplam kaç gün ettiğini hesaplar.
    int tarih2 = teslim_list[0].toInt() + 30 * (teslim_list[1].toInt() + 12 * teslim_list[2].toInt());

    if((tarih2 - tarih1) > 15)           //Eğer iki tarih arasındaki fark 15 günden fazlaysa gecikme süresinin 2 katı borç yazdırılır.
    {
        borc = (tarih2 - tarih1 - 15) * 2;
        return borc;
    }
    else if((tarih2 - tarih1) < 0)      //Eğer teslim edilen tarih alınan tarihten önce girildiyse yanlış girdi kontrolü için -1 döndürür.
    {
        return -1;
    }
    else                                //Eğer  günü geçmediyse borç 0'dır.
        return 0;
}

void odunc_teslim_etme_islemleri_ekran::on_tableView_selectionChanged(const QItemSelection &selected) {

    if (selected.isEmpty()) {
        return;
    }

    QModelIndex index = selected.indexes().first();
    QString uye_no = model1->data(model1->index(index.row(), 0)).toString();
    QString kitap_no = model1->data(model1->index(index.row(), 1)).toString();

    ui->lineEdit_uyeno->setText(uye_no);
    ui->lineEdit_kitapno->setText(kitap_no);
}

void odunc_teslim_etme_islemleri_ekran::on_pushButton_clicked()
{
    QString uye_no = ui->lineEdit_uyeno->text();
    QString kitap_no = ui->lineEdit_kitapno->text();
    QString teslim_tarihi = ui->dateEdit->text();

    if (kitap_no.isEmpty() || uye_no.isEmpty()) {
        QMessageBox::warning(this, "Geçersiz Girdi", "Lütfen tüm bilgileri eksiksiz doldurun.");
        return;
    }

    QSqlQuery sorgu(database);
    sorgu.prepare("SELECT odunc_alma_tarihi FROM odunc_alinan WHERE uye_no = ? AND kitap_no = ?");
    sorgu.addBindValue(uye_no);         //Kitabın ödünç alınma tarihini çekmek için sorgu
    sorgu.addBindValue(kitap_no);

    if (!sorgu.exec() || !sorgu.next()) {
        QMessageBox::critical(this, "Veritabanı Hatası", "Ödünç alınan kayıt bulunamadı.");
        return;
    }

    QString alma_tarihi = sorgu.value(0).toString();

    int borc = borcHesapla(alma_tarihi,teslim_tarihi);   //Ödünç alınma ve teslim edilme tarihlerini yollayıp borç hesaplar

    if(borc != -1)   //Borç -1 gelmediyse hesaplanan borçla birlikte bilgiler teslim edilen tablosuna yazdırılır ve odunc alınan tablosundan silinir.
    {
        QSqlQuery insertQuery(database);
        insertQuery.prepare("INSERT INTO odunc_teslim_edilen (uye_no, kitap_no, alma_tarihi, verme_tarihi, borc) VALUES (?, ?, ?, ?, ?)");
        insertQuery.addBindValue(uye_no);
        insertQuery.addBindValue(kitap_no);
        insertQuery.addBindValue(alma_tarihi);
        insertQuery.addBindValue(teslim_tarihi);
        insertQuery.addBindValue(borc);

        if (!insertQuery.exec()) {
            QMessageBox::critical(this, "Veritabanı Hatası", sorgu.lastError().text());
            return;
        }

        sorgu.prepare("DELETE FROM odunc_alinan WHERE uye_no = ? AND kitap_no = ?");
        sorgu.addBindValue(uye_no);
        sorgu.addBindValue(kitap_no);
        if (!sorgu.exec()) {
            QMessageBox::critical(this, "Veritabanı Hatası", sorgu.lastError().text());
            return;
        }
        QMessageBox::information(this, "Başarılı", "Kitap teslim edildi.");
        show_table1();
        show_table2();
    }
    else    //Eğer borç -1 geldiyse teslim edilme tarihinde sıkıntı vardır demektir.
    {
        QMessageBox::warning(this, "Geçersiz Girdi!", "Lütfen teslim tarihini doğru bir şekilde girin. Teslim tarihi ödünç tarihinden önce olamaz!");
    }
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &odunc_teslim_etme_islemleri_ekran::on_tableView_selectionChanged);
}

// Sude Özsoy - 22100011074
#include "uye_islemleri_ekran.h"
#include "ui_uye_islemleri_ekran.h"

uye_islemleri_ekran::uye_islemleri_ekran(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::uye_islemleri_ekran)
{
    ui->setupUi(this);
    database.setDatabaseName("veritabani.db");
    database.setConnectOptions("QSQLITE_BUSY_TIMEOUT=5000");

    if (!database.open()) {
        QMessageBox::critical(this, "Veritabanı Hatası", "Veritabanı açılamadı!");
        return;
    }

    show_table();

    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,   //Tablodaki verilere tıkladıktan sonraki işlemler için connect
            this, &uye_islemleri_ekran::on_tableView_selectionChanged);


}

uye_islemleri_ekran::~uye_islemleri_ekran()
{
    delete ui;
}

void uye_islemleri_ekran::show_table()  //Güncel tabloyu ekrandaki tableview'a yazdırır.
{
    model1 = new QSqlQueryModel(this);
    model1->setQuery("SELECT * FROM üye", database);
    if (model1->lastError().isValid()) {
        QMessageBox::critical(this, "Veritabanı Hatası", model1->lastError().text());
        return;
    }
    ui->tableView->setModel(model1);

    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &uye_islemleri_ekran::on_tableView_selectionChanged);

}

void uye_islemleri_ekran::on_tableView_selectionChanged(const QItemSelection &selected) //tablodaki bir satıra tıklandığında
{
    if (selected.isEmpty()) {
        return;
    }

    QModelIndex index = selected.indexes().first();
    QString uye_no = model1->data(model1->index(index.row(), 0)).toString();    //seçilen satırdaki bilgileri çekmek için
    QString uye_ad = model1->data(model1->index(index.row(), 1)).toString();
    QString uye_soyad = model1->data(model1->index(index.row(), 2)).toString();

    ui->lineEdit_no->setText(uye_no);       //Çekilen bilgiler LineEditlere yazdırılır.
    ui->lineEdit_name->setText(uye_ad);
    ui->lineEdit_sname->setText(uye_soyad);
}

void uye_islemleri_ekran::on_pushButton_clicked()   //Üye ekleme butonu
{
    QString uye_ad = ui->lineEdit_name->text();
    QString uye_soyad = ui->lineEdit_sname->text();

    if (uye_ad.isEmpty() || uye_soyad.isEmpty()) {  //Eksik bilgi girdisi kontrol
        QMessageBox::warning(this, "Geçersiz Girdi", "Lütfen tüm bilgileri eksiksiz doldurun.");
        return;
    }
    //Veritabanında aynı isim ve soyisimde üye var mı diye kontrol sorgusu
    QSqlQuery kontrol_sorgusu(database);
    kontrol_sorgusu.prepare("SELECT COUNT(*) FROM üye WHERE uye_ad = :uye_ad AND uye_soyad = :uye_soyad");
    kontrol_sorgusu.bindValue(":uye_ad", uye_ad);
    kontrol_sorgusu.bindValue(":uye_soyad", uye_soyad);

    if (kontrol_sorgusu.exec()) {
        kontrol_sorgusu.next();
        int sayac = kontrol_sorgusu.value(0).toInt();
        if (sayac > 0) {                    //Aynı isimde 0'dan fazla kişi bulduğunda kayıt işlemi yapmadan çıkıyor.
            QMessageBox::warning(this, "Kayıt Mevcut", "Bu üye zaten kayıtlı.");
            return;
        }
    } else {
        QMessageBox::critical(this, "Veritabanı Hatası", kontrol_sorgusu.lastError().text());
        return;
    }
    QSqlQuery sorgu(database);
    sorgu.prepare("INSERT INTO üye (uye_ad, uye_soyad) VALUES (:uye_ad, :uye_soyad)");  //Veritabanına ekleme sorgusu
    sorgu.bindValue(":uye_ad", uye_ad);
    sorgu.bindValue(":uye_soyad", uye_soyad);

    if (!sorgu.exec()) {
        QMessageBox::critical(this, "Veritabanı Hatası", sorgu.lastError().text());
    }
    else
    {
        QMessageBox::information(this, "Kayıt!","Kayıt Oluşturuldu.");
    }
    //Oluşturulan yeni üyenin veritabanında otomatik artan numarasını da lineEdite yazdırmak için çekiyoruz.
    int uye_no = sorgu.lastInsertId().toInt();
    ui->lineEdit_no->setText(QString::number(uye_no));

    show_table(); //Güncellenen tabloyu tekrar ekranda gösteriyoruz.
}


void uye_islemleri_ekran::on_pushButton_guncelle_clicked()  //Güncelleme butonu
{
    int uye_no = ui->lineEdit_no->text().toInt();
    QString uye_ad = ui->lineEdit_name->text();
    QString uye_soyad = ui->lineEdit_sname->text();

    if (uye_ad.isEmpty() || uye_soyad.isEmpty() || uye_no <= 0) {
        QMessageBox::warning(this, "Geçersiz Girdi", "Lütfen tüm bilgileri eksiksiz doldurun.");
        return;
    }

    QSqlQuery sorgu(database);
    sorgu.prepare("UPDATE üye SET uye_ad = :uye_ad, uye_soyad = :uye_soyad WHERE uye_no = :uye_no"); //LineEditteki üye no'ya göre bilgileri değiştirir.
    sorgu.bindValue(":uye_ad", uye_ad);
    sorgu.bindValue(":uye_soyad", uye_soyad);
    sorgu.bindValue(":uye_no", uye_no);

    if (!sorgu.exec()) {
        QMessageBox::critical(this, "Veritabanı Hatası", sorgu.lastError().text());
        return;
    }
    else
    {
        QMessageBox::critical(this, "Güncelleme!","Güncelleme İşlemi Başarıyla Gerçekleşti.");
    }

    show_table();
}


void uye_islemleri_ekran::on_pushButton_3_clicked() //Silme Butonu
{
    int uye_no = ui->lineEdit_no->text().toInt();
    QString uye_ad = ui->lineEdit_name->text();
    QString uye_soyad = ui->lineEdit_sname->text();

    if (uye_ad.isEmpty() || uye_soyad.isEmpty() || uye_no <= 0) {
        QMessageBox::warning(this, "Geçersiz Girdi", "Lütfen tüm bilgileri eksiksiz doldurun.");
        return;
    }
    QSqlQuery sorgu(database);
    sorgu.prepare("SELECT COUNT(*) FROM odunc_alinan WHERE uye_no = ?");
    sorgu.addBindValue(uye_no);

    if (!sorgu.exec()) {
        qDebug() << "Sorgu hatası:" << sorgu.lastError().text();
    }
    else
    {
        if (sorgu.next()) {
            int count = sorgu.value(0).toInt();
            if (count > 0) {
                QMessageBox::warning(this, "Geçersiz Girdi", "Bu üye silinemez. Üyenin henüz teslim etmediği kitap var.","OK");
                return;
            }
        }
    }

    sorgu.prepare("DELETE FROM üye WHERE uye_no = ?");
    sorgu.addBindValue(uye_no);

    if (!sorgu.exec()) {
        QMessageBox::critical(this, "Veritabanı Hatası", sorgu.lastError().text());
        return;
    }
    QMessageBox::information(this, "Başarılı", "Girilen üye numarasına göre üye silme işlemi başarıyla gerçekleştirilmiştir.");
    show_table();
}
*/
