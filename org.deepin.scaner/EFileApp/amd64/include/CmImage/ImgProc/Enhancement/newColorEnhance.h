#pragma once
#include "stdafx.h"
//彩色增强算法
//Tim.liu 2019-12-26 10:20  V0.0.1
//Tim.liu 2020-2-6 17:00  V0.0.2 优化书本黑白文档直接黑线，优化光照不均匀出现阴影；
//Tim.liu 2020-2-7 14:22  V0.0.3 优化光照不均匀出现阴影；
//Tim.liu 2020-2-21 14:22  V0.0.4 新增彩色增强算法；
//Tim.liu 2020-3-9 9:32  V0.0.5 文档优化算法更新；
//Tim.liu 2020-3-12 15:15  V0.0.6 文档优化算法出来纯白，纯黑图像问题解决；


class CnewColorEnhance
{
//方法
///////////////////////////////////////////////////////////////////
public:

	//************************************************************************  
	// 函数名称:    black_whiteEnhance
	// 函数说明:    黑白文档增强算法
	// 函数参数:    Mat src    输入RGB原图像
	// 返 回 值:	Mat	   处理后图像
	//************************************************************************ 
	static  Mat black_whiteEnhance(cv::Mat src, bool type = false);

	//************************************************************************  
	//Adaptive Contrast Enhancement（自适应对比度增强，ACE）
	//函数功能：获取图像的局部均值与局部标准差的图
	//函数名称：adaptContrastEnhancement
	//函数参数：Mat &scr：输入图像，为三通道RGB图像；
	//函数参数：Mat &dst：增强后的输出图像，为三通道RGB图像；
	//函数参数：float fratioTop：比例；
	//函数参数：float fratioBot：比例；
	//返回类型：bool
	//************************************************************************  
	static bool photoshopEnhancement(Mat &src, float fratioTop = 0.05 ,float fratioBot = 0.05);

	static vector<float> findThreshold(Mat src,int thresh);

	//************************************************************************  
	// 函数名称:    Color_HomoFilter 
	// 函数说明:    同态滤波彩色增强算法
	// 函数参数:    Mat src    输入RGB原图像
	// 返 回 值:	Mat	   处理后图像
	//************************************************************************ 
	static Mat Color_HomoFilter(cv::Mat src);

	static void ALTMRetinex(const Mat src, Mat &dst, bool LocalAdaptation = false, bool ContrastCorrect = true);

	 //函数功能：获取图像的局部均值与局部标准差的图
	//函数名称：adaptContrastEnhancement
	//函数参数：Mat &scr：输入图像，为三通道RGB图像；
	//函数参数：Mat &dst：增强后的输出图像，为三通道RGB图像；
	//函数参数：int winSize：局部均值的窗口大小，应为单数；
	//函数参数：int maxCg：增强幅度的上限；
	//返回类型：bool
	static bool adaptContrastEnhancement(Mat &scr, Mat &dst, int winSize,int maxCg);

	static void OnYcbcrY(Mat &workImg);
private:
	//************************************************************************  
	// 函数名称:    HomoFilter 
	// 函数说明:    同态滤波算法
	// 函数参数:    Mat src    输入单通道图像
	// 返 回 值:	Mat	   处理后图像
	//************************************************************************ 
	static Mat HomoFilter(cv::Mat src);

	 //导向滤波器
	static Mat guidedFilter(cv::Mat& I, cv::Mat& p, int r, float eps);

	static bool getVarianceMean(Mat src, Mat &localMeansMatrix, Mat &localVarianceMatrix, int winSize);

	static int ImageStretchByHistogram(IplImage *src1,IplImage *dst1);

	//计算图像sobel梯度
	//[in]src			输入图像
	static Mat im2bw(Mat src,Mat &mean_1);

	static int unevenLightCompensate(Mat &image, int blockSize);//图像亮度均化

	static bool imsubtract(Mat src1, Mat &src2);

	static void  fillHole(Mat srcBw, Mat &dstBw);
};