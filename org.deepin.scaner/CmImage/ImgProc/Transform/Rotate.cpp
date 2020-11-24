#include "stdafx.h"
#include "Rotate.h"
#include <math.h>
//V1.0.1

//四边形校正
//在知道4个点击，将任意四边形校正为矩形
//参数:
//src:源图像
//pt:顶点（顺序必须是左上，右上，左下，右下）
//返回值:
//校正后图像
Mat Rotate::keystoneCorrection(Mat src,Point pt[4])
{
	//将标记顶点放入轮廓中
	vector<Point> contour;
	contour.push_back(Point(pt[0].x,pt[0].y));
	contour.push_back(Point(pt[1].x,pt[1].y));
	contour.push_back(Point(pt[2].x,pt[2].y));
	contour.push_back(Point(pt[3].x,pt[3].y));


	//通过将标记顶点放入轮廓中获取最小外接矩形
	RotatedRect rRect;
	rRect = minAreaRect(contour);


	//校正前的顶点位置
	Point2f vertices[4];
	//校正后的顶点位置
	Point2f verdst[4];


	//校正前的顶点位置（顺序必须是左上，右上，左下，右下）
	for(int n =0; n < 4; n++)
	{
		vertices[n].x = contour[n].x;
		vertices[n].y = contour[n].y;
	}


	//校正后的图像尺寸按照最小外接矩形
	int dstw,dsth;
	dstw = rRect.size.width;
	dsth = rRect.size.height;

	//通过计数两点间距离保证图像宽度和高度不会混淆

	//注：外接矩形同左上右上两个点之间的距离做判断，如果距离相近则认为外接矩形的宽是正确的，否则进行倒置
	int nDistance = sqrt(powl((pt[0].x - pt[1].x),2)+powl((pt[0].y - pt[1].y),2));
	int nDistance1 = sqrt(powl((pt[2].x - pt[3].x),2)+powl((pt[2].y - pt[3].y),2));

	if( abs(nDistance - dstw) > dstw*0.1 && abs(nDistance1 -dstw) >dstw *0.1 )
	{
		dstw = rRect.size.height;
		dsth = rRect.size.width;
	}

	//校正后的顶点位置（顺序必须是左上，右上，左下，右下）
	verdst[0] = Point2f(0,0);
	verdst[1] = Point2f(dstw-1,0);
	verdst[2] = Point2f(0,dsth-1);
	verdst[3] = Point2f(dstw-1,dsth-1); 

	//透视变换
	Mat dst = Mat(dsth,dstw,src.type());
	Mat warpMatrix = getPerspectiveTransform(vertices, verdst);
	warpPerspective(src, dst, warpMatrix, dst.size(), INTER_CUBIC, BORDER_CONSTANT);

	return dst;
}

//对矩形四个点进行排序
//[in]pt	:输入四个点
//返回值：排序后的四个点
bool sortByYLocation(Point pt1,Point pt2)
{
	if(pt1.y < pt2.y)
		return true;
	else if(pt1.y == pt2.y && pt1.x < pt2.x)
		return true;
	else 
		return false;
}
bool sortByXLocation(Point pt1,Point pt2)
{
	if(pt1.x<=pt2.x)
		return true;
	else
		return false;
}

Point* Rotate::sortFourPoint(Point pt[4])
{
	Point *re4Pt = new Point[4];
	//sort 4 points
	vector<Point> vpTmp;
	for(int idx=0;idx<4;idx++)
		vpTmp.push_back(pt[idx]);

	sort(vpTmp.begin(),vpTmp.end(),sortByYLocation);
	sort(vpTmp.begin(),vpTmp.begin()+2,sortByXLocation);
	sort(vpTmp.begin()+2,vpTmp.begin()+4,sortByXLocation);

	int idx=0;
	for(vector<Point>::iterator itr = vpTmp.begin();itr!=vpTmp.end();itr++,idx++)
		re4Pt[idx] = *itr;

	return re4Pt;
}


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

//顺时针旋转，旋转后图像尺寸保持不变，因此可能会丢失部分图像
//参数: 
//[in]src		:		源图像
//[in]fAngle	:		顺时针角度(单位°)
//返回值:
//旋转后的图像
Mat Rotate::RotateImage(Mat src, float fAngle)
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
	IplImage *iplDst = Rotate::RotateImage(iplSrc, fAngle);
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

//顺时针旋转
//参数： 
//src：		源图像
//fAngle：	顺时针角度
IplImage* Rotate::RotateImage(IplImage* src, float fAngle)
{
	
	Point2f p2f;
	p2f.x =(float)src->width/2;
	p2f.y =(float)src->height/2;
#if 1
	float map[6];
	CvMat map_matrix = cvMat(2, 3, CV_32F, map); 
	cv2DRotationMatrix(p2f,fAngle,1,&map_matrix);
 
	IplImage *dst_rotate =cvCreateImage(cvGetSize(src),src->depth,src->nChannels);
	cvWarpAffine(src,dst_rotate,&map_matrix,CV_INTER_LINEAR|CV_WARP_FILL_OUTLIERS,cvScalarAll(0));
#else
	Mat dst = cvarrToMat(src,true);
	double map[6];
	Mat map_matrix =Mat(2,3,CV_32F,map);
	map_matrix = getRotationMatrix2D(p2f,fAngle,1.0);

	warpAffine(dst,dst,map_matrix,Size(dst.cols,dst.rows),INTER_CUBIC,BORDER_REFLECT,Scalar(255,255,255));
	IplImage *tmpDst = &IplImage(dst);

	IplImage *dst_rotate =cvCreateImage(cvGetSize(tmpDst),tmpDst->depth,tmpDst->nChannels);
	cvCopy(tmpDst,dst_rotate);
#endif
	return dst_rotate;
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
//旋转图像的角度
//nClockOrention 控制图像的旋转方向:
//…………………………0：顺时针旋转
//…………………………1：逆时针旋转
//rtAngle 为旋转角度（单位°）
//Bicycle 2016年6月28日13:37:17
//支持颜色补边  默认R=0,B=0,G=0
IplImage* Rotate::RotateImage(IplImage* src,int rtAngle,int nClockOrention,int R,int G,int B)
{
	int angle = rtAngle%180;
	IplImage* dst = NULL;
		int width =
			(double)(src->height * sin(angle * CV_PI / 180.0)) +
			(double)(src->width * cos(angle * CV_PI / 180.0 )) + 1;
		int height =
			(double)(src->height * cos(angle * CV_PI / 180.0)) +
			(double)(src->width * sin(angle * CV_PI / 180.0 )) + 1;
		int tempLength = sqrt((double)src->width * src->width + src->height * src->height) + 10;
		int tempX = (tempLength + 1) / 2 - src->width / 2;
		int tempY = (tempLength + 1) / 2 - src->height / 2;
		int flag = -1;

	dst = cvCreateImage(cvSize(width, height), src->depth, src->nChannels);
	
#if 0
	uchar *data=(uchar*)dst->imageData;

	for(int i = 0;i!= dst->height ; i++)
	{
		for(int j=0;j!= dst->width ; j++)
		{
			data[i*dst->widthStep+j*dst->nChannels+2]=R;
			data[i*dst->widthStep+j*dst->nChannels+1]=G;
			data[i*dst->widthStep+j*dst->nChannels+0]=B;
			
		}
	}
#else
	cvZero(dst);
#endif

	IplImage* temp = cvCreateImage(cvSize(tempLength, tempLength), src->depth, src->nChannels);
	
#if 1
	uchar *data1=(uchar*)temp->imageData;

	for(int i1 = 0;i1!= temp->height ; i1++)
	{
		for(int j1=0;j1!= temp->width ; j1++)
		{
			data1[i1*temp->widthStep+j1*temp->nChannels+2]=R;
			data1[i1*temp->widthStep+j1*temp->nChannels+1]=G;
			data1[i1*temp->widthStep+j1*temp->nChannels+0]=B;
		}
	}
#else
	cvZero(temp);
#endif
	cvSetImageROI(temp, cvRect(tempX, tempY, src->width, src->height));
	cvCopy(src, temp, NULL);
	cvResetImageROI(temp);

	flag = 1;
		switch(nClockOrention)
		{
			case 0:
			default:
				flag = 1;
				break;
			case 1:
				flag = -1;
				break;
		}

	float m[6];
	int w = temp->width;
	int h = temp->height;
	m[0] = (float) cos(flag * angle * CV_PI / 180.);
	m[1] = (float) sin(flag * angle * CV_PI / 180.);
	m[3] = -m[1];
	m[4] = m[0];
	// 将旋转中心移至图像中间
	m[2] = w * 0.5f;
	m[5] = h * 0.5f;
	//
	CvMat M = cvMat(2, 3, CV_32F, m);
	cvGetQuadrangleSubPix(temp, dst, &M);
	cvReleaseImage(&temp);
	return dst;
	cvReleaseImage(&dst);
}
IplImage* Rotate::RotateImage(IplImage* src,int rtAngle,int nClockOrention,CvScalar color)
{
	IplImage *dst=cvCloneImage(src);
	IplImage *dst1=cvCloneImage(src);

	int R=color.val[2];
	int G=color.val[1];
	int B=color.val[0];
	//调取代码
	dst1=Rotate::RotateImage(dst,rtAngle,nClockOrention,R,G,B);

	cvReleaseImage(&dst);
	return dst1;
	cvReleaseImage(&dst1);
}
