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

TEMPLATE = app

SOURCES += main.cpp \
    tellduscoreapi.cpp \
    server.cpp \
    device.cpp \
    manager.cpp \
    sensor.cpp

HEADERS += \
    tellduscoreapi.h \
    server.h \
    manager.h \
    singleton.h \
    sensor.h \
    device.h

LIBS    += "/usr/local/lib/libtelldus-core.so"
