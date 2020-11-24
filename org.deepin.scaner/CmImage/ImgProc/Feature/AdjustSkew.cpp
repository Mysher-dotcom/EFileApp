#include "stdafx.h"        // Standard windows header file
#include "AdjustSkew.h"
#include "../Transform/Rotate.h"
#include <cmath>

CAdjustSkew::CAdjustSkew(void)
{
}


CAdjustSkew::~CAdjustSkew(void)
{
}

//获取倾斜角度
//Max 2016-01-21 15:11:28
float CAdjustSkew::getSkew(IplImage * src, 
	float fSkewAngleMax,
	int threshold,
	double param1,
	double param2)
{
	float fSkew = 0;

	IplImage * ColImag = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, src->nChannels);
	cvCopy(src, ColImag);

	if (ColImag->width >1000 && ColImag->height> 1000)
	{
		//金子塔缩小图像，可以提高速度
		//但是缩小有可能导致距离较近的连通域被合并在一起，影响判断
		IplImage *imgT = cvCreateImage(cvSize(ColImag->width*0.5, ColImag->height*0.5),
			ColImag->depth,
			ColImag->nChannels);
		//库函数调用cvPyrDown   
		cvPyrDown(ColImag, imgT, CV_GAUSSIAN_5x5);  //高斯变换   
		cvReleaseImage(&ColImag);

		ColImag = cvCreateImage(cvGetSize(imgT),
			imgT->depth,
			imgT->nChannels);
		cvCopy(imgT, ColImag);

		cvReleaseImage(&imgT);
	}
	IplImage* Graymage = cvCreateImage(cvGetSize(ColImag), IPL_DEPTH_8U, 1);
	if (ColImag->nChannels == 3)
	{
		cvCvtColor(ColImag, Graymage, CV_BGR2GRAY);//; 
	}
	else
	{
		cvCopy(ColImag, Graymage);
	}

	//去噪
	//cvSmooth(Graymage,Graymage,CV_GAUSSIAN,3,3,0,0);  
	//自适应二值化，去除光线不均匀
	/*IplImage *bw = cvCreateImage(cvGetSize(Graymage), Graymage->depth, 1);
	cvAdaptiveThreshold(Graymage, bw, 255);*/
	//对黑色区域进行膨胀，边缘检测
	//cvErode(bw, bw, NULL, 1);
	//CAdaptiveThreshold::AdaptiveThreshold(Graymage, bw, 0, 512);

	/*cvShowImage("t", bw);
	cv::waitKey(0);
	cvSaveImage("D:\\test.jpg",bw);*/

	IplImage *p16SX = cvCreateImage(cvSize(Graymage->width,Graymage->height) ,IPL_DEPTH_16S,1);  
	IplImage *p16SY = cvCreateImage(cvSize(Graymage->width,Graymage->height)  ,IPL_DEPTH_16S,1);  
	p16SX->origin = Graymage->origin;  
	p16SY->origin = Graymage->origin;  
	//第二步：调用cvSobel进行边缘计算  
	cvSobel(Graymage,p16SX,1,0,3);//x方向的边缘  
	cvSobel(Graymage,p16SY,0,1,3);//y方向的边缘  
	//第三步：计算sqrt(x^2+y^2)，得到幅值图像  
	cvMul(p16SX,p16SX,p16SX,1.0);  
	cvMul(p16SY,p16SY,p16SY,1.0);  
	cvAdd(p16SX,p16SY,p16SX,0);  
	//最后一步：将16S图像转化成8bit图像  
	cvConvert(p16SX,Graymage);  
	/*IplImage* CannyIm = cvCloneImage(bw);
	cvCanny(bw, CannyIm, 150, 200, 3);

	cvReleaseImage(&bw);*/

	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* lines = 0;
	//阈值参数。当在一条直线上的像素点数大于threshold时，才将该直线作为检测结果显示出来。该值越大，得到直线越少。
	//int threshold = 100;
	//对概率Hough变换，它是最小线段长度.即当线段长度大于param1时，才将该线段作为检测结果显示。与上一参数类似，不过上一参数为像素数，而该参数为线段长度。
	//double param1 = 700;
	//这个参数表示在同一条直线上进行碎线段连接的最大间隔值(gap), 即当同一条直线上的两条碎线段之间的间隔小于param2时，将其合二为一条长直线。
	//double param2 = 50;

	lines = cvHoughLines2(Graymage, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI / 180, threshold, param1, param2);
	vector<LINESTRUCT> HorizontalLine; HorizontalLine.clear();
	vector<LINESTRUCT> VerticalLine; VerticalLine.clear();

	LineClassify(lines, HorizontalLine, VerticalLine);
	//LinePlot(src,HorizontalLine);
	//LinePlot(src,VerticalLine);
	float angle = GetLineAngle(HorizontalLine);

	fSkew = angle;
	
	cvReleaseImage(&Graymage);
	//cvReleaseImage(&CannyIm);
	cvReleaseImage(&ColImag);
	cvReleaseMemStorage(&storage);
	storage = NULL;
	return fSkew;
}

//基于直线检测的纠偏
//算法来自老王
//Max 2014-11-03 11:47:49
float CAdjustSkew::AdjustSkew(IplImage *src, float fSkewAngleMax)
{
	//金子塔缩小图像，可以提高速度
	//但是缩小有可能导致距离较近的连通域被合并在一起，影响判断
	int nTargetW = 640;
		
	float fRadio =(float) nTargetW / src -> width;

	int nActualW = nTargetW;
	int nActualH = fRadio * src->height;
	IplImage * ColImag = cvCreateImage( cvSize(nActualW, nActualH ), 
		src -> depth, 
		src -> nChannels );   
	//库函数调用cvPyrDown   
	//cvPyrDown( ColImag, imgT, CV_GAUSSIAN_5x5 );  //高斯变换   
	cvResize(src,ColImag);
	
	IplImage* Graymage = cvCreateImage(cvGetSize(ColImag), IPL_DEPTH_8U, 1);	
	if(ColImag->nChannels == 3)
	{
		cvCvtColor(ColImag,Graymage,CV_BGR2GRAY);//; 
	}
	else
	{
		cvCopy(ColImag,Graymage);
	}
	cvReleaseImage(&ColImag);
	//去噪
	cvSmooth(Graymage,Graymage,CV_GAUSSIAN,3,3,0,0);  
	//自适应二值化，去除光线不均匀
	IplImage *bw = cvCreateImage(cvGetSize(Graymage),Graymage->depth,1);
	cvAdaptiveThreshold(Graymage,bw,255);
	cvReleaseImage(&Graymage);
	//对黑色区域进行膨胀，边缘检测
	//cvErode(bw,bw, NULL,1); 

	IplImage* CannyIm=cvCloneImage(bw);
	cvCanny( bw, CannyIm, 150, 200, 3);		
	cvReleaseImage(&bw);
	
	//显示二值图
	//cvNamedWindow("ShowBw",0);
	//cvShowImage("showBW",CannyIm);
	//cvWaitKey();

	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* lines = 0;
	//阈值参数。当在一条直线上的像素点数大于threshold时，才将该直线作为检测结果显示出来。该值越大，得到直线越少。
	int threshold = 100;
	//对概率Hough变换，它是最小线段长度.即当线段长度大于param1时，才将该线段作为检测结果显示。与上一参数类似，不过上一参数为像素数，而该参数为线段长度。
	double param1 = 700;
	//这个参数表示在同一条直线上进行碎线段连接的最大间隔值(gap), 即当同一条直线上的两条碎线段之间的间隔小于param2时，将其合二为一条长直线。
	double param2 = 10;

	float fTh= 0.01;
	float fLen= 0.05;//0.75

	threshold = 100;
	param1 = 200;
	param2 =20;
	lines = cvHoughLines2(CannyIm, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180,threshold, param1, param2);
	cvReleaseImage(&CannyIm);
	
#if 0
	//绘制直线
    for (int i = 0; i < lines->total; i++)
    {
        CvPoint* line = (CvPoint*)cvGetSeqElem(lines, i);
        cvLine(src, line[0], line[1], CV_RGB(255, 0, 0), 3, 8);
    }
	//显示曲线图
	cvNamedWindow("ShowLineInImage",0);
	cvShowImage("ShowLineInImage",src);
	cvWaitKey();
#endif

	vector<LINESTRUCT> HorizontalLine;HorizontalLine.clear();
	vector<LINESTRUCT> VerticalLine;VerticalLine.clear();

	LineClassify(lines,HorizontalLine,VerticalLine);
	cvReleaseMemStorage( &storage );
	storage=NULL;
#if 0
	LinePlot(src,HorizontalLine);
	//LinePlot(src,VerticalLine);
#endif

	float angle = GetLineAngle(HorizontalLine);

	angle = angle *180 / CV_PI;

	return angle;
	//IplImage *dst = Rotate::RotateImage(src,angle);

	//单元测试
#if 0
	cvNamedWindow("canny",0);
	cvShowImage("canny",ColImag);

	cvWaitKey(0);
	cvDestroyWindow("canny");	
	cvSaveImage("D:\\result.jpg",ColImag);
#endif		

	//return dst;
}

void CAdjustSkew::LineClassify(CvSeq* lines,vector<LINESTRUCT>& HorizontalLine,vector<LINESTRUCT>& VerticalLine)
{
	for(int i = 0; i < lines->total; i++ )
	{
		CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);		
		int LineWidth = abs(line[1].x-line[0].x);
		int LineHeight= abs(line[1].y-line[0].y);	

		LINESTRUCT TemLine;
		if (LineWidth>LineHeight) //horizontal line
		{  
			float Ratio =(float)LineHeight/LineWidth;
			TemLine.StartPoint = line[0];
			TemLine.EndPoint = line[1];
			HorizontalLine.push_back(TemLine);
		}else //vertical line
		{   
			float Ratio=(float)LineWidth/LineHeight;
			TemLine.StartPoint = line[0];
			TemLine.EndPoint = line[1];
			VerticalLine.push_back(TemLine);
		}
	}
}

void CAdjustSkew::LinePlot(IplImage*& color_dst,vector<LINESTRUCT> LineVec)
{   
	int nLine = LineVec.size();
	CvPoint LineStartP,LineEndP;
	for(int i = 0; i < nLine; i++ )
	{
		LineStartP = LineVec[i].StartPoint;
		LineEndP   = LineVec[i].EndPoint;
		cvLine( color_dst, LineStartP, LineEndP, CV_RGB(255,0,0), 3, 8 );
	}
}

float CAdjustSkew::GetLineAngle(vector<LINESTRUCT> HorizontalLine)
{   
	float LineAnlge=0;
	vector<LINEAngle> LineAngleVec;
	int nLine = HorizontalLine.size();
	if (nLine<=0)
	{
		return 0;
	}
	float AngleTh=0.05;

	LINEAngle TemLineAngle;
    float rohn=atan2((double)(HorizontalLine[0].StartPoint.x-HorizontalLine[0].EndPoint.x),(double)(HorizontalLine[0].EndPoint.y-HorizontalLine[0].StartPoint.y));
	TemLineAngle.rohn.push_back(rohn);
	TemLineAngle.MeanRohn =rohn;
	TemLineAngle.SumRohn=rohn;
	LineAngleVec.push_back(TemLineAngle);
	for(int i=1;i<nLine;i++)
	{   
		int nAngle = LineAngleVec.size();
		rohn=atan2((double)((HorizontalLine[i].EndPoint.y-HorizontalLine[i].StartPoint.y)),(double)(HorizontalLine[i].EndPoint.x-HorizontalLine[i].StartPoint.x));
	

		float MinAngleDist=10;
		int BelongIndex=0;
		for (int j=0;j<nAngle;j++)
		{
			float AngleDist = (rohn-LineAngleVec[j].MeanRohn);
			if (AngleDist<MinAngleDist)
			{
				MinAngleDist=AngleDist;
				BelongIndex=j;
			}
		}
		////////////////////////
		if (MinAngleDist>AngleTh) 
		{
			LINEAngle TemLineAngle;
			TemLineAngle.MeanRohn=rohn;
			TemLineAngle.SumRohn=rohn;
			TemLineAngle.rohn.push_back(rohn);
			LineAngleVec.push_back(TemLineAngle);
		}else
		{
			LineAngleVec[BelongIndex].rohn.push_back(rohn);
			LineAngleVec[BelongIndex].SumRohn=LineAngleVec[BelongIndex].SumRohn+rohn;
			LineAngleVec[BelongIndex].MeanRohn =LineAngleVec[BelongIndex].SumRohn/LineAngleVec[BelongIndex].rohn.size();
		}
	}

	int MaxCum=0;
	int AngleIndex=0;
	for (int i=0;i<LineAngleVec.size();i++)
	{  
		if (LineAngleVec[i].rohn.size()>MaxCum)
		{
			MaxCum=LineAngleVec[i].rohn.size();
			AngleIndex = i;
		}
	}

	for (int i=0;i<LineAngleVec[AngleIndex].rohn.size();i++)
	{
		LineAnlge=LineAnlge+LineAngleVec[AngleIndex].rohn[i];
	}
	LineAnlge=LineAnlge/LineAngleVec[AngleIndex].rohn.size();
	return LineAnlge;
}

void CAdjustSkew::AdjustSkewImage(IplImage* &img,double angle)
{   
	int nWidth  = img->width;
	int nHeight = img->height;

	float m[6];
	CvMat M = cvMat(2,3,CV_32F,m);
	float Factor  = 1.0;
	m[0]=(float)(Factor*cos(angle));
	m[1]=(float)(Factor*sin(-angle));
	m[3]=-m[1];
	m[4]=m[0];
	//////////////////////旋转中心移至图像中心
	m[2]=nWidth*0.5f;
	m[5]=nHeight*0.5f;	     
	
	IplImage* pDest = cvCloneImage(img);
	cvZero(img);
	cvGetQuadrangleSubPix(pDest,img,&M);//
	cvReleaseImage(&pDest);	
}

//基于直线检测的纠偏
//参数:
//[in]src:	源图像
//返回值:
//Deskew后的图像
Mat CAdjustSkew::Deskew(Mat src)
{
	Mat dst;
	if(!src.data)
	{
		return dst;
	}

    //IplImage *iplSrc =  &(IplImage)src;
    IplImage iplSrcTmp =  (IplImage)src;
    IplImage *iplSrc =  &iplSrcTmp;
	float fAngle= CAdjustSkew::AdjustSkew(iplSrc);
	
	IplImage *iplDst = Rotate::RotateImage(iplSrc,fAngle);
	if(iplDst != NULL)
	{
		dst = cv::cvarrToMat(iplDst,true);  
		cvReleaseImage(&iplDst);
	}

	return dst;
}

