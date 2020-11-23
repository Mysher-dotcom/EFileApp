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

    static Mat Resize(Mat src, float &fRiao);

	static Mat im2bw(Mat src);

    static bool grad(Mat &src ,Mat &mask);
private:
    static bool findMaxConyours(Mat bw, vector<Point2f> &PT4, int type=0);
};
