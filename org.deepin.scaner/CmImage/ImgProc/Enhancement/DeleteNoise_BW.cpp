#include "stdafx.h"
#include "DeleteNoise_BW.h"

//二值图取杂点
//[in/out]src		输入/输出图像
//[uint]nMethod		程度(非以下情况按low处理)
//	0				low
//	1				medium
//	2				high
//返回值：
//	0				正常运行
//	1				不支持的图像尺寸
//	2				不支持的图像类型
//  -1				未能正常结束
//#include "AdaptiveThreshold.h"
int CDeleteNoise_BW::DeNoise(Mat &reSrc,int nMethod/* =0 */)
{
	Mat src;// =reSrc.clone();
	if(reSrc.channels() == 3)
	{
		cv::cvtColor(reSrc,src,CV_BGR2GRAY);
	}
	else
	{
		src = reSrc.clone();
	}

	float ratioR = 9000000.0/float(src.rows*src.cols);
	int originalCols = src.cols;
	int originalRows = src.rows;
	resize(src,src,Size(src.cols*ratioR,src.rows*ratioR),0,0,INTER_CUBIC);
	threshold(src,src,128,255,src.type());
	//限制条件
	if(src.channels() !=1 || src.rows <10 || src.cols <10 )
	{
		if(src.channels() !=1)
			//MessageBox(NULL,TEXT("不支持的图像类型"),TEXT("DeNoise错误信息"),NULL);
			return 2;
		else
			//MessageBox(NULL,TEXT("不支持的图像尺寸"),TEXT("DeNoisze错误信息"),NULL);
			return 1;
	}
	//选取不同的处理模式
	switch(nMethod)
	{
		//medium
	case 1:
		{
			CDeleteNoise_BW::DeNoiseDegree_low(src,3);
			vector<vector<Point> > vvPoint;
			vvPoint = CDeleteNoise_BW::DeNoise_core(src,40);
			drawContours(src,vvPoint,-1,Scalar(255),CV_FILLED);
			resize(src,src,Size(originalCols,originalRows),0,0,INTER_CUBIC);
			threshold(src,reSrc,128,255,src.type());
			return 0;
		}
	break;
		//high
	case 2:
		{
			CDeleteNoise_BW::DeNoiseDegree_low(src,5);
			vector<vector<Point> > vvPoint;
			vvPoint = CDeleteNoise_BW::DeNoise_core(src,70);
			drawContours(src,vvPoint,-1,Scalar(255),CV_FILLED);
			resize(src,src,Size(originalCols,originalRows),0,0,INTER_CUBIC);
			threshold(src,reSrc,128,255,src.type());

			return 0;
		}
	break;
		//low
	default:
			if(CDeleteNoise_BW::DeNoiseDegree_low(src,3))
			{
				resize(src,src,Size(originalCols,originalRows),0,0,INTER_CUBIC);
				threshold(src,reSrc,128,255,src.type());
				
				return 0;
			}
	break;
	}
	return -1;
}

/*****************以下为私有成员***************/
//杂点去除
//[in/out]src		输入/输出图像
//[in]nSize			窗口大小
vector<vector<Point> > CDeleteNoise_BW::DeNoise_core(Mat &src,uint nSize,int nType)
{
	//图像进行反色
	Mat dst =~src;
	Mat element;
	element =getStructuringElement(MORPH_RECT,Size(7,7),Point(3,3));
	//高尺度
	if(nType ==2)
	{
		Mat element_inrect;
		element_inrect =getStructuringElement(MORPH_RECT,Size(3,3),Point(2,2));
		dilate(dst,src,element_inrect);
		src =~dst;
	}
	//将图像的标点同段落链接在一起
	dilate(dst,dst,element);
	//图像求轮廓
	vector<vector<Point> > vectorOfNoiseContours;
	vector<vector<Point> > vectorOfNo_NoiseContours;
	vector<vector<Point> > vectorOfImageContours;

	findContours(dst,vectorOfImageContours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE,Point(0,0));
	//杂点筛选
	for (vector<vector<Point> >::iterator itr = vectorOfImageContours.begin();itr !=vectorOfImageContours.end();itr++)
		if(itr->size() <nSize)
			vectorOfNoiseContours.push_back(*itr);
		else
			vectorOfNo_NoiseContours.push_back(*itr);
	//标点，字符点筛选去除

	return vectorOfNoiseContours;
}

//低处理程度
//[in/out]src		输入/输出图像
//[in]nSize			窗口大小
bool CDeleteNoise_BW::DeNoiseDegree_low(Mat &src,uint nSize)
{
#if 0
	//限制窗口大小
	int minImgLength =min(src.rows,src.cols);
	nSize = nSize <3? 3:nSize; 
	nSize = nSize >minImgLength ? minImgLength:nSize;
	//对图像进行均值滤波
	blur(src,src,Size(nSize,nSize));
	threshold(src,src,128,255,CV_THRESH_BINARY);
	return true;
#else
	//中值滤波
	medianBlur(src,src,nSize);
	return true;
#endif
}

//中等处理程度
//[in/out]src		输入/输出图像
//[in]nSize			窗口大小
bool CDeleteNoise_BW::DeNoiseDegree_medium(Mat &src,uint nSize) 
{
	//首先进行低处理
	CDeleteNoise_BW::DeNoiseDegree_low(src,3);
	//窗口大小
	uint nBoxSize =nSize;
	//杂点去除
	if(false)
		return true;
	else
		return false;
}

