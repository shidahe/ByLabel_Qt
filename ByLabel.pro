#-------------------------------------------------
#
# Project created by QtCreator 2018-05-16T15:20:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ByLabel
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    labelwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    mat_qimage.cpp \
    labelimage.cpp

HEADERS += \
    labelwidget.h \
    mainwindow.h \
    mat_qimage.h \
    labelimage.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += /usr/local/include/opencv -I/usr/local/include
LIBS += `pkg-config opencv --libs`

SUBDIRS += \
    ByLabel.pro

RESOURCES += \
    icons/icons.qrc

DISTFILES +=