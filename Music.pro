#-------------------------------------------------
#
# Project created by QtCreator 2014-07-06T17:30:09
#
#-------------------------------------------------


QT  += core gui multimedia

QT  += widgets

QMAKE_CFLAGS += -std=c++11
QMAKE_CXXFLAGS += -std=c++11

QMAKE_LFLAGS += -Wl,-rpath,.
TARGET = Music
TEMPLATE = app


SOURCES += main.cpp\
        music.cpp \
    csqlite3.cpp

HEADERS  += music.h \
    csqlite3.h

FORMS    += music.ui

RESOURCES +=

RC_FILE = music.rc


unix: LIBS += -L$$PWD -lcppsqlite3

OTHER_FILES += \
    music.rc \
    icon.ico
