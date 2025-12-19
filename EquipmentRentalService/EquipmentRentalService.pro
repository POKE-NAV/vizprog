QT       += core gui
QT += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addeditdialog.cpp \
    adminwindow.cpp \
    database.cpp \
    equipment.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    rental.cpp \
    rentaldialog.cpp \
    reportmanager.cpp \
    user.cpp \
    userwindow.cpp

HEADERS += \
    addeditdialog.h \
    adminwindow.h \
    database.h \
    equipment.h \
    logindialog.h \
    mainwindow.h \
    rental.h \
    rentaldialog.h \
    reportmanager.h \
    user.h \
    userwindow.h

FORMS += \
    addeditdialog.ui \
    logindialog.ui \
    mainwindow.ui \
    rentaldialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
