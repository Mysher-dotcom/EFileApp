#ifndef CMIMAGE_GLOBAL_H
#define CMIMAGE_GLOBAL_H


#include "stdafx.h"


#include <iostream>
using namespace std;

extern "C"
{
    //初始化
    //参数:
    //无
    //返回值:
    //true: 成功
    //false: 失败
    bool mcvInit();

    //获取版本号
    string mcvGetVersion();

    //加载图像
    //参数:
    //无
    //[in] chImgPath: 源图像路径
    //返回值:
    //MImage对象指针,如果失败则为0
    MImage* mcvLoadImage(char* path,long w_Dest=0, long h_Dest=0);

    //克隆MImage对象指针（深拷贝）,对象内存与src无关
    //参数:
    //[in] src: 源图像指针
    //返回值:
    //MImage对象指针,如果失败则为0
    MImage* mcvClone(MImage* src);

	//从内存创建MImage对象指针,对象内存与data无关
	//参数:
	//[in] width: 源图像宽度
	//[in] height: 源图像高度
	//[in] channel: 源图像颜色通道数量,支持1,3（默认位深度为8）
	//[in] data: 源图像内存
	//[in] bFlip: 是否需要对内存做Flip
	//返回值:
	//MImage对象指针,如果失败则为0
	MImage* mcvCreateImageFromArray(int width, int height, int channel, char* data, bool bFlip=false);

    //释放图像,所有MImage对象指针必须通过此函数释放否则内存泄漏
    //参数使用源图像指针的指针,如此可以将用户声明的Mimage对象指针置为0,调用ReleaseImage后,可以通过"if(pt == 0)"判断是否已经释放内存
    //参数:
    //[in] src: 源图像指针的指针
    //返回值:
    //无
    void mcvReleaseImage1(MImage* src);
    uchar* mcvGetImageData(MImage *src);
    //保存图像
    //参数:
    //[in] sz: 存储路径
    //[in] src: 源图像
    //[in] xDPI: X方向DPI数值
    //[in] yDPI: Y方向DPI数值
    //[in] jpgQuanlity: JPG压缩质量（0-100）
    //[in] bBW: 是否保存为真的二值化图像（位深度为1）//返回值:
    //true: 成功
    //false: 失败
    bool mcvSaveImage(char* sz, MImage* src,int xDPI = -1,int yDPI = -1,int jpgQuanlity = 50, bool bBW = false);

	//图像上绘线
	//img			图像指针
	//colorValue	颜色值(char[3])单通道，双通道，后面的颜色值无关紧要
    //pt			起止点s(Point[2])
	//imgInfo		图像信息(int[3]分别对应：height,width,channel,line size)
	void mcvDrawLineOnImageBuffer(char *Img,char *colorValue,MPoint *pt,int *imgInfo);

    //将源对象灰度化,返回目标对象内存与源对象独立
    //参数:
    //[in] src: 源图像
    //返回值:
    //MImage对象指针,如果失败则为0
    MImage* mcvGrayStyle(MImage *src);

    //局部自适应最佳阈值二值化（图像为单通道,每个通道为1个字节,即常见灰度格式）
    //参数:
    //[in] src: 源图像
    //返回值:
    //MImage对象指针,如果失败则为0
    MImage* mcvAdaptiveThreshold(MImage* src);

    //从源图像中检测出矩形目标（如文档）,俗称裁切纠偏,默认背景为黑色,目标边缘不为黑色,俗称裁切纠偏,算法基于轮廓检测,返回检测出的矩形区域其倾斜角度和4个顶点
    //参数:
    //[in] src: 源图像
    //[out] fAngle: 倾斜偏移角度,单位为°
    //[out] ptMPoint: 检测矩形区域的4个顶点
    //[in] rectROI: 关注区域,当其不为0时,只对区域内部分进行计算
    //返回值:
    //true: 成功
    //false: 失败
    bool mcvDetectRect(MImage *src,float &fAngle, MPoint *ptMPoint, MRect rect = MRect(0,0,0,0));
    //图像裁切(任意角度)
    //参数:
    //[in] src: 源图像
    //[in] mRect: 表示包含裁切图像的左右上下位置四个参数的矩阵
    //返回值:
    //MImage对象指针,如果失败则为0
    MImage* mcvCutR(MImage* src,MRectR mrect);
}
#endif // CMIMAGE_GLOBAL_H
