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
DESTDIR = $$PWD/../EFileAppInstall/opt/scaner/bin

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
        #sanehelper/sanehelper.cpp \
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
    helper/deviceinfohelper.cpp\
    hpdfoperation.cpp

HEADERS += \
        mainwindow.h \
        scanmanagerwindow.h \
        #sanehelper/sanehelper.h \
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
    helper/deviceinfohelper.h\
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

#扫描仪


#INCLUDEPATH += $$PWD/../../target_EFile/include/zbar
#INCLUDEPATH += $$PWD/../../target_EFile/include


#LIBS += -L$$PWD/../../target_EFile/lib/libusb-1.0 -lusb-1.0
#LIBS += -L$$PWD/../../target_EFile/lib -lzbar
#LIBS += -L$$PWD/../../target_EFile/lib/hdf -lhpdf
#加载opencv
INCLUDEPATH += $$PWD/./amd64/include/opencv2411 \
               $$PWD/./amd64/include//opencv2411/opencv \
               $$PWD/./amd64/include//opencv2411/opencv2

LIBS += $$PWD/amd64/lib/opencv2411/libopencv_highgui.so \
        $$PWD/amd64/lib/opencv2411/libopencv_flann.so \
        $$PWD/amd64/lib/opencv2411/libopencv_features2d.so \
        $$PWD/amd64/lib/opencv2411/libopencv_core.so \
        $$PWD/amd64/lib/opencv2411/libopencv_calib3d.so \
        $$PWD/amd64/lib/opencv2411/libopencv_imgproc.so \
        $$PWD/amd64/lib/opencv2411/libopencv_photo.so
INCLUDEPATH += $$PWD/amd64/include/hpdf
LIBS += -L$$PWD/amd64/lib/hpdf -lhpdf

INCLUDEPATH += $$PWD/amd64/include/jpeg
LIBS += -L$$PWD/amd64/lib/jpeg  -ljpeg
#拍摄仪
INCLUDEPATH += $$PWD/amd64/include/CamScanSDK
LIBS += -L$$PWD/amd64/lib/CamScanSDK -lCamScanSDK #此目录下所有包含CamScanSDK命名的so文件
INCLUDEPATH += $$PWD/amd64/include/CmImage
LIBS += -L$$PWD/amd64/lib/CmImage -lCmImage
INCLUDEPATH += $$PWD/amd64/include/CamCaptureSDK
LIBS += -L$$PWD/amd64/lib/CamCaptureSDK -lCamCaptureSDK
RC_FILE += logo.rc
