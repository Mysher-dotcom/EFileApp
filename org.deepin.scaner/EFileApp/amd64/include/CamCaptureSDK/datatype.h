#ifndef DATATYPE_H
#define DATATYPE_H

#include <list>
#include <string>
#include <map>
#include <vector>
#include <stdint.h>
#include <iostream>
#include <numeric>
#include <algorithm>
using namespace std;
#include <QMetaType>
#include <qstring.h>
#include "MImage.h"
//图像列表Item数据
typedef struct{
    QString picPath;//图像全路径
    QString picName;//图像名称
}PicItemData;

Q_DECLARE_METATYPE(PicItemData)
typedef int (*callBackAutoCapture)(long nState);
typedef int (*CameraCpatureReciveCallback)(void *data, int size,int w,int h,int nFormatType);
typedef struct {
  char* start;
  size_t length;
} buffer_t;
typedef struct CameraWidthAndHeight
{
    int CameraWidth;
    int CameraHeight;
}CameraWH;

typedef struct IMAGEPARAM{
    //0-no crop 1-autocrop 2-Mutilcrop 3-book 4-ManualCrop
    int nCropType;
    MPoint mp4[4];//手动裁切
    //文档优化
    bool bIsTextEn;
    int nRotate;
    //颜色类型（彩色、灰度、黑白）
    int nColorType;
    //水印
    bool bIsWater;
    bool bIsWaterImage;//是否支持图片水印，true-图片 false-字体
    char* szWaterImagePath;//图片水印路径
    int x;//t图片水印位置
    int y;
    //水印颜色
    int nR;
    int nG;
    int nB;
    //水印字体
    int nFont;
    //水印内容
    char* szWaterContent;
    //图像buf、宽、高、size
    unsigned char* buffer;
    int nWidth;
    int nHeight;
    int nSize;
    //彩色优化
    bool bColorBlance;
    //红印文档
    bool bRedEn;
    //反色
    bool bInverse;
    //灰度
    //bool bGray;
    //黑白
    //滤红
    bool bColorDropOut;
    //去噪
    bool bNoise;
    //缺角修复
    bool bFillBorder;
    //拍书
    //bool bIsBook;
    //多图裁切
    //bool bMutilCrop;
    //去手指
    bool bRemoveFinger;
    //支持连拍
    bool bAutoCampture;
}ImageParam;

struct barcodeInfo{
    int nbarcodeType;
    char cbarcodeRet[1024];
    int x[4];
    int y[4];
};
struct barcodeInfoList
{
    barcodeInfo infos[20];
    int nbarcoderCount;
};

#endif // DATATYPE_H
