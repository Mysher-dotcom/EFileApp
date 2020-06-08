#-------------------------------------------------
#
# Project created by QtCreator 2020-02-27T13:59:39
#
#-------------------------------------------------

QT       += core gui
QT+=printsupport
QT+=multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EFileApp
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.

DEFINES += QT_DEPRECATED_WARNINGS
DESTDIR =../../target_EFile_v2

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#使用DTK库，引用以下配置 vector 2020年02月27日
CONFIG += link_pkgconfig
PKGCONFIG += dtkwidget

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        scanmanagerwindow.cpp \
        sanehelper/sanehelper.cpp \
        scanwindow.cpp \
        helper/globalhelper.cpp \
        listview/picitemdelegate.cpp \
        setwindow.cpp \
        listview/deviceitemdelegate.cpp \
        thread/doscanthread.cpp \
        testctrlwindow.cpp \
        camerawindow.cpp \
        Jpeg.cpp \
        thread/getcamerainfothread.cpp \
        thread/getscannerinfothread.cpp \
        helper/classificationhelper.cpp \
        thread/uploadfilethread.cpp \
        thread/recognizethread.cpp \
        helper/webhelper.cpp \
    helper/deviceinfohelper.cpp \
    hpdfoperation.cpp

HEADERS += \
        mainwindow.h \
        scanmanagerwindow.h \
        sanehelper/sanehelper.h \
        scanwindow.h \
        helper/globalhelper.h \
        listview/listviewitemdata.h \
        listview/picitemdelegate.h \
        setwindow.h \
        listview/deviceitemdelegate.h \
        thread/doscanthread.h \
        testctrlwindow.h \
        camerawindow.h \
        Jpeg.h \
        thread/getcamerainfothread.h \
        thread/getscannerinfothread.h \
        helper/classificationhelper.h \
        thread/uploadfilethread.h \
        thread/recognizethread.h \
        helper/webhelper.h \
    helper/deviceinfohelper.h \
    hpdfoperation.h

FORMS += \
        mainwindow.ui \
        scanmanagerwindow.ui \
        scanwindow.ui \
        setwindow.ui \
        testctrlwindow.ui \
        camerawindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    sources.qrc

#-------------------------------------------------
#INCLUDEPATH 指向需要包含的头文件目录
#LIBS 指向需要包含的具体库文件
#-------------------------------------------------
#LIBS += /lib64/libsane.so.1 #中标麒麟+龙芯 sane路径
LIBS += /usr/lib/x86_64-linux-gnu/libsane.so.1 #x86+uos sane路径

#扫描仪
INCLUDEPATH += $$PWD/../../../6.7/CamScanSDK
#INCLUDEPATH += /usr/local/include/EFILEAPP_CamScanSDK
#INCLUDEPATH += ../../CamScanSDK

#INCLUDEPATH += /usr/local/include/EFILEAPP_CmImage
#LIBS += /usr/local/lib/libopencv_highgui.so \
#        /usr/local/lib/libopencv_core.so \
#        /usr/local/lib/libopencv_imgproc.so \
#        /usr/local/lib/libopencv_photo.so
LIBS += -L$$PWD/../../../6.7/target_EFile -lCamScanSDK #此目录下所有包含CamScanSDK命名的so文件
#LIBS += -L/usr/local/lib/EFILEAPP_CmImage -lCmImage
INCLUDEPATH += $$PWD/../../../6.7/target_EFile/include/jpeg
LIBS += -L$$PWD/../../../6.7/target_EFile/lib/jpeg  -ljpeg
INCLUDEPATH += $$PWD/../../../6.7/target_EFile/include
LIBS += -L$$PWD/../../../6.7/target_EFile/lib/libusb-1.0 -lusb-1.0
INCLUDEPATH += $$PWD/../../../6.7/target_EFile/include/zbar
#INCLUDEPATH += ../target_EFile/include/Zbar
LIBS += -L$$PWD/../../../6.7/target_EFile/lib -lzbar
INCLUDEPATH += $$PWD/../../../6.7/target_EFile/include/hdf
LIBS += -L$$PWD/../../../6.7/target_EFile/lib/hdf -lhpdf
#拍摄仪
#INCLUDEPATH += /usr/local/include/EFILEAPP_USB
#INCLUDEPATH += /usr/local/include/EFILEAPP_JPEG
INCLUDEPATH += $$PWD/../../../6.7/CmImage#INCLUDEPATH += /usr/local/include/EFILEAPP_CmImage#
#LIBS += -L/usr/local/lib/EFILEAPP_USB/libusb-1.0 -lusb-1.0
#LIBS += -L/usr/local/lib/EFILEAPP_JPEG  -ljpeg
LIBS += -L$$PWD/../../../6.7/target_EFile -lCmImage

INCLUDEPATH += $$PWD/../../../6.7/CamCaptureSDK
LIBS += -L$$$PWD/../../../6.7/target_EFile  -lCamCaptureSDK

#加载opencv
#INCLUDEPATH += /usr/local/include/EFILEAPP_opencv2411\
#               /usr/local/include/EFILEAPP_opencv2411/opencv \
#              /usr/local/include/EFILEAPP_opencv2411/opencv2
#LIBS += /usr/local/lib/EFILEAPP_opencv2411/libopencv_highgui.so \
#        /usr/local/lib/EFILEAPP_opencv2411/libopencv_core.so \
#       /usr/local/lib/EFILEAPP_opencv2411/libopencv_flann.so \
#       /usr/local/lib/EFILEAPP_opencv2411/libopencv_features2d.so \
#        /usr/local/lib/EFILEAPP_opencv2411/libopencv_calib3d.so \
#        /usr/local/lib/EFILEAPP_opencv2411/libopencv_imgproc.so \
#        /usr/local/lib/EFILEAPP_opencv2411/libopencv_photo.so\
#        /usr/local/lib/EFILEAPP_opencv2411/libfreetype.so
INCLUDEPATH += $$PWD/../../../6.7/target_EFile/include\
              $$PWD/../../../6.7/target_EFile/include/opencv \
              $$PWD/../../../6.7/target_EFile/include/opencv2

LIBS += -L$$PWD/../../../6.7/target_EFile/lib -lopencv_highgui \
       -L$$PWD/../../../6.7/target_EFile/lib -lopencv_core \
        -L$$PWD/../../../6.7/target_EFile/lib -lopencv_flann \
        -L$$PWD/../../../6.7/target_EFile/lib -lopencv_features2d \
        -L$$PWD/../../../6.7/target_EFile/lib -lopencv_calib3d \
       -L$$PWD/../../../6.7/target_EFile/lib -lopencv_imgproc \
       -L$$PWD/../../../6.7/target_EFile/lib -lopencv_photo

RC_FILE += logo.rc
