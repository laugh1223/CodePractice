#-------------------------------------------------
#
# Project created by QtCreator
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rtspPlayer
TEMPLATE = app


SOURCES += main.cpp \
    audioplayer.cpp \
    videoplayer.cpp \
    mainwindow.cpp

HEADERS  += \
    audioplayer.h \
    videoplayer.h \
    mainwindow.h

FORMS    += \
    mainwindow.ui

INCLUDEPATH+=$$PWD/ffmpeg/include

LIBS += $$PWD/ffmpeg/lib/x64/avcodec.lib \
        $$PWD/ffmpeg/lib/x64/avdevice.lib \
        $$PWD/ffmpeg/lib/x64/avfilter.lib \
        $$PWD/ffmpeg/lib/x64/avformat.lib \
        $$PWD/ffmpeg/lib/x64/avutil.lib \
        $$PWD/ffmpeg/lib/x64/postproc.lib \
        $$PWD/ffmpeg/lib/x64/swresample.lib \
        $$PWD/ffmpeg/lib/x64/swscale.lib
