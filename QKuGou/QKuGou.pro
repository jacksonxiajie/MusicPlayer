#-------------------------------------------------
#
# Project created by QtCreator 2019-04-02T16:03:56
#
#-------------------------------------------------

QT       += core gui
QT        += multimedia
QT        += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QKuGou
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
        main.cpp \
        mainwindow.cpp \
    downloadform.cpp \
    downinfo.cpp \
    mnetworker.cpp \
    deskiyrics.cpp \
    songinfo.cpp \
    lrclabel.cpp \
    mcentralwidget.cpp \
    rightwidget.cpp \
    favorwidget.cpp \
    nativewidget.cpp \
    MyCheckBox.cpp \
    playmodeform.cpp \
    msystemtray.cpp

HEADERS += \
        mainwindow.h \
    downloadform.h \
    downinfo.h \
    mnetworker.h \
    deskiyrics.h \
    songinfo.h \
    lrclabel.h \
    mcentralwidget.h \
    scheckboxheaderview.h \
    rightwidget.h \
    customfunction.h \
    favorwidget.h \
    nativewidget.h \
    MyCheckBox.h \
    playmodeform.h \
    msystemtray.h

FORMS += \
        mainwindow.ui \
    downloadform.ui \
    mcentralwidget.ui \
    rightwidget.ui \
    favorwidget.ui \
    nativewidget.ui \
    playmodeform.ui \
    lrcwidget.ui

RESOURCES += \
    image.qrc \
    widgetstyle.qrc
      rc.qrc

DISTFILES +=
