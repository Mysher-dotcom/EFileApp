#pragma once
#include "stdafx.h"
//检测空白页  
//Time:2017年5月18日10:35:20		bicycle	V0.0.2	
//Time:2018年1月25日15:45:55        bicycle V0.0.3

class CDetectionWhitePage
{
public:
	//新版检测空白页
	//参数：
	//[in]src		:	输入图像
	//[in]detectThreshold	:判断阈值
	//输出：
	//true			：	成功
	//false			：	失败
	static bool isWhitePage_new(Mat src,int detectThreshold =60);

	//检测空白页
	//参数：
	//[in]src		:	输入图像
	//[in]detectThreshold	:判断阈值
	//输出：
	//true			：	成功
	//false			：	失败
	static bool isWhitePage(Mat src,int detectThreshold =60);

private:
	//计算图像轮廓
	//[in]src						:	输入图像
	//[int]leastNumofVector			:	轮廓最少个数
	//[out]vector<vector<Point> >	:	返回轮廓
	static vector<vector<Point> > findImageContours(Mat src);

	//计算权重分
	//[in]vvPoint					:	输入轮廓集
	//[in]leastNumofVector			:	轮廓最少成员个数
	//返回值：
	//权重分[0~100]		
	static int calcuateConditionWeight(vector<vector<Point> > vvPoint,int leastNumofVector =10);

	//************************************************************************  
	// 函数名称:    bwlabel_mask   
	// 函数说明:    二值图像指定连通域搜索保留
	// 函数参数:    Mat bw    输入二值图像
	// 函数参数:    Point vpoint    输入指定坐标点
	// 返 回 值:    Mat  输出图像
	//************************************************************************ 
	static Mat bwlabel_mask(Mat &bw, Point vpoint);

	static bool grad(Mat src ,Mat &mask);
};

