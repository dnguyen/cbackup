#-------------------------------------------------
#
# Project created by QtCreator 2013-03-07T00:25:10
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = ConsoleBackup
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    Backup.cpp \
    json.cpp \
    BackupSystem.cpp

HEADERS += \
    BackupPathsWatcher.h \
    Backup.h \
    json.h \
    BackupSystem.h
