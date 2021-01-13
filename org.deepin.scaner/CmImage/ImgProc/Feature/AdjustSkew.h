#pragma once
//V0.0.1
//纠偏算法
//对文字进行纠偏
//算法来自老王
//Max 2014-11-03 11:41:22
//V0.0.2
//在实战中发现有高拍仪拍摄图像会出现光学不均匀情况
//采用OpenCv自适应二值化完成光学均匀
//Max 2015-09-20 12:12:25
//V0.0.3
//在JBY项目中，A4高拍仪出现倾斜角度一般较低
//但实战中失误比例较大，误判所产生的倾斜角度较高
//添加后期检测，过滤误判
//通过添加预判参数实现，参数默认为0（<0）,即关闭功能
//Max 2015-09-20 12:24:48
//V0.0.4
//（1）添加倾斜角度检测
//（2）添加霍夫变换参数
//Max 2016-01-21 15:10:32
#include "stdafx.h"

class CAdjustSkew
{
public:
#ifndef RECT
 typedef struct tagRECT
 {
        long left;
        long top;
        long right;
        long bottom;
 }RECT;
#endif
	struct LINEINFO 
	{
		vector<RECT> CharRectInLine;
		int ConturTop;
		int ConturBot;
	};

	struct LINESTRUCT 
	{
		CvPoint  StartPoint;
		CvPoint  EndPoint;
		CvPoint  MeanPoint;
	};

	struct LINEAngle
	{
		vector<float> rohn;
		float MeanRohn;
		float SumRohn;
	};
	static void LineClassify(CvSeq* lines,vector<LINESTRUCT>& HorizontalLine,vector<LINESTRUCT>& VerticalLine);
	static void LinePlot(IplImage*& color_dst,vector<LINESTRUCT> LineVec);
	static float GetLineAngle(vector<LINESTRUCT> HorizontalLine);
	static void AdjustSkewImage(IplImage* &img,double angle); 

	//基于直线检测的纠偏
	//算法来自老王
	//Max 2014-11-03 11:47:49
	//取消插值提升准确率
	//Max 2016-09-07 10:28:42
	//fSkewAngleMax 预判最大倾斜角度，单位°
	static float AdjustSkew(IplImage * src, float fSkewAngleMax = 0);

	//获取倾斜角度
	//int threshold 当在一条直线上的像素点数大于threshold时，才将该直线作为检测结果显示出来。该值越大，得到直线越少。
	//double param1 对概率Hough变换，它是最小线段长度.即当线段长度大于param1时，才将该线段作为检测结果显示。与上一参数类似，不过上一参数为像素数，而该参数为线段长度。
	//double param2 这个参数表示在同一条直线上进行碎线段连接的最大间隔值(gap), 即当同一条直线上的两条碎线段之间的间隔小于param2时，将其合二为一条长直线。
	//Max 2016-01-21 15:11:28
	static float getSkew(IplImage * src, 
		float fSkewAngleMax = 0,
		int threshold = 100,
		double param1 = 700,
		double param2 = 50);

	//基于直线检测的纠偏
	//参数:
	//[in]src:	源图像
	//返回值:
	//Deskew后的图像
	static Mat Deskew(Mat src);

private:
	CAdjustSkew(void);
	~CAdjustSkew(void);
};

