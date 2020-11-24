#include "stdafx.h"
//V1.0.0 Bic 2018年9月10日

//GDI 
#include <objidl.h>
#include <Gdiplus.h>
#pragma comment(lib, "Gdiplus.lib")
using namespace Gdiplus;


#include "WaterMark.h"
#include<math.h> 



//设置
static ULONG_PTR m_gdiplusToken;

#include "ConvertWA.h"
 //创建Font
 //参数说明：
 //（1）font：字体：string
 //（2）size：表示字体高度：int
 //（3）isItalic：是否斜体：bool
 //（4）isUnderline：是否下划线：bool
 //（5）isWeight：是否粗体：bool
 //（6）isStrikeOut：是否删除线：bool
 //（7）ptGdiplusFont：返回值
LOGFONT CreateFont(char*  font, 
	int size, 
	bool isItalic, 
	bool isUnderline, 
	bool isWeight, 
	bool isStrikeOut,
	HDC hHDC)
 {
	string fontString =font;//font.fontName;
	std::wstring wfontString = ConvertWA::ConvertWA_A2W(fontString);
	const WCHAR *wFontName =wfontString.c_str();

	LOGFONT lfont;
	
	//lfont.lfHeight =font.fontSize;
	lfont.lfHeight =GetDeviceCaps(hHDC, LOGPIXELSY)*size / 72;

	wcscpy(lfont.lfFaceName,wFontName);

	//加粗
	if(isWeight)
		lfont.lfWeight = 700;
	else
		lfont.lfWeight = 400;

	//斜体
	lfont.lfItalic = isItalic;

	//下划线
	lfont.lfUnderline = isUnderline;

	//无删除线
	lfont.lfStrikeOut = isStrikeOut;

	
	Gdiplus::Font myFont( hHDC, &lfont );

	return lfont;
 }


//V1.1.0 根据CamShop需求修改
//（1）Content：字符内容：string
//（2）Font：字体：string
//（3）Size：0-Auto，>0表示字体高度
//（4）Color：由以下三个值决定
//	（4-1）ColorR：（0-255）
//	（4-2）ColorR：（0-255）
//	（4-3）ColorR：（0-255）
//（5）Transparency：取值如下
//	（5-1）0：Translucent
//	（5-2）1：Opaque
//（6）Layout：取值如下
//	（6-1）0：LeftTop
//	（6-2）1：Center
//	（6-3）2：Diagonal
//	（6-4）3：RightBottom
//Max 2018/8/1
//V1.1.1 解决乱码问题
//Max 2019/1/24
Mat CWaterMark::WaterMark2(Mat srcMat,
	char * content,
	char *  font, long fontSize,
	long colorR, long colorG, long colorB,
	float transparent,
	int layout)
{
	
	//初始化
	//必须创建
 	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL); 

	//水印内容
	//const char* waterContent =content;

	//获取图像的长宽
	int imgHeight = srcMat.rows;
	int imgWidth = srcMat.cols;

	//保证数值不越界
#if 1
	//限定Red通道
	if(colorR>255)
		colorR =255;
	else
	{
		if(colorR<0)
			colorR =0;
	}
	//限定Green通道
	if(colorG>255)
		colorG =255;
	else
	{
		if(colorG<0)
			colorG =0;
	}
	//限定Blue通道
	if(colorB>255)
		colorB =255;
	else
	{
		if(colorB<0)
			colorB =0;
	}
#endif

	//保证图像为RGB
#if 1
	Mat src;
	int srcChannels =srcMat.channels();
	//将图像默认改为彩色图,灰度图后期改回来。
	if (srcMat.channels() == 1)
	{
		cvtColor(srcMat,src,CV_GRAY2BGR);
		srcChannels =1;
	} 
	else
	{
		if(srcMat.channels() ==3)
		{
			srcMat.copyTo(src);
			srcChannels = 3;
		}
		else
		{
			return srcMat;
		}
	}
#endif

	//获取字体
#if 1
	int size = fontSize;
	if(fontSize == 0)
	{
		size = 72;
	}
	
	std::wstring wfont = ConvertWA::ConvertWA_A2W(font);
	FontFamily *pfontFamily = new FontFamily(wfont.c_str());
	if(!pfontFamily->IsAvailable())
	{
		delete pfontFamily;
		pfontFamily = new FontFamily(L"Arial");
	}
    Gdiplus::Font myFont(pfontFamily, size, FontStyleRegular, UnitPixel);
	delete pfontFamily;
#endif

	//Bitmap thrd paras:must be a multiple of four.
	int remainderCols = 4- src.cols % 4;
	if ( remainderCols != 0)
	{
		//srcZerosROI =src(Range(0,src.rows),Range(0,src.cols - remainderCols));
		copyMakeBorder(src,src,0,0,0,remainderCols,BORDER_REFLECT,Scalar(0,0,0));
	}

	//thrd parameter:must be multiple of four
	int nstep =  (src.cols*src.channels()+3)/4*4;
	Gdiplus::Bitmap image(src.cols,src.rows,nstep,PixelFormat24bppRGB,(BYTE*)src.data);
	Gdiplus::BitmapData imageData ={};
	//由Bitmap创建Graphics对象
	Graphics imageGraphics(&image);

	//设置笔刷
	SolidBrush blackBrush(Color(transparent,colorR , colorG, colorB)); //半透明+文字RGB颜色
	//文字格式,默认水平
	StringFormat format;
	//与系统相同的文字边缘处理方式
	imageGraphics.SetTextRenderingHint(TextRenderingHintSystemDefault);

	//测量字符在图片中的像素长度
	RectF boudImg(0,0,imgWidth,imgHeight);
	RectF boudRect(0,0,imgWidth,imgHeight);
	string wString = content;
	std::wstring wwString = ConvertWA::ConvertWA_A2W(wString);
	const WCHAR *waterString = wwString.c_str();
	imageGraphics.MeasureString(waterString,-1,&myFont,boudImg,&boudRect);
	PointF school_site(0,0);

	//	（6-1）0：LeftTop
	//	（6-2）1：Center
	//	（6-3）2：Diagonal
	//	（6-4）3：RightBottom
	//计算位置
	
	if(layout == 0)
	{
		//LeftTop(左上角)
#if 1
		//位置
		int nPointX = 0;
		int nPointY = 0;

		//旋转角度
		float fAngle = 0;
		//旋转位置
		if(nPointX <0 || nPointY <0)
		{
			nPointX =(imgWidth)/2;
			nPointY =(imgHeight)/2;
		}

		//起始点设置
		imageGraphics.TranslateTransform(nPointX,nPointY);
		//旋转角度
		imageGraphics.RotateTransform(fAngle);

		if(fontSize == 0)
		{
			float fx = (float)image.GetWidth() / boudRect.Width;
			float fy = (float)image.GetHeight() / boudRect.Height;

			//尺寸设置
			imageGraphics.ScaleTransform(fx,fx);
		}
#endif
	}
	else if(layout == 1)
	{
		//Center(中心)
#if 1
		//尺寸设置
		float fx = 1;
		if(fontSize == 0)
		{
			fx = (float)image.GetWidth() / boudRect.Width;
		}

		int width = boudRect.Width * fx;
		int height = boudRect.Height * fx;

		//位置
		int nPointX = image.GetWidth() - width;
		int nPointY = image.GetHeight() - height;

		if(nPointX < 0)nPointX = 0;
		if(nPointY < 0)nPointY = 0;

		nPointX *= 0.5;
		nPointY *= 0.5;

		//旋转角度
		float fAngle = 0;

		//起始点设置
		imageGraphics.TranslateTransform(nPointX,nPointY);
		//旋转角度
		imageGraphics.RotateTransform(fAngle);

		imageGraphics.ScaleTransform(fx,fx);
#endif
	}
	else if(layout == 2)
	{
		//Diagonal(对角线)
#if 1
		//旋转角度
		float fAngle = -45;
		//弧度
		double sita = fAngle * CV_PI / 180;
		

		//计算对角线长度
		float lenImg = sqrt(pow((float)image.GetWidth(),2)+pow((float)image.GetHeight(),2));

		//尺寸设置
		float fx = 1;
		if(fontSize == 0)
		{
			//如果尺寸自动，则需要计算对角线/水印宽度的比值，用于放大
			fx = lenImg / boudRect.Width;
		}

		//计算相应的三角函数值
		double cosf = abs(cos(sita));
		double sinf = abs(sin(sita));
		double tanf = abs(tan(sita));

		//
		//fx = fx * cosf;
		if( (boudRect.Width+boudRect.Height)* sinf >(float)image.GetHeight())
			fx = (float)image.GetHeight()/((boudRect.Width+boudRect.Height)* sinf) ;
		int ww = boudRect.Width * fx;
		int hh = boudRect.Height * fx;
		int w = (hh+ww) * 0.5 * sinf;
		int h = (ww-hh) * 0.5 * cosf;
		int h1 = boudRect.Height * tanf * fx;
		
		//位置（中心点偏移）
		int nPointX = image.GetWidth()*0.5 - w;
		int nPointY = image.GetHeight()*0.5 + h;
		if(nPointX < 0 )
		{
			nPointX = 0;
		}
		if(nPointY < 0)
		{
			nPointY = 0;
		}

		//起始点设置
		imageGraphics.TranslateTransform(nPointX,nPointY);
		
		//旋转角度
		imageGraphics.RotateTransform(fAngle);

		//设置尺寸
		imageGraphics.ScaleTransform(fx,fx);

		
#endif
	}
	else if(layout == 3)
	{
		//RightBottom
#if 1
		float fx = 1;
		if(fontSize == 0)
		{
			fx = (float)image.GetWidth() / boudRect.Width;
		}

		//位置
		int nPointX = image.GetWidth() - boudRect.Width * fx;
		int nPointY = image.GetHeight() - boudRect.Height * fx;

		//起始点设置
		imageGraphics.TranslateTransform(nPointX,nPointY);
		//尺寸设置
		imageGraphics.ScaleTransform(fx,fx);
#endif
	}

	//添加水印
	//Status s = imageGraphics.DrawString(waterString,wcslen(waterString),&myFont, school_site, &format, &blackBrush);//把string绘制到图上
	Status s = imageGraphics.DrawString(waterString, -1, &myFont,  
            boudRect, &format, &blackBrush);  

	if(s != Status::Ok)
	{
		pfontFamily = new FontFamily(L"Arial");
		Gdiplus::Font fontTmp(pfontFamily, size, FontStyleRegular, UnitPixel);
		delete pfontFamily;

		s = imageGraphics.DrawString(waterString, -1, &fontTmp,  
            boudRect, &format, &blackBrush);  
	}

	//旋转水印
	imageGraphics.ResetTransform();

	//Bimap转Mat
	Gdiplus::Rect rect(0,0,image.GetWidth(),image.GetHeight());
	image.LockBits(&rect,Gdiplus::ImageLockModeRead,image.GetPixelFormat(),&imageData);

	Mat dst(image.GetHeight(),image.GetWidth(),CV_8UC3,imageData.Scan0,std::abs(imageData.Stride));
	image.UnlockBits(&imageData);

	Mat dstTmp = dst(cv::Rect(0,0,dst.cols -remainderCols,dst.rows)).clone();
	dst.release();

	Mat safeDst;
	if (srcChannels ==1)
	{
		cvtColor(dstTmp,safeDst,CV_RGB2GRAY);
	} 
	else
	{
		if(srcChannels ==3)
			dstTmp.copyTo(safeDst);
		else
		{
			return dstTmp;
		}
	} 

	return safeDst;
}

//水印添加
//V1.0.0 创建
//Bicycle
//#include "LogWriter.h"
//CLogWriter *g_log = CLogWriter::GetLogWriter();
#define  CV_WRITE_LOG 0
 Mat CWaterMark::WaterMark(Mat srcMat,char * msg,int x,int y,long fontSize,char *  font,bool IsItalic,bool IsUnderline,bool IsWeight,
							float angle,float transparent,long colorR,long colorG,long colorB,float stringRatio, float stringRatioH)
{
#if CV_WRITE_LOG
	g_log->Log("1");
#endif

	//水印内容
	const char* waterContent =msg;

	//位置
	int nPointX = x;
	int nPointY = y;

	//旋转角度
	float fAngle = angle;

	//获取图像的长宽
	int imgHeight = srcMat.rows;
	int imgWidth = srcMat.cols;

	//保证数值不越界
#if 1
	//旋转位置
	if(nPointX <0 || nPointY <0)
	{
		nPointX =(imgWidth)/2;
		nPointY =(imgHeight)/2;
	}
	//限定旋转比例位置
	if (stringRatio <0)
		stringRatio =0;
	else
	{
		if (stringRatio >1)
			stringRatio =1;
	}

	//限定Red通道
	if(colorR>255)
		colorR =255;
	else
	{
		if(colorR<0)
			colorR =0;
	}
	//限定Green通道
	if(colorG>255)
		colorG =255;
	else
	{
		if(colorG<0)
			colorG =0;
	}
	//限定Blue通道
	if(colorB>255)
		colorB =255;
	else
	{
		if(colorB<0)
			colorB =0;
	}
#endif

#if CV_WRITE_LOG
	g_log->Log("2");
#endif

	//初始化
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL); 

#if CV_WRITE_LOG
	g_log->Log("3");
#endif

	Mat src;
	int srcChannels =srcMat.channels();
	//将图像默认改为彩色图,灰度图后期改回来。
	if (srcMat.channels() == 1)
	{
		cvtColor(srcMat,src,CV_GRAY2BGR);
		srcChannels =1;
	} 
	else
	{
		if(srcMat.channels() ==3)
		{
			srcMat.copyTo(src);
			srcChannels=3;
		}
		else
		{
			return srcMat;
		}
	}

#if CV_WRITE_LOG
	g_log->Log("4");
#endif

#if 1
	//获取字体
	//HDC hHDC = ::GetDC(NULL);
	//LOGFONT lfont = CreateFont(font, fontSize, 
	//							IsItalic, 
	//							IsUnderline, 
	//							IsWeight, 
	//							false,
	//							hHDC);
	////GDI+只能如此创建
	//Gdiplus::Font myFont( hHDC, &lfont );
	//::ReleaseDC(NULL, hHDC );

	std::wstring wfont = ConvertWA::ConvertWA_A2W(font);
	FontFamily *pfontFamily = new FontFamily(wfont.c_str());
	if(!pfontFamily->IsAvailable())
	{
		delete pfontFamily;
		pfontFamily = new FontFamily(L"Arial");
	}
    Gdiplus::Font myFont(pfontFamily, fontSize, FontStyleRegular, UnitPixel);
	delete pfontFamily;
#endif

#if CV_WRITE_LOG
	g_log->Log("5");
#endif

	//Bitmap thrd paras:must be a multiple of four.
	Mat srcZerosROI;
	Mat srcRemain =src.clone();

	int remainderCols = 4- src.cols % 4;
	if ( remainderCols != 0)
	{
		//srcZerosROI =src(Range(0,src.rows),Range(0,src.cols + remainderCols));
		copyMakeBorder(src,srcZerosROI,0,0,0,remainderCols,BORDER_REFLECT,Scalar(0,0,0));
		srcZerosROI.copyTo(src);
	}

#if CV_WRITE_LOG
	g_log->Log("6");
#endif


	//thrd parameter:must be multiple of four
	Gdiplus::Bitmap image(src.cols,src.rows,src.cols*3,PixelFormat24bppRGB,(BYTE*)src.data);
	Gdiplus::BitmapData imageData ={};
	//由Bitmap创建Graphics对象
	Graphics imageGraphics(&image);

#if CV_WRITE_LOG
	g_log->Log("7");
#endif

	//设置笔刷
	SolidBrush blackBrush(Color(transparent,colorR , colorG, colorB)); //半透明+文字RGB颜色
	//文字格式,默认水平
	StringFormat format;
	//与系统相同的文字边缘处理方式
	imageGraphics.SetTextRenderingHint(TextRenderingHintSystemDefault);

#if CV_WRITE_LOG
	g_log->Log("8");
#endif

	//测量字符在图片中的像素长度
	RectF boudRect(0,0,imgWidth,imgHeight);
	string wString = waterContent;
	std::wstring wwString = ConvertWA::ConvertWA_A2W(wString);
	const WCHAR *waterString = wwString.c_str();
	imageGraphics.MeasureString(waterString,wcslen(waterString),&myFont,boudRect,&boudRect);
	PointF school_site((REAL)-boudRect.Width*stringRatio,(REAL)-boudRect.Height*stringRatioH);

#if CV_WRITE_LOG
	g_log->Log("9");
#endif

	//旋转角度
	imageGraphics.TranslateTransform(nPointX,nPointY);
	imageGraphics.RotateTransform(fAngle);
	imageGraphics.DrawString(waterString,wcslen(waterString),&myFont, school_site, &format, &blackBrush);//把string绘制到图上
	imageGraphics.ResetTransform();

	//Bimap转Mat
	Gdiplus::Rect rect(0,0,image.GetWidth(),image.GetHeight());
	image.LockBits(&rect,Gdiplus::ImageLockModeRead,image.GetPixelFormat(),&imageData);

	Mat dst(image.GetHeight(),image.GetWidth(),CV_8UC3,imageData.Scan0,std::abs(imageData.Stride));
	image.UnlockBits(&imageData);

#if CV_WRITE_LOG
	g_log->Log("10");
#endif

	Mat dstTmp = dst(cv::Rect(0,0,dst.cols -remainderCols,dst.rows)).clone();
	dst.release();

	Mat safeDst;
	if (srcChannels ==1)
	{
		cvtColor(dstTmp,safeDst,CV_RGB2GRAY);
	} 
	else
	{
		if(srcChannels ==3)
			dstTmp.copyTo(safeDst);
		else
		{
			return dstTmp;
		}
	} 

#if CV_WRITE_LOG
	g_log->Log("11");
#endif


	return safeDst;
}