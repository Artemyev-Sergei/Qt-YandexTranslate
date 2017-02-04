#-------------------------------------------------
#
# Project created by QtCreator 2017-01-30T00:36:13
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Qt-YandexTranslate
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    dictionary_api.cpp \
    translate_api.cpp

HEADERS  += \
    mainwindow.h \
    dictionary_api.h \
    translate_api.h

FORMS    += \
    mainwindow.ui
