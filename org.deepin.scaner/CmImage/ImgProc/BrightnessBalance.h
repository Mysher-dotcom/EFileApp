#pragma once
#include "stdafx.h"
//V1.0.2
//光线均匀化算法
//用于文档处理
//Max 2015-09-30 14:00:52
//V1.0.4 封装整理 10.25

class CBrightnessBalance
{
public:
	CBrightnessBalance(void);
	~CBrightnessBalance(void);

	//计算图像中的亮点位置
	//[in]src			输入图像
	//[in]fratioTop		比例(亮点比例)
	//[in]fratioBot		比例(暗点比例)
	static Point findMaxThreshold1(const Mat &src,float fratioTop = 0.2,float fratioBot = 0.2);


};

