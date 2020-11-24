#pragma once
#include "stdafx.h"
//PS图像特效效果
//V0.0.1 创建
//Max 2016-11-23 10:59:46
class CFilterEffect
{
public:
	//怀旧风格
	static Mat nostalgic(Mat src);

	//水波效果
	static Mat waterEffect(Mat src);

	//波浪效果
	static Mat waveEffect(Mat src);

	//素描效果
	static Mat sketchEffect(Mat src);

	//素描效果
	static Mat sketchEffect2(Mat src);

	//浮雕效果
	static Mat reliefEffect(Mat src);

	//渐变映射 
	static Mat shadeEffect(Mat src);

	//照亮边缘
	static Mat glowingEdgeEffect(Mat src);

	//色调映射 
	static Mat shade2Effect(Mat src);

	//马赛克
	static Mat mosaicEffect(Mat src);

	//曝光过度
	static Mat overExposureEffect(Mat src);

	//旋转模糊
	static Mat spinBlurEffect(Mat src);

	//碎片特效
	static Mat fragmentEffect(Mat src);

	//凹陷
	static Mat ellipsoidEffect(Mat src);

	//突出
	static Mat ellipsoid2Effect(Mat src);

	//漩涡
	static Mat vertexEffect(Mat src);

	//黑白
	static Mat blackSideEffect(Mat src);

	//油画效果
	static Mat painterly(Mat src);
	static Mat painterly2(Mat src);
};

