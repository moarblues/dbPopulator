#-------------------------------------------------
#
# Project created by QtCreator 2014-10-24T22:18:53
#
#-------------------------------------------------

QT       += core network sql

QT       -= gui

TARGET = dbPopulator
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

DESTDIR = bin


SOURCES += main.cpp \
    populator.cpp \
    database.cpp \
    files.cpp

unix: LIBS += -Llib/freexl/ -lfreexl

INCLUDEPATH += lib/incl/freexl
DEPENDPATH += lib/incl/freexl

unix: PRE_TARGETDEPS += lib/freexl/libfreexl.a

include(qtxlsx/qtxlsx.pri)

HEADERS += \
    populator.h \
    database.h \
    files.h
