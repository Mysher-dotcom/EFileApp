#pragma once
#include "stdafx.h"
//V 0.1.0 Bicycle  初版调整   Time:2018年1月22日10:13:15
//V 0.1.1Tmp Bicycle 提交版(未做整理)  Time:2018年1月24日14:34:52
//V0.1.3 Time:2018年2月1日13:35:38

//循环填充需要的计数工具
struct SLoop4FloodFill
{
	//Y方向计数
	int nY_Start;
	int nY_End;
	int nY_Step;
	//X方向计数
	int nX_Start;
	int nX_End;
	int nX_Step;

	SLoop4FloodFill(int nY_Start, int nY_End, int nY_Step,int nX_Start, int nX_End, int nX_Step )
	{
		this->nY_Start = nY_Start;
		this->nY_End = nY_End;
		this->nY_Step = nY_Step;

		this->nX_Start = nX_Start;
		this->nX_End = nX_End;
		this->nX_Step = nX_Step;
	}
};

struct SLoopData
{
public:
	int m_nStart;
	int m_nEnd;
	int m_nStep;

	int m_nStart2;
	int m_nEnd2;
	int m_nStep2;

	SLoopData(int nStart, int nEnd, int nStep,
		int nStart2, int nEnd2, int nStep2)
	{
		this->m_nStart = nStart;
		this->m_nEnd = nEnd;
		this->m_nStep = nStep;

		this->m_nStart2 = nStart2;
		this->m_nEnd2 = nEnd2;
		this->m_nStep2 = nStep2;
	}
};

class CDetectRectBySegmation
{
public:
	
	//裁切纠偏
	//参数:
	//[in] src:分割图像，24bit
	//返回值:
	//在原图上以红色框选出检测区域
	static Mat DetectRectPreview(Mat src);

	//裁切纠偏
	//参数:
	//[in] src:分割图像，24bit
	//返回值:
	//矩形位置向量
	static vector<RotatedRect> DoDetectRect(Mat src);

	//裁切纠偏Beta
	//参数:
	//[in] src:分割图像，24bit
	//返回值:
	//矩形位置向量
	static vector<RotatedRect> DoDetectRect2(Mat src);

private:
	//背景颜色
	const static int m_nThBackGround = 70;
private:
	//分割背景
	//参数:
	//[in] src:原图
	//返回值:
	//标记分割结果的图像，尺寸等于原图，位数和原图相同，绿色（0，255，0）表示背景，前景不变
	static Mat BgSegmation(Mat src);

	//查找矩形目标
	//参数:
	//[in] src:背景分割图像，8bit，黑色表示背景，白色表示前景
	//返回值:
	//矩形位置向量
	static vector<vector<cv::Point> > DetectRect(Mat src);

	//对粗分区域进行进一步细分
	//参数:
	//[in] rect:粗分区域
	//[in] img:灰度图
	//返回值:
	//精分区域（4个顶点）
	static vector<Point> AdjustRect(vector<Point> rect, Mat img);

	//通过漫水填充法分割背景
	//参数:
	//[in] src:原图
	//[in] nLoUpDiff:亮度差异浮动值,插值在此以内，都认为是相同颜色，默认为3
	//返回值:
	//标记分割结果的图像，尺寸等于原图，8bit，黑色表示背景，白色表示前景
	static Mat BgSegmationByFloodFill(Mat src,int nLoUpDiff = 2);

	//通过轮廓查找矩形目标
	//参数:
	//[in] src:分割图像，8bit，黑色表示背景，白色表示前景
	//返回值:
	//矩形位置向量
	static vector<vector<cv::Point> > DetectRectByContours(Mat src);

	//通过旋转，然后检测边缘对粗分区域进行进一步细分
	//参数:
	//[in] rect:粗分区域
	//[in] img:灰度图
	//返回值:
	//精分区域（4个顶点）
	static vector<Point> AdjustByRotate(RotatedRect rect, Mat img);

	//通过遍历直线对粗分区域进行进一步细分
	//参数:
	//[in] rect:粗分区域
	//[in] img:灰度图
	//返回值:
	//精分区域（4个顶点）
	static vector<Point> AdjustByFindPoint(vector<Point> rect, Mat img);

	//获取直线上所有的点
	//采用二分法
	//参数:
	//[in] start:起始点坐标
	//[in] end:终止点坐标
	//[out] save:直线上点的集合
	//返回值:
	//无
	static void find_all_point(Point start, Point end, vector<Point> &save);

	//填充RotatedRect（带有角度的矩形）
	//通过找到矩形两条平行边上所有的点，并用直线将平行线对应的点连接，以达到填充效果
	//参数:
	//[in/out] image:源图像
	//[in/out] rect:带有角度的矩形的位置
	//[in/out] color:填充颜色
	//返回值:
	//无
	static void full_rotated_rect(Mat &image, const RotatedRect &rect, const Scalar &color);  

	//查找图像中白色区域
	//参数:
	//[in] src:二值后灰度图像，8bit，图像中含黑色背景和白色前景，一般情况，前景为矩形且无角度
	//返回值:
	//白色区域位置
	static cv::Rect FindWhiteRectInBW(Mat src);

	//旋转
	//参数
	//[in] ptArray:需要旋转的点集
	//[in] angle:旋转角度，单位°
	//[in] width:图像宽度
	//[in] height:图像高度
	//[out] dstWidth:旋转图像宽度
	//[out] dstHeight:旋转图像高度
	//返回值:
	//旋转后的点集
	static vector<Point> rotatePTArray(vector<Point> ptArray, float angle, int width, int height, int &dstWidth, int &dstHeight);

	//查找4个点中的左上角
	//一般而言，4个点关系为矩形的4个顶点，矩形带角度
	//参数
	//[in] ptArray:矩形的4个顶点
	//返回值:
	//左上角
	static Point findPT(vector<Point> ptArray);

	//查找图像中白色区域
	//通过直线逼近的方式
	//参数:
	//[in] src:二值后灰度图像，8bit
	//[in] nTh:二值化阈值
	//[in] nType:目标类型,0-白色，1-黑色
	//返回值:
	//白色前景位置
	static vector<Point> FindWhiteRectInBW2(
			Point ptL1_Start, Point ptL1_End, 
			Point ptL2_Start, Point ptL2_End,
			Mat src, int nTh, int nType);

	//删除包含轮廓
	//[in]vRRect:输入矩形
	//返回筛选后矩形
	static vector<RotatedRect> DoSuperposition(vector<RotatedRect> vRRect);

	//计算背景色
	//src:输入图像
	//返回背景颜色值
	static int calculateBackGroundValue(const Mat &src);

	//计算点到直线的距离
	//pt1:点
	//lpt1，lpt2:直线的端点
	static int calculatePoint2Line(Point2f pt1,Point2f lpt1,Point2f lpt2);

	//调整矩形
	//[in/out]输入/输出矩形:rrect
	static void adjustRectangle(RotatedRect &rrect);

	//计算坐标点是否在矩形内
	//ptf4:矩形四个坐标点
	//ptf:坐标点
	//fAngle:向量旋转角度
	static bool isPointInRect(Point2f *ptf4,Point2f ptf,float fAngle =0);
};

