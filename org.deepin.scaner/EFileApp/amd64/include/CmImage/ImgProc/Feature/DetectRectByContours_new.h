 #pragma once
#include "stdafx.h"
//V1.0.0基于轮廓查找矩形（文档图像裁切纠偏）
//算法框架
//1.插值
//2.menshift
//3.灰度化
//4.通过Canny加强边缘
//5.二值化（使用二值化，则无法处理黑色目标，除非考虑反色）
//6.查找轮廓
//Max 2016-06-01 11:31:11
//V1.0.1
//扩大适用范围(同时解决超出边界和未超出)
//限制：背景单一
//Bicycle 2016-06-24 15:21:18
//V1.0.2
//限制角度不超过45°
//解决本来是正向,裁切纠偏后旋转90°问题
//Max 2016-11-15 15:45:05
//V1.0.3
//对在中心的外接矩形面积同实际面积比例进行放宽(缩小限制比率)
//Bicycle 2017-10-12 18:16:10
//V1.0.4
//1.支持磁条银行卡
//2.支持小底板
//V1.0.5
//放宽面积占比
//Bicycle 2018-01-09 10:42:36
//V1.0.6
//含图片文档裁切不全的补充-Bicycle
//Max
//V1.0.7 Bic 裁切更新
//V1.0.15 Bic 10.15
class CDetectRectByContours_new
{
public:
	//裁切纠偏
	//参数说明:
	//[in]src：源图像
	//[out]angle：倾斜角度
	//[out]pt： 数组CvPoint pt[4]
	//[in]rect：关注区域
	//返回值说明:
	//在图像上标识检测区域
	static bool DetectRect(Mat src, float & angle, Point* pt, Rect rect = Rect(0,0,0,0));

	static bool DetectRect_seg(Mat &src, Point* pt);
private:
	static Mat Resize(Mat src, float &fRiao);

	static Mat im2bw(Mat src);

	static bool findMaxConyours(Mat bw, vector<Point2f> &PT4);
};