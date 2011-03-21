#-------------------------------------------------
#
# Project created by QtCreator 2011-03-17T18:09:16
#
#-------------------------------------------------

QT       += core gui

TARGET = QtTestApp
TEMPLATE = app


SOURCES += \
    ../test/QtTestApp/QtTse3Play.cpp \
    ../test/QtTestApp/main.cpp \
    ../src/QtVuMeter/qvumeter.cpp

HEADERS  += \
    ../test/QtTestApp/QtTse3Play.h \
    ../src/QtVuMeter/qvumeter.h

FORMS    +=
INCLUDEPATH +=  ../src   \
                ../src/3rdparty/tse3/src    \
                ../include

LIBS += ../src/3rdparty/tse3/lib/tse3.lib
