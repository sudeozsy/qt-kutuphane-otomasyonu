QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    kitap_islemleri_ekran.cpp \
    main.cpp \
    mainwindow.cpp \
    odunc_alma_islemleri_ekran.cpp \
    odunc_teslim_etme_islemleri_ekran.cpp \
    tum_2274.cpp \
    uye_islemleri_ekran.cpp

HEADERS += \
    kitap_islemleri_ekran.h \
    mainwindow.h \
    odunc_alma_islemleri_ekran.h \
    odunc_teslim_etme_islemleri_ekran.h \
    uye_islemleri_ekran.h

FORMS += \
    kitap_islemleri_ekran.ui \
    mainwindow.ui \
    odunc_alma_islemleri_ekran.ui \
    odunc_teslim_etme_islemleri_ekran.ui \
    uye_islemleri_ekran.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
