#pragma once
//智能二值化,算法出自CxImage700，移植到OpenCV231，效果相同，速度更快
//Max 2014-03-21 15:27:52  V0.0.1
#include "stdafx.h"

class CAdaptiveThreshold
{
#ifndef RECT
 typedef struct tagRECT
 {
        long left;
        long top;
        long right;
        long bottom;
 }RECT;
#endif

//方法
///////////////////////////////////////////////////////////////////
public:
	//局部阈值
	//参数:
	//[in] src		:	源图像	
	//[in] nMethod	:	求阈值方式
	//			0	:	采用后面4种方式加权值（对于颜色较淡的图像有奇效）
	//			1	:	Ostu 方法【XZhang 01】【Xia 99】
	//			2	:	Kittler 和 Illingworth 方法【Kittler and Illingworth 86】
	//			3	:	最大熵
	//			4	:	位差
	//[in] pBox		:	区域
	//返回值:
	//二值化后图像
	static Mat AdaptiveThreshold(const Mat &src, int method = 0, int nBoxSize = 64, int nBias = 0, float fGlobalLocalBalance = 0.5f);

    //最佳阈值
    //参数:
    //[in] src		:	源图像
    //[in] nMethod	:	求阈值方式
    //			0	:	采用后面4种方式加权值（对于颜色较淡的图像有奇效）
    //			1	:	Ostu 方法【XZhang 01】【Xia 99】
    //			2	:	Kittler 和 Illingworth 方法【Kittler and Illingworth 86】
    //			3	:	最大熵
    //			4	:	位差
    //[in] pBox		:	区域
    //返回值:
    //二值化阈值
    static int  OptimalThreshold(IplImage * src,int method=0,RECT * pBox=NULL);

    //局部阈值
    static bool AdaptiveThreshold(IplImage* src,IplImage* dst,int method = 0, int nBoxSize = 64, int nBias = 0, float fGlobalLocalBalance = 0.5f);
private:
    //根据mask，对每个像素进行阈值化
    //mask尺寸和原图相同
    static bool Threshold(IplImage* src,IplImage* dst,IplImage* mask);
};

