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
    Model/MImageModel/Adapter.cpp \
    Model/MImageModel/MImageDataPool.cpp \
    Util/timeutil.cpp \
    ImgProc/ColorSpace/AdaptiveThreshold.cpp \
    ImgProc/Enhancement/AutoLevel.cpp \
    ImgProc/Enhancement/ColorEnhance.cpp \
    ImgProc/Enhancement/ColorSpace.cpp \
    ImgProc/Enhancement/Curves.cpp \
    ImgProc/Enhancement/DetectionWhitePage.cpp \
    ImgProc/Enhancement/DocumentBackgroundSeparation.cpp \
    ImgProc/Enhancement/FilterEffect.cpp \
    ImgProc/Enhancement/Histogram.cpp \
    ImgProc/Enhancement/HSL.cpp \
    ImgProc/Enhancement/ImageEnhancement.cpp \
    ImgProc/Enhancement/ImgAdjust.cpp \
    ImgProc/Enhancement/Noise.cpp \
    ImgProc/Enhancement/SharpImage.cpp \
    ImgProc/Feature/AdjustSkew.cpp \
    ImgProc/Feature/DetectEdge.cpp \
    ImgProc/Feature/DetectRectByContours.cpp \
    ImgProc/Feature/DetectRectBySegmation.cpp \
    ImgProc/Feature/FillBorder.cpp \
    ImgProc/Feature/PunchHold.cpp \
    ImgProc/Transform/CropRectByCV.cpp \
    ImgProc/Transform/MergerImage.cpp \
    ImgProc/Transform/Rotate.cpp \
    ImgProc/Enhancement/BrightnessBalance.cpp \
    ImgProc/Enhancement/Halftone.cpp \
    ImgProc/Enhancement/newColorEnhance.cpp \
    ImgProc/Enhancement/RemoveNoise.cpp \
    ImgProc/Feature/DetectRectByContours_new.cpp \
    ImgProc/WaterMark/CvText.cpp \
    ImgProc/Enhancement/DeleteNoise_BW.cpp \
    ImgProc/WaterMark/tools.cpp \
    ImgProc/Enhancement/ImageDecoder.cpp \
    ImgProc/Enhancement/Compare.cpp
HEADERS += \
    cmimage.h \
    MImage.h \
    Model/MImageModel/Adapter.h \
    Model/MImageModel/MImageDataPool.h \
    stdafx.h \
    Util/timeutil.h \
    ImgProc/ColorSpace/AdaptiveThreshold.h \
    ImgProc/Enhancement/AutoLevel.h \
    ImgProc/Enhancement/ColorEnhance.h \
    ImgProc/Enhancement/ColorSpace.hpp \
    ImgProc/Enhancement/Curves.hpp \
    ImgProc/Enhancement/DetectionWhitePage.h \
    ImgProc/Enhancement/DocumentBackgroundSeparation.h \
    ImgProc/Enhancement/FilterEffect.h \
    ImgProc/Enhancement/Histogram.h \
    ImgProc/Enhancement/HSL.hpp \
    ImgProc/Enhancement/ImageEnhancement.h \
    ImgProc/Enhancement/ImgAdjust.h \
    ImgProc/Enhancement/Noise.h \
    ImgProc/Enhancement/SharpImage.h \
    ImgProc/Feature/AdjustSkew.h \
    ImgProc/Feature/DetectEdge.h \
    ImgProc/Feature/DetectRectByContours.h \
    ImgProc/Feature/DetectRectBySegmation.h \
    ImgProc/Feature/FillBorder.h \
    ImgProc/Feature/PunchHold.h \
    ImgProc/Transform/CropRectByCV.h \
    ImgProc/Transform/MergerImage.h \
    ImgProc/Transform/Rotate.h\
    ImgProc/Enhancement/BrightnessBalance.h \
    ImgProc/Enhancement/Halftone.h \
    ImgProc/Enhancement/newColorEnhance.h \
    ImgProc/Enhancement/RemoveNoise.h \
    ImgProc/Feature/DetectRectByContours_new.h \
    ImgProc/WaterMark/CvText.h \
    ImgProc/Enhancement/DeleteNoise_BW.h \
    ImgProc/WaterMark/tools.h \
    ImgProc/Enhancement/ImageDecoder.h \
    ImgProc/Enhancement/Compare.h
unix {
    #环境变量
   target.path = /usr/lib
   INSTALLS += target

    #龙芯中标麒麟
    DEFINES += LINUX_loongson

    #龙芯中标麒麟
    contains(DEFINES, LINUX_loongson)
    {
#        #生成路径
#        DESTDIR = "$$PWD"/../../lib/cmimage/Linux/loongson/lib

#        #加载opencv
#        INCLUDEPATH += ../../lib/opencv310/include/opencv \
#                       ../../lib/opencv310/include/opencv2

#        LIBS += "$$PWD"/../../lib/opencv310/lib/Linux/loongson/libopencv_highgui.so \
#                "$$PWD"/../../lib/opencv310/lib/Linux/loongson/libopencv_core.so \
#                "$$PWD"/../../lib/opencv310/lib/Linux/loongson/libopencv_imgproc.so \
#                "$$PWD"/../../lib/opencv310/lib/Linux/loongson/libopencv_imgcodecs.so \
#                "$$PWD"/../../lib/opencv310/lib/Linux/loongson/libopencv_photo.so

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
