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
	//上下极阈值
	typedef pair<int,int> extremeValue;

	static bool BrightnessBalance(IplImage* srcImage);


	//黑白文档图像背景移除(分块法进行处理):nSize >=3 && src.rows >3 && src.cols >3
	//nSize:图像分块的大小
	//[in] preproccessType
	// 0 不做预处理
	// 1 log
	// 2 root
	static void AdaptiveMakeBeautiful(Mat &src,int nSize =64,int preproccessType =0);

	//图像文档化/背景移除/整体提亮算法
	//[in/out]src			输入输出图像
	//[in]nSize				分块尺寸的大小
	//[in]preProccessType	预处理的选择
	//	0	不做处理
	//	1	log
	//	2	root
	static void movingLevel(Mat &src,int nSize=64,int preProccessType =0);
public:
	CBrightnessBalance(void);
	~CBrightnessBalance(void);

	//计算图像sobel梯度
	//[in]src			输入图像
	static Mat im2bw(const Mat src);

	//计算图像中最佳阈值
	//[in]src			输入图像
	static int OptimalThreshold(const Mat src);

	//计算图像中的亮点位置
	//[in]src			输入图像
	//[in]fratioTop		比例(亮点比例)
	//[in]fratioBot		比例(暗点比例)
	static Point findMaxThreshold1(const Mat &src,float fratioTop = 0.2,float fratioBot = 0.2);
	static extremeValue findMaxThreshold(const Mat &src,float fratioTop = 0.2,float fratioBot = 0.2);

	//预处理
	//[in/out]src			输入/输出图像(log/root操作会改变原图)
	//[in]preProccessType	预处理的选择
	//	0	不做处理
	//	1	log
	//	2	root
	//返回值：Mat --> 相应的灰度图像
	static Mat preProcess(Mat &src,int preProccessType);

	//获取阈值模板
	//[in]src			输入图像
	//[in]nSize			分块尺寸的大小
	//返回值:阈值模板图像
	static Mat createMask(Mat gray,int nSize);

	//获取查找表，和调整后的模板
	//[in]mask			输入模板
	//[in]src
	//[out]dst			返回调整后的模板
	//返回值：查找表
	static vector<vector<uchar> > getColorTable(const Mat &mask,const Mat &src,Mat &dst);

	//调整替换图像
	//[in/out]src		输入/输出图像
	//[in]matThr		控制模板
	static void adjustImage(Mat &src,const Mat &matThr,vector<vector<uchar> > colorTable);
	
	//获取阈值
	//[in]pt				图像上下阈值
	//[in]loc				图像位置
	//[in]leftThreshold		左侧阈值
	//[in]topThreshold		上侧阈值
	static int getThresholds(Point pt,Point loc,Size maskSize,int nglobalThreshold,int leftThreshold,int topThreshold);
};

