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

