#-------------------------------------------------
#
# Project created by QtCreator 2020-03-06T14:15:40
#
#-------------------------------------------------

QT       -= gui

TARGET = CamScanSDK
TEMPLATE = lib
QMAKE_CXXFLAGS += -std=c++11
DEFINES += CAMSCANSDK_LIBRARY
VERSION = 1.0.1
# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
#DESTDIR =$$PWD/../target_EFile
DESTDIR = /opt/apps/org.deepin.scanner/files/scanner/bin

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        camscansdk.cpp \
    csanesddk.cpp

HEADERS += \
        camscansdk.h \
        camscansdk_global.h \ 
    datatype.h \
   # errortype.h \
    csanesddk.h \
    errortypescanner.h

unix {
    target.path = /usr/lib
    INSTALLS += target_sane
}
#加载sane
#LIBS += /usr/lib/aarch64-linux-gnu/libsane.so.1
#LIBS += /usr/lib/x86_64-linux-gnu/libsane-kanas.so
DISTFILES += \
    readme.txt
