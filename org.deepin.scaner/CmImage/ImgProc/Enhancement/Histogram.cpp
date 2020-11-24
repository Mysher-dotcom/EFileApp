#include "stdafx.h"
#include "Histogram.h"
#define RGB2GRAY(r,g,b) (((b)*117 + (g)*601 + (r)*306) >> 10)

//绘制灰度直方图
bool CHistogram::DrawHistogram(Mat src, Mat &dst)
{
	if(!src.data)
	{
		return false;
	}

	Mat gray;

	if(src.channels() == 3)
	{
		cvtColor(src, gray, CV_BGR2GRAY);
	}//end if
	else
	{
		gray = src.clone();
	}//end if else

	int bins = 256;
	int hist_size[] = {bins};
	float range[] = { 0, 256 };
	const float* ranges[] = { range};
	MatND hist;
	int channels[] = {0};

	calcHist( &gray, 1, channels, Mat(), // do not use mask
		hist, 1, hist_size, ranges,
		true, // the histogram is uniform
		false );

	double max_val;
	minMaxLoc(hist, 0, &max_val, 0, 0);
	int scale = 2;
	int hist_height=256;
	Mat hist_img = Mat::zeros(hist_height,bins*scale, CV_8UC3);
	for(int i=0;i<bins;i++)
	{
		float bin_val = hist.at<float>(i); 
		int intensity = cvRound(bin_val*hist_height/max_val);  //要绘制的高度
		rectangle(hist_img,Point(i*scale,hist_height-1),
			Point((i+1)*scale - 1, hist_height - intensity),
			CV_RGB(255,255,255));
	}
	
	dst = hist_img.clone();


	return true;
}

//直方图log算法
//实现Mat接口
//参数:
//[in] src		:	源图像	
//返回值:
//处理后图像
Mat CHistogram::HistogramLog(Mat src,bool isCC)
{
	Mat dst;
	if(!src.data)
	{
		return dst;
	}

    //IplImage *iplSrc =  &(IplImage)src;
    IplImage iplSrcTmp =  (IplImage)src;
    IplImage *iplSrc =  &iplSrcTmp;
	
	IplImage *iplDst =  cvCreateImage(cvSize(src.cols,src.rows),8,1);
	if(HistogramLog(iplSrc, iplDst))
	{
		dst = cv::cvarrToMat(iplDst,true);  
	}

	if(iplDst!= NULL)
	{
		cvReleaseImage(&iplDst);
	}

	if( 3 == src.channels() && !isCC)
	{
		vector<Mat> vMat;
		
		for(int idx=0;idx <3;idx++)
			vMat.push_back(dst);

		merge(vMat,dst);
	}

	return dst;
}

Mat CHistogram::HistogramRoot(Mat src,bool isCC)
{
	Mat dst;
	if(!src.data)
	{
		return dst;
	}

    //IplImage *iplSrc =  &(IplImage)src;
    IplImage iplSrcTmp =  (IplImage)src;
    IplImage *iplSrc =  &iplSrcTmp;

	IplImage *iplDst =  cvCreateImage(cvSize(src.cols,src.rows),8,1);
	if(HistogramRoot(iplSrc, iplDst))
	{
		dst = cv::cvarrToMat(iplDst,true);  
	}

	if(iplDst!= NULL)
	{
		cvReleaseImage(&iplDst);
	}

	if( 3 == src.channels() && !isCC)
	{
		vector<Mat> vMat;

		for(int idx=0;idx <3;idx++)
			vMat.push_back(dst);

		merge(vMat,dst);
	}

	return dst;
}

//直方图log算法
//从CxImage移植
//Max 2014-04-23 14:32:26
bool CHistogram::HistogramLog(IplImage *src, IplImage *dst )
{
	if(src == NULL||dst == NULL)
	{
		return false;
	}

	IplImage *bgr = cvCreateImage(cvGetSize(src),8,3);
	if(src->nChannels != 3)
	{
		cvCvtColor(src,bgr,CV_GRAY2BGR);
	}
	else
	{
		cvCopy(src,bgr);
	}

	IplImage *bgr_dst = cvCreateImage(cvGetSize(src),8,3);


	//q(i,j) = 255/log(1 + |high|) * log(1 + |p(i,j)|);
    int x, y, i;
	RGBQUAD color;
	RGBQUAD	yuvClr;

	int YVal, high = 1;

	uchar *data   = (uchar *)bgr->imageData; 
	uchar *data_dst   = (uchar *)bgr_dst->imageData;

    // Find Highest Luminance Value in the Image
	for(y=0; y < bgr_dst->height; y++)
	{
		for(x=0; x < bgr_dst->width; x++)
		{
			int pos = y*bgr_dst->widthStep + x*bgr_dst->nChannels;
			YVal = RGB2GRAY(data[pos+2], data[pos+1], data[pos]);
			if (YVal > high ) high = YVal;
		}
	}


	// Logarithm Operator
	double k = 255.0 / ::log( 1.0 + (double)high );
	
	for( y = 0; y < bgr_dst->height; y++ ){
		for( x = 0; x < bgr_dst->width; x++ ){

			int pos = y*bgr_dst->widthStep + x*bgr_dst->nChannels;

			color.rgbBlue = data[pos];
			color.rgbGreen = data[pos+1];
			color.rgbRed = data[pos+2];

			yuvClr = RGBtoYUV( color );
                
			yuvClr.rgbRed = (int)(k * ::log( 1.0 + (double)yuvClr.rgbRed ) );

			color = YUVtoRGB( yuvClr );
			
			data_dst[pos] = color.rgbBlue;
			data_dst[pos+1] = color.rgbGreen;
			data_dst[pos+2] = color.rgbRed;
		}
	}
	
	
	if(dst->nChannels != 3)
	{
		cvCvtColor(bgr_dst,dst,CV_BGR2GRAY);
	}
	else
	{
		cvCopy(bgr_dst,dst);
	}

	cvReleaseImage(&bgr);
	cvReleaseImage(&bgr_dst);

	return true;
}

//直方图Root算法
//从CxImage移植
//Max 2015-02-09 16:02:25
bool CHistogram::HistogramRoot(IplImage *src, IplImage *dst )
{
	if(src == NULL||dst == NULL)
	{
		return false;
	}

	IplImage *bgr = cvCreateImage(cvGetSize(src),8,3);
	if(src->nChannels != 3)
	{
		cvCvtColor(src,bgr,CV_GRAY2BGR);
	}
	else
	{
		cvCopy(src,bgr);
	}

	IplImage *bgr_dst = cvCreateImage(cvGetSize(src),8,3);

    int x, y, i;
	RGBQUAD color;
	RGBQUAD	 yuvClr;
	double	dtmp;
	int YVal, high = 1;

	uchar *data   = (uchar *)bgr->imageData; 
	uchar *data_dst   = (uchar *)bgr_dst->imageData;

     // Find Highest Luminance Value in the Image
	for(y=0; y < bgr_dst->height; y++)
	{
		for(x=0; x < bgr_dst->width; x++)
		{
			int pos = y*bgr_dst->widthStep + x*bgr_dst->nChannels;
			YVal = (int)RGB2GRAY(data[pos+2], data[pos+1], data[pos]);
			if (YVal > high ) high = YVal;
		}
	}
		

	// Root Operator
	double k = 256.0 / ::sqrt( 1.0 + (double)high );

	for( y = 0; y < bgr_dst->height; y++ )
	{
		for( x = 0; x < bgr_dst->width; x++ )
		{

			int pos = y*bgr_dst->widthStep + x*bgr_dst->nChannels;

			color.rgbBlue = data[pos];
			color.rgbGreen = data[pos+1];
			color.rgbRed = data[pos+2];

			yuvClr = RGBtoYUV( color );

			dtmp = k * ::sqrt( (double)yuvClr.rgbRed );
			if ( dtmp > 255.0 )	dtmp = 255.0;
			if ( dtmp < 0 )	dtmp = 0;
            yuvClr.rgbRed = (int)dtmp;

			color = YUVtoRGB( yuvClr );

			data_dst[pos] = color.rgbBlue;
			data_dst[pos+1] = color.rgbGreen;
			data_dst[pos+2] = color.rgbRed;
		}
	}
	
 
	if(dst->nChannels != 3)
	{
		cvCvtColor(bgr_dst,dst,CV_BGR2GRAY);
	}
	else
	{
		cvCopy(bgr_dst,dst);
	}
	return true;
}


RGBQUAD CHistogram::RGBtoYUV(RGBQUAD lRGBColor)
{
	int Y,U,V,R,G,B;
	R = lRGBColor.rgbRed;
	G = lRGBColor.rgbGreen;
	B = lRGBColor.rgbBlue;

//	Y = (int32_t)( 0.257 * R + 0.504 * G + 0.098 * B);
//	U = (int32_t)( 0.439 * R - 0.368 * G - 0.071 * B + 128);
//	V = (int32_t)(-0.148 * R - 0.291 * G + 0.439 * B + 128);
	Y = (int)(0.299f * R + 0.587f * G + 0.114f * B);
	U = (int)((B-Y) * 0.565f + 128);
	V = (int)((R-Y) * 0.713f + 128);

	Y= min(255,max(0,Y));
	U= min(255,max(0,U));
	V= min(255,max(0,V));
	RGBQUAD yuv;
	yuv.rgbBlue = B;
    yuv.rgbGreen = G;
    yuv.rgbRed = R;
    yuv.rgbReserved = 0;
	return yuv;
}

RGBQUAD CHistogram::YIQtoRGB(RGBQUAD lYIQColor)
{
	int I,Q,R,G,B;
	float Y = lYIQColor.rgbRed;
	I = lYIQColor.rgbGreen - 128;
	Q = lYIQColor.rgbBlue - 128;

	R = (int)( Y + 0.956f * I + 0.621f * Q);
	G = (int)( Y - 0.273f * I - 0.647f * Q);
	B = (int)( Y - 1.104f * I + 1.701f * Q);

	R= min(255,max(0,R));
	G= min(255,max(0,G));
	B= min(255,max(0,B));
	RGBQUAD rgb;
	rgb.rgbBlue = B;
    rgb.rgbGreen = G;
    rgb.rgbRed = R;
    rgb.rgbReserved = 0;
	return rgb;
}

RGBQUAD CHistogram::YUVtoRGB(RGBQUAD lYUVColor)
{
	int U,V,R,G,B;
	float Y = lYUVColor.rgbRed;
	U = lYUVColor.rgbGreen - 128;
	V = lYUVColor.rgbBlue - 128;

//	R = (int32_t)(1.164 * Y + 2.018 * U);
//	G = (int32_t)(1.164 * Y - 0.813 * V - 0.391 * U);
//	B = (int32_t)(1.164 * Y + 1.596 * V);
	R = (int)( Y + 1.403f * V);
	G = (int)( Y - 0.344f * U - 0.714f * V);
	B = (int)( Y + 1.770f * U);

	R= min(255,max(0,R));
	G= min(255,max(0,G));
	B= min(255,max(0,B));
	RGBQUAD rgb;
	rgb.rgbBlue = B;
    rgb.rgbGreen = G;
    rgb.rgbRed = R;
    rgb.rgbReserved = 0;
	return rgb;
}
