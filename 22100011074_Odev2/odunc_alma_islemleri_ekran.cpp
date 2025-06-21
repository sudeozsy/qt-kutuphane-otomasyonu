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

