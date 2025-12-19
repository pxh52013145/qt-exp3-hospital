QT       += core gui widgets sql

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    db/dbmanager.cpp \
    db/historylogger.cpp \
    delegates/patientdelegate.cpp \
    main.cpp \
    mainwindow.cpp \
    models/departmentmodel.cpp \
    models/doctormodel.cpp \
    models/patientmodel.cpp \
    ui/departmenteditdialog.cpp \
    ui/departmentpage.cpp \
    ui/doctoreditdialog.cpp \
    ui/doctorpage.cpp \
    ui/historypage.cpp \
    ui/homepage.cpp \
    ui/loginpage.cpp \
    ui/patienteditdialog.cpp \
    ui/patientpage.cpp

HEADERS += \
    appinfo.h \
    db/dbmanager.h \
    db/historylogger.h \
    entities/patient.h \
    entities/userinfo.h \
    mainwindow.h \
    models/departmentmodel.h \
    models/doctormodel.h \
    models/patientmodel.h \
    delegates/patientdelegate.h \
    ui/departmenteditdialog.h \
    ui/departmentpage.h \
    ui/doctoreditdialog.h \
    ui/doctorpage.h \
    ui/historypage.h \
    ui/homepage.h \
    ui/loginpage.h \
    ui/patienteditdialog.h \
    ui/patientpage.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
