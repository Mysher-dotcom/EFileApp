#pragma once
#include "stdafx.h"

//边缘填充：
//Jessie  2016-08-31 11:21:52
//Bicycle 2017年3月13日17:56:41 V1.0.0
//V1.0.1	bicycle		 修改部分图像二值化背景丢失
//V1.0.2	bicycle		 自动凹陷处周围最多颜色 Time 2017年3月23日14:35:54
//V1.0.3	bicycle		 增加去手指算法 Time 2017年3月28日10:41:18
//V1.0.6	bicycle		 更改暗背景下的皮肤检测条件 Time 2017年4月6日19:02:39
//V1.0.7	bicycle		 更改findSingle中vector索引越界的问题 Time 2017年4月10日15:44:30
//V1.0.8	bicycle		 支持灰度图
//V1.0.9	bicycle		 修改崩溃BUG
//V1.0.10	修改顶角处补边分割的BUG
//V1.0.13	Time:2018年6月6日
//V1.0.19	Time:2018年12月12日
class CFillBorder
{
public:
static bool FillBorder(Mat src, Mat &dst);

//V1.0.2 图像补边
//[in/out]src		输入\输出图像
//[in]offsetNum		边缘收缩量(边缘向内收缩程度)
//[in]mappingScope	收缩量波动范围
// -1				无限向内收缩
// 
//[in]isColorFill	是否纯色(默认：自动背景纯色填充)
//	-1				映射填充
//	0				纯色填充(默认白色)
//	1				自动纯色填充
// -11				【参数同上高拍仪】
// 10
// 11
//[in]color			纯色的颜色
static bool FillBorder2(const Mat &src,Mat &dst,int offsetNum = 0,int mappingScope = -1,int isColorFill =-1,Scalar color =Scalar(255,255,255));
//V1.0.14
static bool FillBorder3(const Mat &src,Mat &dst,float fRatio =1.0,bool isScanner =false,Scalar color =Scalar(-1,-1,-1),bool isMapFill = false,int mappingScope = -1);

//去手指:仅能去除全框口图像的手指
//[in]src			输入图像
//[out]dst			输出图像
//[in]offsetNum		手指范围浮动范围[-10,10]
//[in]stretchNum	手指起止范围拉伸
static bool FillFinger(Mat src,Mat &dst,int offsetNum =5,int stretchNum=0);
private:
//对曲边进行排序和补位
//V1.0.0
static bool correctCurve(vector<Point> &vPoint,int isXsort =0);

//最大轮廓//V1.0.0
//[in]vector<vector<Point> >		图像的轮廓
//[in/out]rMaxRect				轮廓的最小外接矩形				
//[out]vector<Point>			最大的轮廓
static vector<Point> findMaxContour(Mat binaryImg,Mat &binMask,RotatedRect &rMaxRect,bool isScanner = true);

//最大轮廓
static vector<Point> findMaxContour(Mat src);

//取最多的颜色
//[in]vr			红色量
//[in]vg			绿色量
//[in]vb			蓝色量
static bool findMainColor(vector<int> vr,vector<int> vg,vector<int> vb,Scalar &color);


//根据四条边的特点，将不符合规定的凹陷去除
//[in]Singleline	输入的单边
//[in]isXway		是否投影X方向
static bool findSingleLine(vector<Point> &Singleline,bool isXway = false);

//检测肤色
//[in]src			输入图像
//[in]vp			肤色检测区域
//[in]whichSide		哪个侧边
//	0		左边
//	1		下边
//	2		右边
//	3		上边
//[out]bool			是手指返回true
static bool detectSkin(Mat src,vector<Point> vp,int &whichSide );

//填充手指区域
//[in]src			输入图像
//[in]vp			输入轮廓
//[in]whichSide		手指所在幅面的位置
//[in]offsetNum		手指轮廓范围偏移量
//[in]stretchNum	手指起止点拉伸偏移量
static bool FillFingerContour(Mat &src,vector<Point> vp,int whichSide,int offsetNum,int stretchNum);

//延伸轮廓
static bool changeContour(Mat src,vector<Point> &vp,int offsetNum,int stretchNum,bool isXway =false);

//平均背景色
static bool meanBackGroundColor(vector<int> vr,vector<int> vg,vector<int> vb,Scalar &color);

//链接断点
static bool megerline(vector<Point> &vPoint);

//顶角缺陷的填补
static void fourLineRepair(vector<Point> &vpXlow,vector<Point> &vpXup,vector<Point> &vpYlow,vector<Point> &vpYup,int imgCols);

//筛选轮廓点
static void filtrateContour(vector<Point> &vPoint);

};