#pragma once
#include "stdafx.h"
class CRemoveNoise
{
public:
	//去噪
	//[in]src			输入图像
	//[out]dst			输出图像
	//[in]reps			重复次数(默认1)
	//[in]kernel		窗口大小(默认3)
	static bool removeNoise(Mat src,Mat &dst,int reps =1,int kernel =3);
};

