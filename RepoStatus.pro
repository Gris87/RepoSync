#-------------------------------------------------
#
# Project created by QtCreator 2012-03-18T12:31:12
#
#-------------------------------------------------

QT       += core gui

TARGET = RepoStatus
TEMPLATE = app

RC_FILE = Resources.rc
RESOURCES += Resources.qrc

DEFINES += REPOSTATUS

SOURCES += src/main.cpp\
        src/main/mainwindow.cpp \
    src/other/global.cpp \
    src/other/knownprocess.cpp

HEADERS  += src/main/mainwindow.h \
    src/other/global.h \
    src/other/knownprocess.h

FORMS    += src/main/mainwindow.ui

OTHER_FILES += \
    TODO.txt
