#-------------------------------------------------
#
# Project created by QtCreator 2011-03-17T18:09:16
#
#-------------------------------------------------

QT       += core gui

TARGET = SampleUV
TEMPLATE = app

INCLUDEPATH += ../../src/QtVuMeter/

SOURCES += main.cpp\
        mainwindow.cpp \
    ../../src/QtVuMeter/qvumeter.cpp

HEADERS  += mainwindow.h \
    ../../src/QtVuMeter/qvumeter.h

FORMS    += mainwindow.ui
