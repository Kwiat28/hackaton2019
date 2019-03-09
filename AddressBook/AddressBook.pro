#-------------------------------------------------
#
# Project created by QtCreator 2019-03-06T14:47:28
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = AddressBook
TEMPLATE = app

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += main.cpp\
        mainwindow.cpp \
    addressbookentry.cpp \
    addressbook.cpp \
    addressbookcontroller.cpp

HEADERS  += mainwindow.h \
    addressbookentry.h \
    addressbook.h \
    addressbookcontroller.h

FORMS    += mainwindow.ui
