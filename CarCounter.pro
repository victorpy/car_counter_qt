#-------------------------------------------------
#
# Project created by QtCreator 2017-10-25T09:08:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CarCounter
TEMPLATE = app

INCLUDEPATH += /usr/local/include/opencv2
LIBS += `pkg-config opencv --libs --cflags`


SOURCES += main.cpp\
        mainwindow.cpp \
    utils.cpp \
    car.cpp

HEADERS  += mainwindow.h \
    utils.h \
    car.hpp

FORMS    += mainwindow.ui
