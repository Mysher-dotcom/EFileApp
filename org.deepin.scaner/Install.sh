#!/bin/bash
qmake ./EFileApp
make
make clean 
rm Makefile
cp ./EFileApp/EFileApp.sh ./EFileAppInstall/opt/scaner/bin
cp ./EFileApp/simhei.ttf ./EFileAppInstall/opt/scaner/bin
cp ./EFileApp/logo.svg ./EFileAppInstall/opt/scaner/bin
cp ./EFileApp/amd64/lib/opencv2411/libopencv_highgui.so.2.4 ./EFileAppInstall/opt/scaner/bin
cp ./EFileApp/amd64/lib/opencv2411/libopencv_calib3d.so.2.4 ./EFileAppInstall/opt/scaner/bin
cp ./EFileApp/amd64/lib/opencv2411/libopencv_core.so.2.4 ./EFileAppInstall/opt/scaner/bin 
cp ./EFileApp/amd64/lib/opencv2411/libopencv_features2d.so.2.4 ./EFileAppInstall/opt/scaner/bin
cp ./EFileApp/amd64/lib/opencv2411/libopencv_flann.so.2.4 ./EFileAppInstall/opt/scaner/bin
cp ./EFileApp/amd64/lib/opencv2411/libopencv_imgproc.so.2.4 ./EFileAppInstall/opt/scaner/bin
cp ./EFileApp/amd64/lib/opencv2411/libopencv_photo.so.2.4 ./EFileAppInstall/opt/scaner/bin
cp ./EFileApp/amd64/lib/CamScanSDK/libCamScanSDK.so.1 ./EFileAppInstall/opt/scaner/bin
cp ./EFileApp/amd64/lib/CmImage/libCmImage.so.1 ./EFileAppInstall/opt/scaner/bin
cp ./EFileApp/amd64/lib/CamCaptureSDK/libCamCaptureSDK.so.1 ./EFileAppInstall/opt/scaner/bin
cp ./EFileApp/amd64/lib/jpeg/libjpeg.so.9 ./EFileAppInstall/opt/scaner/bin
cp ./EFileApp/amd64/lib/hpdf/libhpdf-2.3.0.so ./EFileAppInstall/opt/scaner/bin
chmod 755 -R ./EFileAppInstall/DEBAIN/*
dpkg -b ./EFileAppInstall/. ./
