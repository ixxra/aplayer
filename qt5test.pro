#-------------------------------------------------
#
# Project created by QtCreator 2014-02-07T00:24:20
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qt5test
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    filemanagerwidget.cpp \
    playlistmodel.cpp \
    playlistwidget.cpp \
    playerwidget.cpp \
    playercontrols.cpp

HEADERS  += mainwindow.h \
    filemanagerwidget.h \
    playlistmodel.h \
    playlistwidget.h \
    playerwidget.h \
    playercontrols.h

FORMS    += mainwindow.ui \
    filemanagerwidget.ui \
    playlistwidget.ui \
    playerwidget.ui
