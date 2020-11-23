 #pragma once
 #include "stdafx.h"

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

	static bool DetectRect_scanner(Mat src, float & angle, Point* pt, Rect rect = Rect(0,0,0,0));

	static bool DetectRect_Book(Mat src, float & angle, Point* pt, Rect rect = Rect(0,0,0,0));

	static bool DetectRect_Multi(Mat src, MRectRArray &mRectArray, Rect rect = Rect(0,0,0,0));

	static bool DetectRect_seg(Mat &src, Point* pt);

	static Mat Resize(Mat src, float &fRiao);

	static Mat im2bw(Mat src);

	static bool grad(Mat &src ,Mat &mask);
private:
	static bool findMaxConyours(Mat bw, vector<Point2f> &PT4, int type=0);

	static bool findMaxConyours_scanner(Mat bw, Mat bw1, vector<Point2f> &PT4);

	//去除二值图像边缘的突出部
	//uthreshold、vthreshold分别表示突出部的宽度阈值和高度阈值
	//type代表突出部的颜色，0表示黑色，1代表白色 
	static void delete_jut(Mat& src, Mat& dst, int uthreshold, int vthreshold, int type);

	static void  fillHole(Mat srcBw, Mat &dstBw);
};