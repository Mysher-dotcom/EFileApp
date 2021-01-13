#include "stdafx.h"
#include "../Transform/Rotate.h"
#include "DetectRectBySegmation.h"
#include "../ColorSpace/AdaptiveThreshold.h"

//V 0.1.0 Bicycle  初版调整   Time:2018年1月22日10:13:15
//V 0.1.1Tmp Bicycle 提交版(未做整理)  Time:2018年1月24日14:34:52
//V 0.1.3 Time:2018年2月1日13:35:53

//裁切纠偏
//参数:
//[in] src:分割图像，24bit
//返回值:
//在原图上以红色框选出检测区域
Mat CDetectRectBySegmation::DetectRectPreview(Mat src)
{
	vector<vector<Point> > vcResult;
	Mat dst;
	if(!src.data)
	{
		return dst;
	}

	//确保为24bit，3通道
	Mat srcEff;
	if(src.channels() == 3)
	{
		srcEff = src.clone();
	}
	else
	{
		cvtColor(src, srcEff, CV_GRAY2BGR); 
	}

	//图像头部存在干扰需要裁切一部分后继续操作
	//int nY = 20;
	//Mat srcROI( src, Rect(0,nY,src.cols,src.rows-nY));
	int nY = 0;
	Mat srcROI = srcEff;

	//插值
#if 1
	//A4宽度8.3inch,200DPI
	//int nDstWidth = 1669;
	int nDstWidth = 830;
	float fRadio = (float)nDstWidth/srcROI.rows;
#else
	float fRadio = 0.4;
#endif

	Mat img = Mat(srcROI.rows*fRadio,srcROI.cols*fRadio,srcROI.type());
	cv::resize(srcROI,img,img.size(),0,0);

	//cv::GaussianBlur(img,img,cv::Size(5,5),1);

	//检测背景
	//得到标记分割结果的图像，尺寸等于原图，位数和原图相同，绿色（0，255，0）表示背景，前景不变
	Mat mask;
	mask = CDetectRectBySegmation::BgSegmation(img);


	//查找矩形目标
	vector<vector<cv::Point> > vcRect =  CDetectRectBySegmation::DetectRect(mask);

	//进一步细分
	for(int i=0;i<vcRect.size();i++)
	{
		vector<Point> ptSrc4Adjust = CDetectRectBySegmation::AdjustRect(vcRect[i], img);
		vcResult.push_back(ptSrc4Adjust);
	}
	

	dst = srcEff.clone();
	//绘制检测结果
	for(int i=0;i<vcResult.size();i++)
	{
		for(int j=0;j<=3;j++)
		{
			//line(dst,P[j]/fRadio,P[(j+1)%4]/fRadio,Scalar(0,0,255),2);
			cv::Point ptS = Point(vcResult[i][j].x/fRadio,vcResult[i][j].y/fRadio);
			cv::Point ptE = Point(vcResult[i][(j+1)%4].x/fRadio,vcResult[i][(j+1)%4].y/fRadio);
			if(ptS.y == nY)
			{
				ptS.y -= nY;
			}
			if(ptE.y == nY)
			{
				ptE.y -= nY;
			}
			line(dst,ptS,ptE,Scalar(0,0,255),1);
			//line(dst,P[j]/fRadio,P[(j+1)%4]/fRadio,vcColor[j],2);
		}
	}

	return dst;
}

//裁切纠偏Beta
//参数:
//[in] src:分割图像，24bit
//返回值:
//矩形位置向量
vector<RotatedRect> CDetectRectBySegmation::DoDetectRect2(Mat src)
{
 	vector<RotatedRect> vcResult;
	if(!src.data || src.channels()!=3)
	{
		return vcResult;
	}

	//图像过小则不进行处理
	if(src.rows <50 || src.cols <50)
		return vcResult;

	//插值
	float fChangeRatio = sqrt(float(src.rows*src.cols)/5000000.0);

	int nDstWidth;
	if(fChangeRatio <=1)
		nDstWidth= src.rows;//830;
	else
		nDstWidth = int((float)src.rows/fChangeRatio);
	float fRadio = (float)nDstWidth/src.rows;
	
	Mat img = Mat(src.rows*fRadio,src.cols*fRadio,src.type());
	cv::resize(src,img,img.size(),0,0,INTER_CUBIC);
	//边缘补色
	int constScalar= CDetectRectBySegmation::calculateBackGroundValue(img);
	//增加边缘
	int nIncreaseEdge = 8;
	copyMakeBorder(img,img,nIncreaseEdge,nIncreaseEdge,nIncreaseEdge,nIncreaseEdge,BORDER_CONSTANT,Scalar(constScalar,constScalar,constScalar));	
	Mat maskTmp = img.clone();
	Rect blackRect;
	blackRect.x = nIncreaseEdge +1;
	blackRect.y = nIncreaseEdge +1;
	blackRect.width = img.cols-2*(nIncreaseEdge+1);
	blackRect.height = img.rows -2*(nIncreaseEdge+1);
	rectangle(img,blackRect,Scalar(constScalar,constScalar,constScalar),nIncreaseEdge*2);
	//补边缘
	cvtColor(maskTmp,maskTmp,CV_BGR2GRAY);
	threshold(maskTmp,maskTmp,128,255,THRESH_BINARY);
	cvtColor(maskTmp,maskTmp,COLOR_GRAY2BGR);
	maskTmp.colRange(2*nIncreaseEdge+1,maskTmp.cols-2*nIncreaseEdge-1).rowRange(2*nIncreaseEdge+1,maskTmp.rows -2*nIncreaseEdge -1)
		 =Mat::zeros(maskTmp.rows - 2*(2*nIncreaseEdge+1),maskTmp.cols -2*(2*nIncreaseEdge+1),maskTmp.type());

	img =img +maskTmp;
	GaussianBlur(img,img,Size(3,3),0,0);
	//检测背景
	//得到标记分割结果的图像，尺寸等于原图，位数和原图相同，绿色（0，255，0）表示背景，前景不变
	Mat mask;
	vector<Mat> vMat;
	split(img,vMat);

	Canny(vMat[0],vMat[0],50,150);
	Canny(vMat[1],vMat[1],50,150);
	Canny(vMat[2],vMat[2],50,150);

	mask = Mat::zeros(img.size(),CV_8UC1);
	uchar *dataB = vMat[0].ptr<uchar>(0);
	uchar *dataG = vMat[1].ptr<uchar>(0);
	uchar *dataR = vMat[2].ptr<uchar>(0);
	uchar *data = mask.ptr<uchar>(0);

	for (int idr=0;idr<mask.rows;idr++)
	{
		for (int idc=0;idc<mask.cols;idc++,dataB++,dataG++,dataR++,data++)
		{
			if(255 == *dataB || 255 ==*dataG || 255 ==*dataR)
				*data =255;
		}
	}
	//查找矩形目标
	vector<vector<cv::Point> > vcRect =  CDetectRectBySegmation::DetectRect(mask);
	//进一步细分
	vector<vector<Point> > vcPoint;
	for(int i=0;i<vcRect.size();i++)
	{
		vector<Point> ptSrc4Adjust = CDetectRectBySegmation::AdjustRect(vcRect[i], img);
		vcPoint.push_back(ptSrc4Adjust);
	}

	for(int n =0; n < vcPoint.size(); n++)
	{
		for(int m =0; m< vcPoint[n].size(); m++)
		{
			vcPoint[n][m].x = (vcPoint[n][m].x-nIncreaseEdge+1)/fRadio;
			vcPoint[n][m].y = (vcPoint[n][m].y-nIncreaseEdge+1)/fRadio;
		}
		RotatedRect rect=minAreaRect(vcPoint[n]);
		
		vcResult.push_back(rect);
	}
	//以下步骤处理两矩形相交情况
	vector<RotatedRect> vRRectTest = CDetectRectBySegmation::DoSuperposition(vcResult);
	vector<RotatedRect> vRRectTest1 = CDetectRectBySegmation::DoSuperposition(vRRectTest);
	return vRRectTest1;
}

//裁切纠偏
//参数:
//[in] src:分割图像，24bit
//返回值:
//矩形位置向量
vector<RotatedRect> CDetectRectBySegmation::DoDetectRect(Mat src)
{
	vector<RotatedRect> vcResult;
	if(!src.data || src.channels()!=3)
	{
		return vcResult;
	}

	//插值
#if 1
	//A4宽度8.3inch,200DPI
	//int nDstWidth = 1669;
	int nDstWidth = 830;
	float fRadio = (float)nDstWidth/src.rows;
#else
	float fRadio = 0.4;
#endif

	Mat img = Mat(src.rows*fRadio,src.cols*fRadio,src.type());
	cv::resize(src,img,img.size(),0,0);

	//检测背景
	//得到标记分割结果的图像，尺寸等于原图，位数和原图相同，绿色（0，255，0）表示背景，前景不变
	Mat mask = CDetectRectBySegmation::BgSegmation(img);

	//查找矩形目标
	vector<vector<cv::Point> > vcRect =  CDetectRectBySegmation::DetectRect(mask);

	//进一步细分
	vector<vector<Point> > vcPoint;
	for(int i=0;i<vcRect.size();i++)
	{
		vector<Point> ptSrc4Adjust = CDetectRectBySegmation::AdjustRect(vcRect[i], img);
		vcPoint.push_back(ptSrc4Adjust);
	}

	for(int n =0; n < vcPoint.size(); n++)
	{
		for(int m =0; m< vcPoint[n].size(); m++)
		{
			vcPoint[n][m].x /= fRadio;
			vcPoint[n][m].y /= fRadio;
		}
		RotatedRect rect=minAreaRect(vcPoint[n]);
		vcResult.push_back(rect);
	}

	return vcResult;
}

//分割背景
//参数:
//[in] src:原图
//返回值:
//标记分割结果的图像，尺寸等于原图，位数和原图相同，绿色（0，255，0）表示背景，前景不变
Mat CDetectRectBySegmation::BgSegmation(Mat src)
{
	Mat dst;
	if(!src.data)
	{
		return dst;
	}
	
	//通过漫水填充法分割背景
	dst = CDetectRectBySegmation::BgSegmationByFloodFill(src);
	return dst;
}

//查找矩形目标
//参数:
//[in] src:背景分割图像，8bit，黑色表示背景，白色表示前景
//返回值:
//矩形位置向量
vector<vector<cv::Point> > CDetectRectBySegmation::DetectRect(Mat src)
{
	vector<vector<cv::Point> > vcResult;
	if(!src.data)
	{
		return vcResult;
	}

	vcResult = CDetectRectBySegmation::DetectRectByContours(src);

	return vcResult;
}

//对粗分区域进行进一步细分
//参数:
//[in] rect:粗分区域
//[in] img:灰度图
//返回值:
//精分区域（4个顶点）
vector<Point> CDetectRectBySegmation::AdjustRect(vector<Point> rect, Mat img)
{
	vector<Point> vcResult;
	if(!img.data)
	{
		return vcResult;
	}

	//vcResult = CDetectRectBySegmation::AdjustByRotate(vcRect[i], img);
	vcResult = CDetectRectBySegmation::AdjustByFindPoint(rect, img);

	return vcResult;
}

//通过漫水填充法分割背景
//参数:
//[in] src:原图
//[in] nLoUpDiff:亮度差异浮动值,插值在此以内，都认为是相同颜色，默认为3
//返回值:
//标记分割结果的图像，尺寸等于原图，8bit，黑色表示背景，白色表示前景
Mat CDetectRectBySegmation::BgSegmationByFloodFill(Mat src,int nLoUpDiff)
{
	//显示过程图像
	bool bShowMask = false;

	Mat dst;
	if(!src.data)
	{
		return dst;
	}

	//对图像进行去噪操作，使其平滑
	Mat res; 
#if 1
	//int spatialRad = 50;  //空间窗口大小
	//int colorRad = 50;   //色彩窗口大小
	//int maxPyrLevel = 2;  //金字塔层数
	//设为5是基本
	int spatialRad = 3;  //空间窗口大小
	//int spatialRad =6;
	//int colorRad = 6;   //色彩窗口大小
	int colorRad =6;
	int maxPyrLevel = 2;  //金字塔层数
	nLoUpDiff = 5;
	//色彩聚类平滑滤波
	pyrMeanShiftFiltering(src, res, spatialRad, colorRad, maxPyrLevel); 
#else
	cv::medianBlur(src, res, 5);
	//cv::medianBlur(src, res, 5);
	//cv::GaussianBlur(src, res,cv::Size(7,7),2);
#endif
	//src = res.clone();
	//生成背景分割的
	dst = Mat::zeros(res.rows,res.cols,CV_8U);
	dst = 255 - dst;

    //IplImage *iplSrc =  &(IplImage)res;
    IplImage iplSrcTmp =  (IplImage)res;
    IplImage *iplSrc =  &iplSrcTmp;
	vector<CvConnectedComp> vectRegions;
	int nConnCompNum = 0;
	//创建临时掩模用于区域填充
	CvRect size;
	size.height=iplSrc->height;
	size.width =iplSrc->width;
	IplImage* pImMask = cvCreateImage(cvSize(size.width+2, size.height+2), 8, 1);
	
	cvZero(pImMask);
	//将连通区填充的灰度级容差置为0
	CvScalar lo_diff = cvScalarAll(nLoUpDiff);
	CvScalar up_diff = cvScalarAll(nLoUpDiff);

	uchar* pSrc = NULL;
	uchar* pMask = NULL;

	int i, j;       
	CvConnectedComp connTemp;
	CvPoint seed_point;
	IplImage mask;
	IplImage* pImgBin = iplSrc;
	IplImage* pImgMask = &mask;
	CvMat m;
	cvGetMat(iplSrc, &m);
	cvGetImage(&m, pImgBin);

	cvGetMat(pImMask, &m);
	cvGetImage(&m, pImgMask);

	int nBinStep = pImgBin->widthStep;
	int nMaskStep = pImgMask->widthStep;

	//逐行扫描
	pSrc = (uchar*)(pImgBin->imageData);
	pMask = (uchar*)(pImgMask->imageData);
	pMask += nMaskStep;
	RNG rng = theRNG();
	//记录填充颜色
	vector<cv::Scalar> vcColor;
	//记录需要过滤的区域，以填充颜色序列号标记
	vector<int> vcColorFilterIndex;
	//区域面积小于这个值则被过滤
	int nAreaThreshold = pImgBin->width * pImgBin->height * 0.005;
	int nHeightThreshold =  pImgBin->height * 0.001;

	/*
	//循环填充需要的计数工具
	struct SLoop4FloodFill
	{
		//Y方向计数
		int nY_Start;
		int nY_End;
		int nY_Step;
		//X方向计数
		int nX_Start;
		int nX_End;
		int nX_Step;

		SLoop4FloodFill(int nY_Start, int nY_End, int nY_Step,int nX_Start, int nX_End, int nX_Step )
		{
			this->nY_Start = nY_Start;
			this->nY_End = nY_End;
			this->nY_Step = nY_Step;

			this->nX_Start = nX_Start;
			this->nX_End = nX_End;
			this->nX_Step = nX_Step;
		}
	};
	*/

	//只遍历上下左右4边，以加减少算法处理工作，快算法速度
	vector<SLoop4FloodFill> vcLoop4FloodFill;
	//上边
	vcLoop4FloodFill.push_back(SLoop4FloodFill(0,pImgBin->height,pImgBin->height-1,
		0,pImgBin->width,1));
	//左边
	vcLoop4FloodFill.push_back(SLoop4FloodFill(0,pImgBin->height,1,
		0,pImgBin->width,pImgBin->width));

	//右边
	vcLoop4FloodFill.push_back(SLoop4FloodFill(0,pImgBin->height,1,
		pImgBin->width-1,pImgBin->width,pImgBin->width));
	//下边
	vcLoop4FloodFill.push_back(SLoop4FloodFill(pImgBin->height-1,pImgBin->height,1,
		0,pImgBin->width,1));
	
#if 0
	cvNamedWindow("pImgBin",0);
	cvShowImage("pImgBin",pImgBin);
	cvWaitKey();

	cvNamedWindow("pImgMask",0);
	cvShowImage("pImgMask",pImgMask);
	cvWaitKey();
#endif

	for(int nLoopIndex = 0; nLoopIndex < vcLoop4FloodFill.size(); nLoopIndex++)
	{
		SLoop4FloodFill loopData = vcLoop4FloodFill[nLoopIndex];

		pSrc = (uchar*)(pImgBin->imageData);
		pMask = (uchar*)(pImgMask->imageData);
		//for (i = 0; i < pImgBin->height; i++,pSrc += nBinStep,pMask += nMaskStep)
		for (i = loopData.nY_Start; i < loopData.nY_End; i += loopData.nY_Step,pSrc += nBinStep,pMask += nMaskStep)
		{
			seed_point.y = i;
			//for (j = 0; j < pImgBin->width; j++)
			for (j = loopData.nX_Start; j < loopData.nX_End; j += loopData.nX_Step)
			{
				//未被填充过
				if (pMask[j+1] == 0 )
				{
					//如果暗则认为是背景
					bool bNeedFilter = false;
					int nPosSrc = i * src.step + j * res.channels();
					Scalar oldVal( res.data[nPosSrc],res.data[nPosSrc+1], res.data[nPosSrc+2] );
					if( res.data[nPosSrc] <= CDetectRectBySegmation::m_nThBackGround &&
						res.data[nPosSrc+1] <= CDetectRectBySegmation::m_nThBackGround &&
						res.data[nPosSrc+2] <= CDetectRectBySegmation::m_nThBackGround)
					{
						bNeedFilter = true;
					}

					seed_point.x = j;
					//填充当前点所在的前景连通区，将其标号置为连通区个数
					//需要采用8连通，4联通会导致连通域数量过大
					//Scalar newVal( rng(256), rng(256), rng(256) );
					Scalar newVal(rng.uniform(200,256), rng.uniform(200,256), rng.uniform(200,256));
				
					cvFloodFill(pImgBin, seed_point, newVal, lo_diff, 
							up_diff, &connTemp, 8, pImgMask); 

					//存储所得连通区
					vectRegions.push_back(connTemp);
					//连通区标号+1
					nConnCompNum++;
					//记录当前区域填充颜色
					vcColor.push_back(newVal);
					
					//如果区域面积过小，则认为也是需要过滤的
					//if(nLoopIndex == 0 && connTemp.rect.height < nHeightThreshold)
					if(true == bNeedFilter)
					{
						vcColorFilterIndex.push_back(vcColor.size()-1);
					}

				}
			}
		}//end for (i = 0; i < pImgBin->height; i++)
	}
	cvReleaseImage(&pImMask);

	//显示过程图像
	if(bShowMask)
	{
		cv::namedWindow("FloodFill",0);
		cv::imshow("FloodFill",res);
		cv::imwrite("D:\\FloodFill.jpg",src);
		//::waitKey();
	}

#if 0
	//查找背景连通域
	//获取最大的填充区域
	CvConnectedComp connMax;
	connMax.area = 0;
	int nIndexMax = -1;
	for(int n =0; n < vectRegions.size(); n++)
	{
		if(connMax.area < vectRegions[n].area)
		{
			connMax = vectRegions[n];
			nIndexMax = n;
		}
	}//end for


	//将背景连通域标记为黑色
	if(nIndexMax != -1)
	{
		for(int row = 0; row < dst.rows; row++)
		{
			for(int col = 0; col < dst.cols; col++)
			{
				int nPosSrc = row * src.step + col * res.channels();
				int nPosDst = row * dst.step + col * dst.channels();
				Scalar oldVal( res.data[nPosSrc],res.data[nPosSrc+1], res.data[nPosSrc+2] );
				if(oldVal == vcColor[nIndexMax])	
				{
					dst.data[nPosDst] = 0;
				}//end if(oldVal == vcColor[nIndexMax])
				else
				{
					for(int n =0; n<vcColorFilterIndex.size();n++)
					{
						if(oldVal == vcColor[vcColorFilterIndex[n]])
						{
							dst.data[nPosDst] = 0;
						}
					}
				}
			}
		}
	}
#else
	for(int row = 0; row < dst.rows; row++)
	{
		for(int col = 0; col < dst.cols; col++)
		{
			int nPosSrc = row * src.step + col * res.channels();
			int nPosDst = row * dst.step + col * dst.channels();
			Scalar oldVal( res.data[nPosSrc],res.data[nPosSrc+1], res.data[nPosSrc+2] );
			
			for(int n =0; n<vcColorFilterIndex.size();n++)
			{
				if(oldVal == vcColor[vcColorFilterIndex[n]])
				{
					dst.data[nPosDst] = 0;
				}
			}
		}
	}
#endif

	//通过形态学变换，保证边缘模糊时可以处理
#if 0
	int nIterations = 1;
	Mat element = getStructuringElement(MORPH_RECT, Size(5,5));
	//Mat element = getStructuringElement(MORPH_CROSS, Size(3,3));
	dilate(dst,dst,element,Point(-1,-1),nIterations);
	erode(dst,dst,element,Point(-1,-1),nIterations);
#endif

	//显示过程图像
	if(bShowMask )
	{
		cv::namedWindow("Mask",0);
		cv::imshow("Mask",dst);
		cv::waitKey();
	}
	//return src.clone();
	return dst;
}


//通过凸包判断点是否在rect中
bool IsPointInRotatedRect(Point pt, RotatedRect rect)
{
	Point2f pf[4];
    rect.points(pf);

	vector<Point> p;
	for(int n =0; n< 4; n++)
	{
		p.push_back(pf[n]);
	}
	p.push_back(pt);

	vector<int> hull;
	convexHull(Mat(p), hull, true);
	//绘制凸包
#if 0
	Mat tmp = Mat::zeros(1000,1000,CV_8UC3);
	for(int n =0; n < 4; n++)
	{
		cv::circle(tmp,pf[n],3,Scalar(0,0,255),-1);
	}
	cv::circle(tmp,pt,3,Scalar(0,255,0),-1);

	int hullcount = (int)hull.size();
    Point pt0 = p[hull[hullcount-1]];
	for(int i = 0; i < hullcount; i++ )
    {
        Point pt1 = p[hull[i]];
        line(tmp, pt0, pt1, Scalar(0, 255, 0), 1,LINE_AA);
        pt0 = pt1;
    }

	cv::namedWindow("tmp",0);
	cv::imshow("tmp",tmp);
	cv::waitKey();
#endif


	if(find(hull.begin(), hull.end(),4) == hull.end())
	{
		return true;
	}
	
	return false;

}

//通过轮廓查找矩形目标
//参数:
//[in] src:分割图像，8bit，黑色表示背景，白色表示前景
//返回值:
//矩形位置向量
vector<vector<cv::Point> > CDetectRectBySegmation::DetectRectByContours(Mat src)

{
	vector<RotatedRect> vcResultRotateRect;
	vector<vector<cv::Point> > vcResult;
	
	if(!src.data)
	{
		return vcResult;
	}

	//通过补边保证贴边情况
	//对检测结果需要减去添加的边缘，坐标系转换
	int nBorderLen = 8;
	Mat srcBorder;
	copyMakeBorder(src, srcBorder, nBorderLen, nBorderLen, nBorderLen, nBorderLen, 0, Scalar(0, 0, 0));

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(srcBorder, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	//drawContours(dst, contours, -1, Scalar(255, 0, 255));
	
	//过滤较小的轮廓
	vector<vector<Point> > contoursTmp;
	int nAreaTh = srcBorder.cols * srcBorder.rows * 0.05*0.05;
	int edgeScope =3+nBorderLen;
	for(int i=0;i<contours.size();i++)
	{		
		//绘制轮廓的最小外结矩形
		RotatedRect rect=minAreaRect(contours[i]);	
		if((rect.center.x <edgeScope || rect.center.x>srcBorder.cols -edgeScope ||rect.center.y < edgeScope ||rect.center.y >srcBorder.rows -edgeScope)
			&&
			(rect.size.width <4 || rect.size.height <4)
			&&
			(rect.angle <= 3 ||(rect.angle >87 &&rect.angle <93)||(rect.angle >177 &&rect.angle<183)||(rect.angle >267&&rect.angle<273))
			)
			;
		else //if(rect.size.area() >= nAreaTh)
		{
			vcResultRotateRect.push_back(rect);
			contoursTmp.push_back(contours[i]);
		}
	}
	contours = contoursTmp;

	//通过形态学变换合并组件（不可靠，试用中）
	vector<RotatedRect> vcResultTmp;
#if 1
	int non_smallContoursNum =0;
	vcResultTmp.clear();
	do
	{
		if(vcResultTmp.size()!=0)
		{
			vcResultRotateRect = vcResultTmp;
			vcResultTmp.clear();
		}
		Mat dst = Mat::zeros(srcBorder.rows,srcBorder.cols,CV_8U);

		//通过绘制实行轮廓使局部组件合并（不可靠，需进一步处理）
		drawContours(dst, contours, -1, Scalar(255, 255, 255),-1);
		//通过膨胀腐蚀使局部组件合并（不可靠）
		int nIterations = 3;
		Mat element = getStructuringElement(MORPH_RECT, Size(5,5));
		dilate(dst,dst,element,Point(-1,-1),nIterations);
		erode(dst,dst,element,Point(-1,-1),nIterations);

		findContours(dst, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		vector<vector<Point> > contoursTmp;
		non_smallContoursNum =0;
		for(int i=0;i<contours.size();i++)
		{		
			//绘制轮廓的最小外结矩形
			RotatedRect rect=minAreaRect(contours[i]);
			if(rect.size.area() >= nAreaTh || true )
			{
				vcResultTmp.push_back(rect);
				contoursTmp.push_back(contours[i]);
			}
			else
				non_smallContoursNum ++;
		}
		contours = contoursTmp;
	}while(vcResultRotateRect.size() != (vcResultTmp.size()+ non_smallContoursNum ));

	if(vcResultTmp.size()!=0)
	{
		vcResultRotateRect = vcResultTmp;
		vcResultTmp.clear();
	}
#endif

	//通过判断轮廓最小外接矩形距离合并组件（试用，费时）
#if 0
	vcResultTmp.clear();
	do
	{
		if(vcResultTmp.size()!=0)
		{
			vcResultRotateRect = vcResultTmp;
			vcResultTmp.clear();
		}

		Mat dst = Mat::zeros(srcBorder.rows,srcBorder.cols,CV_8U);
		//通过绘制实行轮廓使局部组件合并（不可靠，需进一步处理）
		drawContours(dst, contours, -1, Scalar(255, 0, 255),-1);
		for(int n = 0; n < contours.size();n++ )
		{
			if(contours[n].size() == 0)
			{
				continue;
			}
			vector<Point> contour1 = contours[n];
			RotatedRect rect1 = minAreaRect(contour1);
			int nWTh = rect1.size.width * 0.6;
			int nHTh = rect1.size.height * 0.6;
			
			for(int m = 0; m < contours.size();m++ )
			{
				if(n == m || contours[m].size() == 0)
				{
					continue;
				}
				
				vector<Point> contour2 = contours[m];
				RotatedRect rect2 = minAreaRect(contour2);
				
				if(rect1.size.width == 0 || rect1.size.height == 0 ||
					rect2.size.width == 0 || rect2.size.height == 0)
				{
					continue;
				}

				if(IsPointInRotatedRect(rect2.center, rect1))
				{
					//cv::line(dst,rect1.center,rect2.center,cvScalar(255,255,255),20);
					for each(Point pt2 in contours[m])
					{
						cv::line(dst,rect1.center,pt2,cvScalar(255,255,255),1);
					}
					contours[m].clear();
				}
				
			}
		}
		
		findContours(dst, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		//vector<vector<Point> > contoursTmp;
		for(int i=0;i<contours.size();i++)
		{		
			if(contours[i].size() == 0)
			{
				continue;
			}
			//绘制轮廓的最小外结矩形
			RotatedRect rect=minAreaRect(contours[i]);
			if(rect.size.area() >= nAreaTh)
			{
				vcResultTmp.push_back(rect);
				//contoursTmp.push_back(contours[i]);
			}
		}
		//contours = contoursTmp;
	}while(vcResultRotateRect.size() != vcResultTmp.size());

	if(vcResultTmp.size()!=0)
	{
		vcResultRotateRect = vcResultTmp;
		vcResultTmp.clear();
	}
#endif

	for(int n =0; n < vcResultRotateRect.size(); n++)
	{
		vector<cv::Point> vcPT;
		RotatedRect rect = vcResultRotateRect[n];
		if(rect.size.area() < nAreaTh)
			continue;
		
		Point2f P[4];
        rect.points(P);
        for(int j=0;j<=3;j++)
        {
			P[j].x -= nBorderLen;
			P[j].y -= nBorderLen;
			vcPT.push_back(P[j]);
        }
		vcResult.push_back(vcPT);
	}


	return vcResult;
}

//通过旋转，然后检测边缘对粗分区域进行进一步细分
//参数:
//[in] rect:粗分区域
//[in] img:灰度图
//返回值:
//精分区域（4个顶点）
vector<Point> CDetectRectBySegmation::AdjustByRotate(RotatedRect rect, Mat img)
{
	//区域裁切
	//RotatedRect rect = vcRect[i];
	Point2f Pf[4];
	rect.points(Pf);
	CvPoint P[4];
	for(int j=0;j<=3;j++)
	{
		P[j] = Pf[j];
	}
		
	vector<Point> ptSrc;
	for(int n =0; n < 4; n++)
	{
		ptSrc.push_back(Pf[n]);
	}
		
	int dstWidth;
	int dstHeight;
	float fA2 = -rect.angle;
	/*if(abs(fA2)>45)
	{
		float fM  = 1;
		if(fA2 < 0)
		{
			fM = -1;
		}
		fA2 = (90 - abs(fA2))*fM;
	}*/
	vector<Point> ptRotate = rotatePTArray(ptSrc,fA2,img.cols,img.rows,dstWidth,dstHeight);
	vector<Point> ptSrc2 = rotatePTArray(ptRotate,360-fA2,img.cols,img.rows,dstWidth,dstHeight);
		
	//return img;
	Mat regin = Rotate::RotateCut(img,P);
	//灰度化
	Mat gray;
	if(regin.channels() == 3)
	{
		cvtColor(regin, gray, CV_BGR2GRAY); 
	}
	else
	{
		gray = regin.clone();
	}
	//二值化
	int nTh = CAdaptiveThreshold::OptimalThreshold(gray);
	Mat bw;
	cv::threshold(gray, bw, nTh, 255, THRESH_BINARY);
	//return bw;
	//获取更准确位置
	cv::Rect rc = CDetectRectBySegmation::FindWhiteRectInBW(bw);
	//绘制图像判断结果
	cv::rectangle(regin,rc,cv::Scalar(0,0,255),3);
	//return regin;
	vector<Point> vcAdjust;
	vcAdjust.push_back(Point(rc.x,rc.y));
	vcAdjust.push_back(Point(rc.x+rc.width,rc.y));
	vcAdjust.push_back(Point(rc.x+rc.width,rc.y+rc.height));
	vcAdjust.push_back(Point(rc.x,rc.y+rc.height));

	Point ptLT = findPT(ptRotate);
	for(int n =0; n < vcAdjust.size(); n++)
	{
		vcAdjust[n].x += ptLT.x;
		vcAdjust[n].y += ptLT.y;
	}

	int dstWidth2;
	int dstHeight2;
	
	//vector<Point> ptSrc4Adjust = rotatePTArray(vcAdjust,fA2,img.cols,img.rows,dstWidth2,dstHeight2);
	vector<Point> ptSrc4Adjust = rotatePTArray(vcAdjust,360-fA2,img.cols,img.rows,dstWidth,dstHeight);
		
	//绘制图像判断结果
	/*for(int n=0;n<=3;n++)
	{
		cv::Point ptS = ptSrc4Adjust[n];
		cv::Point ptE = ptSrc4Adjust[(n+1)%4];
		line(img,ptS,ptE,Scalar(0,0,255),1);
	}*/
	//绘制图像判断结果
	for(int n=0;n<4;n++)
	{
		cv::Point ptS = Pf[n];
		cv::Point ptE = Pf[(n+1)%4];
		line(img,ptS,ptE,Scalar(255,0,0),1);
	}
	for(int n=0;n<ptRotate.size();n++)
	{
		cv::Point ptS = ptRotate[n];
		cv::Point ptE = ptRotate[(n+1)%4];
		line(img,ptS,ptE,Scalar(0,0,255),1);
	}
	for(int n=0;n<ptSrc2.size();n++)
	{
		cv::Point ptS = ptSrc2[n];
		cv::Point ptE = ptSrc2[(n+1)%4];
		line(img,ptS,ptE,Scalar(0,255,0),1);
	}

	cv::imshow("t",img);
	cv::waitKey();
	//return img;

	return ptSrc4Adjust;
}

//通过遍历直线对粗分区域进行进一步细分
//参数:
//[in] rect:粗分区域
//[in] img:灰度图
//返回值:
//精分区域（4个顶点）
vector<Point> CDetectRectBySegmation::AdjustByFindPoint(vector<Point> rect, Mat img)
{
	vector<Point> ptArrayResult;
	CvPoint P[4];
	for(int j=0;j<=3;j++)
	{
		P[j] = rect[j];
	}
	Mat regin = Rotate::RotateCut(img,P);
	//灰度化
	Mat gray;
	if(regin.channels() == 3)
	{
		cvtColor(regin, gray, CV_BGR2GRAY); 
	}
	else
	{
		gray = regin.clone();
	}
	Mat gray2;
	if(img.channels() == 3)
	{
		cvtColor(img, gray2, CV_BGR2GRAY); 
	}
	else
	{
		gray2 = img.clone();
	}
	
	
	//二值化
	//int nTh = CAdaptiveThreshold::OptimalThreshold(gray);
	int nTh2 = CAdaptiveThreshold::OptimalThreshold(gray);
	//预设背景
	int nTh = CDetectRectBySegmation::m_nThBackGround;
	int nType = 0;
	int nThEff = nTh;
	if(nTh2 < nTh*1.2)
	{
		nType =1;
		nThEff = nTh2;
	}

	ptArrayResult =  CDetectRectBySegmation::FindWhiteRectInBW2(
		P[0], P[1], 
		P[3], P[2],
		gray2, nThEff,nType);
	ptArrayResult =  CDetectRectBySegmation::FindWhiteRectInBW2(
		ptArrayResult[0], ptArrayResult[1], 
		ptArrayResult[3], ptArrayResult[2],
		gray2, nThEff,nType);
#if 0
	Mat bw;
	cv::threshold(gray, bw, nTh, 255, THRESH_BINARY);
	cv::line(img,P[0], P[1],cv::Scalar(0,255,0),10);
	cv::line(img,P[3], P[2],cv::Scalar(0,255,0),10);
	cv::line(img,P[0], P[3],cv::Scalar(0,255,0),10);
	cv::line(img,P[1], P[2],cv::Scalar(0,255,0),10);

	cv::line(img,ptArrayResult[0], ptArrayResult[1],cv::Scalar(0,0,255),10);
	cv::line(img,ptArrayResult[3], ptArrayResult[2],cv::Scalar(0,0,255),10);
	cv::line(img,ptArrayResult[0], ptArrayResult[3],cv::Scalar(0,0,255),10);
	cv::line(img,ptArrayResult[1], ptArrayResult[2],cv::Scalar(0,0,255),10);
	cv::namedWindow("bw",0);
	cv::namedWindow("t",0);
	cv::imshow("bw",bw);
	cv::imshow("t",img);
	cv::waitKey();
#endif

	return ptArrayResult;
}

//获取直线上所有的点
//采用二分法
//参数:
//[in] start:起始点坐标
//[in] end:终止点坐标
//[out] save:直线上点的集合
//返回值:
//无
void CDetectRectBySegmation::find_all_point(Point start, Point end, vector<Point> &save)  
{  
    if (abs(start.x - end.x) <= 1 && abs(start.y - end.y) <= 1)  
    {  
        save.push_back(start);  
        return; /*点重复时返回*/  
    }  

    Point point_center;  
    point_center.x = (start.x + end.x) / 2;  
    point_center.y = (start.y + end.y) / 2;  
   
    find_all_point(start, point_center, save);  /*递归*/  
	save.push_back(point_center);   /*储存中点*/  
    find_all_point(point_center, end, save);  
}

 
//填充RotatedRect（带有角度的矩形）
//通过找到矩形两条平行边上所有的点，并用直线将平行线对应的点连接，以达到填充效果
//参数:
//[in/out] image:源图像
//[in/out] rect:带有角度的矩形的位置
//[in/out] color:填充颜色
//返回值:
//无
void CDetectRectBySegmation::full_rotated_rect(Mat &image, const RotatedRect &rect, const Scalar &color)
{ 
    CvPoint2D32f point[4];  
    Point pt[4];  
    vector<Point> center1, center2;  

    /*画出外框*/  
    cvBoxPoints(rect, point);  
    for (int i = 0; i<4; i++)  
    {  
        pt[i].x = (int)point[i].x;  
        pt[i].y = (int)point[i].y;  
    }  
    line(image, pt[0], pt[1], color, 1);  
    line(image, pt[1], pt[2], color, 1);  
    line(image, pt[2], pt[3], color, 1);  
    line(image, pt[3], pt[0], color, 1);  

    /*填充内部*/  
    find_all_point(pt[0], pt[1], center1);  /*找出两点间直线上的所有点*/  
    find_all_point(pt[3], pt[2], center2);  
    vector<Point>::iterator itor1 = center1.begin(), itor2 = center2.begin();  
    while (itor1 != center1.end() && itor2 != center2.end())  
    {  
        line(image, *itor1, *itor2, color, 1);  /*连接对应点*/  
        itor1++;  
        itor2++;  
    }  

    vector<Point>().swap(center1);  
    vector<Point>().swap(center2);  
	return;
}

//查找图像中白色区域
//参数:
//[in] src:二值后灰度图像，8bit，图像中含黑色背景和白色前景，一般情况，前景为矩形且无角度
//返回值:
//白色区域位置
cv::Rect CDetectRectBySegmation::FindWhiteRectInBW(Mat src)
{
	cv::Rect rcResult = cv::Rect(0,0,src.cols,src.rows);
	if(!src.data || src.channels() != 1 || src.type() != CV_8U)
	{
		return rcResult;
	}

	int width = src.cols;
	int height = src.rows;

	//左
	int nLeft = -1;
	for(int x = 0; x < width; x++)
	{
		for(int y = 0; y < height; y++)
		{
			int npos = y * src.step + x * src.channels();
			if(src.data[npos] != 0)
			{
				nLeft = x;
				break;
			}
		}
		if(nLeft != -1)
		{
			break;
		}
	}

	//右
	int nRight = -1;
	for(int x = width-1; x >=0; x--)
	{
		for(int y = 0; y < height; y++)
		{
			int npos = y * src.step + x * src.channels();
			if(src.data[npos] != 0)
			{
				nRight = x;
				break;
			}
		}
		if(nRight != -1)
		{
			break;
		}
	}

	//上
	int nTop = -1;
	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			int npos = y * src.step + x * src.channels();
			if(src.data[npos] != 0)
			{
				nTop = y;
				break;
			}
		}
		if(nTop != -1)
		{
			break;
		}
	}

	//下
	int nBottom = -1;
	for(int y = height-1; y >= 0; y--)
	{
		for(int x = 0; x < width; x++)
		{
			int npos = y * src.step + x * src.channels();
			if(src.data[npos] != 0)
			{
				nBottom = y;
				break;
			}
		}
		if(nBottom != -1)
		{
			break;
		}
	}

	rcResult = cv::Rect(nLeft,nTop,nRight - nLeft,nBottom - nTop);
	return rcResult;
}

//旋转
//参数
//[in] ptArray:需要旋转的点集
//[in] angle:旋转角度，单位°
//[in] width:图像宽度
//[in] height:图像高度
//[out] dstWidth:旋转图像宽度
//[out] dstHeight:旋转图像高度
//返回值:
//旋转后的点集
vector<Point> CDetectRectBySegmation::rotatePTArray(vector<Point> ptArray, float angle, int width, int height, int &dstWidth, int &dstHeight)
{
	#define PI 3.1415926535
	//角度到弧度转化
	#define RADIAN(angle) ((angle)*PI/180.0)

	vector<Point> ptArrayResult;

	int srcW = width;
	int srcH = height;

	//以图像中心为原点左上角，右上角，左下角和右下角的坐标,用于计算旋转后的图像的宽和高
	Point pLT,pRT,pLB,pRB;
	pLT.x = -srcW/2;pLT.y = srcH/2;
	pRT.x = srcW/2;pRT.y = srcH/2;
	pLB.x = -srcW/2;pLB.y = -srcH/2;
	pRB.x = srcW/2; pRB.y = -srcH/2;
	//旋转之后的坐标
	Point pLTN,pRTN,pLBN,pRBN;
	double sina = sin(RADIAN(-angle));
	double cosa = cos(RADIAN(-angle));
	pLTN.x = pLT.x*cosa + pLT.y*sina;    
	pLTN.y = -pLT.x*sina + pLT.y*cosa;
	pRTN.x = pRT.x*cosa + pRT.y*sina;
	pRTN.y = -pRT.x*sina + pRT.y*cosa;
	pLBN.x = pLB.x*cosa + pLB.y*sina;
	pLBN.y = -pLB.x*sina + pLB.y*cosa;
	pRBN.x = pRB.x*cosa + pRB.y*sina;
	pRBN.y = -pRB.x*sina + pRB.y*cosa;
	//旋转后图像宽和高
	dstWidth = max(abs(pRBN.x - pLTN.x),abs(pRTN.x - pLBN.x));
	dstHeight = max(abs(pRBN.y - pLTN.y),abs(pRTN.y - pLBN.y));

	
	int nW1 = width * 0.5;
	int nH1 = height * 0.5;
	for(int n =0; n < ptArray.size(); n++)
	{
		Point ptSrc = ptArray[n];
		Point ptDst;
		ptDst.x = (ptSrc.x - nW1)*cosa + (ptSrc.y + nH1)*sina;    
		ptDst.y = -(ptSrc.x - nW1)*sina +(ptSrc.y + nH1)*cosa;
		ptDst.x = ptDst.x + nW1; 
		ptDst.y = ptDst.y - nH1;
		ptArrayResult.push_back(ptDst);
	}

	Point ptLT0 = CDetectRectBySegmation::findPT(ptArray);
	Point ptLT1 = CDetectRectBySegmation::findPT(ptArrayResult);

	int nX = ptLT1.x - ptLT0.x;
	int nY = ptLT1.y - ptLT0.y;
	for(int n =0; n < ptArrayResult.size(); n++)
	{
		ptArrayResult[n].x -= nX;
		ptArrayResult[n].y -= nY;
	}

	return ptArrayResult;
}

//查找4个点中的左上角
//一般而言，4个点关系为矩形的4个顶点，矩形带角度
//参数
//[in] ptArray:矩形的4个顶点
//返回值:
//左上角
Point CDetectRectBySegmation::findPT(vector<Point> ptArray)
{
	Point pt;
	//获取左上角
	Point pLeftTop;
	vector<int> vcX;
	vector<int> vcY;
	for(int j=0;j<=3;j++)
	{
		vcX.push_back(ptArray[j].x);
		vcY.push_back(ptArray[j].y);
	}
	sort(vcX.begin(),vcX.end());
	sort(vcY.begin(),vcY.end());
	for(int j=0;j<=3;j++)
	{
		if((ptArray[j].x == vcX[0] || ptArray[j].x == vcX[1])&& 
			(ptArray[j].y == vcY[0] || ptArray[j].y == vcY[1]))
		{
			pLeftTop = ptArray[j];
			break;
		}
	}
	pt = pLeftTop;
	return pt;
}


//查找图像中白色区域
//通过直线逼近的方式
//参数:
//[in] src:二值后灰度图像，8bit
//[in] nTh:二值化阈值
//[in] nType:目标类型,0-白色，1-黑色
//返回值:
//白色前景位置
vector<Point> CDetectRectBySegmation::FindWhiteRectInBW2(
		Point ptL1_Start, Point ptL1_End, 
		Point ptL2_Start, Point ptL2_End,
		Mat src, int nTh, int nType)
{
	vector<Point> vcAdjust;

	//找出两点间直线上的所有点
	vector<Point> center1, center2;  
	find_all_point(ptL1_Start, ptL1_End, center1);  
    find_all_point(ptL2_Start, ptL2_End, center2); 

#if 0
	Mat tmp = src.clone();
	for each(Point pt in center1)
	{
		cv::circle(tmp,pt,5,Scalar(255,0,0));
	}
	for each(Point pt in center2)
	{
		cv::circle(tmp,pt,5,Scalar(255,0,0));
	}
	cv::namedWindow("cc",0);
	cv::imshow("cc",tmp);
	cv::waitKey();
#endif
	/*
	struct SLoopData
	{
	public:
		int m_nStart;
		int m_nEnd;
		int m_nStep;

		int m_nStart2;
		int m_nEnd2;
		int m_nStep2;

		SLoopData(int nStart, int nEnd, int nStep,
			int nStart2, int nEnd2, int nStep2)
		{
			this->m_nStart = nStart;
			this->m_nEnd = nEnd;
			this->m_nStep = nStep;

			this->m_nStart2 = nStart2;
			this->m_nEnd2 = nEnd2;
			this->m_nStep2 = nStep2;
		}
	};
	*/
	vector<SLoopData> vcLoopData;
	//上两点
	vcLoopData.push_back(SLoopData(0,center1.size(),1,0,center2.size(),1));
	//下两点
	vcLoopData.push_back(SLoopData(center1.size()-1,-1,-1,center2.size()-1,-1,-1));

	for(int i =0; i < vcLoopData.size(); i++ )
	{
		vector<Point>::iterator itor1 = center1.begin(), itor2 = center2.begin();  
		Point pt0 = cv::Point(-1,-1);
		Point pt1 = cv::Point(-1,-1);
		int nStart = vcLoopData[i].m_nStart;
		int nEnd = vcLoopData[i].m_nEnd;
		int nStep = vcLoopData[i].m_nStep;

		int nStart2 = vcLoopData[i].m_nStart2;
		int nEnd2 = vcLoopData[i].m_nEnd2;
		int nStep2 = vcLoopData[i].m_nStep2;

		//需要退出循环
		bool bBreak = false;
		for(int n = nStart, n2 = nStart2; n != nEnd &&  n2 != nEnd2; n += nStep, n2 += nStep2)
		{  
			itor1 = center1.begin() + n;
			itor2 = center2.begin() + n2;
			vector<Point> centerTmp;
			find_all_point(*itor1, *itor2, centerTmp); 
			bool bShow = false;
			if(bShow)
			{
                Mat tmp ;
				cv::cvtColor(src,tmp,CV_GRAY2BGR);
                for(int m = 0; m < centerTmp.size(); m++)
				{
                    Point pt = centerTmp[m];
					cv::circle(tmp,pt,5,Scalar(255,0,0));
				}
				cv::namedWindow("cc",0);
				cv::imshow("cc",tmp);
				cv::waitKey();
			}
			//统计和背景不同的点数
			int nCount = 0;
			int nTotal = 0;
			for(int m = 0; m < centerTmp.size(); m++)
			{
				if(centerTmp[m].y < 0 || centerTmp[m].y >= src.rows ||
					centerTmp[m].x < 0 || centerTmp[m].x >= src.cols)
				{
					continue;
				}
				int nPos = centerTmp[m].y * src.step + centerTmp[m].x;
				if(nPos < 0 || nPos >= src.step* src.rows)
				{
					continue;
				}
				nTotal ++;
				if(src.data[nPos] > nTh)
				{
					////目标和背景颜色差异大，此版本为目标为白
					/*pt0 = *itor1;
					pt1 = *itor2;
					break;*/
					nCount++;
				}
			}
			
			//计算和背景不同的点数比例
			float fRadio = (float)nCount/nTotal;
			float fRadioTh = 0.05;
			if(nType == 1)
			{
				//目标和背景颜色差异小，此版本为目标为黑色（背景也为黑色）
				fRadioTh = 0.005;
			}
			if(fRadio >= fRadioTh)
			{
				pt0 = *itor1;
				pt1 = *itor2;
				bBreak = true;
			}
			
			if(true == bBreak)
			{
				break;
			}//end if(pt0.x != -1)
		} //end for(int n = nStart; n != nEnd; n+=nStep)
		vcAdjust.push_back(pt0);
		vcAdjust.push_back(pt1);
	}//end for(int i =0; i < vcLoopData.size(); i++ )
	//需要按0，1，3，2顺序存储
	Point ptTmp = vcAdjust[2];
	vcAdjust[2] = vcAdjust[3];
	vcAdjust[3] = ptTmp;
	return vcAdjust;
}

//删除包含轮廓
//[in]vRRect:输入矩形
//返回筛选后矩形
vector<RotatedRect> CDetectRectBySegmation::DoSuperposition(vector<RotatedRect> vRRect)
{
	vector<vector<Point2f> > vvPtf;
	vector<vector<Point2f> > vvPtfTmp;
	vector<Point2f> vPtf;
	Point2f ptf4[4];
	
	//中心点到直线的距离
	int center2LineDis =0;
	for (vector<RotatedRect>::iterator itr =vRRect.begin();itr!=vRRect.end();itr++)
	{
		(*itr).points(ptf4);
		for(int idx=0;idx<4;idx++)
			vPtf.push_back(ptf4[idx]);
		vvPtf.push_back(vPtf);
		vPtf.clear();

		RotatedRect rectTmp = *itr;
		rectTmp.size.width += min((int)(rectTmp.size.width*0.1),15);
		rectTmp.size.height += min((int)(rectTmp.size.height*0.1),15);
		
		rectTmp.points(ptf4);
		for(int idx=0;idx<4;idx++)
			vPtf.push_back(ptf4[idx]);
		vvPtfTmp.push_back(vPtf);
		vPtf.clear();
	}
	vPtf.clear();
	int idx =0;
	vector<int> vIdx;
	vector<Point> vP;
	for (vector<vector<Point2f> >::iterator itr = vvPtf.begin();itr != vvPtf.end(); itr++,idx++)
	{
		vPtf = *itr;
		int idxIn =0;
		int numInRect =0;
		bool isInRect = false;
		for(vector<vector<Point2f> >::iterator itrIn =vvPtfTmp.begin();itrIn!=vvPtfTmp.end();itrIn++,idxIn++)
		{
			if(idx == idxIn)
				continue;

			for(int idx1=0;idx1<4;idx1++)
			{
				 ptf4[idx1].x =(*itrIn)[idx1].x;
				 ptf4[idx1].y = (*itrIn)[idx1].y;
			}

			numInRect =0;
			for(int idArr=0;idArr <4;idArr++)
				if(CDetectRectBySegmation::isPointInRect(ptf4,Point((int)vPtf[idArr].x,(int)vPtf[idArr].y),vRRect[idxIn].angle))	
					numInRect++;
			//删除项
			if(numInRect >=3)
				isInRect =true;

			if(numInRect ==2 && vP.size() ==0)
				vP.push_back(Point(idx,idxIn));
			else if(numInRect ==2)
			{
				bool isExist =false;

				for (vector<Point>::iterator itr = vP.begin();itr != vP.end();itr++)
				{
					if((*itr).y == idx && (*itr).x ==idxIn)
						isExist =true;
				}	
				
				if(!isExist)
					vP.push_back(Point(idx,idxIn));
			}
		}//找相交点个数
		if(!isInRect)
			vIdx.push_back(idx);
	}
	//删除重叠项
	for (vector<int>::iterator itr = vIdx.begin();itr !=vIdx.end();)
	{
		idx = *itr;
		bool isExist =false;
		for (vector<Point>::iterator itrIn = vP.begin();itrIn != vP.end();itrIn++)
			if(itrIn->x == idx || itrIn->y ==idx)
			{
				isExist =true;
				break;
			}

		if(isExist)
			itr = vIdx.erase(itr);
		else
			itr++;
	}

	//合并平行相交项
	bool isWidth = true;
	
	vector<RotatedRect> vRRectTmp;
	for (vector<Point>::iterator itr = vP.begin(); itr != vP.end();itr++)
	{
		Point pt =*itr;
		if(vRRect[pt.x].size.height > 0 &&vRRect[pt.y].size.height >0)
		{
			vRRectTmp.push_back(vRRect[pt.x]);
			vRRectTmp.push_back(vRRect[pt.y]);
		}

		vRRect[pt.x].size.height =0;
		vRRect[pt.y].size.height =0;
	}
	vector<RotatedRect> vMergeTmp;
	RotatedRect rrectTmp;
	for (vector<RotatedRect>::iterator itr = vRRectTmp.begin(); itr!= vRRectTmp.end() ;itr+=2)
	{
		RotatedRect rrect1 = *itr;
		RotatedRect rrect2 = *(itr+1);
		CDetectRectBySegmation::adjustRectangle(rrect1);
		CDetectRectBySegmation::adjustRectangle(rrect2);
		rrectTmp = rrect2;

		RotatedRect rrect2Tmp =rrect2;
		rrect2Tmp.size.width *=1.1;
		rrect2Tmp.size.height *=1.1;
		Rect rectTmp = rrect2Tmp.boundingRect();
		Point2f ptf4Tmp[4];
		rrect2Tmp.points(ptf4Tmp);

		if(abs(abs((int)rrect1.angle) -abs((int)rrect2.angle))<10 )
		{
			rrect1.points(ptf4);

			for (int idxArr =1;idxArr<=4;idxArr++)
			{
					if(idxArr != 4&&CDetectRectBySegmation::isPointInRect(ptf4Tmp,ptf4[idxArr],rrect2Tmp.angle) &&CDetectRectBySegmation::isPointInRect(ptf4Tmp,ptf4[idxArr-1],rrect2Tmp.angle))
					{
						idx = idxArr;
						break;
					}
					else if(idxArr ==4 && CDetectRectBySegmation::isPointInRect(ptf4Tmp,ptf4[0],rrect2Tmp.angle) &&CDetectRectBySegmation::isPointInRect(ptf4Tmp,ptf4[3],rrect2Tmp.angle))
					{
						idx=4;
						break;
					}	
			}
			int idx1,idx2,idx3,idx4;
			
			switch(idx)
			{
			case 1:
				{
					idx1 =2;
					idx2 =3;

					idx3 =1;
					idx4 =2;
				}
				break;

			case 2:
				{
					idx1 =0;
					idx2 =3;

					idx3 =0;
					idx4 =1;
				}
				break;

			case 3:
				{
					idx1 =0;
					idx2 =1;

					idx3 =1;
					idx4 =2;
				}
				break;

			case 4:
				{
					idx1 =1;
					idx2 =2;

					idx3 =2;
					idx4 =3;
				}
				break;

			default:
				{
					idx1 = idx2 =idx3 =idx4 =0;
				}
				break;
			}

			float difx,dify;
			if(!(0==idx1 && 0== idx2))
			{
				center2LineDis = CDetectRectBySegmation::calculatePoint2Line(rrect2.center,ptf4[idx1],ptf4[idx2]);
				int nWidth =sqrt((ptf4[idx1].x-ptf4[idx2].x)*(ptf4[idx1].x-ptf4[idx2].x)+
								(ptf4[idx1].y-ptf4[idx2].y)*(ptf4[idx1].y-ptf4[idx2].y));

				if(abs(nWidth - rrect1.size.width) < abs(nWidth - rrect1.size.height))
					isWidth =true;
				else
					isWidth =false;
			}

			//单位长度的坐标变换
			if(isWidth)
			{
				difx = float(abs(ptf4[idx3].x-ptf4[idx4].x))/(float)rrect1.size.height *((float)center2LineDis -(float)rrect2.size.height/2.0)/2.0;
				dify = float(abs(ptf4[idx3].y-ptf4[idx4].y))/(float)rrect1.size.height *((float)center2LineDis -(float)rrect2.size.height/2.0)/2.0;
			}
			else
			{
				difx = float(abs(ptf4[idx3].x-ptf4[idx4].x))/(float)rrect1.size.width*((float)center2LineDis -(float)rrect2.size.width/2.0)/2.0;
				dify = float(abs(ptf4[idx3].y-ptf4[idx4].y))/(float)rrect1.size.width*((float)center2LineDis -(float)rrect2.size.width/2.0)/2.0;
			}
			//推算中心点移动距离
			if(center2LineDis > CDetectRectBySegmation::calculatePoint2Line(Point2f(rrectTmp.center.x-difx,rrectTmp.center.y-dify),ptf4[idx1],ptf4[idx2]))
			{
				rrectTmp.center.x -=difx;
				rrectTmp.center.y -=dify;
				rrectTmp.size.width += 2*difx;
				rrectTmp.size.height += 2*dify;
			}
			else
			{
				rrectTmp.center.x +=difx;
				rrectTmp.center.y +=dify;
				rrectTmp.size.width += 2*difx;
				rrectTmp.size.height += 2*dify;
			}
		}
		else
			continue;
		vMergeTmp.push_back(rrectTmp);
		if(center2LineDis != 0)
		{
			int len1 = center2LineDis;
		}
		
	}//筛选合并矩形

	vector<RotatedRect> vReRRect;
	RotatedRect rrect;
	for (vector<int>::iterator itr= vIdx.begin(); itr!= vIdx.end();itr++,idx++)
	{
		if(vRRect[*itr].size.height>0)
			vReRRect.push_back(vRRect[*itr]);
	}

	for (vector<RotatedRect>::iterator itr = vMergeTmp.begin();itr != vMergeTmp.end();itr++)
		vReRRect.push_back(*itr);

	return vReRRect;
}

//计算背景色
//src:输入图像
//返回背景颜色值
int CDetectRectBySegmation::calculateBackGroundValue(const Mat &src)
{
	if(3 != src.channels())
		return 40;
	int maxBackGroundValue =0;

	Mat topRegion = src.rowRange(5,10).clone();
	Mat botRegion = src.rowRange(src.rows-11,src.rows-6).clone();
	Mat leftRegion = src.colRange(5,10).clone();
	Mat rightRegion = src.colRange(src.cols-11,src.cols-6).clone();
	//图像指针
	uchar *dataTop = topRegion.ptr<uchar>(0);
	uchar *dataBot = botRegion.ptr<uchar>(0);
	uchar *dataLeft =leftRegion.ptr<uchar>(0);
	uchar *dataRight = rightRegion.ptr<uchar>(0);
	int maxValue[10] ={0};
	//top
	for (int idr=0;idr<topRegion.rows;idr++)
	{
		for(int idc=0;idc<topRegion.cols;idc++,dataTop+=3)
		{
			if(
				abs(*dataTop-*(dataTop+1)) <10 &&
				abs(*dataTop -*(dataTop+2)) <10 &&
				abs(*(dataTop+1)-*(dataTop+2))<10
				)
				maxValue[int(*dataTop/25.6)] +=1;
		}
	}
	//Bot
	for (int idr=0;idr<botRegion.rows;idr++)
	{
		for (int idc=0;idc<botRegion.cols;idc++,dataBot+=3)
		{
			if(
				abs(*dataBot -*(dataBot+1)) <10 &&
				abs(*dataBot -*(dataBot+2)) <10 &&
				abs(*(dataBot+1) -*(dataBot+2))<10
				)
				maxValue[int(*dataBot/25.6)] +=1;
		}
	}
	//Left
	for (int idr=0;idr<leftRegion.rows;idr++)
	{
		for (int idc=0;idc<leftRegion.cols;idc++,dataLeft +=3)
		{
			if(
				abs(*dataLeft -*(dataLeft+1)) <10 &&
				abs(*dataLeft -*(dataLeft+2)) <10 &&
				abs(*(dataLeft+1) - *(dataLeft+2)) <10
				)
				maxValue[int(*dataLeft/25.6)] +=1;
		}
	}
	//right
	for (int idr=0;idr<rightRegion.rows;idr++)
	{
		for (int idc=0;idc<rightRegion.cols;idc++,dataRight+=3)
		{
			if(
				abs(*dataRight -*(dataRight+1)) <10 &&
				abs(*dataRight -*(dataRight+2)) <10 &&
				abs(*(dataRight+1) -*(dataRight+2)) <10
				)
				maxValue[int(*dataRight/25.6)] +=1;
		}
	}

	int maxIdx=0;
	for (int idx=0;idx<9;idx++)
	{
		if(maxValue[idx] > maxValue[maxBackGroundValue])
			maxBackGroundValue = idx;
	}
	maxBackGroundValue = (maxBackGroundValue)*25.6 +15;

	return maxBackGroundValue;
}

//计算点到直线的距离
//pt1:点
//pt2，pt3:直线的端点
int CDetectRectBySegmation::calculatePoint2Line(Point2f p1,Point2f lp1,Point2f lp2)
{
	double a, b, c;
	// 化简两点式为一般式
	// 两点式公式为(y - y1)/(x - x1) = (y2 - y1)/ (x2 - x1)
	// 化简为一般式为(y2 - y1)x + (x1 - x2)y + (x2y1 - x1y2) = 0
	// A = y2 - y1
	// B = x1 - x2
	// C = x2y1 - x1y2
	a = lp2.y - lp1.y;
	b = lp1.x - lp2.x;
	if(a ==0 && 0==b)
		return 0;
	c = lp2.x * lp1.y - lp1.x * lp2.y;
	// 距离公式为d = |A*x0 + B*y0 + C|/√(A^2 + B^2)
	int dis = (int)abs(a * p1.x + b * p1.y + c) / sqrt(a * a + b * b);
	return dis;
}

//调整矩形
//[in/out]输入/输出矩形:rrect
void CDetectRectBySegmation::adjustRectangle(RotatedRect &rrect)
{
	float fWidth=0;
	
	if(rrect.angle <-45)
	{
		rrect.angle +=90;
		fWidth = rrect.size.width;
		rrect.size.width = rrect.size.height;
		rrect.size.height =fWidth;
	}

	if(rrect.angle >45)
	{
		rrect.angle -=90;
		fWidth = rrect.size.width;
		rrect.size.width = rrect.size.height;
		rrect.size.height =fWidth;
	}
	
	return ;
}

//计算坐标点是否在矩形内
//ptf4:矩形四个坐标点
//ptf:坐标点
bool CDetectRectBySegmation::isPointInRect(Point2f *ptf4,Point2f ptf,float fAngle)
{
	Point2f ptf4Vector[4];
	int nQuadrant[4]={0};
	fAngle *=CV_PI/180.0*(-1); 

	for (int idx=0;idx<4;idx++)
	{
		float fDifx = float(ptf.x - ptf4[idx].x);
		float fDify = float(ptf.y - ptf4[idx].y);
		int nDifx =fDifx*cos(fAngle)-fDify*sin(fAngle);
		int nDify =fDifx*sin(fAngle)+fDify*cos(fAngle);

		//第一象限
		if(nDifx >=0 &&nDify >=0)
			nQuadrant[0]++;
		//第二象限
		if(nDifx <0 &&nDify >=0)
			nQuadrant[1]++;
		//第三象限
		if(nDifx <0 &&nDify <0)
			nQuadrant[2]++;
		//第四象限
		if(nDifx >0 &&nDify <0)
			nQuadrant[3]++;
	}
	//判断四个向量是否在相邻的两个象限
	int firstIdx =-1;
	int secIdx =-1;
	int countNum =0;
	for (int idx=0;idx<4;idx++)
	{
		if(nQuadrant[idx] !=0)
		{
			if(firstIdx ==-1)
				firstIdx =idx;
			else if(secIdx ==-1 &&firstIdx != -1)
				secIdx =idx;
			
			countNum++;
		}
	}

	if(countNum<=2)
		if(abs(firstIdx -secIdx) ==1 || abs(firstIdx -secIdx) ==3 ||(countNum==1&&(firstIdx ==-1||secIdx==-1)))
			return false;
	return true;
}
