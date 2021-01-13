#ifndef CAMCAPTURESDK_H
#define CAMCAPTURESDK_H
#include "datatype.h"
#include "camcapturesdk_global.h"
#include "Jpeg.h"
#include <QThread>

extern "C"{
    /*
    **dec：扫描仪初始化
    **Return：0表示成功，其他返回值详见错误码
    */
    long Cam_Init();
    /*
    ** dec：获取扫描仪设备个数
    ** param：nDevCount-设备总数（out）
    ** reurn：0表示成功，其他返回值详见错误码
    */
    long Cam_GetDevCount(long& nDevCount);
    /*
    ** dec：获取扫描仪设备名称
    ** param：nDevIndex-设备号（in），szDevKanas-设备kanas（out），szDevVendor-设备vendor（out），szDevModel-szDevType-设备Dev（out），
    ** reurn：0表示成功，其他返回值详见错误码
    */
    long Cam_GetDevName(long nDevIndex,char* szDevName);
    long Cam_GetCameraFPS(long nDevIndex,long &nCameraFPS);
    long Cam_SetCameraFPS(long nDevIndex,int nFrameRate);
    long Cam_GetCameraResolutionCount(long nDevIndex,long nFormatType,long &nResolutionCount);
    long Cam_GetCameraResolutionName(long nResolutionIndex,long &nWidth,long &nHeight);
    long Cam_SetCameraResolution(long nDevIndex,long nWidth,long nHeight);
    long Cam_GetCameraFormatCount(long nDevIndex,long &nFormatCount);
    long Cam_GetCameraFormatName(long nFormatIndex,char *szFormat);
    long Cam_SetCameraFormat(long nDevIndex,long nFormatType);
    long Cam_GetPIDVID(long nDevIndex,int &nPID,int &nVID);
    long Cam_OpenDev(long nDevIndex);
    long Cam_CameraCaptureStart(long nDevIndex,CameraCpatureReciveCallback CameraRecvFun);
    long Cam_CameraCaptureStop();
    long Cam_CameraCaptureFile(long nDevIndex,char* szFile,ImageParam imgParam);
    //自动裁切
    long Cam_SetAutoCrop(int nIsCrop);
    //获取设备权限
    long Cam_GetDevLicense(long nDevIndex,int& bIsLicense);
    //解码
    long Cam_readBufFromJpegBuf(uchar * jpgBuf,long lsize,long &lWidth,long &lHeight,uchar * * bgrBuff);
    long Cam_readBufFromJpeg(char * filepath,uchar * * bgrBuff,JPEGInfo &jpgInfo,int desW,int desH);
    long Cam_saveImageFormJpeg(uchar * src,int width,int height,int bpp,char * filePath,JPEGInfo jpgInfo);
    long Cam_saveImageFormJpegBuf(uchar * src,int width,int height,int bpp,uchar ** filePathBuf,JPEGInfo jpgInfo);
    //条码识别
    long Cam_BarcodeRecognizeBuffer(char* data,int width,int height,int nChannel,barcodeInfoList & infos);
    //自动连拍 开启
    long Cam_AutoCamptureStart(long nDevIndex,callBackAutoCapture funAutoCapture);
    long Cam_AutoCamptureStop(long nDevIndex);
    //leftRotate
    long Cam_LeftRotate();
    //RightRotate
    long Cam_RightRotate();
    //long Cam_savePDF(QStringList listPath);
}

#endif // CAMCAPTURESDK_H
