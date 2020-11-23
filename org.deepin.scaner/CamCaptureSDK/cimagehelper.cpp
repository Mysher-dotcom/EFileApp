#include "cimagehelper.h"
#include "errortype.h"
#include "MImage.h"
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
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <QFile>
#include <sys/ioctl.h>
#include <iostream>
#include <QDir>
#include <QCoreApplication>
#include "datatype.h"
#include "zbar.h"
using namespace zbar;
#include "cpng.h"
typedef struct CaptureControlLock{

    pthread_attr_t attr;
    pthread_mutex_t * Capturelock;
}CC_Lock;

extern int gDecodeWidth;
extern int gDecodeHeight;
extern int gDecodeSize;
extern CC_Lock *CameraCapture_lock ;
extern bool isInitCaptureLock;
extern CameraCpatureReciveCallback gCameraRecv;
CImageHelper::CImageHelper()
{
    bIsLoadSDK = false;
    m_nautoCaptureState = 0;
   // ThreadImageDectetID=0;
}
void CImageHelper::InitImageSDK(){
    if(!bIsLoadSDK)
    {
        mcvInit();
        bIsLoadSDK = true;
    }

}
int CImageHelper::WriteFile(unsigned char* srcData,int len,FILE * destfb)
{

    int write_length = 0;
    write_length = fwrite(srcData,sizeof(char),len,destfb);
    usleep(100);
    return write_length;
}
bool CImageHelper::Rotate(uchar* srcBuf,int &nSize,int &nWidth,int &nHeight,int nRotateA)
{
   /* MImage* src = NULL;
    if(srcBuf)
    {
      src  = mcvCreateImageFromArray(nWidth,nHeight,3,(char*)srcBuf,false);
    }
    MImage* srcRotateLeft = mcvRotateImage(src,nRotateA);
    if(src)
    {
        mcvReleaseImage(&src);
        src = NULL;
        //srcBuf = NULL;
    }

    nWidth = srcRotateLeft->width;
    nHeight = srcRotateLeft->height;
    nSize = srcRotateLeft->width*srcRotateLeft->height*3;
    //nSize =
   // *dstBuf = new uchar[nSize];
    srcBuf = mcvGetImageData(srcRotateLeft);
    if(srcRotateLeft)
    {
        mcvReleaseImage(&srcRotateLeft);
        srcRotateLeft = NULL;
        //srcBuf = NULL;
    }
    //memcpy(srcBuf,(char*)dstBuf,nSize);*/
    return true;
}
bool CImageHelper::SaveImage(uchar* srcBuf,int nSize,int nWidth,int nHeight)
{
    JPEGInfo jpgInfo;
    memset(&jpgInfo,0,sizeof(jpgInfo));
    jpgInfo.xResolution.b = 1000;
    jpgInfo.xResolution.a = 200*jpgInfo.xResolution.b;
    jpgInfo.yResolution.b = 1000;
    jpgInfo.yResolution.a =200*jpgInfo.yResolution.b;
    jpgInfo.compression = 50;
    long lret = m_decode.saveImageToJpeg(srcBuf,nWidth,nHeight,8,"/tmp/saveimage.jpg",jpgInfo);

}

bool CImageHelper::DrawLine(char*ImgBuff,int &nSize,int &nWidth,int &nHeight,int nCropType)
{
    int imgInfo[4];
    imgInfo[0]=nHeight;
    imgInfo[1]=nWidth;
    imgInfo[2]=3;
    imgInfo[3]=5;
    char color[3];
    color[0] = 0;
    color[1] = 255;
    color[2] = 0;
    MPoint p[2];
    float fAngle = 0;
    MPoint mp4[4];
    if(!bIsLoadSDK)
    {
        mcvInit();
        bIsLoadSDK = true;
    }
    MImage* src = NULL;
    if(ImgBuff)
    {
      src  = mcvCreateImageFromArray(nWidth,nHeight,3,ImgBuff,false);
    }
    MRectRArray mrectRA;
    if(nCropType==1)
    {
        MRectR rectR;
       if(mcvDetectRect(src,fAngle,mp4,MRect(0,0,0,0)))
       {

           if(fAngle < -45)
           {
               fAngle += 90;
           }
           rectR.m_fAngle = fAngle;
           for(int i = 0;i < 4;i++)
           {
               rectR.m_pt[i].x= mp4[i].x;
               rectR.m_pt[i].y = mp4[i].y;
               if(rectR.m_fAngle == 0)
               {
                   rectR.m_pt[i].x = rectR.m_pt[i].x < src->width?rectR.m_pt[i].x:src->width;
                   rectR.m_pt[i].y = rectR.m_pt[i].y < src->height?rectR.m_pt[i].y:src->height;
               }
           }

        }
       else {
           rectR.m_pt[0].x= 0;
           rectR.m_pt[0].y = 0;
           rectR.m_pt[1].x= src->width;
           rectR.m_pt[1].y = 0;
           rectR.m_pt[2].x= src->width;
           rectR.m_pt[2].y = src->height;
           rectR.m_pt[3].x= 0;
           rectR.m_pt[3].y = src->height;
       }
       mrectRA.m_nCount = 1;
       mrectRA.m_rectR[0] = rectR;
    }
    else {
      //  mrectRA = mcvDetectRectMulti(src,MRect(0,0,0,0));
    }
   if(src)
   {
       mcvReleaseImage1(src);
       src = NULL;

   }
   for (int var = 0; var < mrectRA.m_nCount; ++var) {
       for (int i =0;i<4;i++) {
           p[0] = mrectRA.m_rectR[var].m_pt[i];
           p[1] = mrectRA.m_rectR[var].m_pt[(i+1) % 4];
          mcvDrawLineOnImageBuffer(ImgBuff,color,p,imgInfo);
       }
   }


    return true;

}

long CImageHelper::AutoCropPreviewFlipAngle(unsigned char *srcData, int srcDataType,int srcDataLen, int srcDataWidth,  int srcDataHeight,
                            int &x, int &y, int &x1, int &y1, int &x2,int &y2,int &x3, int &y3, int FlipAngle)
{

    MImage *src = mcvCreateImageFromArray(srcDataWidth,srcDataHeight,3,(char*)srcData,false);

    float fAngle = 0;
    MPoint mp4[4];
    if(mcvDetectRect(src,fAngle,mp4,MRect(0,0,0,0)))
    {
        MRectR rectR;
        if(fAngle < -45)
        {
            fAngle += 90;
        }
        rectR.m_fAngle = fAngle;
        for(int i = 0;i < 4;i++)
        {
            rectR.m_pt[i].x= mp4[i].x;
            rectR.m_pt[i].y = mp4[i].y;
            if(rectR.m_fAngle == 0)
            {
                rectR.m_pt[i].x = rectR.m_pt[i].x < src->width?rectR.m_pt[i].x:src->width;
                rectR.m_pt[i].y = rectR.m_pt[i].y < src->height?rectR.m_pt[i].y:src->height;
            }
        }
        x=rectR.m_pt[0].x;
        y=rectR.m_pt[0].y;
        x1=rectR.m_pt[1].x;
        y1=rectR.m_pt[1].y;
        x2=rectR.m_pt[2].x;
        y2=rectR.m_pt[2].y;
        x3=rectR.m_pt[3].x;
        y3=rectR.m_pt[3].y;
        mcvReleaseImage1(src);
        return ERROR_GOOD;
    }
     return ERROR_NO_VAILD;
}
bool CImageHelper::CompareImgae(char* srcData,int width,int height,int size)
{
  /*  if(m_ImageLastCompare==NULL)
    {
        unsigned char* dstBuf = NULL;
        unsigned char* srcBuf = new unsigned char[size];
        memcpy(srcBuf,srcData,size);
        long Width = width;
        long Height = height;
        m_decode.readBufFromJpegBuf(srcBuf,size,Width,Height,&dstBuf);
        delete srcBuf;
        srcBuf = NULL;
        //imgParam.buffer = NULL;
        MImage* srcCut  = NULL;
        MImage* srcImage = mcvCreateImageFromArray(Width,Height,3,(char*)dstBuf,false);
        delete dstBuf;
        dstBuf = NULL;
       // ProcessImage(imgParam,szFile);
        m_ImageLastCompare = srcImage;
        if(srcImage)
        {
            mcvReleaseImage1(srcImage);
            srcImage = NULL;
        }
    }
    else {
        unsigned char* dstBuf = NULL;
        unsigned char* srcBuf = new unsigned char[size];
        memcpy(srcBuf,srcData,size);
        long Width = width;
        long Height = height;
        m_decode.readBufFromJpegBuf(srcBuf,size,Width,Height,&dstBuf);
        delete srcBuf;
        srcBuf = NULL;
        //imgParam.buffer = NULL;
        MImage* srcImage = mcvCreateImageFromArray(Width,Height,3,(char*)dstBuf,false);
        delete dstBuf;
        dstBuf = NULL;
       /* if(gCameraRecv)
        {
            gCameraRecv(imgParam.buffer,imgParam.nSize,imgParam.nWidth,imgParam.nHeight,0);
        }
*/
    /*    bool bRe = false;
        bRe = mcvCompare(srcImage,m_ImageLastCompare,200,NULL,0);
        if(bRe)
        {
            m_nautoCaptureState++;
        }
        else {

            //ProcessImage(imgParam,szFile);
        }
        mcvReleaseImage1(m_ImageLastCompare);
        m_ImageLastCompare = srcImage;
    }
*/
    return true;
}
void CImageHelper::ProcessImage(ImageParam imgParam,char* szFilePath)
{
    printf("szpath is %s\n",szFilePath);

    if(imgParam.nCropType==0&&!imgParam.bIsTextEn&&!imgParam.bIsWater&&imgParam.nRotate==0
            &&imgParam.nColorType==0&&!imgParam.bColorBlance&&!imgParam.bRedEn
            &&!imgParam.bInverse&&!imgParam.bColorDropOut&&!imgParam.bFillBorder&&!imgParam.bNoise
            &&!imgParam.bRemoveFinger)
    {
         char* strF = substrend(szFilePath,2);
         printf("str is %s\n",strF);
        if(strcmp(strF,"png")==0)
        {
            printf("12121212\n");
            unsigned char* dstBuf = NULL;
            unsigned char* srcBuf = new unsigned char[imgParam.nSize];
            memcpy(srcBuf,imgParam.buffer,imgParam.nSize);
            long Width = imgParam.nWidth;
            long Height = imgParam.nHeight;
            qDebug("ProcessImage Width is %d,Height is %d\n",Width,Height);
            m_decode.readBufFromJpegBuf(srcBuf,imgParam.nSize,Width,Height,&dstBuf);
            delete srcBuf;
            srcBuf = NULL;
            imgParam.buffer = NULL;
            //MImage* srcCut  = NULL;
            MImage* src = mcvCreateImageFromArray(Width,Height,3,(char*)dstBuf,false);
            delete dstBuf;
            dstBuf = NULL;
            CPNG m_png;
            pic_data out;
            unsigned char* dst = mcvGetImageData(src);
            //将rgb转rgba
            out.width=src->width;
            out.height=src->height;
            //printf("121212\n");
            out.rgba = new unsigned char[src->width*src->height*3];
            memset(out.rgba,0,src->width*src->height*3);
            memcpy(out.rgba,dst,src->width*src->height*3);
            out.alpha_flag = 0;
            out.bit_depth=8;
            int pos = 0;

            //qDebug("data is %s,%s",out.rgba[0],out.rgba[1]);
            m_png.write_png_file(szFilePath,&out);
            delete out.rgba;
            mcvReleaseImage1(src);
            src=NULL;
        }
        else
        {
            FILE *ofp = NULL;
            if (NULL != (ofp = fopen (szFilePath, "w+")))
            {
                WriteFile(imgParam.buffer,imgParam.nSize,ofp);
            }
            fclose(ofp);
        }

        return;

    }
    unsigned char* dstBuf = NULL;
    unsigned char* srcBuf = new unsigned char[imgParam.nSize];
    memcpy(srcBuf,imgParam.buffer,imgParam.nSize);
    long Width = imgParam.nWidth;
    long Height = imgParam.nHeight;
    qDebug("ProcessImage Width is %d,Height is %d\n",Width,Height);
    m_decode.readBufFromJpegBuf(srcBuf,imgParam.nSize,Width,Height,&dstBuf);
    delete srcBuf;
    srcBuf = NULL;
    imgParam.buffer = NULL;
    MImage* srcCut  = NULL;
    MImage* src = mcvCreateImageFromArray(Width,Height,3,(char*)dstBuf,false);
    delete dstBuf;
    dstBuf = NULL;
    MRectRArray rectRA;
    MRectR rectR;
    float fAngle = 0;
    MPoint mp4[4];
    MRectR rc_Left;
    MRectR rc_Right;
    //书本中轴裁切比例
    double dMiddlePersent=0.5;
    //书本中轴比例补充
    double dMiddlePersentBook = 0.1;
    //先做裁切再做优化
    switch(imgParam.nCropType)
    {
    case 0:
        srcCut = mcvClone(src);
        if(src)
        {
            mcvReleaseImage1(src);
            src = NULL;
        }
        break;
    case 1:
        if(mcvDetectRect(src,fAngle,mp4,MRect(0,0,0,0)))
        {
            if(fAngle < -45)
            {
                fAngle += 90;
            }
            rectR.m_fAngle = fAngle;
            for(int i = 0;i < 4;i++)
            {
                rectR.m_pt[i].x= mp4[i].x;
                rectR.m_pt[i].y = mp4[i].y;
                if(rectR.m_fAngle == 0)
                {
                    rectR.m_pt[i].x = rectR.m_pt[i].x < src->width?rectR.m_pt[i].x:src->width;
                    rectR.m_pt[i].y = rectR.m_pt[i].y < src->height?rectR.m_pt[i].y:src->height;
                }
            }
        }
        else {
            rectR.m_pt[0].x = 0;
            rectR.m_pt[0].y = 0;
            rectR.m_pt[1].x = src->width;
            rectR.m_pt[1].y = 0;
            rectR.m_pt[2].x = src->width;
            rectR.m_pt[2].y = src->height;
            rectR.m_pt[3].x = 0;
            rectR.m_pt[3].y = src->height;
        }
        rectRA.m_nCount = 1;
        rectRA.m_rectR[0] = rectR;
        break;
    case 2:
        break;
    case 3:
        break;
    case 4:
        break;
    default:
        srcCut = mcvClone(src);
        if(src)
        {
            mcvReleaseImage1(src);
            src = NULL;
        }
        break;
    }

    if(imgParam.nCropType!=0)
    {
        int i = 0;
        char FilePath[256]={0};
        char filePathLater[256]={0};
        strcpy(FilePath,szFilePath);
        strcpy(filePathLater,szFilePath);
        memset(szFilePath,0,sizeof(szFilePath));
        for (int var = 0; var < rectRA.m_nCount; ++var) {
            if(imgParam.nCropType!=3)
            {
                //单图裁切和多图裁切
                 srcCut = mcvCutR(src,rectRA.m_rectR[var]);
            }
             MImage* srcGary = NULL;
             //颜色模式，彩色、灰度、黑白
             switch (imgParam.nColorType) {
             case 0:
                 srcGary = mcvClone(srcCut);
                 if(srcCut)
                 {
                     mcvReleaseImage1(srcCut);
                     srcCut = NULL;
                 }
                 break;
             case 1:
                 //灰度
                 srcGary = mcvGrayStyle(srcCut);
                 if(srcCut)
                 {
                     mcvReleaseImage1(srcCut);
                     srcCut = NULL;
                 }
                 break;
             case 2:
                 //黑白
                 srcGary = mcvAdaptiveThreshold(srcCut);
                 if(srcCut)
                 {
                     mcvReleaseImage1(srcCut);
                     srcCut = NULL;
                 }
                 break;
             default:
                 srcGary = mcvClone(srcCut);
                 if(srcCut)
                 {
                     mcvReleaseImage1(srcCut);
                     srcCut = NULL;
                 }
                 break;
             }
            //判断文件是否存在，存在则删除文件；
             if(imgParam.nCropType==3)
             {
                 char* str = substrendR(FilePath,strlen(FilePath)-4);
                 char* strF = substrend(FilePath,2);
                 strcat(str,"-Left");
                 strcat(str,".");
                 strcat(str,strF);
                 strcpy(filePathLater,str);
                 free(str);
                 str = NULL;
             }

            QFile tempFile(filePathLater);
            if (tempFile.exists()){
                if(imgParam.nCropType==3)
                {
                     if (tempFile.exists()){
                         char* str = substrendR(FilePath,strlen(FilePath)-4);
                         char* strF = substrend(FilePath,2);
                          strcat(str,"-Right");
                          strcat(str,".");
                          strcat(str,strF);
                          strcpy(filePathLater,str);
                          free(str);
                          free(strF);
                          str = NULL;
                          strF = NULL;
                     }
                }
                else {
                    if(imgParam.nCropType==2)
                    {
                        //文件存在，则重命名
                        i++;
                        char* str = substrendR(FilePath,strlen(FilePath)-4);
                        char* strF = substrend(FilePath,2);
                        strcat(str,"-");
                        sprintf(filePathLater,"%s%02d.%s",str,i,strF);
                        free(str);
                        free(strF);
                        str = NULL;
                        strF = NULL;
                        qDebug("File is %s\n",str);
                    }
                    else
                    {
                        tempFile.remove(filePathLater);
                    }
                }

             }

            char* tmp = substrend(FilePath,2);

            if(strcmp(tmp,"jpg")==NULL)
            {
                 free(tmp);
                 tmp = NULL;

                 uchar* dst = mcvGetImageData(srcGary);
                 //hpdfoperation pdfop;
                 //pdfop.rgb2pdf(dst,srcGary->width,srcGary->height,"/home/viisan/Documents/111.pdf",0);
                 JPEGInfo jpgInfo;
                 memset(&jpgInfo,0,sizeof(jpgInfo));
                 jpgInfo.xResolution.b = 1000;
                 jpgInfo.xResolution.a = 200*jpgInfo.xResolution.b;
                 jpgInfo.yResolution.b = 1000;
                 jpgInfo.yResolution.a =200*jpgInfo.yResolution.b;
                 jpgInfo.compression = 50;
                 long lret = m_decode.saveImageToJpeg(dst,srcGary->width,srcGary->height,srcGary->channel*8,filePathLater,jpgInfo);
                 mcvReleaseImage1(srcGary);
                 srcGary=NULL;
            }
            else if(strcmp(tmp,"png")==NULL)
            {
                //printf("22222222222222222222222222222222222222222222222222\n");
                CPNG m_png;
                pic_data out;
                unsigned char* dst = mcvGetImageData(srcGary);
                //将rgb转rgba
                out.width=srcGary->width;
                out.height=srcGary->height;
                //printf("121212\n");
                out.rgba = new unsigned char[srcGary->width*srcGary->height*3];
                memset(out.rgba,0,srcGary->width*srcGary->height*3);
                memcpy(out.rgba,dst,srcGary->width*srcGary->height*3);
                out.alpha_flag = 0;
                out.bit_depth=8;
                int pos = 0;

                //qDebug("data is %s,%s",out.rgba[0],out.rgba[1]);
                m_png.write_png_file(filePathLater,&out);
                delete out.rgba;
                mcvReleaseImage1(srcGary);
                srcGary=NULL;
            }
            else
            {
                 mcvSaveImage(filePathLater,srcGary,200,200);
                 mcvReleaseImage1(srcGary);
                 srcGary=NULL;
            }
            if(imgParam.nCropType==2||imgParam.nCropType==3)
            {
               // szFilePath = szFilePath + ";"

                strcat(szFilePath,filePathLater);
                strcat(szFilePath,";");
            }
            else {
               strcat(szFilePath,filePathLater);
            }
            mcvReleaseImage1(srcCut);
            srcCut=NULL;

        }
        if(src)
         {
             mcvReleaseImage1(src);
             src = NULL;
         }
    }
    else {
        int i = 0;
        char FilePath[256]={0};
        char filePathLater[256]={0};
        strcpy(FilePath,szFilePath);
        strcpy(filePathLater,szFilePath);
        memset(szFilePath,0,sizeof(szFilePath));
         MImage* srcGary = NULL;
         //颜色模式，彩色、灰度、黑白
         switch (imgParam.nColorType) {
         case 0:
             srcGary = mcvClone(srcCut);
             if(srcCut)
             {
                 mcvReleaseImage1(srcCut);
                 srcCut = NULL;
             }
             break;
         case 1:
             //灰度
             srcGary = mcvGrayStyle(srcCut);
             if(srcCut)
             {
                 mcvReleaseImage1(srcCut);
                 srcCut = NULL;
             }
             break;
         case 2:
             //黑白
             srcGary = mcvAdaptiveThreshold(srcCut);
             if(srcCut)
             {
                 mcvReleaseImage1(srcCut);
                 srcCut = NULL;
             }
             break;
         default:
             srcGary = mcvClone(srcCut);
             if(srcCut)
             {
                 mcvReleaseImage1(srcCut);
                 srcCut = NULL;
             }
             break;
         }
        //判断文件是否存在，存在则删除文件；
         if(imgParam.nCropType==3)
         {
             char* str = substrendR(FilePath,strlen(FilePath)-4);
             char* strF = substrend(FilePath,2);
             strcat(str,"-Left");
             strcat(str,".");
             strcat(str,strF);
             strcpy(filePathLater,str);
             free(str);
             str = NULL;
         }

        QFile tempFile(filePathLater);
        if (tempFile.exists()){
            if(imgParam.nCropType==3)
            {
                 if (tempFile.exists()){
                     char* str = substrendR(FilePath,strlen(FilePath)-4);
                     char* strF = substrend(FilePath,2);
                      strcat(str,"-Right");
                      strcat(str,".");
                      strcat(str,strF);
                      strcpy(filePathLater,str);
                      free(str);
                      free(strF);
                      str = NULL;
                      strF = NULL;
                 }
            }
            else {
                if(imgParam.nCropType==2)
                {
                    //文件存在，则重命名
                    i++;
                    char* str = substrendR(FilePath,strlen(FilePath)-4);
                    char* strF = substrend(FilePath,2);
                    strcat(str,"-");
                    sprintf(filePathLater,"%s%02d.%s",str,i,strF);
                    free(str);
                    free(strF);
                    str = NULL;
                    strF = NULL;
                    qDebug("File is %s\n",str);
                }
                else
                {
                    tempFile.remove(filePathLater);
                }
            }

         }

        char* tmp = substrend(FilePath,2);
        printf("tmp is %s\n",tmp);
        if(strcmp(tmp,"jpg")==NULL)
        {
             free(tmp);
             tmp = NULL;

             uchar* dst = mcvGetImageData(srcGary);
             //hpdfoperation pdfop;
             //pdfop.rgb2pdf(dst,srcGary->width,srcGary->height,"/home/viisan/Documents/111.pdf",0);
             JPEGInfo jpgInfo;
             memset(&jpgInfo,0,sizeof(jpgInfo));
             jpgInfo.xResolution.b = 1000;
             jpgInfo.xResolution.a = 200*jpgInfo.xResolution.b;
             jpgInfo.yResolution.b = 1000;
             jpgInfo.yResolution.a =200*jpgInfo.yResolution.b;
             jpgInfo.compression = 50;
             long lret = m_decode.saveImageToJpeg(dst,srcGary->width,srcGary->height,srcGary->channel*8,filePathLater,jpgInfo);
             mcvReleaseImage1(srcGary);
             srcGary=NULL;
        }
        else if(strcmp(tmp,"png")==NULL)
        {
            //printf("1111111111111111111111111111111111111\n");
            CPNG m_png;
            pic_data out;
            unsigned char* dst = mcvGetImageData(srcGary);
            //将rgb转rgba
            out.width=srcGary->width;
            out.height=srcGary->height;
            //printf("121212\n");
            out.rgba = new unsigned char[srcGary->width*srcGary->height*3];
            memset(out.rgba,0,srcGary->width*srcGary->height*3);
            memcpy(out.rgba,dst,srcGary->width*srcGary->height*3);
            out.alpha_flag = 0;
            out.bit_depth=8;
            int pos = 0;

            //qDebug("data is %s,%s",out.rgba[0],out.rgba[1]);
            m_png.write_png_file(filePathLater,&out);
            free(out.rgba);
            mcvReleaseImage1(srcGary);
            srcGary=NULL;
        }
        else
        {
             mcvSaveImage(filePathLater,srcGary,200,200);
             mcvReleaseImage1(srcGary);
             srcGary=NULL;
        }
        if(imgParam.nCropType==2||imgParam.nCropType==3)
        {
           // szFilePath = szFilePath + ";"

            strcat(szFilePath,filePathLater);
            strcat(szFilePath,";");
        }
        else {
           strcat(szFilePath,filePathLater);
        }
        mcvReleaseImage1(srcCut);
        srcCut=NULL;

    }
    if(src)
     {
         mcvReleaseImage1(src);
         src = NULL;
     }

}
 char* CImageHelper::substrend(char * str, int n)
 {
     char * substr = (char*) malloc (n+1);
     int length = strlen(str);
     if(n >= length)//若截自取长度大百于字符度串长度，则问直接截取全部答字符串
     {
     strcpy(substr, str);
     return substr;
     }
     int k = 0;
     for(int i = strlen(str) - n - 1; i < strlen(str); i++)
     {
     substr[k++] = str[i];
     }
     substr[k] = '\0';
     return substr;
 }
 char* CImageHelper::substrendR(char * str, int n)
 {
     char * substr = (char*) malloc (n+1);
     int length = strlen(str);
     if(n >= length)//若截自取长度大百于字符度串长度，则问直接截取全部答字符串
     {
     strcpy(substr, str);
     return substr;
     }
     int k = 0;
     for(int i = 0; i < n; i++)
     {
     substr[k++] = str[i];
     }
     substr[k] = '\0';
     return substr;
 }
void CImageHelper::UnInitImageSDK()
{


}

