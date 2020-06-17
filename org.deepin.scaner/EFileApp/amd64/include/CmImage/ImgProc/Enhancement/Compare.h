#pragma once
#include "stdafx.h"
//图像防偷换
//V0.0.1 实现功能
//Jessie 2016-10-20 16:19:54
//V0.0.3 
//1、添加AutoLeavel,均衡光照或阴影带来的噪音问题
//2、进一步去掉二值化后的噪声，添加形态学相减结果更好滤波。
//Jessie 2016-10-19 14:34:16
//Mat retinex_result(Mat gray);
class CCompare
{
public:
static bool Compare(Mat Src1,Mat Src2,int nThreshold,vector<CvRect> rect);

};