#-------------------------------------------------
#
# Project created by QtCreator 2020-03-13T10:13:14
#
#-------------------------------------------------

QT       -= gui

TARGET = CamCaptureSDK
TEMPLATE = lib
QMAKE_CXXFLAGS += -std=c++11
DEFINES += CAMCAPTURESDK_LIBRARY
VERSION = 1.0.6
# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
#DESTDIR =../维山闪拍
DESTDIR =/opt/apps/org.deepin.scanner/files/scanner/bin
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#INCLUDEPATH += /usr/local/include/维山闪拍
#LIBS += -L/usr/local/lib/维山闪拍/libusb-1.0 -lusb-1.0
INCLUDEPATH += /opt/apps/org.deepin.scanner/files/scanner/bin/include/jpeg
INCLUDEPATH += /opt/apps/org.deepin.scanner/files/scanner/bin/include/hdf
INCLUDEPATH += /opt/apps/org.deepin.scanner/files/scanner/bin/include/png
LIBS += -L/opt/apps/org.deepin.scanner/files/scanner/bin/lib/jpeg  -ljpeg
LIBS += -L/opt/apps/org.deepin.scanner/files/scanner/bin/lib/hdf -lhpdf
LIBS += -L/opt/apps/org.deepin.scanner/files/scanner/bin/lib/png -lpng
INCLUDEPATH += /opt/apps/org.deepin.scanner/files/scanner/bin/include/libusb-1.0
LIBS += -L/opt/apps/org.deepin.scanner/files/scanner/bin/lib/libusb-1.0 -lusb-1.0
INCLUDEPATH += /opt/apps/org.deepin.scanner/files/scanner/bin/include/zbar
LIBS += -L/opt/apps/org.deepin.scanner/files/scanner/bin/lib -lzbar
INCLUDEPATH +=/opt/apps/org.deepin.scanner/files/scanner/bin/include/CmImage
LIBS += /opt/apps/org.deepin.scanner/files/scanner/bin/libCmImage.so.1


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

SOURCES += \
        camcapturesdk.cpp \
    ccapmpturev4l.cpp \
    ccampturedecode.cpp \
    ccampturerender.cpp \
    Jpeg.cpp \
    cimagehelper.cpp \
    cstartdraw.cpp \
    autocapturethread.cpp
   # cpng.cpp
    #hpdfoperation.cpp
#
HEADERS += \
        camcapturesdk.h \
        camcapturesdk_global.h \ 
    ccapmpturev4l.h \
    ccampturedecode.h \
    ccampturerender.h \
    errortype.h \
    datatype.h \
    Jpeg.h \
    cimagehelper.h \
    cstartdraw.h \
    autocapturethread.h
    #cpng.h
   # hpdfoperation.h


unix {
    target.path = /usr/lib
    INSTALLS += target
}

#RESOURCES += \
#    ../target_Campture/img/res.qrc

DISTFILES += \
    readme.txt

