#pragma once
#include "stdafx.h"
//#include "DetectEdge.h"
//文档背景分割
//V1.0.0 创建
//Max 2016-11-10 11:39:16

class CDocumentBackgroundSeparation
{
public:

	//文档背景分割
	//参数:
	//[in]matSrc:	源图像
	//返回值:
	//无效Mat:	操作失败
	//有效Mat:	分割后的二值图像（其实是灰度，但颜色为二值，0-前景，255-背景）
	static cv::Mat DocumentBackgroundSeparation(cv::Mat matSrc);
};

