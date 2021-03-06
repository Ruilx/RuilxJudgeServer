#-------------------------------------------------
#
# Project created by QtCreator 2016-05-04T15:34:32
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QMAKE_CXXFLAGS += --std=c++11

TARGET = RuilxJudgeServer
TEMPLATE = app


SOURCES += main.cpp\
        mainw.cpp \
    core/dockerdaemon.cpp \
    utils/logsystem.cpp \
    core/network.cpp \
    core/helper/handle.cpp \
    core/dockerrest.cpp \
    core/compileoutput.cpp \
    core/dockerrun.cpp \
    core/output.cpp

HEADERS  += mainw.h \
    core/dockerdaemon.h \
    utils/logsystem.h \
    core/network.h \
    core/helper/handle.h \
    core/dockerrest.h \
    core/compileoutput.h \
    core/dockerrun.h \
    core/output.h
