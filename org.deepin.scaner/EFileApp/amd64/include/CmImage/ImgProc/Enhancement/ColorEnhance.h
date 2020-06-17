#pragma once
#include "stdafx.h"
/*彩色图像增强
*版本号：V0.0.4		bicycle
*自动彩色增强，根据白色背景的多少分为彩色保真，背景增亮两种模式
*V0.0.5 图像黑白化
*/
class CColorEnhance
{
public:
//彩色图像增强(保真)
//[in]src			输入图像
//[out]dst			输出图像
//[in]bool			是否自动检测白色背景(白色背景：实现背景增强否则实现颜色保真)
// -1				自动检测
// 0				增亮
// 1				保真
static bool colorEnhance(Mat src,Mat &dst,int isAutoDetectWhiteBackGroud = -1);

//图像阴影增强[仅支持彩色图像]
//[in]src			输入图像
//[out]dst			输出图像
static bool shadowEnhance(const Mat &src,Mat &dst);

//图像Hue色调调节
//[in]src			输入图像
//[out]dst			输出图像
//[in]adjustScope	
//-100~100			调节范围
static bool adjustHue(const Mat &src,Mat &dst,int adjustScope =0);

//图像黑白化
//[in]src			输入图像
//[out]dst			输出图像
//[in]kerner_size	滤波器尺寸
static Mat whiteAndBlack(const Mat &src,const int kernerl_size=41);

private:
//图像增强
//[in/out]src		输入/输出图像
//[in]mask			模板
static bool colorEnhanceWithShadow(Mat &src,Mat mask,Mat mask1);

public:
//找寻图像暗/亮通道
//[in]src			输入图像
//[in/out]dst		输出图像
//[in]isDarkChannel	是否是寻找暗通道
static bool findExtremeImg(const Mat &src,Mat &dst,Mat &dst1,bool isDarkChannel = true);
};

