//#include "StdAfx.h"
#include "Adapter.h"
#include "MImageDataPool.h"


namespace CAdapter
{
	//MImage 转换 IplImage
	//IplImage* mimg2Ipl(MImage* mimg)
	//{
	//	if(mimg == 0)
	//	{
	//		return 0;
	//	}
	//
	//	IplImage *ipl = cvCreateImage(cvSize(mimg->width,mimg->height),8,mimg->channel);
	//	::memcpy(ipl->imageData,mimg->data,mimg->widthStep*mimg->height);
	//
	//	return ipl;
	//}

	//IplImage 转换 MImage
	//MImage* Ipl2mimg(IplImage* ipl)
	//{
	//	if(ipl == 0)
	//	{
	//		return 0;
	//	}
	//
	//	MImage *mimg  = new MImage();
	//	mimg->width = ipl->width;
	//	mimg->height = ipl->height;
	//	mimg->channel = ipl->nChannels;
	//	mimg->widthStep = ipl->widthStep;
	//	int nSize = mimg->widthStep * mimg->height;
	//	mimg->data = new char[nSize];
	//	::memcpy(mimg->data,ipl->imageData,nSize);
	//	return mimg;
	//}

	//Creat an object of cv::Mat by width,height and channels
	cv::Mat CreateMat(int nWidth, int nHeight, int nChannels)
	{
		int nType = CV_8UC3;
		if (nChannels == 1)
		{
			nType = CV_8UC1;
		}
		Mat src(nHeight, nWidth, nType);
		return src;

	}

	//gray style
	cv::Mat GrayStyle(cv::Mat src)
	{
		Mat gray;
		if (src.channels() == 3)
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
	//IplImage * const Mat2Ipl(Mat src)
	//{
	//	IplImage *iplSrc =  &(IplImage)src;
	//	return iplSrc;
	//}

	//Convert IplImage* to cv::Mat
	//return the object of cv::Mat
	//Need not to free the ptr of IplImage for that it share the memroy with mat
	Mat Ipl2Mat(IplImage *ipl)
	{
		Mat dst;
		if (ipl != NULL)
		{

		dst = cv::cvarrToMat(ipl, true);

		//dst = cv::Mat(iplDst, true);


		}
		return dst;
	}

	//MImg 转换 Mat 内存池
	cv::Mat mimg2Mat(MImage* mimg)
	{
		cv::Mat matDst;
		if (mimg == NULL)
		{
			return matDst;
		}
		matDst = CMImageDataPool::Access(mimg->dataIndex);
		return matDst;

		//int nChannel = mimg->channel;

		//int nType = CV_8UC3;
		//switch (nChannel)
		//{
		//case 3:
		//default:
		//	nType = CV_8UC3;
		//	break;
		//case 1:
		//	nType = CV_8UC1;
		//	break;
		//};
		//
		//matDst = cv::Mat(mimg->height,mimg->width, nType);
		//
		//uchar* data = matDst.data;

		//int nstep = matDst.step;
		//int nstep2 =  (matDst.cols*nChannel+3)/4*4;

		//for (int n = 0; n < matDst.rows; n++)
		//{
		//	::memcpy(data+n*nstep, mimg->data + n*nstep2, nstep);
		//}
		//
		////cv::flip(matDst, matDst,0);

		///*imshow("",matDst);
		//cv::waitKey(0);*/

		return matDst;
	}

	//MImg 转换 Mat 本地内存
	cv::Mat mimg2MatLoc(MImage* mimg)
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

		matDst = cv::Mat(mimg->height, mimg->width, nType);

		uchar* data = matDst.data;

		int nstep = matDst.step;
		int nstep2 = (matDst.cols*nChannel + 3) / 4 * 4;

		for (int n = 0; n < matDst.rows; n++)
		{
			::memcpy(data + n*nstep, mimg->data + n*nstep2, nstep);
		}

		//cv::flip(matDst, matDst,0);
		/*imshow("",matDst);
		cv::waitKey(0);*/

		return matDst;
	}

	//Mat 转换 MImage
	MImage* Mat2mimg(cv::Mat mat)
	{
		if (mat.empty())
		{
			return 0;
		}

		MImage *mimg = new MImage();
		mimg->width = mat.cols;
		mimg->height = mat.rows;
		mimg->channel = mat.channels();

		int nstep = mat.step;
		int nstep2 = (mat.cols*mimg->channel + 3) / 4 * 4;
		mimg->widthStep = nstep2;
		int nSize = mimg->widthStep * mimg->height;
		//MImage中不再保存图像内存，改为保存
		/*mimg->data = new char[nSize];
		uchar* data = mat.data;
		for (int n = 0; n < mat.rows; n++)
		{
			::memcpy(mimg->data + n*nstep2,data+n*nstep, nstep);
		}*/
		//::memcpy(mimg->data,mat.data,nSize);
		unsigned long long nIndex = CMImageDataPool::Add(mat);
		mimg->dataIndex = nIndex;
		return mimg;
	}
	MImage* Mat2mimgLoc(cv::Mat mat)
	{
		if (mat.empty())
		{
			return 0;
		}

		MImage *mimg = new MImage();
		mimg->width = mat.cols;
		mimg->height = mat.rows;
		mimg->channel = mat.channels();

		int nstep = mat.step;
		int nstep2 = (mat.cols*mimg->channel + 3) / 4 * 4;
		mimg->widthStep = nstep2;
		int nSize = mimg->widthStep * mimg->height;

		mimg->data = new char[nSize];
		uchar* data = mat.data;
		for (int n = 0; n < mat.rows; n++)
		{
			::memcpy(mimg->data + n*nstep2, data + n*nstep, nstep);
		}

		return mimg;
	}


	//将内存保存到本地
	void mimgGetData2Place(MImage* mimg)
	{
		if (mimg != 0)
		{
			Mat mat;
			mat = CMImageDataPool::Access(mimg->dataIndex);
			if (mat.data != 0)
			{
				int nstep = mat.step;
				int nstep2 = (mat.cols*mimg->channel + 3) / 4 * 4;
				int nSize = mimg->widthStep * mimg->height;
				mimg->data = new char[nSize];
				uchar* data = mat.data;
				for (int n = 0; n < mat.rows; n++)
				{
					::memcpy(mimg->data + n*nstep2, data + n*nstep, nstep);
				}
			}
		}
	}

	//Mat对象图像内存考入MImage对象
	//参数:
	//[in] mat : 源图像，Mat对象
	//[in] mimage : 目标图像，MImage指针，需要事先申请内存，且需要和mat在图像尺寸，颜色通道相同
	//返回值:
	//true : 成功
	//false : 失败
	bool CopyMat2mimg(cv::Mat mat, MImage * mimg)
	{
		if (!mat.data || mimg == NULL || mat.rows != mimg->height || mat.cols != mimg->width || mat.channels() != mimg->channel)
		{
			return false;
		}

		int nstep = mat.step;
		int nstep2 = (mat.cols*mimg->channel + 3) / 4 * 4;
		uchar* data = mat.data;
		for (int n = 0; n < mat.rows; n++)
		{
			::memcpy(mimg->data + n*nstep2, data + n*nstep, nstep);
		}

		return true;
	}

#if 0
	//MImage 转换 CxImage
	CxImage* mimg2ximg(MImage* mimg)
	{
		if (mimg == NULL)
		{
			return NULL;
		}

		CxImage * dst = new CxImage();

		//cvFlip(src);
		unsigned char *data = (unsigned char *)mimg->data;

		dst->CreateFromArray(data, mimg->width, mimg->height, mimg->channel * 8, mimg->widthStep, true);

		return dst;
	}

	//CxImage 转换 MImage
	MImage* ximg2mimg(CxImage* ximg)
	{
		if (ximg == 0 || ximg->GetWidth() == 0)
		{
			return 0;
		}
		CxImage imgTmp;
		imgTmp.CreateFromArray(ximg->GetBits(),
			ximg->GetWidth(),
			ximg->GetHeight(),
			ximg->GetBpp(),
			ximg->GetEffWidth(), true);
		if (imgTmp.GetBpp() == 1)
		{
			imgTmp.GrayScale();
		}

		MImage *mimg = new MImage();
		mimg->width = imgTmp.GetWidth();
		mimg->height = imgTmp.GetHeight();
		mimg->channel = imgTmp.GetBpp() / 8;
		mimg->widthStep = imgTmp.GetEffWidth();
		int nSize = mimg->widthStep * mimg->height;
		mimg->data = new char[nSize];
		::memcpy(mimg->data, (char*)imgTmp.GetBits(), nSize);
		if (mimg != 0)
		{
			return mimg;
		}
		return 0;
	}
#endif
}