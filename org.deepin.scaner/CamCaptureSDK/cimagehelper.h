#ifndef CIMAGEHELPER_H
#define CIMAGEHELPER_H

#include "cmimage.h"
#include "datatype.h"
#include "ccampturedecode.h"
#include<QThread>
#include <QObject>

class CImageHelper
{
public:
    CImageHelper();
    void InitImageSDK();
    void ProcessImage(ImageParam imgParam,char* szFilePath);
    void UnInitImageSDK();
    CCamptureDecode m_decode;
    int WriteFile(unsigned char* srcData,int len,FILE * destfb);
    long AutoCropPreviewFlipAngle(unsigned char *srcData, int srcDataType,int srcDataLen, int srcDataWidth,  int srcDataHeight,
                                int &x, int &y, int &x1, int &y1, int &x2,int &y2,int &x3, int &y3, int FlipAngle = 0);

    bool DrawLine(char*ImgBuff,int &nSize,int &nWidth,int &nHeight,int nCropType);
    bool Rotate(uchar* srcBuf,int &nSize,int &nWidth,int &nHeight,int nRotateA);

    bool SaveImage(uchar* srcBuf,int nSize,int nWidth,int nHeight);
    //对比
    bool CompareImgae(char* srcData,int width,int height,int size);
     pthread_t m_ThreadCaptureID;

     char *substrend(char * str, int n);
      char *substrendR(char * str, int n);
public:
    MRectRArray m_RectArray;
private:
    static  void *AutoDectet_doing(void *ptr);
    bool bIsLoadSDK;
    MImage* m_ImageLastCompare;
    int m_nautoCaptureState;
};

#endif // CIMAGEHELPER_H
