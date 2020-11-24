#include "stdafx.h"
#include "ImgAdjust.h"
//Version 0.1.4
#include <iostream> 
#include "HSL.hpp"
#include "../ColorSpace/AdaptiveThreshold.h"

ImgAdjust::ImgAdjust(void)
{
}

ImgAdjust::~ImgAdjust(void)
{
}

//[in] src :输入图像
//[in] color：0—彩色； 1—R；2—G;3—B;
//[in] hue：色彩0~360
//[in] saturation：饱和度 0~200
//[in] brightness：亮度 0~200
//返回值：
//[out]  dst：输出图像
//jessie 2017-07-07 16:12:05
Mat ImgAdjust::HslAdjust(Mat src,int color, int hue, int saturation, int brightness)  
{  
    Mat dst;  
    HSL hsl;
    hsl.channels[color].hue = hue - 180;  
    hsl.channels[color].saturation = saturation - 100;  
    hsl.channels[color].brightness = brightness - 100;  
  
    //hsl.adjust(src, dst);
	//return dst;
	dst = src.clone();
	hsl.adjustByOpencv(dst);

	return dst;
}  

//锐化
//参数：
//[in] src :输入图像
//[in] sigma：取>0,默认3.0;
//[in] nAmount：取值>0，默认200，越大效果越明显
//[in] thresholds：0~255,一般取1
//返回值：
//[out]  dst：输出图像
//jessie 2017-08-03 10:15:47
//Bicycle 2018-3-5 将一些对锐化结果影响微乎其微的步骤舍去
Mat ImgAdjust::Unsharp(const Mat &src,float sigma,int nAmount,int thresholds)
{
	//Mat lowContrastMask,imgBlurred,imgDst;
	Mat imgBlurred;
	float amount = nAmount/100.0f;

	if(sigma!=0)
	{
		GaussianBlur(src, imgBlurred, Size(), sigma, sigma);
	}
	else
	{
		GaussianBlur(src, imgBlurred, Size(1,1), sigma, sigma);
	}
#if 1 //jessie老版本
	//lowContrastMask = abs(src-imgBlurred)<thresholds;
	//imgDst = src*(1+amount)+imgBlurred*(-amount);
	//src.copyTo(imgDst, lowContrastMask);
	imgBlurred = src*(1+amount) +imgBlurred*(-amount);
	return imgBlurred;
#else
	imgDst = src.clone();
	uchar *dataSrc = imgDst.ptr<uchar>(0);
	uchar *dataGaussian = imgBlurred.ptr<uchar>(0);

	if(3 == src.channels())
	{
		for (int idr =0;idr<src.rows;idr++)
		{
			for (int idc=0;idc<src.cols;idc++,dataSrc+=3,dataGaussian+=3)
			{
				int difB = abs(*dataSrc - *dataGaussian);
				int difG = abs(*(dataSrc+1) -*(dataGaussian+1));
				int difR = abs(*(dataSrc+2) -*(dataGaussian+2));

				difB = difB <thresholds ? 0:255;
				difG = difG <thresholds ? 0:255;
				difR = difR <thresholds ? 0:255;

				*dataGaussian =saturate_cast<uchar>(*dataSrc*(1+amount) -*dataGaussian*amount);
				*(dataGaussian+1) = saturate_cast<uchar>(*(dataSrc+1)*(1+amount) -*(dataGaussian+1)*amount);
				*(dataGaussian+2) = saturate_cast<uchar>(*(dataSrc+2)*(1+amount) -*(dataGaussian+2)*amount);

				if(!(0 == difR && 0 ==difG && 0==difB))
				{
					*dataSrc = *dataGaussian;
					*(dataSrc+1) =*(dataGaussian+1);
					*(dataSrc+2) =*(dataGaussian+2);
				}
			}
		}
	}//彩色图像
	else
	{
		for (int idr =0;idr<src.rows;idr++)
		{
			for (int idc=0;idc<src.cols;idc++,dataSrc++,dataGaussian++)
			{
				int dif = abs(*dataSrc - *dataGaussian);
				dif = dif <thresholds ? 0:255;

				*dataGaussian =saturate_cast<uchar>(*dataSrc*(1+amount) -*dataGaussian*amount);

				if(0 != dif)
				{
					*dataSrc = *dataGaussian;
					*(dataSrc+1) =*(dataGaussian+1);
					*(dataSrc+2) =*(dataGaussian+2);
				}
			}
		}
	}//灰度图像
#endif
	//return imgDst;
}

//彩色平衡
//参数：
//[in] imageSource: 输入图像
//[in]  KR :红色分量比例 0~200
//[in]  KG 绿色分量 比例  0~200
//[in]  KB： 红色分量比例  0~200
//返回值：
//[out]: dst :输出图像
//2017-07-07 17:47:06
Mat ImgAdjust::ColorBlance(Mat imageSource,  int KR, int KG,int KB)  
{  
	//非彩色图返回原图
	if( 3 != imageSource.channels())
	{
		Mat	rgb_mat = imageSource.clone();
		return rgb_mat;
	}

	Mat result = imageSource.clone();
	uchar *data = result.ptr<uchar>(0);

	float fkb = (float)KB/100.0;
	float fkg = (float)KG/100.0;
	float fkr = (float)KR/100.0;

	for (int idr=0;idr<result.rows;idr++)
	{
		for (int idc=0;idc<result.cols;idc++,data+=3)
		{
			*data = saturate_cast<uchar>(*data*fkb);
			*(data+1) =saturate_cast<uchar>(*(data+1)*fkg);
			*(data+2) =saturate_cast<uchar>(*(data+2)*fkr);
 		}
	}

	return result;
} 

//曲线调整
#include "Curves.hpp"

using namespace std;
using namespace cv;

static string window_name = "Photo";
string curves_window = "Adjust Curves";
//Mat curves_mat;
int channel = 0;
Curves  curves;
Mat m_dst;
Mat m_src;
void invalidate()
{
	Mat curves_mat = Mat::ones(256, 256, CV_8UC3);
	double z[256];
	curves.draw(curves_mat,z);

	Mat dst;
	curves.adjust(m_src, dst);
	dst.copyTo(m_dst);
}

void Myinvalidate(Mat &dst,double z[256])
{
	Mat	curves_mat = Mat::ones(256, 256, CV_8UC3);
	curves.draw(curves_mat,z);
	curves.adjust(m_src, dst);
}
void MyAdjustChannel(int channel)
{
	switch (channel) {
	case 3:
		curves.CurrentChannel = &curves.BlueChannel;
		break;
	case 2:
		curves.CurrentChannel = &curves.GreenChannel;
		break;
	case 1:
		curves.CurrentChannel = &curves.RedChannel;
		break;
	default:
		curves.CurrentChannel = &curves.RGBChannel;
		break;
	}
	Mat dst;
	double z[256];
	Myinvalidate(dst,z);
}
void callbackAdjustChannel(int , void *)
{
	switch (channel) {
	case 3:
		curves.CurrentChannel = &curves.BlueChannel;
		break;
	case 2:
		curves.CurrentChannel = &curves.GreenChannel;
		break;
	case 1:
		curves.CurrentChannel = &curves.RedChannel;
		break;
	default:
		curves.CurrentChannel = &curves.RGBChannel;
		break;
	}
	invalidate();
}

void callbackMouseEvent(int mouseEvent, int x, int y, int flags, void* param)
{
	switch(mouseEvent) {
	case CV_EVENT_LBUTTONDOWN:
		curves.mouseDown(x, y);
		invalidate();
		break;
	case CV_EVENT_MOUSEMOVE:
		if ( curves.mouseMove(x, y) )
			invalidate();
		break;
	case CV_EVENT_LBUTTONUP:
		curves.mouseUp(x, y);
		invalidate();
		break;
	}
	return;
}

//曲线调整
//参数：
//[in] src: 输入图像
//[out]  z :返回拟合曲线
//[in] x :添加点的x坐标
//[in] y :添加点的y坐标
//[in] AdjustChannel: 通道参数 0—彩色； 1—R；2—G;3—B;
//返回值：
//[out]: dst :输出图像
//2017-07-13 09:49:59
Mat ImgAdjust::MyCurves(Mat src,double z[256],int x,int y,int AdjustChannel)
{
#if 0 //jessie
	curves.reSet();
	//read image file
	MyAdjustChannel(AdjustChannel);
	if ( !src.data ) {
		cout << "error read image" << endl;
	}

	m_src = src.clone();
	//src.copyTo(m_src);
	curves.mouseDown(x,y);
	curves.mouseMove(x,y);
	curves.mouseUp(x,y);

	Mat dst;
	
	Myinvalidate(dst,z);

	m_dst.release();
	m_src.release();

	return dst;
#else
	Mat dst = src.clone();
	uchar *data = dst.ptr<uchar>(0);
	int nStep =3;
	int firstLoc =0;

	if(src.channels() != 3)
		AdjustChannel =0;

	switch(AdjustChannel)
	{
	case 0:
		nStep =1;
		break;

	case 1:
		firstLoc =2;
		break;

	case 2:
		firstLoc =1;
		break;

	default:
		firstLoc =0;
		break;
	}

	data += firstLoc;
	for (int idr=0;idr<src.rows;idr++)
		for (int idc=0;idc<src.cols*src.channels();idc++,data+=nStep)
			*data = z[*data];

	return dst;
#endif
}

//阴影高光
Mat calrgbHist(Mat src)
{
	//设定bin数目
	int histSize = 256;
	//设定取值范围（R，G，B）
	float range[] = {0, 255};
	const float* histRange = {range};
	bool uniform = true;
	bool accumulate = false;
	Mat m_hist;
	 //计算直方图
	calcHist(&src,1,0,Mat(),m_hist,1,&histSize,&histRange,uniform,accumulate);
	
	return m_hist;
}

Mat Mysaturation(Mat src,int Saturation)  
{  
    Mat dst;  

	Mat hsv;//OPENCV 中 H、S、V、顺序分别为3*x+0  3*x+1   3*x+2
	//opencv中的 H分量是 0~180， S分量是0~255， V分量是0~255
	//	但是HSV颜色空间却规定的是，H范围0~360，S范围0~1，V范围0~1
	//	所以你需要自己转换一下，H*2，V/255，S/255
	cv::cvtColor (src,hsv,CV_BGR2HSV);
	Mat fhsv;
	hsv.convertTo (fhsv,CV_32F);

	vector<Mat> nChannel(3);  
	vector<Mat> mergenChannel(3); 
	split(fhsv,nChannel);

	Mat result; 
	mergenChannel[0]=nChannel[0]; 
	mergenChannel[1]=nChannel[1];
	mergenChannel[2]=nChannel[2];
	double maxVal = 0; //最大值一定要赋初值，否则运行时会报错
	double minVal = 0; 
    Point maxLoc;
	Point minLoc;
    minMaxLoc(mergenChannel[1], &minVal, &maxVal, &minLoc, &maxLoc);

	double saturation=Saturation*0.01;

	mergenChannel[1]=(saturation-1)*nChannel[1]*(255/(maxVal-minVal)) + nChannel[1];
	
	
	cv::merge(mergenChannel,result);
	Mat unitmat;
	result.convertTo (unitmat,CV_8UC3);
	Mat rgbmat;
	cv::cvtColor (unitmat,rgbmat,CV_HSV2BGR);
	
	return rgbmat;
} 

Mat norm_0_255(const Mat& src,int Saturation) 
{  
	// Create and return normalized image:  
	Mat dst1,dst; 
	Mat result;
	
	switch(src.channels()) {  
	case 1:  
		cv::normalize(src, dst, 0, 255, NORM_MINMAX, CV_8UC1);  
		//invalid(dst);
		break;  
	case 3:  
		//cout<<src<<endl;
		cv::normalize(src, result, 0, 255, NORM_MINMAX, CV_8UC3);  
		dst= Mysaturation(result,Saturation);
		break;  
	default:  
		src.copyTo(dst);  
		break;  
	}  
	return dst;  
}  
Mat AdjustGamma(Mat X,int Gamma,int Saturation)
{
	Mat I,dst;  
	if(Gamma==0)
	{
		X.copyTo(I);
	}
	else
	{
		float gamma = 1000.0/Gamma;//1/2.2; 
		pow(X, gamma, I); 
	}
	dst=norm_0_255(I,Saturation);
	
	return dst;
}

int isShadow(Mat src,Mat hsitmat,float &sumH,float &sumL)
{
	if(src.empty ())
	{
		return -1;
	}
	Mat X;
	for(int i=0;i<51;i++)
		{
			sumL = sumL+hsitmat.at<float>(i,0);
		}
		for(int i=200;i<256;i++)
		{
			sumH = sumH+hsitmat.at<float>(i,0);
		}
		if(sumL>0.25*src.rows*src.cols*src.channels())
		{
			cout<<"阴影"<<endl;
			src.convertTo(X, CV_32F);  
			
		    return 1;

		}
		if(sumH>0.25*src.rows*src.cols*src.channels())
		{
			cout<<"高光"<<endl;
			src.convertTo(X, CV_32F);  
			
			return 2;
		}
		return -1;
}
Mat ImgAdjust::ShadowHighlight(Mat src,int amount,int Saturation)
{
	if(src.empty())
	{
		return src;
	}
	float sumH=0;
	float sumL=0;
	Mat X,dst;
	src.convertTo(X, CV_32F);
	if(src.channels()==3)
	{
		Mat r_hist,g_hist,b_hist;
		vector<Mat> rgb_plannes;
		split(src,rgb_plannes);
		r_hist=calrgbHist(rgb_plannes[0]);
		g_hist=calrgbHist(rgb_plannes[1]);
		b_hist=calrgbHist(rgb_plannes[2]);
		Mat hsitmat;
		r_hist.copyTo (hsitmat);
		hsitmat.col(0)=r_hist.col(0)+g_hist.col(0)+b_hist.col(0);
		int Sh = isShadow(src,hsitmat,sumH,sumL);
		switch(Sh)
		{
		    case 1:dst=AdjustGamma(X,amount,Saturation);//阴影
					break;
			case 2:dst=AdjustGamma(X,amount,Saturation);//高光
					break;
			case -1:src.copyTo(dst);
					break;
		}
	}
	else
	{
		Mat hsitmat;
		hsitmat = calrgbHist(src);
		int Sh = isShadow(src,hsitmat,sumH,sumL);
		
		switch(Sh)
		{
		    case 1:dst=AdjustGamma(X,amount,Saturation);//阴影
				break;
			case 2:dst=AdjustGamma(X,amount,Saturation);//阴影
				break;
			case -1:src.copyTo(dst);
				break;
		}
	}
	
	return dst;
}

//去脏污
bool ImgAdjust::RmoveDust(Mat &srcIn,int mask,int contoursize)
{
	if(srcIn.empty())
	{
		return false;
	}
	
	//缩放尺寸
	float fMin = srcIn.rows;
	bool isRows =true;
	if(srcIn.rows > srcIn.cols)
	{
		fMin = srcIn.cols;
		isRows =false;
	}

	Mat src;
	float fRatio =1.0;
	float fminW = 1600.0;
	if( fMin >fminW )
	{
		fRatio = fminW/fMin;
		
		if(isRows)
			resize(srcIn,src,Size(srcIn.cols*fRatio,fminW),0,0,INTER_CUBIC);
		else
			resize(srcIn,src,Size(fminW,srcIn.rows*fRatio),0,0,INTER_CUBIC);
	}
	else
		src = srcIn.clone();

	//int inpaintRadius=40;
	//Mat dst,srcImage1;
	//Mat srcImage=src.clone();
	//srcImage1=Unsharp(srcImage,3.0,10,1);
	Mat grad_x, grad_y,dst;
	Mat abs_grad_x, abs_grad_y;


	//求 X方向梯度
	Sobel( src, grad_x, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT );
	convertScaleAbs( grad_x, abs_grad_x );

	//求Y方向梯度
	Sobel( src, grad_y, CV_16S, 0, 1, 3, 1, 1, BORDER_DEFAULT );
	convertScaleAbs( grad_y, abs_grad_y );

	//合并梯度(近似)
	addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, dst );
	
	Mat bw;
	bw =CAdaptiveThreshold::AdaptiveThreshold(dst, 0, 64, 0, 0.5);
	//Canny( dst, bw, 50, 150,3);
	Mat element = getStructuringElement(MORPH_RECT, Size(3,3));
	dilate(bw,bw,element);
	Mat bwcontours = Mat::zeros(src.rows,src.cols,CV_8UC1);
	
	Mat bwmat;
	bw.copyTo (bwmat);
	Mat bwmat1;
	bw.copyTo (bwmat1);
	vector<vector<Point> > contours;
	vector<vector<Point> > contours1;
	vector<vector<Point> > midtcontours;
	vector<vector<Point> > resultcontours;
	
	findContours(bwmat,contours1,CV_RETR_CCOMP,CV_CHAIN_APPROX_NONE);
	for(int i=0;i<contours1.size();i++)
	{
		if(contours1[i].size()<250&&contours1[i].size()>contoursize/*&&contours[i].size()>2&&contourArea(contours[i]) <20*/)
		{
			midtcontours.push_back (contours1[i]);
		}
		else if(contours1[i].size()> 1000&&contourArea(contours1[i]) >1500/*&&contours[i].size()>2*/)
		{
			midtcontours.push_back (contours1[i]);
			drawContours(bwmat1,  contours1, i, Scalar(0,0,0),3); 
		}
	}
		
	drawContours(bwmat1,  midtcontours, -1, Scalar(0,0,0), CV_FILLED);
	findContours(bwmat1,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
			
	vector<vector<Point> > contoursNew;
		
	for(int i=0;i<contours.size();i++)
	{
		if(contours[i].size()<200+contoursize&&contourArea(contours[i])<1500/*&&contours[i].size()>2&&contourArea(contours[i]) <20*/)
		{
			contoursNew.push_back (contours[i]);
		}
	}

	drawContours(bwcontours,contoursNew,-1,Scalar(255,255,255),CV_FILLED);
	vector<Rect> dustRect;
	for (vector<vector<Point> >::iterator itr = contoursNew.begin();itr !=contoursNew.end(); itr++)
	{
		RotatedRect rrect = minAreaRect(*itr);
		int nWidth = int(rrect.size.width+rrect.size.height);

		int nX = rrect.center.x - nWidth/2;
		nX = nX <0 ?0:(nX >=src.cols?src.cols-1:nX);
			
		int nY =rrect.center.y - nWidth/2;
		nY = nY <0 ?0:(nY >=src.rows?src.rows-1:nY);

		nWidth =nX+nWidth>src.cols? src.cols-1-nX:nWidth;
		nWidth =nY+nWidth>src.rows? src.rows-1-nY:nWidth;

		dustRect.push_back(Rect(nX,nY,nWidth,nWidth));
	}
	int idx=0;
		for (vector<Rect>::iterator itr = dustRect.begin();itr!=dustRect.end();itr++,idx++)
	{
		Rect rectTmp =*itr;
			
		if(rectTmp.x <20 ||rectTmp.y <20 ||rectTmp.x > srcIn.cols-20||rectTmp.y >srcIn.rows -20 ||rectTmp.width ==0 ||rectTmp.height ==0)
			continue;
			
		Mat srcROI = src(rectTmp);
		Mat maskROI = bwcontours(rectTmp);

		Mat srcROI_rs;

        cv::inpaint(srcROI,maskROI,srcROI_rs,3,0);

		Rect rTmp =*itr;
		int nx =rTmp.x/fRatio;
		int ny =rTmp.y/fRatio;
		int nWid = rTmp.width/fRatio;
			
		nx = nx <0 ?0:(nx >=srcIn.cols?srcIn.cols-1:nx);
		ny = ny <0 ?0:(ny >=srcIn.rows?srcIn.rows-1:ny);
			
		nWid =nx+nWid>srcIn.cols? srcIn.cols-1-nx:nWid;
		nWid =ny+nWid>srcIn.rows? srcIn.rows-1-ny:nWid;

 		Mat oriSrcROI = srcIn(Rect(nx,ny,nWid,nWid));
		resize(srcROI_rs,oriSrcROI,Size(nWid,nWid));
	}
		
	return true;
}

////////////////////////////////////////////////////////////////////////////图像去色
//图像去色
void ImgAdjust::colorDropout(Mat &src,int indexOfHue,int scopeRadius/* =5 */,int enHanceLightness/* = 0 */)
{
	//int nEnhanceScope = scopeRadius;
	//scopeRadius = scopeRadius >60 ?60 :scopeRadius;
	if( 3 != src.channels() || src.rows <6 || src.cols <6 || scopeRadius <=0)
		return;
	//limit scope of HUE
	scopeRadius = scopeRadius/2;
	indexOfHue = indexOfHue <0?0:(indexOfHue>360?360:indexOfHue);
#if 0
	scopeRadius = scopeRadius <0?0:(scopeRadius>90?90:scopeRadius);
	scopeRadius = scopeRadius == 90?scopeRadius+1:scopeRadius;
#else
	scopeRadius = scopeRadius <0?0:(scopeRadius>90?90:scopeRadius);
	scopeRadius = scopeRadius == 90?scopeRadius+1:scopeRadius;
#endif
	int leftValueColor,rightValueColor,startHueValue;
	startHueValue = indexOfHue/2;
	rightValueColor =startHueValue + scopeRadius;
	leftValueColor  =startHueValue - scopeRadius;

	bool isOver,isDown;
	isDown = isOver =false;

	if(rightValueColor >180)
	{
		rightValueColor -=180;
		isOver =true;
	}

	if(leftValueColor <0)
	{
		leftValueColor +=180;
		isDown =true;
	}

	//change rgb 2 hsv 
	Mat src_hsv;
	cvtColor(src,src_hsv,CV_BGR2HLS);
	uchar *data = src_hsv.ptr<uchar>(0);
	uchar *dataSrc = src.ptr<uchar>(0);
	//灰色通道
	//Mat grayMask;
	//const int nScopeLimit = 30;
	//cvtColor(src,grayMask,COLOR_BGR2GRAY);
	//uchar *dataGray = grayMask.ptr<uchar>(0);

	if(scopeRadius >30)
	{
		enHanceLightness = (scopeRadius -30)*3;
	}

	for (int idr=0;idr<src_hsv.rows;idr++)
	{
		for (int idc=0;idc<src_hsv.cols;idc++,data+=3,dataSrc+=3)
		{
			if(
				(
				(*data >leftValueColor && *data <rightValueColor && !isDown && !isOver)||
				((*data>leftValueColor ||*data <rightValueColor) && (isOver || isDown))
				)
				&&
				*(data +1) >30 
				&&
				*(data+2)  >50
				)
			{
				uchar nValue = max(*dataSrc,max(*(dataSrc+1),*(dataSrc+2)));
				nValue = saturate_cast<uchar>(nValue + enHanceLightness);

				*dataSrc = nValue;
				*(dataSrc+1) = nValue;
				*(dataSrc+2) = nValue;
			}
		}
	}
	
	return;
}

Mat ImgAdjust::colorDropout1(const Mat &src,int nColorChannel,int nFilterThreshold)
{
	if(nColorChannel <0 || nColorChannel >3)
		nColorChannel =3;

	if(nFilterThreshold >40)
		nFilterThreshold =40;

	int nColor1 =0;
	int nColor2 =2;
	switch(nColorChannel)
	{
		case 0:
			nColor1 =1;
			nColor2 =2;
			break;
		
		case 1:
			nColor1 = 0;
			nColor2 = 2;
			break;

		case 2:
			nColor1 = 0;
			nColor2 = 1;
			break;
	}

	const uchar *data = src.ptr<uchar>(0);
	Mat dst =Mat::zeros(src.rows,src.cols,CV_8UC1);
	uchar *dataDst = dst.ptr<uchar>(0);

	 double dbGary = 0.0f;
	for(int idr =0;idr <src.rows;idr++)
	{
		for(int idc=0;idc<src.cols;idc++,data+=3,dataDst++)
		{
			dbGary =0; 
			if(3 == nColorChannel)
			{
				dbGary +=((double)*data)*0.114;
				dbGary +=((double)*(data+1))*0.587;
				dbGary +=((double)*data)*0.299;
				if(dbGary > 255)
					dbGary =255;
				else
					*dataDst =(uchar)dbGary;
			}
			else if(
				*(data+nColorChannel)-*(data+nColor2) > nFilterThreshold &&
				*(data+nColorChannel)-*(data+nColor1) > nFilterThreshold &&
				nFilterThreshold >0
				)
				*dataDst = 255;
			else
				*dataDst = *(data+nColorChannel);

		}
	}

	return dst;
}

//图像锐化
#if 0
#define _MX_FILTER_15
#endif

#if 1
#define _MX_FILTER_16
#endif

bool ImgAdjust::sharp(Mat src,Mat &dst)
{
	if(!src.data || src.channels() !=3)
		return false;
	
	long r,g,b;
	int nRow0,nRow1,nRow2,nCol0,nCol1,nCol2;
	dst = src.clone();
	uchar *data = dst.ptr<uchar>(0);
	uchar *dataOri = src.ptr<uchar>(0);

	for (int idr=1;idr<src.rows-1;idr++)
	{
		nRow0 = idr-1;
		nRow1 = idr;
		nRow2 = idr+1;

		//nRow0 = nRow0 <0 ? 0:nRow0;
		//nRow2 = nRow2 >(src.rows -1) ? (src.rows-1):nRow2;
		for (int idc=1;idc<src.cols-1;idc++)
		{
			 r = g = b =0;
			 nCol0 = idc -1;
			 nCol1 = idc ;
			 nCol2 = idc +1;

			 //nCol0 = nCol0 <0?0:nCol1;
			 //nCol2 = nCol2 >(src.cols -1)?(src.cols-1):nCol2;

			 //第一行
			 r -= *(dataOri+nRow0*src.cols*3+nCol0*3 +0);
			 g -= *(dataOri+nRow0*src.cols*3+nCol0*3 +1);
			 b -= *(dataOri+nRow0*src.cols*3+nCol0*3 +2);

			 r -= *(dataOri+nRow0*src.cols*3+nCol1*3 +0);
			 g -= *(dataOri+nRow0*src.cols*3+nCol1*3 +1);
			 b -= *(dataOri+nRow0*src.cols*3+nCol1*3 +2);

			 r -= *(dataOri+nRow0*src.cols*3+nCol2*3 +0);
			 g -= *(dataOri+nRow0*src.cols*3+nCol2*3 +1);
			 b -= *(dataOri+nRow0*src.cols*3+nCol2*3 +2);
			 //第二行
			 r -= *(dataOri+nRow1*src.cols*3+nCol0*3 +0);
			 g -= *(dataOri+nRow1*src.cols*3+nCol0*3 +1);
			 b -= *(dataOri+nRow1*src.cols*3+nCol0*3 +2);

#ifdef _MX_FILTER_15
			 r += *(dataOri+nRow1*src.cols+nCol1*3 +0)<<4-*(dataOri+nRow1*src.cols+nCol1*3 +0);
			 g += *(dataOri+nRow1*src.cols+nCol1*3 +1)<<4-*(dataOri+nRow1*src.cols+nCol1*3 +1);
			 b += *(dataOri+nRow1*src.cols+nCol1*3 +2)<<4-*(dataOri+nRow1*src.cols+nCol1*3 +2);
#endif

#ifdef  _MX_FILTER_16
			 r += *(dataOri+nRow1*src.cols*3+nCol1*3 +0)<<4;
			 g += *(dataOri+nRow1*src.cols*3+nCol1*3 +1)<<4;
			 b += *(dataOri+nRow1*src.cols*3+nCol1*3 +2)<<4;
#else
			 r += *(dataOri+nRow1*src.cols+nCol1*3 +0)*9;
			 g += *(dataOri+nRow1*src.cols+nCol1*3 +1)*9;
			 b += *(dataOri+nRow1*src.cols+nCol1*3 +2)*9;
#endif		
			 r -= *(dataOri+nRow1*src.cols*3+nCol2*3 +0);
			 g -= *(dataOri+nRow1*src.cols*3+nCol2*3 +1);
			 b -= *(dataOri+nRow1*src.cols*3+nCol2*3 +2);

			 //第三行
			 r -= *(dataOri+nRow2*src.cols*3+nCol0*3 +0);
			 g -= *(dataOri+nRow2*src.cols*3+nCol0*3 +1);
			 b -= *(dataOri+nRow2*src.cols*3+nCol0*3 +2);

			 r -= *(dataOri+nRow2*src.cols*3+nCol1*3 +0);
			 g -= *(dataOri+nRow2*src.cols*3+nCol1*3 +1);
			 b -= *(dataOri+nRow2*src.cols*3+nCol1*3 +2);

			 r -= *(dataOri+nRow2*src.cols*3+nCol2*3 +0);
			 g -= *(dataOri+nRow2*src.cols*3+nCol2*3 +1);
			 b -= *(dataOri+nRow2*src.cols*3+nCol2*3 +2);

#ifdef _MX_FILTER_15
			 *data = (uchar)(min(255,max(0,int(r/7))));
			 *(data+1) = (uchar)(min(255,max(0,int(g/7))));
			 *(data+2) = (uchar)(min(255,max(0,int(b/7))));
#endif

#ifdef _MX_FILTER_16
			 *(data + nRow1*src.cols*3 +nCol1*3 +0) = (uchar)(min(255,max(0,int(r>>3))));
			 *(data + nRow1*src.cols*3 +nCol1*3 +1) = (uchar)(min(255,max(0,int(g>>3))));
			 *(data + nRow1*src.cols*3 +nCol1*3 +2) = (uchar)(min(255,max(0,int(b>>3))));
#else
			 *data = (uchar)(min(255,max(0,int(r))));
			 *(data+1) = (uchar)(min(255,max(0,int(g))));
			 *(data+2) = (uchar)(min(255,max(0,int(b))));
#endif

		}
	}

	
	return true;
}
