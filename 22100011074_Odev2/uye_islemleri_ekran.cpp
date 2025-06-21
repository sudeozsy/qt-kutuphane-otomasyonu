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


