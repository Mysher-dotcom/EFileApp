#pragma once
#include "stdafx.h"
//Version 0.1.2  优化算法速度
//Version 0.1.3  饱和度BUG更改
//Version 0.1.4  锐化算法的内存占用优化

class ImgAdjust
{
public:
	ImgAdjust(void);
	~ImgAdjust(void);
public:
	//色调饱和度
	//参数：
	//[in] src :输入图像
	//[in] color：0—彩色； 1—R；2—G;3—B;
	//[in] hue：色彩0~360
	//[in] saturation：饱和度 0~200
	//[in] brightness：亮度 0~200
	//返回值：
	//[out]  dst：输出图像
	static Mat HslAdjust(Mat src,int color=0, int hue=180, int saturation=100, int brightness=100);


	//锐化
	//参数：
	//[in] src :输入图像
	//[in] sigma：0~500;
	//[in] amount : 0~100
	//[in] thresholds：0~255

	//返回值：
	//[out]  dst：输出图像
    static Mat Unsharp(const Mat &src,float sigma = 3.0,int nAmount= 200,int thresholds = 1);

	//彩色平衡
	//参数：
	//[in] imageSource: 输入图像
	//[in] KR: 红色分量比例 0~200
	//[in] KG：绿色分量 比例  0~200
	//[in] KB：红色分量比例  0~200
	//返回值：
	//[out]: dst :输出图像
	//2017-07-07 17:47:06
    static Mat ColorBlance(Mat imageSource,  int KR = 100, int KG= 100,int KB= 100);

	//曲线调整
	//参数：
	//[in] src: 输入图像
	//[in]  z :返回拟合曲线
	//[in] x :添加点的x坐标
	//[in] y :添加点的y坐标
	//[in] AdjustChannel: 通道参数 0—彩色； 1—R；2—G;3—B;
	//返回值：
	//[out]: dst :输出图像
	//2017-07-13 09:49:59
    static Mat MyCurves(Mat src,double z[256],int x,int y,int AdjustChannel = 0);

	//阴影高光
	//参数：
	//[in] src: 输入图像
	//[in]  amount :参数调整
	//[in] saturation :色调
	//返回值：
	//[out]: dst :输出图像
	//2017-07-27 11:41:51
	static Mat ShadowHighlight(Mat src,int amount,int saturation);

	//去脏污
	//参数：
	//[in/out] src: 输入/输出图像
	//[in]  mask :参数半径（默认取值为3）
	//[in] contoursize :脏污大小 (一般小脏物点取值为40，划痕类赃物取220左右)
	//返回值：bool
	static bool RmoveDust(Mat &src,int mask = 3,int contoursize = 40);

	//图像去色：
	//[in/out]src	：	输入/输出图像
	//[in]indexOfHue:	输入颜色范围[0,360]
	//[in]scopeRadius:	颜色范围半径[0,180]
	//[in]enHanceLightness: 亮度增加程度[0,200]
	//注：限定彩色图像，灰色黑白图像没有效果;颜色半径是两头并进，180的时候刚好图像颜色可以完全滤除
	//颜色大致范围：360°/0°红、60°黄、120°绿、180°青、240°蓝、300°洋红
	static void colorDropout(Mat &src,int indexOfHue,int scopeRadius=5,int enHanceLightness =0);
	static Mat colorDropout1(const Mat &src,int nColorChannel,int nFilterThreshold);

	//图像锐化
	static bool sharp(Mat src,Mat &dst);
};

