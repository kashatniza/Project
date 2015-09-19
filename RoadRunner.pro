#-------------------------------------------------
#
# Project created by QtCreator 2015-08-11T11:03:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RoadRunner
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    semaphore.cpp \
    car.cpp \
    scene.cpp \
    mapimage.cpp

HEADERS  += mainwindow.h \
    semaphore.h \
    car.h \
    scene.h \
    mapimage.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
