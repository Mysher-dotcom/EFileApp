#pragma once
#include "stdafx.h"
//检测目标边缘：
//V1.0.0
//参数设置 0：X方向Sobel;
//         1：Y方向Sobel;
//         2：整体方向Sobel;
//         3：Laplace变换;
//         4：Canny检测(固定参数);
//Jessie 2016-07-04 19:48:52
//V1.1.0
//添加基于高斯核函数的边缘检测算法
//Max 2016-11-14 11:31:52
class CDetectEdge
{
public:
//边缘检测
//参数：
//[in]Src		：	源图像
//[in]nMethod	：	边缘检测方式， 具体如下
//		0		：	X方向Sobel;
//		1		：	Y方向Sobel;
//		2		：	整体方向Sobel;	
//		3		：	Laplace变换;
//		4		：	Canny检测(固定参数);
//返回值：
//无效Mat		：	失败
//Mat			：	边缘检测图像
static Mat DetectEdge(Mat Src,int nMethod = 2);

//边缘检测基于，高斯核函数
//参数：
//[in]src		：	源图像
//[in]nSize		：	高斯核尺寸
//返回值：
//无效Mat		：	失败
//Mat			：	边缘检测图像
static Mat DetectEdge_Gaussion(Mat src,int nSize = 31);
};
