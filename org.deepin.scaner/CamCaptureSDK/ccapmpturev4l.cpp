#include "ccapmpturev4l.h"
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
#include <linux/videodev2.h>
#include <linux/uvcvideo.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <iostream>

#include <QDebug>
#include<QString>
#include <QDir>
#include <QDateTime>
#include <strings.h>
#include <cassert>
#include <cstdio>
#include <libusb-1.0/libusb.h>
#include <QObject>

#include <pthread.h>
typedef struct CaptureControlLock{

    pthread_attr_t attr;
    pthread_mutex_t * Capturelock;
}CC_Lock;
#define CHAR_MAX_LEN 256
extern  bool gPictureSaveFlag;
extern CC_Lock *CameraCapture_lock ;
extern bool isInitCaptureLock;
extern long gDecodeWidth;
extern long gDecodeHeight;
extern long gDecodeSize;
CameraCpatureReciveCallback gCameraRecv;
CCapmptureV4L::CCapmptureV4L()
{
    vecDevName.clear();
    vecResolution.clear();
    vecFormat.clear();
    camera = NULL;
    ThreadCaptureID = 0;
    captureRuning = false;
    m_nCurrentWidh = 0;
    m_nCurrentHeight = 0;
    m_nCurrenetFormat = 0;
}

bool GreaterSort (CameraWH a,CameraWH b)
{
    return (a.CameraWidth>b.CameraWidth);
}

bool LessSort (CameraWH a,CameraWH b)
{
    return (a.CameraWidth<b.CameraWidth);
}
long CCapmptureV4L::GetCameraDevCount(long &nDevCount)
{
    vecDevName.clear();
    FILE *stream = NULL;
    long nRetValue = ERROR_GOOD;
    char buf[1024] = {0};
    memset(buf,0,sizeof(buf));
    stream = popen( "ls /dev/video*", "r" );
    int readNumber = fread( buf, sizeof(char), sizeof(buf), stream);
    if(readNumber <= 0 )
    {
        pclose(stream);
        return nRetValue;
    }
    pclose(stream);
    string srcinfo =buf;
    string strSrc;
    istringstream isSrc(srcinfo);
    int i = 0;
   // bool bIsMysher = false;

    while(getline(isSrc,strSrc,'\n'))
    {
        qDebug("CCapmptureV4L::CameraNumberCount::%s",strSrc.c_str());
        if(strSrc.find("video*",0)  ==  strSrc.npos)
        {
            int fd = -1;
            fd = open(strSrc.data(),O_RDWR);
            if(fd < 0) {
                return ERROR_OPEN_FAILED;
            }
            struct v4l2_format fmt;
            fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;

            if( ioctl(fd,VIDIOC_G_FMT,&fmt) < 0){
               close(fd);
               continue;
            }
           //vecDevName.push_back(strSrc);
            int nPID=0,nVID=0;
            char szDevNodeName[256] = {0};
            GetCameraDevName((char*)strSrc.c_str(),szDevNodeName);
            nRetValue = GetCameraPIDVID(szDevNodeName,nPID,nVID);
            if(nRetValue==0)
            {
                  QString hex = QString("%1").arg(nVID, 4, 16, QLatin1Char('0'));
                  int index = hex.indexOf("2e5a",0,Qt::CaseInsensitive);
                  if(index==0)
                  {
                    // bIsMysher = true;
                     vecDevName.insert(vecDevName.begin(),strSrc);
                     vecLicense.insert(vecLicense.begin(),1);
                   }
                   else {
                     vecDevName.push_back(strSrc);
                     vecLicense.push_back(0);
                    }
             }
            else {
                 vecDevName.push_back(strSrc);
                 vecLicense.push_back(2);
                 // qDebug("请修改USB权限！");
                           // nRetValue= ERROR_DEV_NOUSBLICENSE;
                         // break;
           }
        }
     }
   /* if(!bIsMysher&&nRetValue==0)
        {
             qDebug("no license\n");
             nRetValue= ERROR_DEV_NOCHECK;
        }*/
    qDebug("CCapmptureV4L::CameraNumberCount::OK ,the number is::%d",vecDevName.size());
    nDevCount = vecDevName.size();
    return nRetValue;
}
long CCapmptureV4L::Cam_GetDevLicense(long nDevIndex,int& bIsLicense)
{
    bIsLicense = vecLicense.at(nDevIndex);
    return ERROR_GOOD;
}
long CCapmptureV4L::GetCameraDevNodeName(long nDevCount,char* szNodeName)
{
    if(nDevCount > vecDevName.size()-1)
    {
        return ERROR_NO_VAILD;
    }
    strcpy(szNodeName,vecDevName.at(nDevCount).c_str());
    if(strcmp(szNodeName,"")==NULL)
        return ERROR_NO_VAILD;
    qDebug("CCapmptureV4L::GetCameraDevNodeName is %s\n",szNodeName);
    return ERROR_GOOD;
}
long CCapmptureV4L::GetCameraDevName(char* szNodeName,char* szCameraName){
    if(szNodeName == NULL){
        return ERROR_NO_VAILD;
    }
    int fd = -1;
    fd = open(szNodeName,O_RDWR);
    if(fd < 0) {
         return ERROR_OPEN_FAILED;
    }
    struct v4l2_capability cap;

    if(ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0)
    {
        close(fd);
        return ERROR_IOCTL_FAILED;
    }
    close(fd);
    string tmpDevName;
    string szName;
    tmpDevName = (char*)cap.card;
   // strcpy((char*)tmpDevName.c_str(),(char*)cap.card);
    int nPos_1 = tmpDevName.find(":");
    szName =tmpDevName.substr(0,nPos_1);
    memcpy(szCameraName,szName.c_str(),strlen(szName.c_str()));
    qDebug("CCapmptureV4L::GetCameraDevName is %s\n",szCameraName);
    return ERROR_GOOD;
}
long CCapmptureV4L::GetCameraFPS(char* szNodeName,long &nCameraFPS){
    if(szNodeName == NULL){
        return ERROR_NO_VAILD;
    }
    int fd = -1;
    fd = open(szNodeName,O_RDWR);
    if(fd < 0) {
        return ERROR_OPEN_FAILED;
    }
    struct v4l2_streamparm parm;
    int ret = -1;
    memset(&parm, 0, sizeof parm);
    parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(fd, VIDIOC_G_PARM, &parm);
    if (ret < 0) {
         close(fd);
        return ERROR_IOCTL_FAILED;
    }
    close(fd);
    qDebug("CCapmptureV4L::CameraFPS is %d\n",parm.parm.capture.timeperframe.denominator);
    nCameraFPS = parm.parm.capture.timeperframe.denominator;
    nFrame = nCameraFPS;
    return ERROR_GOOD;
}
long CCapmptureV4L::SetCameraFPS(char* szNodeName,int nFrameRate){
    if(szNodeName == NULL){
        return ERROR_NO_VAILD;
    }
    int fd = -1;
    fd = open(szNodeName,O_RDWR);
    if(fd < 0) {
        return ERROR_OPEN_FAILED;
    }
    struct v4l2_streamparm parm;
    int ret = -1;
    memset(&parm, 0, sizeof parm);
    parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(fd, VIDIOC_G_PARM, &parm);
    if (ret < 0) {
         close(fd);
        return ERROR_IOCTL_FAILED;
    }
    parm.parm.capture.timeperframe.numerator = 1;
    parm.parm.capture.timeperframe.denominator = nFrameRate;
     ret = ioctl(fd, VIDIOC_S_PARM, &parm);
     if (ret < 0) {
          close(fd);
         return ERROR_IOCTL_FAILED;
     }
    return ERROR_GOOD;
}
long CCapmptureV4L::GetCameraResolutionCount(char* szNodeName,long nFormatType,long &nResolutionCount){
   vecResolution.clear();
    if(szNodeName == NULL){
        return ERROR_NO_VAILD;
    }
    int fd = -1;
    fd = open(szNodeName,O_RDWR);
    if(fd < 0) {
        return ERROR_OPEN_FAILED;
    }
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    struct v4l2_fmtdesc fmt_1;
    struct v4l2_frmsizeenum frmsize;
    fmt_1.index = 0;
    fmt_1.type = type;
    CameraWH CamWH;
    CamWH.CameraHeight = 0;
    CamWH.CameraWidth  = 0;
    while (ioctl(fd, VIDIOC_ENUM_FMT, &fmt_1) >= 0)
    {
        frmsize.pixel_format = fmt_1.pixelformat;
        frmsize.index = 0;

        string FntType = (char*)fmt_1.description;
        int posYUV = FntType.find("YV",0);
        int posJPG =   FntType.find("PE",0);
        int videoType = 0;
        if(posYUV != string::npos && posYUV > 0){
            videoType = 1;
        }
        else if(posJPG != string::npos &&posJPG > 0 ){
            videoType = 0;
        }
        else{
            videoType = -1;
        }
        if(videoType != nFormatType)
        {
            fmt_1.index++;
            continue;
        }
        while (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) >= 0)
        {
            if(frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE)
            {
                CamWH.CameraHeight = frmsize.discrete.height;
                CamWH.CameraWidth = frmsize.discrete.width;
            }
            else if(frmsize.type == V4L2_FRMSIZE_TYPE_STEPWISE)
            {
                //  printf("line:%d %d,%d\n",__LINE__, frmsize.discrete.width, frmsize.discrete.height);
                  CamWH.CameraHeight = frmsize.discrete.height;
                  CamWH.CameraWidth = frmsize.discrete.width;
            }
            else
            {
                  CamWH.CameraHeight = 0;
                  CamWH.CameraWidth  = 0;
            }
            if(CamWH.CameraHeight > 0 &&  CamWH.CameraWidth  > 0)
            {
                  vecResolution.push_back(CamWH);
            }
            frmsize.index++;
        }
        CamWH.CameraHeight = 0;
        CamWH.CameraWidth  = 0;
        fmt_1.index++;
      }
    close(fd);
    if(vecResolution.size() > 1){
         sort(vecResolution.begin(),vecResolution.end(),GreaterSort);

         int   LastTimeWidth = 0;
         int   LastTimeHeight = 0;
          for(int i =0;i < vecResolution.size();i++ )
          {
              if(vecResolution[i].CameraWidth ==LastTimeWidth && vecResolution[i].CameraHeight ==  LastTimeHeight)
              {
                  vecResolution.erase(vecResolution.begin()+i);
                    i--;
              }
                 LastTimeWidth  = vecResolution[i].CameraWidth;
                 LastTimeHeight = vecResolution[i].CameraHeight;
          }
   }
    nResolutionCount = vecResolution.size();
    qDebug("CCapmptureV4L::GetCameraResolutionCount is %d\n",nResolutionCount);
    return ERROR_GOOD;
}
long CCapmptureV4L::GetCameraResolutionName(long nResolutionIndex,long &nWidth,long &nHeight){
    nWidth = vecResolution.at(nResolutionIndex).CameraWidth;
    nHeight = vecResolution.at(nResolutionIndex).CameraHeight;
     qDebug("CCapmptureV4L::GetCameraResolutionName w is %d h is %d\n",nWidth,nHeight);
    return ERROR_GOOD;
}
long CCapmptureV4L::GetCameraFormatCount(char* szNodeName,long &nDevCount){
   vecFormat.clear();
    if(szNodeName == NULL)
    {
         return 0;
    }

    int fd = -1;
    fd = open(szNodeName,O_RDWR);
    if(fd < 0) {
        return ERROR_OPEN_FAILED;
    }

    struct v4l2_format fmt;
    fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if( ioctl(fd,VIDIOC_G_FMT,&fmt) < 0){
       close(fd);
        return 0;
    }

    struct v4l2_fmtdesc fmtdesc;
    fmtdesc.index=0;
    fmtdesc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    int ret = -1;
    while(ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc)!=-1)
    {

            string str = (char*)fmtdesc.description;
            if(strcmp(str.c_str(),"Motion-JPEG")==NULL)
            {
                  vecFormat.insert(vecFormat.begin(),"MJPG");
            }
           // else {
              // vecFormat.push_back(str);
           // }

            fmtdesc.index++;
    }
    close(fd);
    qDebug("CCapmptureV4L::GetCameraVideoFormatCout is %d",vecFormat.size());
    nDevCount = vecFormat.size();
    if(nDevCount==0)
    {
        return NULL;
    }
    return ERROR_GOOD;
}
long CCapmptureV4L::GetCameraFormatName(long nDevIndex,char *szFormatName){
    strcpy(szFormatName,vecFormat.at(nDevIndex).data());
    qDebug("CCapmptureV4L::GetCameraFormatName is %s\n",szFormatName);
    return ERROR_GOOD;
}
long CCapmptureV4L::GetCameraPIDVID(char* szDevName,int &pid,int &vid){
    unsigned char usbName[CHAR_MAX_LEN];
    libusb_context *context = NULL;
    libusb_device **list = NULL;
    int rc = 0;
    ssize_t count = 0;


    rc = libusb_init(&context);
    if(rc)
    {
        return ERROR_INIT_FAILED;
    }

    count = libusb_get_device_list(context, &list);
    if(count <0)
    {
        return ERROR_NO_VAILD;
    }

    long ret = ERROR_NOFIND;
    for (size_t idx = 0; idx < count; ++idx) {
        libusb_device *device = list[idx];
        libusb_device_descriptor desc = {0};

        rc = libusb_get_device_descriptor(device, &desc);


        if(desc.bDeviceClass == 0xef)  //bDeviceClass  0xef is Camrea
        {

            struct libusb_device_handle *usb_p=NULL;
            bzero(usbName,sizeof(usbName));
            rc = libusb_open(list[idx],&usb_p);

            if (LIBUSB_SUCCESS != rc) {
                    qDebug("Could not open USB device,return is %d\n",rc);
                    return ret;
                }
           if(usb_p)
            {
                libusb_get_string_descriptor_ascii(usb_p, desc.iProduct, usbName, CHAR_MAX_LEN);//     desc.iSerialNumber;
                libusb_close(usb_p);
                usb_p = NULL;
                if(strcmp((const char *)usbName,(const char *)szDevName) == 0){
                    pid = desc.idProduct;
                    vid  = desc.idVendor;
                    qDebug("Vendor:Device = %04x:%04x:%04x\n", desc.idVendor, desc.idProduct,desc.bDeviceClass);
                    qDebug("bus %d, device %d\n",libusb_get_bus_number(device), libusb_get_device_address(device));
                    qDebug("name::%s\n",usbName);
                     ret = ERROR_GOOD;
                }
            }
         }

     }
    libusb_exit(context);
    return ret;
}
long CCapmptureV4L::SetCameraResolution(long nWidth,long nHeight)
{
    m_nCurrentWidh = nWidth;
    m_nCurrentHeight = nHeight;
    qDebug("CCapmptureV4L::SetCameraResolution W is %d,H is %d\n",m_nCurrentWidh,m_nCurrentHeight);
    return ERROR_GOOD;
}
long CCapmptureV4L::SetCameraFormat(long nFormatType)
{
    m_nCurrenetFormat = nFormatType;
    return ERROR_GOOD;
}
long CCapmptureV4L::CameraCaptureStart(char * szNodeName,CameraCpatureReciveCallback CameraRecvFun){

     if(szNodeName == NULL || CameraRecvFun == 0 || m_nCurrentWidh <= 0 || m_nCurrentHeight <= 0  ){
      return ERROR_NO_VAILD;
    }
    int fd = open(szNodeName, O_RDWR | O_NONBLOCK, 0); //非阻塞模式
    if (fd < 0)
    {
        qDebug("Error::%s::Open %s filed\n",__func__,szNodeName);
        return ERROR_OPEN_FAILED;
    }

    if(camera != NULL)  //del 20171114
    {
        close(fd);
        return ERROR_MALLOC_FAILED;
    }

    if(camera == NULL)
    {
        camera = ( CamCapArgv*)malloc(sizeof (CamCapArgv));
        memset((char*)camera,0,sizeof(CamCapArgv));
        camera->CameraRecv = NULL;
    }

    if(camera == NULL)
    {
        close(fd);
        return ERROR_MALLOC_FAILED;
    }

    camera->fd = fd;

    camera->width = m_nCurrentWidh;
    camera->height = m_nCurrentHeight;
    camera->buffer_count = 0;
    camera->buffers = NULL;
    camera->CameraRecv = NULL;
    camera->FPS = 15;
    camera->subType = m_nCurrenetFormat;
    qDebug("CCapmptureV4L::CameraCaptureStart subtype is %d",m_nCurrenetFormat);
    memset(camera->DeviceName,0,sizeof(  camera->DeviceName));
    strcpy(camera->DeviceName,szNodeName);

    if( Camera_init (camera) != ERROR_GOOD){
        UninitCameraArgv();
        return ERROR_INIT_FAILED;
    }

    if(Camera_Start(camera) !=  ERROR_GOOD){
        UninitCameraArgv();
        return ERROR_INIT_FAILED;
    }

    camera->CameraRecv = CameraRecvFun;
    gCameraRecv = CameraRecvFun;
    if(pthread_create(&ThreadCaptureID, NULL, Capture_doing,this) != 0)
    {
        UninitCameraArgv();
        return ERROR_PTHREAD_FAILED;
    }

    return ERROR_GOOD;
}
long CCapmptureV4L::CameraCaptureStop(){
    if(captureRuning){
             captureRuning = false;
             void *pthread_result = NULL;
             pthread_join(ThreadCaptureID, &pthread_result);//等待g_mzThreadCaptureID 的线程结束
             qDebug("%s\n",(char*)pthread_result);
             ThreadCaptureID = 0;
             UninitCameraArgv();
    }
    //Camera_Stop(camera);

    return ERROR_GOOD;
}


void CCapmptureV4L::UninitCameraArgv(){
    if(camera== NULL)
     {
        return ;
    }

    if(camera->fd > 0){
        close(camera->fd);
    }

    if(camera->buffers)
    {
        free(camera->buffers);
        camera->buffers = NULL;
    }
    free(camera);
    camera = NULL;
}

int CCapmptureV4L:: xioctl(int fd, int request, void* arg)
{
    for (int i = 0; i < 100; i++) {
        int r = ioctl(fd, request, arg);
        if (r != -1 || errno != EINTR) return r;
    }
    return -1;
}

int CCapmptureV4L::Camera_init(CamCapArgv* camera){
  if(camera == NULL){
      return ERROR_NO_VAILD;
  }

  struct v4l2_capability cap;
  if (xioctl(camera->fd, VIDIOC_QUERYCAP, &cap) == -1)  /* 获取设备支持的操作 */
  {
      qDebug("Error:: %s ::VIDIOC_QUERYCAP Error\n",__func__);
      return ERROR_IOCTL_FAILED;
  }

  if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) //Is a video capture device
  {
      qDebug("Error:: %s ::not Is a video capture device\n ",__func__);
      return ERROR_IOCTL_FAILED;
  }

  if (!(cap.capabilities & V4L2_CAP_STREAMING)) //streaming I/O ioctls
  {
      qDebug(" Error:: %s ::streaming I/O ioctls Error\n",__func__);
      return ERROR_IOCTL_FAILED;
  }


  //设置帧率
    struct v4l2_streamparm Stream_Parm;
    memset(&Stream_Parm, 0, sizeof(struct v4l2_streamparm));
    Stream_Parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    Stream_Parm.parm.capture.timeperframe.denominator =camera->FPS;;
    Stream_Parm.parm.capture.timeperframe.numerator = 1;

    int retFPS= xioctl(camera->fd, VIDIOC_S_PARM, &Stream_Parm);
    if(retFPS < 0){
        qDebug("Error:: %s ::VIDIOC_S_PARM Error\n",__func__);
        return ERROR_IOCTL_FAILED;
    }

    struct v4l2_format format;   //set width height  and capture format
    memset(&format, 0, sizeof format);
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.width = camera->width;
    format.fmt.pix.height = camera->height;
    format.fmt.pix.pixelformat = camera->subType == 0 ? V4L2_PIX_FMT_MJPEG : V4L2_PIX_FMT_YUYV;

    format.fmt.pix.field =  V4L2_FIELD_ANY;

    if (xioctl(camera->fd, VIDIOC_S_FMT, &format) == -1)
    {
        qDebug("Error:: %s ::VIDIOC_S_FMT Error\n",__func__);
        return ERROR_IOCTL_FAILED;
     }

    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof req);
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (xioctl(camera->fd, VIDIOC_REQBUFS, &req) == -1)
    {
        qDebug("Error:: %s ::VIDIOC_REQBUFS Error\n",__func__);
        return ERROR_IOCTL_FAILED;
    }
    camera->buffer_count = req.count;
    camera->buffers = (buffer_t*)calloc(req.count, sizeof (buffer_t));
    if(camera->buffers == NULL)
    {
        printf("Error:: %s ::calloc camera buffers Error\n",__func__);
        return ERROR_MALLOC_FAILED;
    }

    size_t buf_max = 0;
    for (size_t i = 0; i < camera->buffer_count; i++)
    {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(camera->fd, VIDIOC_QUERYBUF, &buf) == -1)
        {
            qDebug("Error::%s ::VIDIOC_QUERYBUF Error\n",__func__);
        }

        if (buf.length > buf_max) {
            buf_max = buf.length;
        }
        camera->buffers[i].length = buf.length;
        camera->buffers[i].start =
        (char*) mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, camera->fd, buf.m.offset);
        if (camera->buffers[i].start == MAP_FAILED)
        {
            qDebug("Error::%s ::mmap Error\n",__func__);
        }
    }

    return ERROR_GOOD;
}

int CCapmptureV4L::Camera_Start(CamCapArgv* camera){

    if(camera == NULL){
      return ERROR_NO_VAILD;
    }

    for (size_t i = 0; i < camera->buffer_count; i++)
    {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (xioctl(camera->fd, VIDIOC_QBUF, &buf) == -1)
        {
            qDebug("Error::%s ::VIDIOC_QBUF Error\n",__func__);
            return ERROR_IOCTL_FAILED;
        }

    }
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(camera->fd, VIDIOC_STREAMON, &type) == -1)
    {
        qDebug("Error::%s ::VIDIOC_STREAMON Error\n",__func__);
        return ERROR_IOCTL_FAILED;
    }

    return ERROR_GOOD;

}

int CCapmptureV4L::Camera_Frame(CamCapArgv* camera){


   if(camera == NULL){
       return ERROR_NO_VAILD;
   }

   struct timeval timeout;
     timeout.tv_sec = 1;
     timeout.tv_usec = 0;
     fd_set fds;
     FD_ZERO(&fds);
     FD_SET(camera->fd, &fds);
     int r = select(camera->fd + 1, &fds, 0, 0, &timeout);
     if (r <0)
     {
         qDebug("info::%s ::Select Error\n",__func__);
         return ERROR_SELECT_FAILED;
     }
     if (r == 0) {
         qDebug("info::%s ::S usleep(100);elect Timeout\n",__func__);
         return ERROR_TIMEOUT_FAILED;
     }

     return Camera_Capture(camera);
}

int CCapmptureV4L:: Camera_Capture(CamCapArgv* camera)
{
    if(camera == NULL){
        return ERROR_NO_VAILD;
    }

    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof buf);
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    if (xioctl(camera->fd, VIDIOC_DQBUF, &buf) == -1) {
        qDebug("Error::%s ::VIDIOC_DQBUF\n",__func__);
        return ERROR_IOCTL_FAILED;
    }

    if(camera->CameraRecv != NULL)
    {
        if(isInitCaptureLock){
            pthread_mutex_lock(CameraCapture_lock->Capturelock);
        }
        //nBufferSize = buf.bytesused;
        //if(gPictureSaveBuf != NULL)
        {
            //memcpy(gPictureSaveBuf,camera->buffers[buf.index].start,buf.bytesused);
            nBufferSize =buf.bytesused;
            gDecodeWidth = camera->width;
            gDecodeHeight= camera->height;
            gDecodeSize = nBufferSize;

        }

        if(isInitCaptureLock){
            pthread_mutex_unlock(CameraCapture_lock->Capturelock);
        }
        Imagebuffer = camera->buffers[buf.index].start;
      //  memcpy(Imagebuffer,camera->buffers[buf.index].start,buf.bytesused);
        camera->CameraRecv(camera->buffers[buf.index].start,buf.bytesused,camera->width,camera->height,camera->subType);

    }

    if (xioctl(camera->fd, VIDIOC_QBUF, &buf) == -1) {
        qDebug("Error::%s ::VIDIOC_QBUF\n",__func__);
         return ERROR_IOCTL_FAILED;
    }
    return ERROR_GOOD;

 }

void CCapmptureV4L::Camera_Stop(CamCapArgv* camera)
{
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(camera->fd, VIDIOC_STREAMOFF, &type) == -1){
             qDebug("Error::%s ::VIDIOC_STREAMOFF\n",__func__);
    }
}

void CCapmptureV4L::Camera_Finish(CamCapArgv* camera){
  for (size_t i = 0; i < camera->buffer_count; i++) {
    munmap(camera->buffers[i].start, camera->buffers[i].length);
  }
}

void CCapmptureV4L::Camera_Close(CamCapArgv* camera){
    UninitCameraArgv();
}

void *CCapmptureV4L::Capture_doing(void *ptr)
{

   //  qDebug("SDK  thread tid = %u\n", pthread_self());
    CCapmptureV4L *CamCap = (CCapmptureV4L*)ptr;
    CamCap->captureRuning = true;

    while(CamCap->captureRuning)
     {

            int ret  = CamCap->Camera_Frame(CamCap->camera);
           if(ERROR_IOCTL_FAILED == ret)
            {
                //获取不到流视频 设备可能断开了,尝试重连
               break;
            }
           int ntime = 1000/CamCap->nFrame;
            usleep(ntime);
     }
        CamCap->Camera_Stop(CamCap->camera);
        CamCap->Camera_Finish(CamCap->camera);
        CamCap->Camera_Close(CamCap->camera);
        pthread_exit((void*)"Capture End");

}

long CCapmptureV4L::Cam_RegAutoCaptureCallBack()
{
    m_autoCapture = new AutoCaptureThread();
    if(m_autoCapture)
    {
        m_autoCapture->moveToThread(&m_autoCaptureThread);
        m_autoCaptureThread.start();
        connect(m_autoCapture,SIGNAL(AutoCapture()),m_autoCapture,SLOT(AutoCaptureFun()));
    }
    if(m_autoCapture)
    {
        emit m_autoCapture->AutoCapture();
    }

}
