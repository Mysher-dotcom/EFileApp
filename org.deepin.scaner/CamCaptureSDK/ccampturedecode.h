#ifndef CCAMPTUREDECODE_H
#define CCAMPTUREDECODE_H

#include "Jpeg.h"
class CCamptureDecode
{
public:
    CCamptureDecode();
    long readBufFromJpegBuf(uchar * jpgBuf,long lsize,long &lWidth,long &lHeight,uchar * * bgrBuff);
    long saveImageToJpeg(uchar * src,int width,int height,int bpp,char * filePath,JPEGInfo jpgInfo);
    long getJPEGInfoFromImage(uchar * src,JPEGInfo &jpgInfo);
    long readBufFromJpeg(char * filepath,uchar * * bgrBuff,JPEGInfo &jpgInfo,int desW,int desH);
    long Cam_saveImageFormJpeg(uchar * src,int width,int height,int bpp,char * filePath,JPEGInfo jpgInfo);
    long Cam_saveImageFormJpegBuf(uchar * src,int width,int height,int bpp,uchar ** filePathBuf,JPEGInfo jpgInfo);
    CJpeg jpeg;
};

#endif // CCAMPTUREDECODE_H
