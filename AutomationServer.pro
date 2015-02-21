#-------------------------------------------------
#
# Project created by QtCreator 2015-02-03T01:45:17
#
#-------------------------------------------------

QT       += core
QT       -= gui
QT       += network
TARGET = AutomationServer
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += c++11

TEMPLATE = app

SOURCES += main.cpp \
    server.cpp \
    device.cpp \
    manager.cpp \
    databaseitem.cpp \
    tellstick.cpp \

HEADERS += \
    server.h \
    manager.h \
    singleton.h \
    device.h \
    databaseitem.h \
    common.h \
    tellstick.h

LIBS    += "/usr/local/lib/libtelldus-core.so"
