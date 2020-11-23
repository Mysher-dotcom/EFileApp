#ifndef CCAPMPTUREV4L_H
#define CCAPMPTUREV4L_H

#include "datatype.h"
#include "errortype.h"

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sstream>
#include <linux/videodev2.h>
#include <linux/uvcvideo.h>
#include <QObject>
#include <QThread>
#include "autocapturethread.h"
class CCapmptureV4L:public QObject
{
    Q_OBJECT
public:
    CCapmptureV4L();
   // explicit CCapmptureV4L(QObject *parent = nullptr);

    long GetCameraDevCount(long &nDevCount);
    long GetCameraDevNodeName(long nDevCount,char* szNodeName);
    long GetCameraDevName(char* szNodeName,char* szCameraName);
    long GetCameraFPS(char* szNodeName,long &nCameraFPS);
    long SetCameraFPS(char* szNodeName,int nFrameRate);
    long GetCameraResolutionCount(char* szNodeName,long nFormatType,long &nResolutionCount);
    long GetCameraResolutionName(long nResolutionIndex,long &nWidth,long &nHeight);
    long GetCameraFormatCount(char* szNodeName,long &nDevCount);
    long GetCameraFormatName(long nDevIndex,char *szFormatName);
    long GetCameraPIDVID(char* szDevName,int &pid,int &vid);
    long SetCameraResolution(long nWidth,long nHeight);
    long SetCameraFormat(long nFormatType);
    long CameraCaptureStart(char * DszNodeName,CameraCpatureReciveCallback CameraRecvFun);
    long CameraCaptureStop();
    long Cam_GetDevLicense(long nDevIndex,int& bIsLicense);
    long Cam_RegAutoCaptureCallBack();
 public:
    int m_nCurrentWidh;
    int m_nCurrentHeight;
    int m_nCurrenetFormat;
    int nBufferSize;
    char* Imagebuffer;
    bool captureRuning;
    typedef struct CameraCaptureArgv{
        int fd;
        int width;
        int height;
        int FPS;
        int subType;
        char DeviceName[256];
        size_t buffer_count;
        buffer_t* buffers;
        CameraCpatureReciveCallback CameraRecv;
    }CamCapArgv;

    CamCapArgv  *camera;
    public:
    vector<string> vecDevName;
    vector<CameraWH> vecResolution;
    vector<string> vecFormat;
    vector<int>vecLicense;

    AutoCaptureThread *m_autoCapture;
    QThread m_autoCaptureThread;

private:
    pthread_t ThreadCaptureID;
    void UninitCameraArgv();
    int xioctl(int fd, int request, void* arg);
    int Camera_init(CamCapArgv* camera);
    int Camera_Start(CamCapArgv* camera);
    int Camera_Frame(CamCapArgv* camera);
    int Camera_Capture(CamCapArgv* camera);
    void Camera_Stop(CamCapArgv* camera);
    void Camera_Finish(CamCapArgv* camera);
    void Camera_Close(CamCapArgv* camera);
    static  void *Capture_doing(void *ptr);
    static void *AutoCaptureFunc(void* param);
    long nFrame;
};

#endif // CCAPMPTUREV4L_H
