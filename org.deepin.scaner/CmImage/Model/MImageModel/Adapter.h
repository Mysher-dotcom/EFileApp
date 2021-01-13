#pragma once
#include "MImage.h"
#include "stdafx.h"

class CAdapter
{
public:

	//Creat an object of cv::Mat by width,height and channels
	static cv::Mat CreateMat(int nWidth, int nHeight,int nChannels);

	//gray style
	static cv::Mat GrayStyle(cv::Mat src);

	//Convert IplImage* to cv::Mat
	//return the object of cv::Mat
	//Need not to free the ptr of IplImage for that it share the memroy with mat
	static Mat Ipl2Mat(IplImage *ipl);

	//MImg to Mat pool
	static cv::Mat mimg2Mat(MImage* mimg);
	//MImg tp Mat local
	static cv::Mat mimg2MatLoc(MImage *mimg);

	//Mat to MImage
	static MImage* Mat2mimg(cv::Mat mat);
	static MImage* Mat2mimgLoc(cv::Mat mat);

	static void mimgGetData2Place(MImage* mimg);

	static bool CopyMat2mimg(cv::Mat mat, MImage * mimage);
};

