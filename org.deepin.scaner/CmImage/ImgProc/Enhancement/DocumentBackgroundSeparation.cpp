#include "stdafx.h"
#include "DocumentBackgroundSeparation.h"
#include "../Feature/DetectEdge.h"
using namespace cv;

//文档背景分割
//参数:
//[in]matSrc:	源图像
//返回值:
//无效Mat:	操作失败
//有效Mat:	分割后的二值图像（其实是灰度，但颜色为二值，0-前景，255-背景）
cv::Mat CDocumentBackgroundSeparation::DocumentBackgroundSeparation(cv::Mat src)
{
	cv::Mat dst;

	if(!src.data)
	{
		//操作失败，返回空mat
		return dst;
	}

	//灰度化
	Mat gray;
	if(src.channels() == 3)
	{
		cvtColor(src,gray, CV_BGR2GRAY);  
	}
	else
	{
		gray = src.clone();
	}

#if 0
	// Sobel x&y
	Mat grad = CDetectEdge::DetectEdge(gray,2);
	//反色
	Mat negtive = 255 -grad;
	//二值化
	//内存共享方式从mat创建Ipliamge
	IplImage *iplNegtive =  &(IplImage)negtive;
	int nThreshold = CAdaptiveThreshold::OptimalThreshold(iplNegtive,1);
	cv::threshold(negtive,dst,nThreshold,255,0);

	

#else
	//均值核
	dst = CDetectEdge::DetectEdge_Gaussion(gray);
#endif

	//通过腐蚀扩大笔画
	erode(dst, dst, Mat());  
	/*erode(dst, dst, Mat());  
	dilate(dst, dst, Mat());  
	dilate(dst, dst, Mat());*/
	

	return dst;
}
