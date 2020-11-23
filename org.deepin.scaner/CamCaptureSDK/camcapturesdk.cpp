#include "camcapturesdk.h"
#include "errortype.h"
#include "ccapmpturev4l.h"
#include "ccampturedecode.h"
#include "cimagehelper.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <asm/types.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

#include <errno.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <iostream>

#include <QObject>
#include "cstartdraw.h"
#include "autocapturethread.h"
#include "datatype.h"
#include "cmimage.h"
#include "zbar.h"
using namespace zbar;
typedef struct CaptureControlLock{

    pthread_attr_t attr;
    pthread_mutex_t * Capturelock;
}CC_Lock;

CC_Lock *CameraCapture_lock = NULL;
bool isInitCaptureLock = false;
static int gThreadCaptureID;
long g_nCurrentDev = 0;
CCamptureDecode g_CamptureDecode;
CCapmptureV4L g_CamptureV4L;
CImageHelper g_ImageHelper;
CStartDraw *g_startDraw;
bool gbStopAutoCapture =true;
//QThread m_m_receiveVideoThread;
//unsigned char*  gPictureSaveBuf = NULL;
CameraCpatureReciveCallback gCameraRecvFun;
long gDecodeWidth;
long gDecodeHeight;
long gDecodeSize;
int gnIsCrop = 0;
int gbLeftRotate = 0;
int gbRightRotate = 0;
bool gbRotate = false;
int gnRotateA = 0;
int InitCamraCaptureLock()
{
    CameraCapture_lock = (CC_Lock*)malloc(sizeof(CC_Lock));
    if(NULL == CameraCapture_lock)
    {
        return -1;
    }
    if(pthread_attr_init(&(CameraCapture_lock->attr))!= 0)
    {
        free(CameraCapture_lock);
        CameraCapture_lock = NULL;
        return -2;
    }
    if(pthread_attr_setdetachstate(&(CameraCapture_lock->attr),PTHREAD_CREATE_DETACHED) != 0)
    {
        free(CameraCapture_lock);
        CameraCapture_lock = NULL;
        return -3;
    }

    CameraCapture_lock->Capturelock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    if(CameraCapture_lock ->Capturelock == NULL)
    {
        free(CameraCapture_lock);
        CameraCapture_lock = NULL;
        return -4;
    }

    pthread_mutex_init(CameraCapture_lock->Capturelock, NULL);

    isInitCaptureLock = true;

    return 0;
}

int CloseCamraCaptureLock()
{
    if(NULL != CameraCapture_lock->Capturelock)
    {
        free(CameraCapture_lock->Capturelock);
        CameraCapture_lock->Capturelock = NULL;
    }

    if(NULL != CameraCapture_lock)
    {
        free(CameraCapture_lock);
        CameraCapture_lock = NULL;
    }

    isInitCaptureLock = false;
    return 0;
}

int OpenCamraCaptureLock()
{

    if(CameraCapture_lock != NULL)
    {
        return 1;
    }

    if(InitCamraCaptureLock() < 0)
    {
        return -1;
    }

    return 0;
}

/*
**dec：扫描仪初始化
**Return：0表示成功，其他返回值详见错误码
*/
long Cam_Init()
{
     g_startDraw = new CStartDraw();
    return ERROR_GOOD;
}
/*
** dec：获取扫描仪设备个数
** param：nDevCount-设备总数（out）
** reurn：0表示成功，其他返回值详见错误码
*/
long Cam_GetDevCount(long& nDevCount){
    return g_CamptureV4L.GetCameraDevCount(nDevCount);;
}
/*
** dec：获取扫描仪设备名称
** param：nDevIndex-设备号（in），szDevKanas-设备kanas（out），szDevVendor-设备vendor（out），szDevModel-szDevType-设备Dev（out），
** reurn：0表示成功，其他返回值详见错误码
*/
long Cam_GetDevName(long nDevIndex,char* szDevName){
    //先获取结点名称在获取设备名称
    char szDevNodeName[256] = {0};
    long nFPS =0;
    g_CamptureV4L.GetCameraDevNodeName(nDevIndex,szDevNodeName);
    g_CamptureV4L.GetCameraDevName(szDevNodeName,szDevName);
    g_CamptureV4L.GetCameraFPS(szDevNodeName,nFPS);
    return ERROR_GOOD;
}
//bIsLicense --0,普通设备，不是2e5a的设备;1--授权设备;2---设备没有usb权限
long Cam_GetDevLicense(long nDevIndex,int& bIsLicense)
{
     return g_CamptureV4L.Cam_GetDevLicense(nDevIndex,bIsLicense);
}
long Cam_GetPIDVID(long nDevIndex,int &nPID,int &nVID)
{
    char szDevNodeName[256] = {0};
    char szDevName[256]={0};
    g_CamptureV4L.GetCameraDevNodeName(nDevIndex,szDevNodeName);
    g_CamptureV4L.GetCameraDevName(szDevNodeName,szDevName);
    return  g_CamptureV4L.GetCameraPIDVID(szDevName,nPID,nVID);
}
/*
** dec：打开设备
** param：nDevIndex-设备号（in）
** reurn：0表示成功，其他返回值详见错误码
*/
long Cam_OpenDev(long nDevIndex){

}
int ReceiveData(void* data,int size,int w,int H,int nFormatType)
{
    if(nFormatType==1)
    {
        gCameraRecvFun(data,size,w,H,nFormatType);
        return 0;
    }

    unsigned char* dstBuf = NULL;
    long lWidth=640,lHeight=480;
    long nRet = g_CamptureDecode.readBufFromJpegBuf((uchar*)data,size,lWidth,lHeight,&dstBuf);

    if(nRet ==0)
    {
         if(g_CamptureV4L.vecLicense.at(g_nCurrentDev))
         {
             if(gnIsCrop!=0)
             {
                  long nsize = lWidth*lHeight*3;
                  int nWidth = lWidth;
                  int nHeight = lHeight;
                 // emit  g_startDraw->SendVideoBuffer(dstBuf,nsize,lWidth,lHeight);
                  g_ImageHelper.DrawLine((char*)dstBuf,size,nWidth,nHeight,gnIsCrop);
                  //uchar *dstLBuf = new unsigned char[size];
                  //memset(dstLBuf,0,size);
                  // unsigned char* dstLBuf = NULL;
                  if(gbRotate)
                  {
                      g_ImageHelper.Rotate(dstBuf,size,nWidth,nHeight,gnRotateA);
                      //g_ImageHelper.SaveImage(dstBuf,size,nWidth,nHeight);
                       gCameraRecvFun((char*)dstBuf,size,nWidth,nHeight,0);

                  }
                  else {
                      //dstLBuf = dstBuf;
                       gCameraRecvFun((char*)dstBuf,size,nWidth,nHeight,0);
                  }
                   //gCameraRecvFun((char*)dstLBuf,size,nWidth,nHeight,0);
                  delete dstBuf;
                  dstBuf = NULL;
              }
              else
              {
                  gCameraRecvFun(dstBuf,size,lWidth,lHeight,nFormatType);
                  delete dstBuf;
                  dstBuf = NULL;

              }
         }
         else {
             gCameraRecvFun(dstBuf,size,lWidth,lHeight,nFormatType);
             delete dstBuf;
             dstBuf = NULL;
         }


    }

    return 0;
}
long Cam_CameraCaptureStart(long nDevIndex,CameraCpatureReciveCallback CameraRecvFun)
{
    g_CamptureV4L.CameraCaptureStop();
    if(g_CamptureV4L.captureRuning){
        return ERROR_BUSY;
    }
     if(!isInitCaptureLock){ //打开锁
         OpenCamraCaptureLock();
         char szDevNodeName[256] = {0};
         gCameraRecvFun = CameraRecvFun;
         g_nCurrentDev = nDevIndex;
         g_CamptureV4L.GetCameraDevNodeName(nDevIndex,szDevNodeName);
         long nRet = g_CamptureV4L.CameraCaptureStart(szDevNodeName,ReceiveData);
         return nRet;
     }
     else {
         return ERROR_MALLOC_FAILED;
     }

}
long Cam_GetCameraFPS(long nDevIndex,long &nCameraFPS){
    char szDevNodeName[256] = {0};
    g_CamptureV4L.GetCameraDevNodeName(nDevIndex,szDevNodeName);
    g_CamptureV4L.GetCameraFPS(szDevNodeName,nCameraFPS);
    return ERROR_GOOD;
}
long Cam_SetCameraFPS(long nDevIndex,int nFrameRate){
    char szDevNodeName[256] = {0};
    g_CamptureV4L.GetCameraDevNodeName(nDevIndex,szDevNodeName);
    g_CamptureV4L.SetCameraFPS(szDevNodeName,nFrameRate);
    return ERROR_GOOD;
}
long Cam_GetCameraResolutionCount(long nDevIndex,long nFormatType,long &nResolutionCount){
    char szDevNodeName[256] = {0};
    g_CamptureV4L.GetCameraDevNodeName(nDevIndex,szDevNodeName);
    g_CamptureV4L.GetCameraResolutionCount(szDevNodeName,nFormatType,nResolutionCount);
    return ERROR_GOOD;
}
long Cam_GetCameraResolutionName(long nResolutionIndex,long &nWidth,long &nHeight){
    return g_CamptureV4L.GetCameraResolutionName(nResolutionIndex,nWidth,nHeight);
}
long Cam_GetCameraFormatCount(long nDevIndex,long &nFormatCount)
{
    char szDevNodeName[256] = {0};
    g_CamptureV4L.GetCameraDevNodeName(nDevIndex,szDevNodeName);
    return g_CamptureV4L.GetCameraFormatCount(szDevNodeName,nFormatCount);
}
long Cam_GetCameraFormatName(long nFormatIndex,char *szFormat)
{
    return g_CamptureV4L.GetCameraFormatName(nFormatIndex,szFormat);
}
long Cam_SetCameraResolution(long nDevIndex,long nWidth,long nHeight)
{
    return g_CamptureV4L.SetCameraResolution(nWidth,nHeight);
}
long Cam_SetCameraFormat(long nDevIndex,long nFormatType)
{
    return g_CamptureV4L.SetCameraFormat(nFormatType);
}
long Cam_CameraCaptureFile(long nDevIndex,char* szFile,ImageParam imgParam)
{

    imgParam.nHeight = g_CamptureV4L.camera->height;
    imgParam.nWidth = g_CamptureV4L.camera->width;
    if(isInitCaptureLock){
        pthread_mutex_lock(CameraCapture_lock->Capturelock);
    }
    imgParam.buffer = (unsigned char*)g_CamptureV4L.Imagebuffer;
    imgParam.nSize = g_CamptureV4L.nBufferSize;
    if(isInitCaptureLock)
    {
        pthread_mutex_unlock(CameraCapture_lock->Capturelock);
    }
    int bIsLicense = 0;
    Cam_GetDevLicense(nDevIndex,bIsLicense);
    g_ImageHelper.InitImageSDK();
    if(bIsLicense!=1)
    {
         imgParam.bNoise = false;
         imgParam.bRedEn = false;
         imgParam.bInverse = false;
         imgParam.bIsWater = false;
         imgParam.bIsTextEn = false;
         imgParam.bFillBorder = false;
         imgParam.bColorBlance = false;
         imgParam.bColorDropOut = false;
         imgParam.bAutoCampture = false;
         imgParam.bIsWaterImage = false;
         imgParam.bRemoveFinger = false;
    }

    g_ImageHelper.ProcessImage(imgParam,szFile);

    return ERROR_GOOD;

}
long Cam_CameraCaptureStop()
{
    if(isInitCaptureLock)
    {
        pthread_mutex_unlock(CameraCapture_lock->Capturelock);
   }

    int ret = g_CamptureV4L.CameraCaptureStop();
    CloseCamraCaptureLock();

    return ret;
}

long Cam_SetAutoCrop(int nIsCrop)
{
    gnIsCrop = nIsCrop;

    if(g_startDraw)
    {
       //g_startDraw->moveToThread(&m_m_receiveVideoThread);
      // m_m_receiveVideoThread.start();
       QObject::connect(g_startDraw,SIGNAL(SendVideoBuffer(void *, int ,int ,int )),g_startDraw,SLOT(handleVideoBuffer(void *, int ,int ,int )));
    }

    return ERROR_GOOD;


}
long Cam_readBufFromJpegBuf(uchar * jpgBuf,long lsize,long &lWidth,long &lHeight,uchar * * bgrBuff)
{
    return g_CamptureDecode.readBufFromJpegBuf(jpgBuf,lsize,lWidth,lHeight,bgrBuff);
}
long Cam_readBufFromJpeg(char * filepath,uchar * * bgrBuff,JPEGInfo &jpgInfo,int desW,int desH)
{
    return g_CamptureDecode.readBufFromJpeg(filepath,bgrBuff,jpgInfo,desW,desH);
}
long Cam_saveImageFormJpeg(uchar * src,int width,int height,int bpp,char * filePath,JPEGInfo jpgInfo)
{
    return g_CamptureDecode.saveImageToJpeg(src,width,height,bpp,filePath,jpgInfo);
}
long Cam_saveImageFormJpegBuf(uchar * src,int width,int height,int bpp,uchar ** filePathBuf,JPEGInfo jpgInfo){
    return g_CamptureDecode.Cam_saveImageFormJpegBuf(src,width,height,bpp,filePathBuf,jpgInfo);
}
//条码识别

long Cam_BarcodeRecognizeBuffer(char* data,int width,int height,int nChannel,barcodeInfoList & infos)
{

    //做二值化，否则识别不出二维码
    MImage * src = mcvCreateImageFromArray(width,height,nChannel,data,false);
    MImage* srcGray = mcvAdaptiveThreshold(src);
    uchar* dst = mcvGetImageData(srcGray);
    if(src)
    {
        mcvReleaseImage1(src);
        src = NULL;
    }
    bool bNULL = false;

    zbar_processor_t *processor = zbar_processor_create(0);
    if(zbar_processor_init(processor,0,0)){
        zbar_processor_error_spew(processor,0);
    }
    zbar_image_t *zimage = zbar_image_create();
    zbar_image_set_format(zimage,*(unsigned long*)"Y800");
    zbar_image_set_size(zimage,srcGray->width,srcGray->height);
    size_t bloblen = srcGray->width*srcGray->height;
    unsigned char* blob = (unsigned char*)calloc(bloblen,sizeof(unsigned char));
    zbar_image_set_data(zimage,blob,bloblen,zbar_image_free_data);
    memcpy(blob,dst,bloblen);
    dst = NULL;
    if(srcGray)
    {
        mcvReleaseImage1(srcGray);
        srcGray = NULL;
    }
    zbar_process_image(processor,zimage);
    const zbar_symbol_t *sym = zbar_image_first_symbol(zimage);
    infos.nbarcoderCount = 0;
    for (;sym;sym=zbar_symbol_next(sym))
    {
        bNULL = true;
        zbar_symbol_type_t typ = zbar_symbol_get_type(sym);
        if(typ == ZBAR_PARTIAL)
            continue;
        const char*out = zbar_symbol_get_data(sym);

        memset(infos.infos[infos.nbarcoderCount].cbarcodeRet,0,1024);
        memcpy(infos.infos[infos.nbarcoderCount].cbarcodeRet,out,zbar_symbol_get_data_length(sym));
        for (int i =0;i<4&&i<zbar_symbol_get_loc_size(sym);i++) {
            infos.infos[infos.nbarcoderCount].x[i] = zbar_symbol_get_loc_x(sym,i);
            infos.infos[infos.nbarcoderCount].y[i] = zbar_symbol_get_loc_y(sym,i);
        }
        infos.infos[infos.nbarcoderCount].nbarcodeType = zbar_symbol_get_type(sym);
        infos.nbarcoderCount++;
        qDebug("code is %s & %s\n",infos.infos[infos.nbarcoderCount-1].cbarcodeRet,out);
    }
    if(!bNULL)
    {
        memset(infos.infos[infos.nbarcoderCount].cbarcodeRet,0,1024);
        qDebug("code is %s\n",infos.infos[infos.nbarcoderCount-1].cbarcodeRet);
    }
    zbar_image_destroy(zimage);
    zimage = NULL;
    zbar_processor_destroy(processor);
    processor = NULL;

    return 0;

}


//自动连拍 开启
long Cam_AutoCamptureStart(long nDevIndex,callBackAutoCapture funAutoCapture)
{

    //开启线程
    g_CamptureV4L.Cam_RegAutoCaptureCallBack();
    return 0;
}
long Cam_AutoCamptureStop(long nDevIndex)
{
    gbStopAutoCapture =true;
    return 0;
}
//leftRotate
long Cam_LeftRotate()
{
    gbRotate=true;
   // gbLeftRotate = true;
    gbLeftRotate++;
    if(gbLeftRotate==5)
        gbLeftRotate=1;
    if(gbRightRotate!=0)
    {
        gnRotateA = gbRightRotate*(double)90.0-gbLeftRotate*(double)90.0;
    }
    else {
        gnRotateA = 360-gbLeftRotate*(double)90.0;
    }
    printf("left rotate is %d\n",gnRotateA);
    return 0;
}
//RightRotate
long Cam_RightRotate()
{
    gbRotate=true;
    gbRightRotate++;
    if(gbRightRotate==5)
        gbRightRotate=1;

    if(gbLeftRotate!=0)
    {
        gnRotateA = (360-gbLeftRotate*(double)90.0)+gbRightRotate*(double)90.0;
    }

    else {
        gnRotateA = gbRightRotate*(qreal)90.0;
    }
    printf("right rotate is %d\n",gnRotateA);
    return 0;
}
/*
long Cam_savePDF(QStringList listPath)
{
    return g_ImageHelper.Cam_savePDF(listPath);
}*/
