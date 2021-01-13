
#include "Adapter.h"
#include "MImageDataPool.h"


//Creat an object of cv::Mat by width,height and channels
cv::Mat CAdapter::CreateMat(int nWidth, int nHeight,int nChannels)
{
	int nType = CV_8UC3;
	if(nChannels == 1)
	{
		nType = CV_8UC1;
	}
	Mat src(nHeight,nWidth,nType);
	return src;

}

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

//convet cv::Mat to IplIamge*(the other object of OpenCV)
//return the ptr of IplImage
//Need not to free the ptr of IplImage for that it share the memroy with mat
//IplImage * const CAdapter::Mat2Ipl(Mat src)
//{
//	IplImage *iplSrc =  &(IplImage)src;
//	return iplSrc;
//}

//Convert IplImage* to cv::Mat
//return the object of cv::Mat
//Need not to free the ptr of IplImage for that it share the memroy with mat
Mat CAdapter::Ipl2Mat(IplImage *ipl)
{
	Mat dst;
	if(ipl != NULL)
	{
		dst = cv::cvarrToMat(ipl,true);  

	}
	return dst;
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


cv::Mat CAdapter::mimg2MatLoc(MImage* mimg)
{
	cv::Mat matDst;
	if (mimg == NULL)
	{
		return matDst;
	}
	int nChannel = mimg->channel;

	int nType = CV_8UC3;
	switch (nChannel)
	{
	case 3:
	default:
		nType = CV_8UC3;
		break;
	case 1:
		nType = CV_8UC1;
		break;
	};
	
	matDst = cv::Mat(mimg->height,mimg->width, nType);
	
	uchar* data = matDst.data;

	int nstep = matDst.step;
	int nstep2 =  (matDst.cols*nChannel+3)/4*4;

	for (int n = 0; n < matDst.rows; n++)
	{
		::memcpy(data+n*nstep, mimg->data + n*nstep2, nstep);
	}
	
	//cv::flip(matDst, matDst,0);
	/*imshow("",matDst);
	cv::waitKey(0);*/

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

MImage* CAdapter::Mat2mimgLoc(cv::Mat mat)
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
	
	mimg->data = new char[nSize];
	uchar* data = mat.data;
	for (int n = 0; n < mat.rows; n++)
	{
		::memcpy(mimg->data + n*nstep2,data+n*nstep, nstep);
	}

	return mimg;
}


void CAdapter::mimgGetData2Place(MImage* mimg)
{
	if(mimg != 0)
	{
		Mat mat;
		mat = CMImageDataPool::Access(mimg->dataIndex);
		if(mat.data != 0)
		{
			int nstep = mat.step;
			int nstep2 =  (mat.cols*mimg->channel+3)/4*4;
			int nSize = mimg->widthStep * mimg->height;
			mimg->data = new char[nSize];
			uchar* data = mat.data;
			for (int n = 0; n < mat.rows; n++)
			{
				::memcpy(mimg->data + n*nstep2,data+n*nstep, nstep);
			}
		}
	}
}

bool CAdapter::CopyMat2mimg(cv::Mat mat, MImage * mimg)
{
	if(!mat.data || mimg == NULL || mat.rows != mimg->height || mat.cols != mimg->width || mat.channels() != mimg->channel)
	{
		return false;
	}

	int nstep = mat.step;
	int nstep2 =  (mat.cols*mimg->channel+3)/4*4;
	uchar* data = mat.data;
	for (int n = 0; n < mat.rows; n++)
	{
		::memcpy(mimg->data + n*nstep2,data+n*nstep, nstep);
	}

	return true;
}

