
#include "Adapter.h"
#include "MImageDataPool.h"

//gray style
cv::Mat CAdapter::GrayStyle(cv::Mat src)
{
	Mat gray;
	if(src.channels() == 3)
	{
		cvtColor(src, gray, CV_BGR2GRAY); 
	}
	else
	{
		gray = src.clone();
	}
	return gray;
}


cv::Mat CAdapter::mimg2Mat(MImage* mimg)
{
	cv::Mat matDst;
	if (mimg == NULL)
	{
		return matDst;
	}
	matDst = CMImageDataPool::Access(mimg->dataIndex);
	return matDst;

}

MImage* CAdapter::Mat2mimg(cv::Mat mat)
{
	if(mat.empty())
	{
		return 0;
	}

	MImage *mimg  = new MImage();
	mimg->width = mat.cols;
	mimg->height = mat.rows;
	mimg->channel = mat.channels();
	
	int nstep = mat.step;
	int nstep2 =  (mat.cols*mimg->channel+3)/4*4;
	mimg->widthStep = nstep2;
	int nSize = mimg->widthStep * mimg->height;
	
	unsigned long long nIndex =  CMImageDataPool::Add(mat);
	mimg->dataIndex = nIndex;
	return mimg;
}

