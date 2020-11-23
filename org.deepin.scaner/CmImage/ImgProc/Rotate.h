#pragma once
#include "stdafx.h"
//描述：图像旋转
//说明：根据设置角度旋转图形
//版本
//V1.0.0  Max 2016-10-11 18:36:51
//V1.0.1  Bicycle 2018年1月29日18:25:25  修改四边形校正出错BUG
class Rotate
{
public:
	//四边形校正
	//在知道4个点击，将任意四边形校正为矩形
	//参数:
	//src:源图像
	//pt:顶点（顺序必须是左上，右上，左下，右下）
	//返回值:
	//校正后图像
    //static Mat keystoneCorrection(Mat src,Point pt[4]);

	//对矩形四个点进行排序
	//[in]pt	:输入四个点
	//返回值：排序后的四个点
    //static Point* sortFourPoint(Point pt[4]);

	//根据4个顶点对图像进行透视变换得到矩形图像
	//参数:
	//[in]src	:	源图像
	//[in]pt	:	4个顶点
	//返回值:
	//旋转裁切后的图像
	static Mat RotateCut(Mat src, CvPoint pt[4]);

	//顺时针旋转，旋转后图像尺寸保持不变，因此可能会丢失部分图像
	//参数: 
	//[in]src		:		源图像
	//[in]fAngle	:		顺时针角度(单位°)
	//返回值:
	//旋转后的图像
    //static Mat RotateImage(Mat src, float fAngle);

	//顺时针旋转,旋转后图像根据内容会改变尺寸
	//参数： 
	//src：		源图像
	//fAngle：	顺时针角度（单位°）
	static Mat RotateImage2(Mat src, float fAngle);

	//顺时针旋转，旋转后图像尺寸保持不变，因此可能会丢失
	//参数： 
	//src：		源图像
	//fAngle：	顺时针角度
    //static IplImage* RotateImage(IplImage* src,float fAngle);

	//顺时针旋转,旋转后图像根据内容会改变尺寸
	//参数： 
	//src：		源图像
	//fAngle：	顺时针角度（单位°）
	static IplImage* RotateImage2(IplImage* src, float fAngle);

	//旋转图像的角度
	//nClockOrention 控制图像的旋转方向
	//…………………………0：顺时针旋转
	//…………………………1：逆时针旋转
	//rtAngle 为旋转角度（单位°)
	//Bicycle 2016年6月28日13:37:17
	//支持不同颜色补边  默认黑色 2016-07-05 12:07:08 Bic
    //static IplImage* RotateImage(IplImage* src,int rtAngle,int nClockOrention,int R=0,int G=0,int B=0);
    //static IplImage* RotateImage(IplImage* src,int rtAngle,int nClockOrention,CvScalar color=CV_RGB(0,0,0));
};

