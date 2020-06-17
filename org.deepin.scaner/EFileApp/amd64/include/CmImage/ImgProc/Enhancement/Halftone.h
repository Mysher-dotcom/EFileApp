#pragma once
#include "stdafx.h"
//网点算法
//对彩色或灰度图像进行处理，生成(二值化）灰度图
//V1.0.0
//Max 2017-11-17 16:10:07
class Halftone
{
public:
	//网点增加算法（生成二值化后灰度图8bit）
	//Halftone
	//Convert to （binary） 8 bit by Halftone
	//参数说明:
	//input:
	//nFilterMatrix:
	// 0 - DarkerPhtto
	// 1 - DarkerPhoto + Text
	// 2 - LighterPhoto
	// 3 - LighterPhot + Text
	// 4 - ErrorDiffusion
	//nEdgeThreshold:边缘增强，取值范围:(0,255]，nEdgeThreshold>0则nFilterThreshold无效
	//nFilterThreshold:取值范围:[0,40]，彩色图像判断阈值，nEdgeThreshold= 0 且彩色原图才有效
	//返回值:
	//二值化后的灰度图
	static  Mat ConvertTo1BitHalftone(Mat input, int nColorChannel = 3, int nFilterMatrix = 0, int nEdgeThreshold = 70, int nFilterThreshold = 40);

private:
	static Mat ColorConvertTo1BitWithHalfTone8x8(const Mat &input, int nColorChannel, int filterMatrix[8][8], int nFilterThreshold);
	static Mat ColorConvertTo1BitWithErrorDiffusion(const Mat &input, int nColorChannel, int nFilterThreshold);
	static Mat GrayConvertTo1BitWithHalfTone8x8(const Mat &input, int filterMatrix[8][8]);
	static Mat GrayConvertTo1BitWithErrorDiffusion(Mat input);
	static Mat EdgeEnhanceConvolutionFilter(const Mat &input, int nColorChannel, double xFilterMatrix[3][3], double yFilterMatrix[3][3], int nEdgeThreshold, int nFilterThreshold);
	static Mat EdgeEnhanceConvolutionFilterWithGray(const Mat &input, double xFilterMatrix[3][3], double yFilterMatrix[3][3], int nEdgeThreshold);
};

