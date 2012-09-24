#-------------------------------------------------
#
# Project created by QtCreator 2012-03-18T12:31:12
#
#-------------------------------------------------

QT       += core gui

TARGET = RepoSync
TEMPLATE = app

RC_FILE = Resources.rc
RESOURCES += Resources.qrc

DEFINES += REPOSYNC

CONFIG (debug, debug|release) {
    DESTDIR = debug/
    OBJECTS_DIR = debug/gen
    MOC_DIR = debug/gen
    RCC_DIR = debug/gen
} else {
    DESTDIR = release/
    OBJECTS_DIR = release/gen
    MOC_DIR = release/gen
    RCC_DIR = release/gen
}

SOURCES +=  src/main.cpp\
            src/main/mainwindow.cpp \
            src/other/global.cpp \
            src/other/knownprocess.cpp

HEADERS  += src/main/mainwindow.h \
            src/other/global.h \
            src/other/knownprocess.h

FORMS    += src/main/mainwindow.ui

OTHER_FILES += \
            TODO.txt
