#pragma once
#include "MImage.h"
#include "stdafx.h"

class CAdapter
{
public:

	//gray style
	static cv::Mat GrayStyle(cv::Mat src);


	//MImg to Mat pool
	static cv::Mat mimg2Mat(MImage* mimg);
	//Mat to MImage
	static MImage* Mat2mimg(cv::Mat mat);
};

