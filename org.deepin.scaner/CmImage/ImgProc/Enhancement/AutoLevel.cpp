#include "stdafx.h"
#include "AutoLevel.h"
#include "../Transform/Rotate.h"
#include "Histogram.h"
#include "../Feature/FillBorder.h"
#include "../ColorSpace/AdaptiveThreshold.h"
#include "DocumentBackgroundSeparation.h"
#include "../Feature/DetectRectByContours.h"
//using namespace cv;
//版本号：V0.0.8

//手动调整直方图拉伸所需全局变量和函数
#if 1
#define WINDOW_NAME "[色阶调整]"
#define WINDOW_NAME2 "[图像预览]"

int g_nThresholdSlider_Low_Min = 0;
int g_nThresholdSlider_High_Min = 0;

int g_nThresholdSlider_Low = 50;
int g_nThresholdSlider_High = 200;

int g_nThresholdSlider_Low_Max = 255;
int g_nThresholdSlider_High_Max = 255;
int g_nThresholOtus = 0;

Mat g_src;
Mat g_dst;
Mat g_hist;
Mat g_hist2;

void on_Trackbar(int,void*)
{
	g_dst = g_src.clone();

	CAutoLevel::StrechHistogram(g_dst.data,g_dst.cols,g_dst.rows,g_dst.channels()*8,g_dst.step,g_nThresholdSlider_Low,g_nThresholdSlider_High);
	g_hist2 = g_hist.clone();

	float fRadio = (float)g_hist2.cols / g_nThresholdSlider_Low_Max;
	cv::line(g_hist2,cv::Point(g_nThresholdSlider_High*fRadio,0),cv::Point(g_nThresholdSlider_High*fRadio,g_hist2.rows),cv::Scalar(255,0,0));
	cv::line(g_hist2,cv::Point(g_nThresholdSlider_Low*fRadio,0),cv::Point(g_nThresholdSlider_Low*fRadio,g_hist2.rows),cv::Scalar(0,0,255));
	cv::line(g_hist2,cv::Point(g_nThresholOtus*fRadio,0),cv::Point(g_nThresholOtus*fRadio,g_hist2.rows),cv::Scalar(0,255,0));

	imshow(WINDOW_NAME,g_hist2);
	imshow(WINDOW_NAME2,g_dst);
}
#endif

//自动调整色阶
bool CAutoLevel::AdjustLevelAuto(Mat src,Mat &dst)
{
	int nThreshold_Low = 50;
	if(!src.data)
	{
		return false;
	}

	if(src.channels() == 1||1)
	{
		vector<Mat> vcMat;
		Mat r,g,b;
		cv::split( src, vcMat);

		for(int n =0; n < 3; n++)
		{
            //IplImage *ipl = &IplImage(vcMat[n]);
            IplImage iplTmp = IplImage(vcMat[n]);
            IplImage *ipl = &iplTmp;
			int nThreshold = CAdaptiveThreshold::OptimalThreshold(ipl);
			CAutoLevel::StrechHistogram(vcMat[n].data, vcMat[n].cols, vcMat[n].rows, vcMat[n].channels()*8, vcMat[n].step,nThreshold_Low,nThreshold);
		}
		cv::merge(vcMat,dst);
	}
	else
	{

		dst = src.clone();
        //IplImage *ipl = &IplImage(dst);
        IplImage iplTmp = IplImage(dst);
        IplImage *ipl = &iplTmp;
		int nThreshold = CAdaptiveThreshold::OptimalThreshold(ipl);

		CAutoLevel::StrechHistogram(dst.data, dst.cols, dst.rows, dst.channels()*8, dst.step, nThreshold_Low, nThreshold,0,255);
	}

	return true;
}

//手动调整色阶
bool CAutoLevel::AdjustLevelCustom(Mat src,Mat &dst)
{
	if(!src.data)
	{
		return false;
	}

	g_src = src.clone();
	if(!CHistogram::DrawHistogram(g_src, g_hist))
	{
		return false;
	}

	g_nThresholdSlider_Low = 50;
	g_nThresholdSlider_High = 200;

	Mat gray;
	if(src.channels() == 3)
	{
		cvtColor(src, gray, CV_BGR2GRAY);
	}//end if
	else
	{
		gray = src.clone();
	}//end if else

//	IplImage *ipl;
//	ipl=&IplImage(gray);
    IplImage iplTmp = IplImage(gray);
    IplImage *ipl = &iplTmp;
	g_nThresholOtus = CAdaptiveThreshold::OptimalThreshold(ipl);

	cv::namedWindow(WINDOW_NAME,0);
	cv::namedWindow(WINDOW_NAME2,0);
#ifndef _MX_OPENCV_231_STATICLIB
	cv::resizeWindow(WINDOW_NAME2, 480, 640);
	cv::resizeWindow(WINDOW_NAME, 510, 510);
#endif


	char TrackbarName_Low[50];
	char TrackbarName_High[50];

	sprintf(TrackbarName_Low,"低(%d-%d)",g_nThresholdSlider_Low_Min,g_nThresholdSlider_Low_Max);
	sprintf(TrackbarName_High,"高(%d-%d)",g_nThresholdSlider_Low_Min,g_nThresholdSlider_Low_Max);

	cv::createTrackbar(TrackbarName_Low, WINDOW_NAME, &g_nThresholdSlider_Low, g_nThresholdSlider_Low_Max, on_Trackbar);
	cv::createTrackbar(TrackbarName_High, WINDOW_NAME, &g_nThresholdSlider_High, g_nThresholdSlider_Low_Max, on_Trackbar);

	on_Trackbar(g_nThresholdSlider_Low, 0);
	on_Trackbar(g_nThresholdSlider_High, 0);

	waitKey(0);

	dst = g_dst.clone();

	return true;
}

//拉伸直方图算法
//Max 2013-5-6 16:16:03
bool CAutoLevel::StrechHistogram(unsigned char* pSrc,int nSrcWidth,int nSrcHeight,int nSrcBpp,int nSrcStep,
		int nLow,int nHigh,int nMin ,int nMax )
{
	if (pSrc == NULL)
	{
		return false;
	}
	//int nSrclineByte = (((nSrcWidth*nSrcBpp)+31)>>5)<<2;
	int nSrclineByte = nSrcStep;
	
	int nMinDiffer = 1; 

	if (nHigh>nMax)
	{
		nHigh = nMax;
	}

	if (nLow<nMin)
	{
		nLow = nMin;
	}
	if(nLow >= nHigh)
	{
		nLow = 0;
	}

	if (nHigh == 255&&nLow==0)
	{
		return true;
	}

	//彩色24位图像
	if (nSrcBpp == 24)
	{
		//计算3个通道拉伸值
		float fChangeDiffer = 0;
		int nDiffer = 0;
		nDiffer = nHigh-nLow;

		fChangeDiffer = (float)_MX_MAXVALUE/nDiffer;


		//计算修正值
		int nRValue[256] = {0};
		int nGValue[256] = {0};
		int nBValue[256] = {0};
		for (int n = 0; n<256; n++)
		{
			nRValue[n] = CheckValue((int)((n - nLow) * fChangeDiffer+0.5));
			nGValue[n] = CheckValue((int)((n - nLow) * fChangeDiffer+0.5));
			nBValue[n] = CheckValue((int)((n - nLow) * fChangeDiffer+0.5));
		}

		//修正

		for (int y = 0; y < nSrcHeight; y++)
		{
			//int nPos = y*nSrclineByte;
			for (int x = 0; x < nSrcWidth; x++)
			{
				int nPos = y*nSrclineByte+x*3;

				pSrc[nPos+2] = nRValue[pSrc[nPos+2]];
				pSrc[nPos+1] = nGValue[pSrc[nPos+1]];
				pSrc[nPos]   = nBValue[pSrc[nPos]];
			}
		}
	}
	else if (nSrcBpp == 8)
	{
		//计算3个通道拉伸值
		float fChangeDiffer = 0;
		int nDiffer = 0;
		nDiffer = nHigh-nLow;
		
		fChangeDiffer = (float)_MX_MAXVALUE/nDiffer;
		
		
		//计算修正值
		int nGrayValue[256] = {0};
		for (int n = 0; n<256; n++)
		{
			nGrayValue[n] = CheckValue((int)((n - nLow) * fChangeDiffer+0.5));
		}
		
		//修正
		
		for (int y = 0; y < nSrcHeight; y++)
		{
			//int nPos = y*nSrclineByte;
			for (int x = 0; x < nSrcWidth; x++)
			{
				int nPos = y*nSrclineByte+x;
				pSrc[nPos]   = nGrayValue[pSrc[nPos]];
			}
		}
	}

	return true;
}

//自动色阶 V0.0.2 bicycle Time:2017年5月16日15:46:20
//参数：
//[in]src			输入图像
//[out]dst			输出图像
//[in]maxValue		亮度最大值
//[in]minValue		亮度最小值
//--- maxValue,minValue--- 有负值则参数失效
//[in]fGmax			最大增益(0.3,1.5]
//[in]fGmin			最小增益[0,0.3]
//--- fGmax,fGmin---为1则对应的参数失效
//返回值：
//调整后图像
//注：因为后四个参数有功能重叠部分，如果同时存在。先maxValue,minValue，后fGmax,fGmin。
bool CAutoLevel::AdjustLevelAutoUnis(Mat src,Mat &dst,int maxValue /* =255 */,int minValue /* =0 */,
										float fGmaxIn /* =-1 */,float fGminIn/* =-1 */)
{
	//imwrite("D:\\testPrev.jpg",src);
	//if( 3 != src.channels())
	//{
	//	dst = src.clone();
	//	return false;
	//}
	float fGmax,fGmin;
	fGmax = fGmin =1;
	//判断maxValue,minValue范围
	maxValue = maxValue >255 ? 255:maxValue;
	//maxValue = maxValue <1	 ?	1:maxValue;
	minValue = minValue >254 ? 254:minValue;
	//minValue = minValue <1	 ?	0:minValue;
#if 1
	vector<Mat> vectorOfChannelImage;
	//split colorful image
	if(src.channels() == 3)
		split(src,vectorOfChannelImage);
	else
		vectorOfChannelImage.push_back(src);
	//find max/min value in image 
	int arrMaxValueInImage[3] ={0};
	int arrMinValueInImage[3] ={0};
	int maxValueInImage,minValueInImage,midValueInImage;
	maxValueInImage =minValueInImage =midValueInImage =0;
	float arrFgmax[3]={0.0};
	float arrFgmin[3]={0.0};
	//find max value in image
	int idxOfvectorOfImage =0;
	if(maxValue >0 && minValue >=0)
	{
		for (vector<Mat>::iterator itr = vectorOfChannelImage.begin();itr != vectorOfChannelImage.end(); itr++,idxOfvectorOfImage++)
		{
			Mat grayImage = *itr;

			if(!CAutoLevel::findMaxValueofImage(grayImage,arrMaxValueInImage[idxOfvectorOfImage],arrMinValueInImage[idxOfvectorOfImage]))
				return false;
			fGmax = 255.0 / ((float)arrMaxValueInImage[idxOfvectorOfImage]+0.00001);
			//limit of fGmax scope
			fGmax = fGmax > 1.5? 1.5:fGmax;
			fGmax = fGmax < 0.3? 0.3:fGmax;
			fGmin = (float)minValue / ((float)arrMinValueInImage[idxOfvectorOfImage]+0.00001);
			//limit of fGmin scope
			fGmin = fGmin >0.3? 0.3:fGmin;
			fGmin = fGmin <0?   0:fGmin;

			arrFgmax[idxOfvectorOfImage] =fGmax;
			arrFgmin[idxOfvectorOfImage] =fGmin;
		}

		if(src.channels() ==3)
		{
			fGmax = min(arrFgmax[0],min(arrFgmax[1],arrFgmax[2]));
			fGmin = (arrFgmin[0] +arrFgmin[1] +arrFgmin[2])/3.0;
			maxValueInImage = min(min(arrMaxValueInImage[0],arrMaxValueInImage[1]),arrMaxValueInImage[2]);
			minValueInImage = max(max(arrMinValueInImage[0],arrMinValueInImage[1]),arrMinValueInImage[2]);
			midValueInImage = (maxValueInImage +minValueInImage)/2.0;
			//midValueInImage = /*(maxValue + minValue)/2.0*/35;
		}
		else
		{
			fGmax = arrFgmax[0];
			fGmin = arrFgmin[0];
			maxValueInImage = arrMaxValueInImage[0];
			minValueInImage = arrMinValueInImage[0];
			midValueInImage = (maxValueInImage + minValueInImage)/2.0;
			//midValueInImage = (maxValue + minValue)/2.0;
		}
	}
	else
	{
		maxValue =255;minValue=0;
	}
	//判断特殊情况
	if(maxValueInImage -minValueInImage < 50)
	{
		minValueInImage =0;
		midValueInImage = maxValueInImage/2;
	}

	//lookup table 
	int newImageValueLookupTable[256] ={0};
	uchar newImageValueLookupTableResult[256]={0};
	//create lookup table use maxValue&minValue

	//minValueInImage = 109;
	//maxValueInImage = 218;
	//midValueInImage = 160;
	for (float idx =0; idx<256;idx++)
	{
		if(idx >= midValueInImage)
			newImageValueLookupTable[(int)idx] = (idx * (1.0+ (fGmax-1.0) *(float)(idx-midValueInImage)/
			(float)(maxValueInImage -midValueInImage)))*((float)maxValue/255.0) +minValue;
		else
		{
			newImageValueLookupTable[(int)idx] = (idx * (1.0 +(fGmin-1.0) *(float)(midValueInImage-idx)/
			(float)(midValueInImage -minValueInImage)));
			
			newImageValueLookupTable[(int)idx] += minValue; 
		}
	}
	//create lookup table use fGmax&fGmin
	for (float idx =0; idx<256;idx++)
	{
		if(idx >= midValueInImage)
			newImageValueLookupTableResult[(int)idx] = saturate_cast<uchar>(newImageValueLookupTable[(int)idx] * (1.0+ (fGmaxIn-1.0) *(float)(idx-midValueInImage)/
			(float)(maxValueInImage -midValueInImage)));
		else
			newImageValueLookupTableResult[(int)idx] = saturate_cast<uchar>(newImageValueLookupTable[(int)idx] * (1.0 +(fGminIn-1.0) *(float)(midValueInImage-idx)/
				(float)(midValueInImage -minValueInImage)));
	}

#if 0
	for(auto itr =vectorOfChannelImage.begin(); itr!= vectorOfChannelImage.end() ; itr++)
	{
		Mat grayImage = *itr;
		//replace image value by lookup table
		uchar *grayImageData = grayImage.ptr<uchar>(0);
		for (int idx =0; idx <grayImage.rows*grayImage.cols;idx++)
			*grayImageData ++ = saturate_cast<uchar>(newImageValueLookupTableResult[*grayImageData]);
		*itr =grayImage;
	}
	if(src.channels() == 3)
		merge(vectorOfChannelImage,dst);
	else
		dst = vectorOfChannelImage[0];
#else
	vectorOfChannelImage.clear();
	dst = src.clone();
	uchar *data = dst.ptr<uchar>(0);
	int nCilChannel = dst.cols *dst.channels();
	for (int idr =0;idr <src.rows;idr++)
	{
		for (int idc=0;idc<nCilChannel;idc++)
		{
			*data ++=newImageValueLookupTableResult[*data];
		}
	}
#endif//合并Endif

#endif
	//imwrite("D:\\testNow.jpg",dst);
	return true;
}

//背景色平滑 V0.0.3 bicycle Time:2017年5月17日14:09:13
//[in]src						输入图像
//[out]dst						输出图像
//[in]isBackGroundColorSmooth	背景色平滑/变白(默认：变白(false));
//返回值：
//true:成功，false：失败
//注：函数只支持彩色图像！
bool CAutoLevel::backGroundColorProcessing(Mat src,Mat &Redst,bool isBackGroundColorSmooth /* = false */)
{
	Mat dst = src.clone();
	double dRatio = 3000000.0/double(src.rows*src.cols);
	if(dRatio < 1)
	{
		resize(dst,dst,Size(src.rows*dRatio,src.cols*dRatio));
	}

	if( src.channels() != 3)
		return false;	
	isBackGroundColorSmooth = !isBackGroundColorSmooth;

	//判断
	if(CAutoLevel::isNeedCut(dst))
	{
		float fAngle;
		CvPoint pt[4];
        IplImage iplTmp = IplImage(dst);
        IplImage *ipl = &iplTmp;
        CDetectRectByContours::DetectRect(ipl,fAngle,pt);
		dst = Rotate::RotateCut(dst,pt);
	}

	vector<Mat> vMat;
	split(dst,vMat);
	float backgroundColorThreshold_r;
	float backgroundColorThreshold_g;
	float backgroundColorThreshold_b;
	dst.release();
	
	float fRatio =0.9;
    IplImage iplTmp = IplImage(vMat[0]);
    IplImage *ipl = &iplTmp;
    backgroundColorThreshold_r = (float)CAdaptiveThreshold::OptimalThreshold(ipl)*fRatio;

    IplImage iplTmp1 = IplImage(vMat[1]);
    IplImage *ipl1 = &iplTmp1;
    backgroundColorThreshold_g = (float)CAdaptiveThreshold::OptimalThreshold(ipl1)*fRatio;

    IplImage iplTmp2 = IplImage(vMat[2]);
    IplImage *ipl2 = &iplTmp2;
    backgroundColorThreshold_b = (float)CAdaptiveThreshold::OptimalThreshold(ipl2)*fRatio;

	vMat.clear();
	const float CRatio = 300.0/min(min(backgroundColorThreshold_r,backgroundColorThreshold_g),backgroundColorThreshold_b);
	const float CRatioSmooth = 290.0/max(max(backgroundColorThreshold_r,backgroundColorThreshold_g),backgroundColorThreshold_b);
	//查询表
	uchar bookCheckR[256];
	for (int idx =0;idx <256;idx++)
		if(!isBackGroundColorSmooth)
			bookCheckR[idx] = saturate_cast<uchar>( (backgroundColorThreshold_r*CRatioSmooth)/ (1.0+pow((float)2.718,(float)(-1.0*((float)idx -backgroundColorThreshold_r)/23)))+10);
		else
			bookCheckR[idx] = saturate_cast<uchar>( (backgroundColorThreshold_r*CRatio)/ (1.0+pow((float)2.718,(float)(-1.0*((float)idx -backgroundColorThreshold_r)/15))));

	uchar bookCheckG[256];
	for (int idx =0;idx <256;idx++)
		if(!isBackGroundColorSmooth)
			bookCheckG[idx] = saturate_cast<uchar>( (backgroundColorThreshold_g*CRatioSmooth)/ (1.0+pow((float)2.718,(float)(-1.0*((float)idx -backgroundColorThreshold_g)/23)))+10);
		else
			bookCheckG[idx] = saturate_cast<uchar>( (backgroundColorThreshold_g*CRatio)/ (1.0+pow((float)2.718,(float)(-1.0*((float)idx -backgroundColorThreshold_g)/15))));

	uchar bookCheckB[256];
	for (int idx =0;idx <256;idx++)
		if(!isBackGroundColorSmooth)
			bookCheckB[idx] = saturate_cast<uchar>( (backgroundColorThreshold_b*CRatioSmooth)/ (1.0+pow((float)2.718,(float)(-1.0*((float)idx -backgroundColorThreshold_b)/23)))+10);
		else
			bookCheckB[idx] = saturate_cast<uchar>( (backgroundColorThreshold_b*CRatio)/ (1.0+pow((float)2.718,(float)(-1.0*((float)idx -backgroundColorThreshold_b)/15))));

	uchar *data = Redst.ptr<uchar>(0);
	for (int idr=0;idr<Redst.rows;idr++)
	{
		for (int idc=0;idc<Redst.cols;idc++,data+=3)
		{
			if(*(data+2) >backgroundColorThreshold_r*fRatio && *(data+1) >backgroundColorThreshold_g*fRatio && *data >backgroundColorThreshold_b*fRatio )
			{
				*(data+2) = bookCheckR[*(data+2)];
				*(data+1) = bookCheckG[*(data+1)];
				*data = bookCheckB[*data];
			}
		}
	}

	return true;
}

//判断图像是否需要裁切[背景平滑内嵌函数]
//[in]src			输入图像
//返回值：
//true,需要裁切false，不需要裁切
bool CAutoLevel::isNeedCut(Mat src)
{
	cvtColor(src,src,CV_RGB2GRAY);
	threshold(src,src,128,255,THRESH_BINARY);
	int zerosRectNum =0;
	int rowLength = src.rows/30;
	int colLength = src.cols/30;
	Mat topRect = src.rowRange(0,rowLength);
	Mat botRect = src.rowRange(src.rows-rowLength,src.rows);
	Mat leftRect = src.colRange(0,colLength);
	Mat rightRect = src.colRange(src.cols -colLength,src.cols);

	uchar *topData;
	int imgRow,imgCol;
	imgRow =imgCol =0;
	for(int idx=0;idx<4;idx++)
	{
		int nonZerosNum =0;
		switch(idx)
		{
		case  0:
			{topData = topRect.ptr<uchar>(0);imgRow=topRect.rows;imgCol=topRect.cols;}
			break;

		case 1:
			{topData = botRect.ptr<uchar>(0);imgRow=botRect.rows;imgCol=botRect.cols;}
			break;

		case 2:
			{topData = leftRect.ptr<uchar>(0);imgRow=leftRect.rows;imgCol=leftRect.cols;}
			break;

		case 3:
			{topData =rightRect.ptr<uchar>(0);imgRow=rightRect.rows;imgCol=rightRect.cols;}
			break;
		}
		for (int idr=0;idr<imgRow;idr++)
		{
			for (int idc=0;idc<imgCol;idc++,topData++)
			{
				if(*topData > 100)
					nonZerosNum++;
			}
		}
		if((float)nonZerosNum/(float)(imgCol*imgRow)<0.5)
			zerosRectNum++;
	}

	if(zerosRectNum>=2)
		return true;
	else
		return false;
}

//寻找图像中的最亮点
//[in]src			灰度图
//[out]maxValue		最大值
//[out]minValue		最小值
bool CAutoLevel::findMaxValueofImage(Mat src,int &maxValue,int &minValue,float numOfMax,float numOfMin)
{

	//scale the image
	Mat dst;
#if 0
	if(src.rows >10000 && src.cols >10000)
		resize(src,dst,Size(src.rows/200,src.cols/200));
	else if(src.rows >100 && src.cols >100)
		resize(src,dst,Size(src.rows/20,src.cols/20));
	else
		dst = src.clone();
#endif
	 double dRatio = 1000000.0/double(src.rows*src.cols);
	 if(dRatio < 1)
	 {
		 resize(src,dst,Size(src.rows*dRatio,src.cols*dRatio));
	 }
	 else
		 dst = src.clone();

	//put image value in vector
	vector<int> vectorOfImageValue;
	uchar *srcData = dst.ptr<uchar>(0);
	for(int idx =0;idx<dst.rows*dst.cols;idx++,srcData++)
		vectorOfImageValue.push_back(*srcData);
	//sort of vector
	sort(vectorOfImageValue.begin(),vectorOfImageValue.end());
	// find max/min value in image
	int idxMin = vectorOfImageValue.size()*numOfMin;
	int idxMax = vectorOfImageValue.size()*numOfMax;
	maxValue = vectorOfImageValue[idxMax];
	minValue = vectorOfImageValue[idxMin];

	return true;
}

//文档增强（Document Enhancement）
//参数
//[in]src		:	源图像
//[out]dst		:	目标图像
//[in]nBoxSize	:	窗口尺寸
//					（1） > 0，按窗口对每个区域图像分别进行处理
//					（2） <=0，对全图进行处理
//[in]nMinTh	:	最低阈值（直接采用此阈值）
//[in]nMaxTh	:	算法会计算最优化阈值，如果阈值大于nMaxTh，则使其等于nMaxTh
//返回值
//true	:	操作成功
//false	:	操作失败
bool CAutoLevel::DocumentEnhancement(Mat src,Mat &dst, int nBoxSize, int nMinTh, int nMaxTh)
{
	if(!src.data)
	{
		return false;
	}
	int nW = src.cols;
	int nH = src.rows;

	dst = src.clone();

	Mat mask = CDocumentBackgroundSeparation::DocumentBackgroundSeparation(dst);

	for(int y = 0; y < nH; y += nBoxSize)
	{
		int nHT = nBoxSize;
		if(y + nHT >= nH)
		{
			nHT = nH - y - 1;
		}//end fi
		for(int x= 0; x < nW; x+= nBoxSize)
		{
			int nWT = nBoxSize;
			if(x + nWT >= nW)
			{
				nWT = nW - x - 1;
			}//end if

			//不重新分配内存，公用内存
			Mat dstROI( dst, Rect(x,y,nWT,nHT));
			Mat maskROI( mask, Rect(x,y,nWT,nHT));
			//Mat srcROI = dst(Rect(x,y,nWT,nHT));
			Mat dstTmp = dstROI.clone();
			CAutoLevel::AdjustLevelAuto2(dstTmp,dstROI,nMinTh,nMaxTh,maskROI);
			/*cv::imshow("",maskROI);
			cv::waitKey(-1);*/
		}//end for


	}//end for

	//图像锐化
#if 0
	//laplace算子
	Mat kernel = (Mat_<float>(3, 3) << 0, -1, 0, -1, 5, -1, 0, -1, 0);
	//CxIamge Sharp算子
	//Mat kernel = (Mat_<float>(3, 3) << -1,-1,-1,-1,15,-1,-1,-1,-1);
	cv::filter2D(dst, dst, dst.depth(), kernel); 
#endif

	//边缘填充
	Mat dstT = dst.clone();
	CFillBorder::FillBorder(dstT, dst);

	return true;
}

//直方图非线性增强算法，考虑蒙板
//参数:
//pSrc:			源图像
//nSrcWidth:	源图像宽度
//nSrcHeight:	源图像高度
//nSrcBpp:		源图像位数（8灰度，24彩色）
//nSrcStep:		源图像每行内存长度
//nLow:			直方图增强低阈值（直方图非线性增强原理：将高阈值和低阈值之间波形拉伸至整个直方图）
//nHigh:		直方图增强高阈值（直方图非线性增强原理：将高阈值和低阈值之间波形拉伸至整个直方图）
//nMin:			直方图低阈值最低值（低阈值低于此强制等于此）
//nMax:			直方图高阈值最高值（高阈值高于此强制等于此）
//pMask:		直方图增强蒙板（对于蒙板中卫0时按正常高低阈值增强，如果等于1则按增补阈值增强）
//nMaskStep:	直方图增强蒙板每行内存长度
//nMaskHigh:	直方图增强高阈值增补值（当蒙板等于1时，按此增补值增强）
bool CAutoLevel::StrechHistogram2(unsigned char* pSrc,int nSrcWidth,int nSrcHeight,int nSrcBpp,int nSrcStep,
        int nLow,int nHigh,int nMin,int nMax, unsigned char* pMask, int nMaskStep, int nMaskHigh)
{
	if (pSrc == NULL)
	{
        return false;
	}


	//int nSrclineByte = (((nSrcWidth*nSrcBpp)+31)>>5)<<2;
	int nSrclineByte = nSrcStep;
	int nMasklineByte = nMaskStep;
	
	int nMinDiffer = 1; 

	//对于蒙板为0的图像(前景)采用另外的阈值进行直方图增强
	int nLowMask = nLow +10;
	int nHighMask = nHigh + nMaskHigh;
	
	//对于蒙板不为0的图像(背景)采用更进一步阈值进行直方图增强
	nHigh = nHigh - nMaskHigh;
	
	//对蒙板不为0的直方图增强阈值进行检测
	if (nHigh>nMax)
	{
		nHigh = nMax;
	}
	if (nLow<nMin)
	{
		nLow = nMin;
	}
	if (nHigh == 255&&nLow==0)
	{
        return true;
	}
	//对蒙板为0的直方图增强阈值进行检测
	if (nHighMask>nMax)
	{
		nHighMask = nMax;
	}
	if (nLowMask<nMin)
	{
		nLowMask = nMin;
	}
	if (nHighMask == 255&&nLowMask ==0)
	{
        return true;
	}

	//对蒙板不为0的直方图计算修正值
	float fChangeDiffer = 0;
	int nDiffer = 0;
	nDiffer = nHigh-nLow;
	fChangeDiffer = (float)_MX_MAXVALUE/nDiffer;
	//计算修正值
	int nGrayValue[256] = {0};
	for (int n = 0; n<256; n++)
	{
		nGrayValue[n] = CheckValue((int)((n - nLow) * fChangeDiffer+0.5));
	}

	//对蒙板为0的直方图计算修正值
	nDiffer = nHighMask-nLowMask;
	fChangeDiffer = (float)_MX_MAXVALUE/nDiffer;
	//计算修正值
	int nGrayValueMask[256] = {0};
	for (int n = 0; n<256; n++)
	{
		nGrayValueMask[n] = CheckValue((int)((n - nLowMask) * fChangeDiffer+0.5));
	}

	//源图像通道数量
	int nSrcChannels = nSrcBpp/8;

	//彩色24位图像
	//修正
	for (int y = 0; y < nSrcHeight; y++)
	{
		for (int x = 0; x < nSrcWidth; x++)
		{
			int nPos = y*nSrclineByte+x*nSrcChannels;
			int nPosMask = y*nMasklineByte+x;
			if(pMask== NULL || pMask[nPosMask] != 0)
			{
				//背景
				if (nSrcBpp == 24)
				{
					//RGB
#if 0
					pSrc[nPos+2] = nGrayValue[pSrc[nPos+2]];
					pSrc[nPos+1] = nGrayValue[pSrc[nPos+1]];
					pSrc[nPos]   = nGrayValue[pSrc[nPos]];
#else
					pSrc[nPos+2] = 255;
					pSrc[nPos+1] = 255;
					pSrc[nPos]   = 255;
#endif
				}
				else
				{
#if 0 
					//Gray
					pSrc[nPos]   = nGrayValue[pSrc[nPos]];
#else
					pSrc[nPos]   = 255;
#endif
				}
			}//end if(pMask== NULL || pMask[nPosMask] != 0)
			else
			{
				if (nSrcBpp == 24)
				{
#if 0
					//RGB
					pSrc[nPos+2] = nGrayValueMask[pSrc[nPos+2]];
					pSrc[nPos+1] = nGrayValueMask[pSrc[nPos+1]];
					pSrc[nPos]   = nGrayValueMask[pSrc[nPos]];
#else
					pSrc[nPos+2] = 0;
					pSrc[nPos+1] = 0;
					pSrc[nPos]   = 0;
#endif
				}
				else
				{
					//Gray
					pSrc[nPos]   = nGrayValueMask[pSrc[nPos]];
				}
					
			}//end if(pMask== NULL || pMask[nPosMask] != 0) else
		}//end for (int x = 0; x < nSrcWidth; x++)
	}//end for (int y = 0; y < nSrcHeight; y++)
	

    return true;
}

//自动调整色阶,考虑蒙版
//参数
//[in]src		:	源图像
//[out]dst		:	目标图像
//[in]nMinTh	:	最低阈值（直接采用此阈值）
//[in]nMaxTh	:	算法会计算最优化阈值，如果阈值大于nMaxTh，则使其等于nMaxTh
//[in]mask		:	蒙板（文档化使用）
//返回值
//true	:	操作成功
//false	:	操作失败
bool CAutoLevel::AdjustLevelAuto2(Mat src,Mat &dst,int nMinTh, int nMaxTh, Mat mask)
{
	int nThreshold_Low = nMinTh;
	if(!src.data)
	{
		return false;
	}

	if(src.channels() == 3)
	{
		//RGB
		vector<Mat> vcMat;
		Mat r,g,b;
		cv::split( src, vcMat);
        //IplImage *ipl = &IplImage(dst);
        IplImage iplTmp = IplImage(dst);
        IplImage *ipl = &iplTmp;
		int nThresholdGray = CAdaptiveThreshold::OptimalThreshold(ipl);
	
		for(int n =0; n < 3; n++)
		{
            //IplImage *ipl = &IplImage(vcMat[n]);
            IplImage iplTmp = IplImage(vcMat[n]);
            IplImage *ipl = &iplTmp;
			int nThreshold = CAdaptiveThreshold::OptimalThreshold(ipl,0);
			
			float fRadioWeight = 0.5;
			nThreshold = nThresholdGray*(1-fRadioWeight) + nThreshold*fRadioWeight;

			if(nThreshold > nMaxTh)
			{
				nThreshold = nMaxTh;
			}
			CAutoLevel::StrechHistogram2(vcMat[n].data, vcMat[n].cols, vcMat[n].rows, vcMat[n].channels()*8, vcMat[n].step,nThreshold_Low,nThreshold,
				0,255,
				mask.data,mask.step);
		}
		cv::merge(vcMat,dst);

	}
	else
	{
		//灰度
        //IplImage *ipl = &IplImage(dst);
        IplImage iplTmp = IplImage(dst);
        IplImage *ipl = &iplTmp;
		int nThreshold = CAdaptiveThreshold::OptimalThreshold(ipl);
		if(nThreshold > nMaxTh)
		{
			nThreshold = nMaxTh;
		}
		CAutoLevel::StrechHistogram2(dst.data, dst.cols, dst.rows, dst.channels()*8, dst.step,nThreshold_Low,nThreshold,
			0,255,
			mask.data,mask.step);

	}

	return true;
}
