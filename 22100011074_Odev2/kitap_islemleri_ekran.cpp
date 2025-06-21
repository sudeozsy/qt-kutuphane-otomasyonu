// Sude Özsoy - 22100011074
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
