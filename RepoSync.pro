#-------------------------------------------------
#
# Project created by QtCreator 2012-03-18T12:31:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RepoSync
TEMPLATE = app



# Warnings as errors - BEGIN
QMAKE_CFLAGS_WARN_ON   += -Werror
QMAKE_CXXFLAGS_WARN_ON += -Werror
# Warnings as errors - END

# Release optimization - BEGIN
QMAKE_CFLAGS_RELEASE -= -O1
QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_CFLAGS_RELEASE += -O3

QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
# Release optimization - END



CONFIG += \
            warn_on \
            c++11

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
