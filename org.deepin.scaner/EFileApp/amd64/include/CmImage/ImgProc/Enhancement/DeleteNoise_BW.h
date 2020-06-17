#pragma once
#include "stdafx.h"
//类说明:
//功能：去除杂点[分为三档:low,medium,high]
//限制：仅限二值化图(B/W)
//版本号：V0.0.1	Time:2017年4月13日15:54:58  Bicycle

class CDeleteNoise_BW
{
public:
	//二值图取杂点
	//[in/out]src		输入/输出图像
	//[uint]nMethod		程度(非以下情况按low处理)
	//	0				low
	//	1				medium
	//	2				high
	//返回值：
	//	0				正常运行
	//	1				不支持的图像尺寸
	//	2				不支持的图像类型
	//  -1				未能正常结束
	static int DeNoise(Mat &src,int nMethod=0);

private:
	//杂点去除
	//[in/out]src		输入/输出图像
	//[in]nSize			窗口大小[>=3]
	//[in]nType			调用核类型
	static vector<vector<Point> > DeNoise_core(Mat &src,uint nSize,int nType =0);

	//低处理程度
	//[in/out]src		输入/输出图像
	//[in]nSize			窗口大小[>=3]
	static bool DeNoiseDegree_low(Mat &src,uint nSize);

	//中等处理程度
	//[in/out]src		输入/输出图像
	//[in]nSize			窗口大小[>=3]
	static bool DeNoiseDegree_medium(Mat &src,uint nSize);
};

