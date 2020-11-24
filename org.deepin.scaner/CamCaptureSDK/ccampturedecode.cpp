#include "ccampturedecode.h"

CCamptureDecode::CCamptureDecode()
{

}
long CCamptureDecode::readBufFromJpegBuf(uchar * jpgBuf,long lsize,long &lWidth,long &lHeight,uchar * * bgrBuff)
{
    return jpeg.readBufFromJpegBuf(jpgBuf,lsize,lWidth,lHeight,bgrBuff);
}
long CCamptureDecode::saveImageToJpeg(uchar * src,int width,int height,int bpp,char * filePath,JPEGInfo jpgInfo)
{
    return jpeg.saveImageToJpeg(src,width,height,bpp,filePath,jpgInfo);
}
long CCamptureDecode::getJPEGInfoFromImage(uchar * src,JPEGInfo &jpgInfo)
{
    return jpeg.getJPEGInfoFromImage(src,jpgInfo);
}
long CCamptureDecode::readBufFromJpeg(char * filepath,uchar * * bgrBuff,JPEGInfo &jpgInfo,int desW,int desH)
{
    return jpeg.readBufFromJpeg(filepath,bgrBuff,jpgInfo,desW,desH);
}
long CCamptureDecode::Cam_saveImageFormJpeg(uchar * src,int width,int height,int bpp,char * filePath,JPEGInfo jpgInfo)
{
    return jpeg.saveImageToJpeg(src,width,height,bpp,filePath,jpgInfo);
}
long CCamptureDecode::Cam_saveImageFormJpegBuf(uchar * src,int width,int height,int bpp,uchar ** filePathBuf,JPEGInfo jpgInfo)
{
    return jpeg.saveImageFormJpegBuf(src,width,height,bpp,filePathBuf,jpgInfo);
}
