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
        yandextranslate.cpp \
    dictionaryentry.cpp

HEADERS  += yandextranslate.h \
    dictionaryentry.h

FORMS    += yandextranslate.ui
