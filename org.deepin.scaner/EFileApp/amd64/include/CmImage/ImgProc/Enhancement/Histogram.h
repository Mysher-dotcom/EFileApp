#pragma once
#include "stdafx.h"
//直方图操作
//Max 2016-08-23 14:13:56

#ifndef RGBQUAD
    typedef struct tagRGBQUAD{
        unsigned char rgbBlue;
        unsigned char rgbGreen;
        unsigned char rgbRed;
        unsigned char rgbReserved;
    }RGBQUAD;
#endif

class CHistogram
{
public:
	//绘制灰度直方图
	static bool DrawHistogram(Mat src, Mat &dst);

	//直方图log算法
	//实现Mat接口
	//参数:
	//[in] src		:	源图像	
	//返回值:
	//处理后图像
	static Mat HistogramLog(Mat src,bool isCC =false);

	//直方图log算法
	//从CxImage移植
	//Max 2014-04-23 14:32:26
	static bool HistogramLog(IplImage *src, IplImage *dst );

	//直方图Root算法
	//从CxImage移植
	//Max 2015-02-09 16:02:25
	static bool HistogramRoot(IplImage *src, IplImage *dst );
	static Mat HistogramRoot(Mat src,bool isCC = false);

private:
	static RGBQUAD RGBtoYUV(RGBQUAD lRGBColor);
	static RGBQUAD YIQtoRGB(RGBQUAD lYIQColor);
	static RGBQUAD YUVtoRGB(RGBQUAD lYUVColor);
};

