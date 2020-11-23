#-------------------------------------------------
#
# Project created by QtCreator 2019-10-15T11:11:01
#
#-------------------------------------------------

#QT       -= core gui

TARGET = CmImage
TEMPLATE = lib

#去除qt依赖
#CONFIG -= qt

DEFINES += CMIMAGE_LIBRARY


SOURCES += cmimage.cpp \
    ImgProc/Adapter.cpp \
    ImgProc/AdaptiveThreshold.cpp \
    ImgProc/DetectRectByContours_new.cpp \
    ImgProc/ImageDecoder.cpp \
    ImgProc/MImageDataPool.cpp \
    ImgProc/Rotate.cpp \
    ImgProc/tools.cpp \
    ImgProc/BrightnessBalance.cpp

HEADERS += \
    cmimage.h \
    MImage.h \
    stdafx.h \
    ImgProc/Adapter.h \
    ImgProc/AdaptiveThreshold.h \
    ImgProc/DetectRectByContours_new.h \
    ImgProc/ImageDecoder.h \
    ImgProc/MImageDataPool.h \
    ImgProc/Rotate.h \
    ImgProc/tools.h \
    ImgProc/BrightnessBalance.h
unix {
    #环境变量
   target.path = /usr/lib
   INSTALLS += target

    #龙芯中标麒麟
    DEFINES += LINUX_loongson

    #龙芯中标麒麟
    contains(DEFINES, LINUX_loongson)
    {

DESTDIR = /opt/apps/org.deepin.scanner/files/scanner/bin
#加载opencv
INCLUDEPATH += /opt/apps/org.deepin.scanner/files/scanner/bin/include \
               /opt/apps/org.deepin.scanner/files/scanner/bin/include/opencv \
               /opt/apps/org.deepin.scanner/files/scanner/bin/include/opencv2

LIBS += /opt/apps/org.deepin.scanner/files/scanner/bin/lib/libopencv_highgui.so \
        /opt/apps/org.deepin.scanner/files/scanner/bin/lib/libopencv_core.so \
        /opt/apps/org.deepin.scanner/files/scanner/bin/lib/libopencv_flann.so \
        /opt/apps/org.deepin.scanner/files/scanner/bin/lib/libopencv_features2d.so \
        /opt/apps/org.deepin.scanner/files/scanner/bin/lib/libopencv_calib3d.so \
        /opt/apps/org.deepin.scanner/files/scanner/bin/lib/libopencv_imgproc.so \
        /opt/apps/org.deepin.scanner/files/scanner/bin/lib/libopencv_photo.so

        #编译后处理
        TEST_CMD_LINE = echo "Hello World"
        QMAKE_POST_LINK += $$quote($$TEST_CMD_LINE)

        TEST_CMD_LINE1 = sh copy.sh
        QMAKE_POST_LINK += $$quote($$TEST_CMD_LINE1)


    }


}
