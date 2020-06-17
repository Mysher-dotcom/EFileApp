#pragma once
#include "stdafx.h"


namespace CAdapter
{
	//MImage 转换 IplImage
	//IplImage* mimg2Ipl(MImage* mimg);

	//IplImage 转换 MImage
	//MImage* Ipl2mimg(IplImage* ipl);

	//Creat an object of cv::Mat by width,height and channels
	cv::Mat CreateMat(int nWidth, int nHeight,int nChannels);

	//gray style
	cv::Mat GrayStyle(cv::Mat src);

	//convet cv::Mat to IplIamge*
	//return the ptr of IplImage
	//Need not to free the ptr of IplImage for that it share the memroy with mat
	//* It's unavailable，because the ptr which the function return is null when it is convey to another function
	// IplImage * Mat2Ipl(Mat src) ;

	//Convert IplImage* to cv::Mat
	//return the object of cv::Mat
	//Need not to free the ptr of IplImage for that it share the memroy with mat
	cv::Mat Ipl2Mat(IplImage *ipl);

	//MImg 转换 Mat 内存池
	cv::Mat mimg2Mat(MImage* mimg);
	//MImg 转换 Mat 本地内存
	cv::Mat mimg2MatLoc(MImage *mimg);

	//Mat 转换 MImage
	MImage* Mat2mimg(cv::Mat mat);
	MImage* Mat2mimgLoc(cv::Mat mat);

	//将内存保存到本地
	void mimgGetData2Place(MImage* mimg);

	//Mat对象图像内存考入MImage对象
	//参数:
	//[in] mat : 源图像，Mat对象
	//[in] mimage : 目标图像，MImage指针，需要事先申请内存，且需要和mat在图像尺寸，颜色通道相同
	//返回值:
	//true : 成功
	//false : 失败
	bool CopyMat2mimg(cv::Mat mat, MImage * mimage);
#if 0
	//MImage 转换 CxImage
	CxImage* mimg2ximg(MImage* mimg);

	//CxImage 转换 MImage
	MImage* ximg2mimg(CxImage* ximg);
#endif
};

