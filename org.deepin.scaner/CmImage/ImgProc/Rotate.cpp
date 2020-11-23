#include "stdafx.h"
#include "Rotate.h"
#include <math.h>
//V1.0.1
//根据4个顶点对图像进行透视变换得到矩形图像
//参数:
//[in]src	:	源图像
//[in]pt	:	4个顶点
//返回值:
//旋转裁切后的图像
Mat Rotate::RotateCut(Mat src, CvPoint pt[4])
{
	vector<Point> contour;
    contour.push_back(Point(pt[0].x,pt[0].y));
    contour.push_back(Point(pt[1].x,pt[1].y));
    contour.push_back(Point(pt[2].x,pt[2].y));
    contour.push_back(Point(pt[3].x,pt[3].y));
    RotatedRect rRect;
    rRect = minAreaRect(contour);

    int dstw,dsth;
    Point2f vertices[4];
    Point2f verdst[4];

	double fA1 = 0;
	double fA2 = 0;
	double fA3 = 0;
	double fA4 = 0;

	if(pt[1].x != pt[0].x)
	{
		fA1 = (double)(pt[1].y - pt[0].y)/(double)(pt[1].x - pt[0].x);
	}
	if(pt[2].x != pt[1].x)
	{
		fA2 = (double)(pt[2].y - pt[1].y)/(double)(pt[2].x - pt[1].x);
	}
	if(pt[3].x != pt[2].x)
	{
		fA3 = (double)(pt[3].y - pt[2].y)/(double)(pt[3].x - pt[2].x);
	}
	if(pt[0].x != pt[3].x)
	{
		fA4 = (double)(pt[0].y - pt[3].y)/(double)(pt[0].x - pt[3].x);
	}

	fA1 = atan(fA1)*180/CV_PI;
	fA2 = atan(fA2)*180/CV_PI;
	fA3 = atan(fA3)*180/CV_PI;
	fA4 = atan(fA4)*180/CV_PI;

	//如果为0
	if(fA1 == 0 || fA2 == 0)
	{
		Rect rect = rRect.boundingRect();
		rect.x =rect.x<0?0:rect.x;
		rect.y =rect.y<0?0:rect.y;
		if(rect.height > src.rows)
		{
			rect.height = src.rows;
		}
		if(rect.width > src.cols)
		{
			rect.width = src.cols;
		}
		if(rect.x + rect.width >= src.cols)
		{
			rect.width = src.cols - 1 - rect.x;
		}
		if(rect.y + rect.height >= src.rows)
		{
			rect.height = src.rows - 1 - rect.y;
		}
		Mat dst = src(Rect(rect)).clone();
		return dst;
	}

	int nL1 = sqrt(pow((float)(pt[1].y - pt[0].y),2) + pow((float)(pt[1].x - pt[0].x),2));
	int nL2 = sqrt(pow((float)(pt[2].y - pt[1].y),2) + pow((float)(pt[2].x - pt[1].x),2));

    if(rRect.size.width>rRect.size.height)
    {
        rRect.points(vertices);
        dstw = rRect.size.width;
        dsth = rRect.size.height;
        verdst[0] = Point2f(0,dsth);
        verdst[1] = Point2f(0,0);
        verdst[2] = Point2f(dstw,0);
        verdst[3] = Point2f(dstw,dsth); 
    }
    else 
    {
        rRect.points(vertices);
        dstw = rRect.size.height;
        dsth = rRect.size.width;
        verdst[0] = Point2f(dstw,dsth);
        verdst[1] = Point2f(0,dsth);
        verdst[2] = Point2f(0,0);
        verdst[3] = Point2f(dstw,0);
    }
    Mat dst = Mat(dsth,dstw,CV_8UC1);
    Mat warpMatrix = getPerspectiveTransform(vertices, verdst);
    warpPerspective(src, dst, warpMatrix, dst.size(), INTER_LINEAR, BORDER_CONSTANT);

	double dAbs1 = abs(fA1);
	double dAbs2 = abs(fA2);

	float fAdjust  = 0;
	if(dAbs1 > dAbs2 && nL1 > nL2)
	{
		if(fA1 > 0)
		{
			fAdjust = -90;
		}
		else
		{
			fAdjust = 90;
		}
	}
	else if(dAbs1 < dAbs2 && nL1 < nL2)
	{
		if(fA2 > 0)
		{
			fAdjust = -90;
		}
		else
		{
			fAdjust = 90;
		}
	}
	if(fAdjust != 0)
	{
		Mat dstTmp = dst.clone();
		dst = RotateImage2(dstTmp,fAdjust);
	}

	return dst;
}
//顺时针旋转,旋转后图像根据内容会改变尺寸
//参数： 
//src：		源图像
//fAngle：	顺时针角度（单位°）
Mat Rotate::RotateImage2(Mat src, float fAngle)
{
	Mat dst;
	if(!src.data)
	{
		return dst;
	}

	if(fAngle == 0)
	{
		dst = src.clone();
	}

    //IplImage *iplSrc =  &(IplImage)src;
    IplImage iplSrcTmp =  (IplImage)src;
    IplImage *iplSrc = &iplSrcTmp;
	IplImage *iplDst = Rotate::RotateImage2(iplSrc, fAngle);
	if(iplDst != NULL)
	{
		dst = cv::cvarrToMat(iplDst,true);  
		cvReleaseImage(&iplDst);
	}

	return dst;
}
//顺时针旋转,旋转后图像根据内容会改变尺寸
//参数： 
//src：		源图像
//fAngle：	顺时针角度（单位°）
IplImage* Rotate::RotateImage2(IplImage* src, float fAngle)
{
	//参考
	//http://blog.csdn.net/xiaowei_cqu/article/details/7616044
 
	//旋转角度
	float fAngle_radian =fAngle*CV_PI/180;
	//旋转后新尺寸
	int dst_w =int(src->height*fabs(sin(-fAngle_radian) )+src->width*fabs(cos(-fAngle_radian) ));
	int dst_h =int(src->width*fabs(sin(-fAngle_radian) )+src->height*fabs(cos(-fAngle_radian) ));
	//旋转中心
	Point2f p2f;
	p2f.x =(float)src->width/2;
	p2f.y =(float)src->height/2;
	//仿射变换Map
	float map[6];
	CvMat map_matrix = cvMat(2, 3, CV_32F, map); 
	cv2DRotationMatrix(p2f,fAngle,1,&map_matrix);
	//对图像进行平移
	//对上述的仿射变换矩阵map_matrix进行平移
	map[2] +=float(dst_w -src->width)/2;
	map[5] +=float(dst_h -src->height)/2;
	//判断旋转方向对图像进行偏移纠正
	if(fAngle <0)
		map[2]--;
	else
		if(fAngle >0)
			map[5]--;
	//旋转后的图像尺寸
	IplImage *dst_rotate =cvCreateImage(cvSize(dst_w,dst_h),src->depth,src->nChannels);
	//仿射变换
	cvWarpAffine(src,dst_rotate,&map_matrix,CV_INTER_LINEAR|CV_WARP_FILL_OUTLIERS,cvScalarAll(0));

	return dst_rotate;
}
