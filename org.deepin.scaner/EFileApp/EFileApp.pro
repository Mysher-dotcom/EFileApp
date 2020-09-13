#-------------------------------------------------
#
# Project created by QtCreator 2020-02-27T13:59:39
#
#-------------------------------------------------

QT       += core gui
QT+=printsupport
QT+=multimedia
QT+=dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EFileApp
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.

DEFINES += QT_DEPRECATED_WARNINGS
DESTDIR =/opt/apps/org.deepin.scaner/files/scaner/bin #$$PWD/../../target_EFile

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
        helper/deviceinfohelper.cpp\
        hpdfoperation.cpp \
        thread/mergepdfthread.cpp \
        progressbarwindow.cpp \
        drawinterface/drawinterface.cpp

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
        helper/deviceinfohelper.h\
        hpdfoperation.h \
        thread/mergepdfthread.h \
        progressbarwindow.h \
        drawinterface/drawinterface.h

FORMS += \
        mainwindow.ui \
        scanmanagerwindow.ui \
        scanwindow.ui \
        setwindow.ui \
        testctrlwindow.ui \
        camerawindow.ui \
    progressbarwindow.ui

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
#LIBS += /usr/lib/x86_64-linux-gnu/libsane.so.1 #x86+uos sane路径
#LIBS += /usr/lib/aarch64-linux-gnu/libsane.so.1 #arm+uos sane路径
LIBS += /opt/apps/org.deepin.scaner/files/scaner/bin/lib/sane/libsane.so.1

#扫描仪

INCLUDEPATH += /opt/apps/org.deepin.scaner/files/scaner/bin/include/jpeg
INCLUDEPATH += /opt/apps/org.deepin.scaner/files/scaner/bin/include/hdf
LIBS += -L/opt/apps/org.deepin.scaner/files/scaner/bin/lib/jpeg  -ljpeg
LIBS += -L/opt/apps/org.deepin.scaner/files/scaner/bin/lib/hdf -lhpdf
#拍摄仪
INCLUDEPATH += /opt/apps/org.deepin.scaner/files/scaner/bin/include/CamScanSDK
INCLUDEPATH += /opt/apps/org.deepin.scaner/files/scaner/bin/include/CmImage
INCLUDEPATH += /opt/apps/org.deepin.scaner/files/scaner/bin/include/CamCaptureSDK
LIBS += /opt/apps/org.deepin.scaner/files/scaner/bin/libCamScanSDK.so.1 #此目录下所有包含CamScanSDK命名的so文件
LIBS += /opt/apps/org.deepin.scaner/files/scaner/bin/libCmImage.so.1
LIBS += /opt/apps/org.deepin.scaner/files/scaner/bin/libCamCaptureSDK.so.1

#加载opencv
INCLUDEPATH += /opt/apps/org.deepin.scaner/files/scaner/bin/include \
               /opt/apps/org.deepin.scaner/files/scaner/bin/include/opencv \
               /opt/apps/org.deepin.scaner/files/scaner/bin/include/opencv2

LIBS += /opt/apps/org.deepin.scaner/files/scaner/bin/lib/libopencv_highgui.so \
        /opt/apps/org.deepin.scaner/files/scaner/bin/lib/libopencv_core.so \
        /opt/apps/org.deepin.scaner/files/scaner/bin/lib/libopencv_flann.so \
        /opt/apps/org.deepin.scaner/files/scaner/bin/lib/libopencv_features2d.so \
        /opt/apps/org.deepin.scaner/files/scaner/bin/lib/libopencv_calib3d.so \
        /opt/apps/org.deepin.scaner/files/scaner/bin/lib/libopencv_imgproc.so \
        /opt/apps/org.deepin.scaner/files/scaner/bin/lib/libopencv_photo.so



RC_FILE += logo.rc

#TRANSLATIONS += $$PWD/translations/en_us.ts
TRANSLATIONS = $$files($$PWD/translations/*.ts)

#CONFIG(release, debug|release) {
#    TRANSLATIONS = $$files($$PWD/translations/*.ts)
#    #遍历目录中的ts文件，调用lrelease将其生成为qm文件
#    for(tsfile, TRANSLATIONS) {
#        qmfile = $$replace(tsfile, .ts$, .qm)
#        system(lrelease $$tsfile -qm $$qmfile) | error("Failed to lrelease")
#    }
    #将qm文件添加到安装包
#    dtk_translations.path = /usr/share/$$TARGET/translations
#    dtk_translations.files = $$PWD/translations/*.qm
#    INSTALLS += dtk_translations
#}


