#-------------------------------------------------
#
# Project created by QtCreator 2011-03-17T18:09:16
#
#-------------------------------------------------

QT       += core gui

TARGET = QtTestApp
TEMPLATE = app

    QMAKE_CXXFLAGS += -D_CRT_SECURE_NO_WARNINGS
    QMAKE_CXXFLAGS += -D_UNICODE
    QMAKE_CXXFLAGS += -DUNICODE

SOURCES += \
    ../test/QtTestApp/QtTse3Play.cpp \
    ../test/QtTestApp/main.cpp \
    ../test/QtTestApp/qttse3playvisual.cpp \
    ../src/QtVuMeter/qvumeter.cpp \
    ../src/QtVuMeter/midioutcombobox.cpp \
    ../src/QtVuMeter/midiincombobox.cpp

HEADERS  += \
    ../test/QtTestApp/QtTse3Play.h \
    ../src/QtVuMeter/qvumeter.h \
    ../test/QtTestApp/qttse3playvisual.h \
    ../src/QtVuMeter/qvumeter.h \
    ../src/QtVuMeter/midioutcombobox.h \
    ../src/QtVuMeter/midiincombobox.h

FORMS    += \
    ../test/QtTestApp/qttse3playvisual.ui


INCLUDEPATH +=  ../src   \
                ../src/3rdparty/tse3/src    \
                ../include  \
                ../src/QtVuMeter

DESTDIR = ../bin

CONFIG(debug){
    LIBS += ../src/3rdparty/tse3/lib/tse3_x64du.lib
}else{
    LIBS += ../src/3rdparty/tse3/lib/tse3_x64u.lib
}
