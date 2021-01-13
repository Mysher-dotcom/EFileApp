#pragma once
#include "stdafx.h"
//锐化图像
//V1.0.0
//Max 2016-11-10 17:04:44
class CSharpImage
{
public:
	//滤波核为拉普拉斯核3x3
	//遍历内存实现
	static void sharpenImage0(const cv::Mat &image, cv::Mat &result);

	//滤波核为拉普拉斯核3x3
	//filter2D实现
	static void sharpenImage1(const cv::Mat &image, cv::Mat &result);
};

