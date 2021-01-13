#include "cmimage.h"
#include "./ImgProc/ColorSpace/AdaptiveThreshold.h"

#include "./ImgProc/Enhancement/ColorSpace.hpp"
#include "./ImgProc/Enhancement/Curves.hpp"
#include "./ImgProc/Enhancement/DocumentBackgroundSeparation.h"
#include "./ImgProc/Enhancement/FilterEffect.h"
#include "./ImgProc/Enhancement/Histogram.h"
#include "./ImgProc/Enhancement/HSL.hpp"
#include "./ImgProc/Enhancement/SharpImage.h"
#include "./ImgProc/Enhancement/Halftone.h"
#include "./ImgProc/Enhancement/AutoLevel.h"
#include "./ImgProc/Enhancement/BrightnessBalance.h"
#include "./ImgProc/Enhancement/newColorEnhance.h"
#include "./ImgProc/Enhancement/ImageEnhancement.h"
#include "./ImgProc/Enhancement/ImgAdjust.h"
#include "./ImgProc/Enhancement/ColorEnhance.h"
#include "./ImgProc/Enhancement/FilterEffect.h"
#include "./ImgProc/Enhancement/Noise.h"
#include "./ImgProc/Enhancement/RemoveNoise.h"
#include "./ImgProc/Enhancement/DeleteNoise_BW.h"
#include "./ImgProc/Enhancement/DetectionWhitePage.h"
#include "./ImgProc/Enhancement/ImageDecoder.h"
#include "./ImgProc/Enhancement/Compare.h"

#include "./ImgProc/Feature/AdjustSkew.h"
#include "./ImgProc/Feature/DetectEdge.h"
#include "./ImgProc/Feature/DetectRectByContours.h"
#include "./ImgProc/Feature/DetectRectBySegmation.h"
#include "./ImgProc/Feature/FillBorder.h"
#include "./ImgProc/Feature/PunchHold.h"

#include "./ImgProc/Transform/CropRectByCV.h"
#include "./ImgProc/Transform/MergerImage.h"
#include "./ImgProc/Transform/Rotate.h"


//#include "Model/MImageModel/MImageDataPool.h"


//#include "./ImgProc/Libjpeg/Jpeg.h"



//#include <QString>
//#include "./Util/timeutil.h"
//QString g_start_day = "2019-10-15";
int g_avialable_day = 35;
bool g_init = false;

//初始化
//参数:
//无
//返回值:
//true: 成功
//false: 失败
bool mcvInit()
{
#if 0
    QString cur = TimeUtil::getCurrentDate();
    int days = TimeUtil::getDayDiff(cur,g_start_day);
    if(days <= g_avialable_day&&days > 0){
        g_init = true;
        return true;
    }
    g_init = false;
    return false;
#else
    /*
	time_t tt;
    time( &tt );
    tt = tt + 8*3600;  // transform the time zone
    tm* t= gmtime( &tt );
    if ( t->tm_year + 1900 > 2020 || (t->tm_mon + 1 > 10 && t->tm_year + 1900 == 2020))
	{
		g_init = false;
		return false;
    }
    */
	g_init = true;
    return true;
#endif
}

//获取版本号
string mcvGetVersion()
{
    if(!g_init){
        return "";
    }
    return "V3.0.0.0";
}

//加载图像
//参数:
//无
//[in] chImgPath: 源图像路径
//返回值:
//MImage对象指针,如果失败则为0
MImage* mcvLoadImage(char* path,long w_Dest, long h_Dest)
{
    if(!g_init){
        return 0;
    }
	
    cv::Mat srcMat;
    int xDPI = 96;
    int yDPI = 96;

    srcMat = cv::imread(path);

    MImage* src = CAdapter::Mat2mimg(srcMat);
    if(src!=0)
    {
        src->xDPI = xDPI;
        src->yDPI = yDPI;
    }
    return src;
}

//克隆MImage对象指针（深拷贝）,对象内存与src无关
//参数:
//[in] src: 源图像指针
//返回值:
//MImage对象指针,如果失败则为0
MImage* mcvClone(MImage* src)
{
    if(!g_init){
        return 0;
    }

    if(src->dataIndex !=-1)
    {
        Mat mat_src = CMImageDataPool::Access(src->dataIndex);
        Mat mat_dst = mat_src.clone();
        MImage *dst = CAdapter::Mat2mimg(mat_dst);
        return dst;
    }
    return NULL;
}

//将源图像复制到目标图像指定位置
//参数:
//[in/out] dst: 目标图像
//[in] src: 源图像指针
//pt: 复制位置
//返回值:
//true: 成功
//false: 失败
bool mcvCopyToRect(MImage* src, MImage* copy, MPoint pt)
{
    if(!g_init){
        return false;
    }

    Mat matSrc = CAdapter::mimg2Mat(src);
    Mat matCopy = CAdapter::mimg2Mat(copy);
    if(matSrc.data == 0 || matCopy.data == 0)
    {
        return false;
    }
    Mat matDst;

    //IplImage *iplSrc = &(IplImage)matSrc;
    IplImage iplSrcTmp = (IplImage)matSrc;
    IplImage *iplSrc = &iplSrcTmp;

    //IplImage *iplCopy = &(IplImage)matCopy;
    IplImage iplCopyTmp = (IplImage)matCopy;
    IplImage *iplCopy = &iplCopyTmp;

    CvRect rc = cvRect(pt.x,
        pt.y,
        copy->width,
        copy->height);

    ::cvSetImageROI(iplSrc,rc);

    ::cvCopy(iplCopy,iplSrc);


    return true;
}

//获取图像为宽度
int mcvGetWidthStep(MImage *src)
{
    if(!g_init){
        return 0;
    }

    if(src->dataIndex !=-1)
    {
        Mat mat_src = CMImageDataPool::Access(src->dataIndex);
        return mat_src.step;
    }
    return 0;
}

//从内存创建MImage对象指针,对象内存与data无关
//参数:
//[in] width: 源图像宽度
//[in] height: 源图像高度
//[in] channel: 源图像颜色通道数量,支持1,3（默认位深度为8）
//[in] data: 源图像内存
//[in] bFlip: 是否需要对内存做Flip
//返回值:
//MImage对象指针,如果失败则为0
MImage* mcvCreateImageFromArray(int width, int height, int channel, char* data, bool bFlip)
{
	int nType = CV_8UC3;
	if(channel == 1)
	{
		nType = CV_8UC1;
	}
	Mat srcMat(height,width,nType); 
	//IplImage *ipl = CAdapter::Mat2Ipl(srcMat);
	IplImage temp = (IplImage)srcMat;
	IplImage *ipl = &temp;
	int nSize = ipl->height * ipl->widthStep;
	if(false == bFlip)
	{
		::memcpy(ipl->imageData, data, nSize);
	}
	else
	{
		for(int y = 0; y < ipl->height; y++)
		{
			int nPos0 = (ipl->height -  y - 1) * ipl->widthStep;
			int nPos = y * ipl->widthStep;

			::memcpy(ipl->imageData + nPos0, data+nPos, ipl->widthStep);
		}

	}

	MImage* src = CAdapter::Mat2mimg(srcMat);

	return src;
}

//释放图像,所有MImage对象指针必须通过此函数释放否则内存泄漏
//参数使用源图像指针的指针,如此可以将用户声明的Mimage对象指针置为0,调用ReleaseImage后,可以通过"if(pt == 0)"判断是否已经释放内存
//参数:
//[in] src: 源图像指针的指针
//返回值:
//无
void mcvReleaseImage(MImage** src)
{
    if(!g_init){
        return;
    }

    if(src != NULL && *src != NULL)
    {
        int nType = mcvGetMImageMemType(*src);
        if(nType == 0)
        {
			//delete src;
            mcvReleaseImage1(*src);
        }
        else
        {
            delete *src;
            *src = NULL;
        }
    }
}
void mcvReleaseImage1(MImage* src)
{
    if(!g_init){
        return;
    }
    if(src != NULL)
    {
		CMImageDataPool::Delete(src->dataIndex);
    }
}

//保存图像
//参数:
//[in] sz: 存储路径
//[in] src: 源图像
//[in] xDPI: X方向DPI数值
//[in] yDPI: Y方向DPI数值
//[in] jpgQuanlity: JPG压缩质量（0-100）
//[in] bBW: 是否保存为真的二值化图像（位深度为1）//返回值:
//true: 成功
//false: 失败
bool mcvSaveImage(char* sz, MImage* src, int xDPI, int yDPI, int jpgQuanlity, bool bBW)
{

    if(!g_init){
        return false;
    }
	
    bool bResult = false;
    if(src == 0 || sz == 0)
    {
        return bResult;
    }

    //如果输出格式是JPG则不支持二值化
    //提升鲁棒性
    string strPath = string(sz);
    if(strPath.length() < 4)
    {
        return false;
    }
    if(strPath.substr(strPath.length() - 4) == ".jpg")
    {
        bBW = false;
    }

    if(xDPI == -1 || yDPI == -1)
    {
        xDPI = src->xDPI;
        yDPI = src->yDPI;
    }

    if(false == bResult)
    {
        //int nResult = cvSaveImage(sz,ipl);
        Mat srcMat = CAdapter::mimg2Mat(src);
        if(srcMat.data == 0)
        {
            return false;
        }
        if(imwrite(sz,srcMat))
            return true;
        else
            return false;
    }


    return bResult;
}

//绘制直线
//参数:
//[in] src: 源图像
//[in] pt0: 起始点
//[in] pt1: 终止点
//[in] color: 直线颜色
//[in] thickness: 线宽度
//返回值:
//true: 成功
//false: 失败
bool mcvDrawLine(MImage* src, MPoint pt0, MPoint pt1, MColor color, int thickness)
{
    if(!g_init){
        return false;
    }

    if(src == NULL)
    {
        return false;
    }
    Mat srcMat = CAdapter::mimg2Mat(src);
    cv::line(srcMat,cvPoint(pt0.x,pt0.y),cvPoint(pt1.x,pt1.y),CV_RGB(color.m_nR,color.m_nG,color.m_nB),thickness);
    return true;
}

//图像上绘线
//img			图像指针
//colorValue	颜色值(char[3])单通道，双通道，后面的颜色值无关紧要
//pt			起止点(Point[2])
//imgInfo		图像信息(int[3]分别对应：height,width,channel,line size)
void mcvDrawLineOnImageBuffer(char *Img,char *colorValue,MPoint *pt,int *imgInfo)
{
	Point ptTmp[2];
	ptTmp[0] = Point(pt[0].x,pt[0].y);
	ptTmp[1] = Point(pt[1].x,pt[1].y);
	CImageDecoder::drawLineOnImgBuf(Img,colorValue,ptTmp,imgInfo);
}

//绘制任意角度矩形
//参数:
//[in] src: 源图像
//[in] rectR: 带角度矩形
//[in] color: 绘制颜色
//[in] thickness: 线宽度
//返回值:
//true: 成功
//false: 失败
bool mcvDrawRectR(MImage* src, MRectR rectR, MColor color, int thickness)
{
    if(!g_init){
        return false;
    }

    if(src == NULL)
    {
        return false;
    }
    int nW = src->width;
    //IplImage *iplSrc = CAdapter::mimg2Ipl(src);
    Mat srcMat = CAdapter::mimg2Mat(src);
    if(srcMat.data != 0)
    {
        for(int n = 0; n < 4; n++)
        {
            int nStart = n;
            int nEnd = n+1;
            if(nEnd >= 4)
            {
                    nEnd = 0;
            }

            cv::line(srcMat,
            cvPoint(rectR.m_pt[nStart].x,rectR.m_pt[nStart].y),
            cvPoint(rectR.m_pt[nEnd].x,rectR.m_pt[nEnd].y),
            CV_RGB(color.m_nR,color.m_nG,color.m_nB),thickness);
        }
    }
    return true;
}

//弹出一个可以调节尺寸的窗口显示Mimage对象,窗口为模态,意味着需要将窗口关闭才可以进行之后的操作
//参数:
//[in] src: 源图像
//返回值:
//无
void mcvShowImage(MImage* src)
{
    if(!g_init){
        return;
    }

    if(src != NULL)
    {
        cv::Mat srcMat = CMImageDataPool::Access(src->dataIndex);
        if(srcMat.data != 0)
        {
            cv::namedWindow("mcvShowImage",0);
            cv::imshow("mcvShowImage",srcMat);
            cv::waitKey();
        }
    }
}

//将源对象灰度化,返回目标对象内存与源对象独立
//参数:
//[in] src: 源图像
//返回值:
//MImage对象指针,如果失败则为0
MImage * mcvGrayStyle(MImage *src)
{
    if(!g_init){
        return 0;
    }
	
    Mat mat_src = CAdapter::mimg2Mat(src);
    //调用实验代码
    Mat dst;
    cvtColor(mat_src,dst,COLOR_RGB2GRAY);
    MImage* mimg = CAdapter::Mat2mimg(dst);

    return mimg;
}

//二值化（图像为单通道,每个通道为1个字节,即常见灰度格式）
//参数:
//[in] src: 源图像
//[in] nThreshold: 二值化阈值（0-255）
//返回值:
//MImage对象指针,如果失败则为0
MImage* mcvThreshold(MImage* src, int nThreshold)
{
    if(!g_init){
        return 0;
    }

    Mat matSrc = CAdapter::mimg2Mat(src);
    Mat gray = CAdapter::GrayStyle(matSrc);
    cv::threshold(gray,gray,nThreshold,255,0);
    MImage* mimg = CAdapter::Mat2mimg(gray);
    return mimg;
}

//自适应最佳阈值二值化（图像为单通道,每个通道为1个字节,即常见灰度格式）
//参数:
//[in] src: 源图像
//返回值:
//MImage对象指针,如果失败则为0
MImage* mcvAdaptiveThreshold(MImage* src)
{
    if(!g_init){
        return 0;
    }

    Mat matSrc = CAdapter::mimg2Mat(src);
    //gray style
    Mat gray;
    if(matSrc.channels() == 3)
    {
        cvtColor(matSrc, gray, CV_BGR2GRAY);
    }
    else
    {
        gray = matSrc.clone();
    }
    gray = CAdaptiveThreshold::AdaptiveThreshold(gray);

    MImage* mimg = CAdapter::Mat2mimg(gray);

    return mimg;
}

//自适应最佳阈值二值化(二值化前采用Log)（图像为单通道,每个通道为1个字节,即常见灰度格式）
//参数: 
//[in] src: 源图像
//返回值:
//MImage对象指针,如果失败则为0
MImage* mcvAdaptiveThreshold2(MImage* src)
{
	Mat matSrc = CAdapter::mimg2Mat(src);
	Mat gray = CAdapter::GrayStyle(matSrc);
	cv::adaptiveThreshold(gray,gray,255,CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY,65,0);
	MImage* mimg = CAdapter::Mat2mimg(gray);
	return mimg;
}

//Bernsen阈值二值化（图像为单通道,每个通道为1个字节,即常见灰度格式）,采用改进Bernsen算法,常用于光线不好,导致图像边缘模糊的情况
//参数: 
//[in] src: 源图像
//返回值:
//MImage对象指针,如果失败则为0
MImage* mcvBernsenThreshold(MImage* src)
{
	Mat mat_src = CAdapter::mimg2Mat(src);
	Mat mat_dst ;
	//调用功能代码
	GaussianBlur(mat_src,mat_src,Size(3,3),0,0);
	CAdaptiveThreshold::Bernsen(mat_src,mat_dst,4,0.2);

	MImage* mimg = CAdapter::Mat2mimg(mat_dst);
	return mimg;
}

//自适应最佳阈值二值化（图像为单通道,每个通道为1个字节,即常见灰度格式）
//参数: 
//[in] src: 源图像
//[in] nMethod	:	求阈值方式
//			0	:	采用后面4种方式加权值（对于颜色较淡的图像有奇效）【默认】
//			1	:	Ostu 方法【XZhang 01】【Xia 99】
//			2	:	Kittler 和 Illingworth 方法【Kittler and Illingworth 86】
//			3	:	最大熵
//			4	:	位差
//[in]nBoxSize	:	框口大小【默认64】
//返回值:
//MImage对象指针,如果失败则为0
MImage* mcvAdaptiveThresholdEx(MImage* src,int nMethod,int nBoxSize)
{
	Mat matSrc = CAdapter::mimg2Mat(src);
	Mat gray = CAdapter::GrayStyle(matSrc);
	
	CAdaptiveThreshold::AdaptiveThreshold(gray,nMethod,nBoxSize);
	MImage* mimg = CAdapter::Mat2mimg(gray);

	return mimg;
}

//最佳阈值二值化（图像为单通道,每个通道为1个字节,即常见灰度格式）
//参数: 
//[in] src: 源图像
//返回值:
//MImage对象指针,如果失败则为0
MImage* mcvOptimalThreshold(MImage* src)
{
	Mat matSrc = CAdapter::mimg2Mat(src);
	//gray style
	Mat gray;
	if(matSrc.channels() == 3)
	{
		cvtColor(matSrc, gray, CV_BGR2GRAY); 
	}
	else
	{
		gray = matSrc.clone();
	}
	int threshold = CAdaptiveThreshold::OptimalThreshold(gray);
	cv::threshold(gray,gray,threshold,255,CV_THRESH_BINARY);
	MImage* mimg = CAdapter::Mat2mimg(gray);
	return mimg;
}

//自适应梯度二值化算法
//[in]src				输入图像
//[out]dst				输出图像
MImage* mcvGradientAdaptiveThreshold(MImage *src,float threshold)
{

    return src;
}

//直方图自适应二值化算法
//[in]src				输入图像
//[out]dst				输出图像
MImage* mcvHistAdaptiveThreshold(MImage *src)
{
    return src;
}

//从源图像中检测出矩形目标（如文档）,俗称裁切纠偏,默认背景为黑色,目标边缘不为黑色,俗称裁切纠偏,算法基于轮廓检测,返回检测出的矩形区域其倾斜角度和4个顶点
//参数:
//[in] src: 源图像
//[out] fAngle: 倾斜偏移角度,单位为°
//[out] ptMPoint: 检测矩形区域的4个顶点
//[in] rectROI: 关注区域,当其不为0时,只对区域内部分进行计算
//返回值:
//true: 成功
//false: 失败
bool mcvDetectRect(MImage *src,float &fAngle, MPoint *ptMPoint, MRect rect)
{
    if(!g_init){
        return false;
    }

    Mat matSrc = CAdapter::mimg2Mat(src);
    if(matSrc.data == 0)
    {
        return false;
    }

    Point pt[4];
    Rect cvR =Rect(rect.m_nLeft,rect.m_nTop,rect.m_nRight - rect.m_nLeft, rect.m_nBotton - rect.m_nTop);
    //bool bresult = CDetectRectByContours::DetectRect(matSrc,fAngle,pt,cvR);
    bool bresult = CDetectRectByContours_new::DetectRect(matSrc,fAngle,pt,cvR);

    if(bresult == true)
    {
        for(int n =0; n < 4; n++)
        {
            ptMPoint[n].x=pt[n].x;
            ptMPoint[n].y=pt[n].y;
        }
        return true;
    }
    return false;
}

//从源图像中检测出矩形目标（如文档）,俗称裁切纠偏,默认背景为黑色,目标边缘不为黑色,俗称裁切纠偏,算法基于轮廓检测,返回检测出的矩形区域其倾斜角度和4个顶点
//参数: 
//[in] src: 源图像
//[out] fAngle: 倾斜偏移角度,单位为°
//[out] ptMPoint: 检测矩形区域的4个顶点
//[in] rectROI: 关注区域,当其不为0时,只对区域内部分进行计算
//返回值:
//true: 成功
//false: 失败
bool mcvDetectRect_scanner(MImage *src,float &fAngle, MPoint *ptMPoint, MRect rect)
{
	Mat matSrc = CAdapter::mimg2Mat(src);
	if(matSrc.data == 0)
	{
		return false;
	}
	//IplImage* iplSrc = CAdapter::Mat2Ipl(matSrc);
#if 0 //old version
	IplImage *iplSrc = &(IplImage)matSrc;
	CvPoint pt[4];
	CvRect cvR = cvRect(rect.m_nLeft,rect.m_nTop,rect.m_nRight - rect.m_nLeft, rect.m_nBotton - rect.m_nTop);
	bool bresult = CDetectRectByContours::DetectRect(iplSrc,fAngle,pt,cvR);
#else
	Point pt[4];
	Rect cvR =Rect(rect.m_nLeft,rect.m_nTop,rect.m_nRight - rect.m_nLeft, rect.m_nBotton - rect.m_nTop);
	//bool bresult = CDetectRectByContours::DetectRect(matSrc,fAngle,pt,cvR);
	bool bresult = CDetectRectByContours_new::DetectRect_scanner(matSrc, fAngle, pt, cvR );
#endif
	if(bresult)
	{
		for(int n =0; n < 4; n++)
		{
			ptMPoint[n].x=pt[n].x;
			ptMPoint[n].y=pt[n].y;
		}
		return true;
	}
	return false;
}

MRectRArray mcvDetectRect1(MImage* src1, MRect rect )
{
    MRectRArray mRectArray;
	
    if(!g_init){
        return mRectArray;
    }

    const int halfBorder = 20;
    Mat matSrc = CAdapter::mimg2Mat(src1);
    if(matSrc.data == 0)
    {
        return mRectArray;
    }

    MRectR rrect;
    bool bresult =false;

    Point pt[4];
    Rect cvR = Rect(rect.m_nLeft,rect.m_nTop,rect.m_nRight - rect.m_nLeft, rect.m_nBotton - rect.m_nTop);

    //bresult = CDetectRectByContours::DetectRect(matSrc,rrect.m_fAngle,pt,cvR);
    bresult = CDetectRectByContours_new::DetectRect(matSrc,rrect.m_fAngle,pt,cvR);

    for (int idx=0;idx <4;idx++)
    {
        rrect.m_pt[idx].x = pt[idx].x;
        rrect.m_pt[idx].y = pt[idx].y;
    }

    if(bresult == true)
    {
        mRectArray.m_nCount =1;
        mRectArray.m_rectR[0]=rrect;
    }
    return mRectArray;
}

//从源图像中检测出多个矩形目标（如文档）,俗称多图裁切,默认背景为黑色,目标边缘不为黑色,俗称裁切纠偏,算法基于轮廓检测,返回检测出的矩形区域其倾斜角度和4个顶点
//参数:
//[in] src: 源图像
//[in] rectROI: 关注区域,当其不为0时,只对区域内部分进行计算
//返回值:
//检测出的矩形区域列表,为0时可能意味失败,具体见MRectRArray定义
MRectRArray mcvDetectRectMulti(MImage* src1, MRect rect)
{
    MRectRArray mRectArray;

    if(!g_init){
        return mRectArray;
    }

    const int halfBorder = 20;
    Mat matSrc = CAdapter::mimg2Mat(src1);
    if(matSrc.data == 0)
    {
        return mRectArray;
    }
    //IplImage* ipl = CAdapter::Mat2Ipl(matSrc);
    //IplImage *ipl = &(IplImage)matSrc;
    IplImage iplTmp = (IplImage)matSrc;
    IplImage *ipl = &iplTmp;
    Mat matReSize = CAdapter::CreateMat(cvGetSize(ipl).width+halfBorder*2,cvGetSize(ipl).height+halfBorder*2,ipl->nChannels);
    //IplImage *iplReSize = CAdapter::Mat2Ipl(matReSize);
    //IplImage *iplReSize = &(IplImage)matReSize;
    IplImage iplReSizeTmp = (IplImage)matReSize;
    IplImage *iplReSize = &iplReSizeTmp;
	CvPoint pt;
	pt.x = halfBorder;
	pt.y = halfBorder;
    cvCopyMakeBorder(ipl,iplReSize,pt,IPL_BORDER_REPLICATE,cvScalarAll(0));

    int nMinW =  640;
    int nMaxW =  4160;

    int width = iplReSize->width;
    float scale = 0.5;
    if (width > nMinW && width <= nMaxW)
    {
        scale = 0.5;
    }
    else if (width < nMinW)
    {
        scale = 1.0;
    }
    else
    {
        scale = (float)nMinW / width;
    }
    //scale = (float)320 / width;

    CCropRectByCV cropRect;
    if(rect.m_nBotton != 0 && rect.m_nRight != 0)
    {
        cropRect.SetEffectRect(cvRect(
            rect.m_nLeft,
            rect.m_nTop,
            rect.m_nRight - rect.m_nLeft,
            rect.m_nBotton - rect.m_nTop));
    }
    else
    {
        cropRect.SetEffectRect(cvRect(
            0,
            0,
            iplReSize->width,
            iplReSize->height));
    }

    cropRect.Process(iplReSize,scale);
    CvPoint * * points = new CvPoint *();
    float * * angles = new float *();
    int boxCount = 0;
    cropRect.GetBoxes(points,angles,boxCount);


    for (int i = 0; i < boxCount ; i ++)
    {
        MRectR box;
        box.m_fAngle = (*angles)[i];
        for (int j = 0 ;j < 4; j++)
        {
            box.m_pt[j].x = (* points)[i * 4 + j].x-halfBorder;
            box.m_pt[j].y = (* points)[i * 4 + j].y-halfBorder;
        }
        mRectArray.m_rectR[i] = box;
        mRectArray.m_nCount ++;
    }
    delete angles;
    delete points;

    return mRectArray;
}

//新版多图裁切（支持扫描仪）
//参数:
//[in] src: 源图像
//[in] rectROI: 关注区域,当其不为0时,只对区域内部分进行计算
//返回值:
//检测出的矩形区域列表,为0时可能意味失败,具体见MRectRArray定义
MRectRArray mcvDetectRectMultiForScanner(MImage *src1,MRect rect)
{
    MRectRArray mrrect;

    if(!g_init){
        return mrrect;
    }

    Mat src = CAdapter::mimg2Mat(src1);
    //Mat src;
    if(src1 != 0)
    {
        src = CMImageDataPool::Access(src1->dataIndex);
    }

    vector<cv::RotatedRect> vc = CDetectRectBySegmation::DoDetectRect(src);

    int idr=0;
    for (vector<cv::RotatedRect>::iterator itr = vc.begin();itr!=vc.end();itr++,idr++)
    {
        RotatedRect rrect = *itr;
        MRectR box;
        box.m_fAngle = rrect.angle;
        Point2f pt4[4];
        rrect.points(pt4);

        for(int idx =0;idx<4;idx++)
            {
                box.m_pt[idx].x = pt4[idx].x;
                box.m_pt[idx].y = pt4[idx].y;
            }
        mrrect.m_rectR[idr] = box;
        mrrect.m_nCount ++;
    }
    return mrrect;
}

MRectRArray mcvDetectRectMultiForScannerBeta(MImage *src1,MRect rect)
{
    MRectRArray mrrect;

    if(!g_init){
        return mrrect;
    }

    Mat src = CAdapter::mimg2Mat(src1);
    //Mat src;
    if(src1 != 0)
    {
        src = CMImageDataPool::Access(src1->dataIndex);
    }

    vector<cv::RotatedRect> vc = CDetectRectBySegmation::DoDetectRect2(src);

    int idr=0;
    for (vector<cv::RotatedRect>::iterator itr = vc.begin();itr!=vc.end();itr++,idr++)
    {
        RotatedRect rrect = *itr;
        MRectR box;
        box.m_fAngle = rrect.angle;
        Point2f pt4[4];
        rrect.points(pt4);

        for(int idx =0;idx<4;idx++)
        {
            box.m_pt[idx].x = pt4[idx].x;
            box.m_pt[idx].y = pt4[idx].y;
        }
        mrrect.m_rectR[idr] = box;
        mrrect.m_nCount ++;
    }
    return mrrect;
}

//尺寸重定义
//参数:
//[in] src: 源图像
//[in] nType: 插值方式,具体取值如下
//     0: 最近邻插值；
//     1: 线性插值；
//     2: 区域插值；
//     3: 三次样条插值；
//     4: Lanczos插值
//[in] nWidth: 输出图像宽度
//[in] nHeight: 输出图像高度
//[in] fRadio: 输出图像宽大或缩小比例
//返回值:
//MImage对象指针,如果失败则为0
MImage* mcvResize(MImage *src,int nWidth,int nHeight,float fRadio,int nType)
{
    if(!g_init){
        return 0;
    }

    Mat mat_src = CAdapter::mimg2Mat(src);
    Mat dst;
    if(nWidth!=0&&nHeight!=0)
    {
        if(!mat_src.empty())
        {
            resize(mat_src,dst,Size( nWidth,nHeight),(0,0),(0,0),nType);
        }
    }
    else
    {
        if(!mat_src.empty())
        {
            resize(mat_src,dst,Size(0,0),fRadio,fRadio,nType);
        }
    }
    MImage* mimg = CAdapter::Mat2mimg(dst);

    return mimg;
}

//旋转
//参数:
//[in] src: 源图像
//[in] fAngle: 旋转角度,单位为°
//返回值:
//MImage对象指针,如果失败则为0
MImage* mcvRotateImage(MImage* src, float fAngle)
{
    if(!g_init){
        return 0;
    }

    Mat matSrc = CAdapter::mimg2Mat(src);
    Mat matDst= Rotate::RotateImage2(matSrc,-fAngle);
    MImage* mimg = CAdapter::Mat2mimg(matDst);
    return mimg;
}

//旋转(保持尺寸)
//参数:
//[in] src: 源图像
//[in] fAngle: 旋转角度,单位为°
//返回值:
//MImage对象指针,如果失败则为0
MImage* mcvRotateImageKeepSize(MImage* src, float fAngle)
{
    if(!g_init){
        return 0;
    }

    Mat mat_src =CAdapter::mimg2Mat(src);
    //旋转中心点
    Point2f p2f;
    p2f.x =(float)mat_src.cols/2;
    p2f.y =(float)mat_src.rows/2;
    //仿射变换矩阵
    double map[6];
    Mat map_matrix =Mat(2,3,CV_32F,map);
    map_matrix = getRotationMatrix2D(p2f,-fAngle,1.0);
    //仿射变换
    warpAffine(mat_src,mat_src,map_matrix,Size(mat_src.cols,mat_src.rows),INTER_CUBIC,BORDER_REFLECT,Scalar(255,255,255));
    MImage *mimg = CAdapter::Mat2mimg(mat_src);
    return mimg;
}

//逆时针,旋转90°
//参数:
//[in] src: 源图像
//[in] fAngle: 旋转角度,单位为°
//返回值:
//MImage对象指针,如果失败则为0
MImage* mcvRotateImageLeft(MImage* src)
{

    if(!g_init){
        return 0;
    }

    Mat mat_src = CAdapter::mimg2Mat(src);
    Mat mat_transpose;
    transpose(mat_src,mat_transpose);
    Mat mat_flip;
    flip(mat_transpose,mat_flip,1);
    MImage* mimg = CAdapter::Mat2mimg(mat_flip);
    return mimg;
}

//图像合并
//参数:
//Img1			:	输入图像1
//Img2			：  输入图像2
//bVertical		:	是否垂直
//bFrame		:	是否对每个图像在合并后加边框
MImage* mcvMergerImage(MImage* Img1,MImage* Img2,bool bVertical /* = true */, bool bFrame /* = false */)
{
    if(!g_init){
        return 0;
    }

    //转换图像类型
    Mat matSrc1 = CAdapter::mimg2Mat(Img1);
    Mat matSrc2 = CAdapter::mimg2Mat(Img2);

    //IplImage *ipl_src1 = &(IplImage)matSrc1;
    //IplImage *ipl_src2 = &(IplImage)matSrc2;

    IplImage ipl_src1Tmp = (IplImage)matSrc1;
    IplImage *ipl_src1 = &ipl_src1Tmp;

    IplImage ipl_src2Tmp = (IplImage)matSrc2;
    IplImage *ipl_src2 = &ipl_src2Tmp;

    IplImage *ipl_dst = CMergerImage::MergerImg(ipl_src1,ipl_src2,bVertical,bFrame);
    Mat matDst = CAdapter::Ipl2Mat(ipl_dst);
    MImage *dst =CAdapter::Mat2mimg(matDst);
    cvReleaseImage(&ipl_dst);
    return dst;
}


//图像裁切
//参数:
//[in] src: 源图像
//[in] mRect: 表示包含裁切图像的左右上下位置四个参数的矩阵
//返回值:
//MImage对象指针,如果失败则为0
MImage* mcvCut(MImage* src,MRect mrect)
{
    if(!g_init){
        return 0;
    }

    Mat matSrc = CAdapter::mimg2Mat(src);

    CvRect rect;
    rect.x=mrect.m_nLeft;
    rect.width=mrect.m_nRight-rect.x;
    rect.width =(rect.x +rect.width) > src->width ? (src->width -rect.x):rect.width;
    rect.y=mrect.m_nTop;
    rect.height=mrect.m_nBotton-rect.y;
    rect.height =(rect.y+ rect.height) > src->height ?(src->height -rect.y):rect.height;
    //创建Mat
    if(rect.width <0 ||rect.height <0)
    {
        rect.x =0;
        rect.y =0;
        rect.width = src->width;
        rect.height = src->height;
    }

    Mat matDst = CAdapter::CreateMat(rect.width,rect.height,matSrc.channels());

    matSrc(rect).copyTo(matDst);
    //将Mat放入Map
    MImage* mimg = CAdapter::Mat2mimg(matDst);
    return mimg;
}

//图像裁切(任意角度)
//参数:
//[in] src: 源图像
//[in] mRect: 表示包含裁切图像的左右上下位置四个参数的矩阵
//返回值:
//MImage对象指针,如果失败则为0
MImage* mcvCutR(MImage* src,MRectR mrect)
{
    if(!g_init){
        return 0;
    }

    Mat matSrc;
    if(src != 0)
    {
        matSrc = CMImageDataPool::Access(src->dataIndex);
    }
    if(matSrc.data == 0)
    {
        return 0;
    }

    int nMaxX = 0;
    int nMaxY = 0;
    int nMinX = 65535;
    int nMinY = 65535;
    for(int n = 0; n < 4; n ++)
    {
        if(nMaxX < mrect.m_pt[n].x)
        {
            nMaxX = mrect.m_pt[n].x;
        }
        if(nMaxY < mrect.m_pt[n].y)
        {
            nMaxY = mrect.m_pt[n].y;
        }
        if(nMinX > mrect.m_pt[n].x)
        {
            nMinX = mrect.m_pt[n].x;
        }
        if(nMinY > mrect.m_pt[n].y)
        {
            nMinY = mrect.m_pt[n].y;
        }
    }//end for

    //解决BUG当图像角度为0°时调用DetectRect裁切纠偏后尺寸变小
    if(mrect.m_fAngle == 0 && nMaxX-nMinX == src->width && nMaxY-nMinY == src->height && mrect.m_pt[1].x == mrect.m_pt[2].x)
    {
        MImage* mimg = CAdapter::Mat2mimg(matSrc);
        return mimg;
    }

    CvPoint pt[4];
    for(int n = 0; n < 4; n ++)
    {
        pt[n].x = mrect.m_pt[n].x;
        pt[n].y = mrect.m_pt[n].y;
    }


    Mat matDst = Rotate::RotateCut(matSrc,pt);

    MImage* mimg = NULL;
    mimg = CAdapter::Mat2mimg(matDst);
    return mimg;
}

MImage* mcvCutR2(MImage* src,MRectR *mrect0)
{
    if(!g_init){
        return 0;
    }

    MRectR mrect = *mrect0;
    Mat matSrc = CAdapter::mimg2Mat(src);

    //IplImage *iplSrc = CAdapter::Mat2Ipl(matSrc);
    //IplImage *iplSrc = &(IplImage)matSrc;
    IplImage iplSrcTmp = (IplImage)matSrc;
    IplImage *iplSrc = &iplSrcTmp;

    int nMaxX = 0;
    int nMaxY = 0;
    int nMinX = 65535;
    int nMinY = 65535;
    for(int n = 0; n < 4; n ++)
    {
        if(nMaxX < mrect.m_pt[n].x)
        {
            nMaxX = mrect.m_pt[n].x;
        }
        if(nMaxY < mrect.m_pt[n].y)
        {
            nMaxY = mrect.m_pt[n].y;
        }
        if(nMinX > mrect.m_pt[n].x)
        {
            nMinX = mrect.m_pt[n].x;
        }
        if(nMinY > mrect.m_pt[n].y)
        {
            nMinY = mrect.m_pt[n].y;
        }
    }//end for

    //解决BUG当图像角度为0°时调用DetectRect裁切纠偏后尺寸变小
    if(mrect.m_fAngle == 0 && nMaxX-nMinX == src->width && nMaxY-nMinY == src->height)
    {
        Mat matDst = matSrc.clone();
        MImage* mimg = CAdapter::Mat2mimg(matDst);
        return mimg;
    }

    CvPoint pt[4];
    for(int n = 0; n < 4; n ++)
    {
        pt[n].x = mrect.m_pt[n].x;
        pt[n].y = mrect.m_pt[n].y;
    }

    Mat matDst = Rotate::RotateCut(matSrc,pt);

    MImage* mimg = CAdapter::Mat2mimg(matDst);
    return mimg;
}

//文字增强，背景平滑/滤除，自动色阶 V0.0.1
//[in]src			输入图像
//[out]dst			输出图像
//[in]nType
//	1				文字增强
//	2				背景平滑
//	3				背景滤除
//	4				自动色阶
//注：出现其它数字默认转换为0,不做处理,四个数字可以任意组合
//[in]maxValue		亮度最大值
//[in]minValue		亮度最小值
//--- maxValue,minValue--- 有负值则参数失效
//[in]fGmax			最大增益(0.3,1.5]
//[in]fGmin			最小增益[0,0.3]
//--- fGmax,fGmin---为1则对应的参数失效
MImage* mcvImageStretch(MImage *src,int nType/* =0 */,int maxValue /* =255 */,int minValue /* =0 */,float fGmax /* =1 */,float fGmin/* =1 */)
{
    if(!g_init){
        return 0;
    }

    Mat mat_src = CAdapter::mimg2Mat(src);
    Mat dst;
    CImageEnhancement::imageStretch(mat_src,dst,nType,maxValue,minValue,fGmax,fGmin);
    MImage *Mdst = CAdapter::Mat2mimg(dst);
    return Mdst;
}

//背景色平滑
//参数：
//[in/out]src					输入/输出图像
//[in]isBackgroundColorWhite	是否为背景色平滑(true:平滑,false:白色)
//返回值：
//true成功，false失败
bool mcvBackGroundSmooth(MImage *src,bool isBackgroundColorSmooth /* =true */)
{
    if(!g_init){
        return false;
    }

    Mat mat_src =CAdapter::mimg2Mat(src);
    bool bre = CAutoLevel::backGroundColorProcessing(mat_src,mat_src,isBackgroundColorSmooth);

    return bre;
}


//检测空白页
//参数：
//[in]src				输入图像
//[in]detectThreshold	判断阈值
//返回值：
//true成功，false失败
bool mcvDetectBlankPage(MImage *src,int detectThreshold )
{
    if(!g_init){
        return false;
    }

    Mat mat_src = CAdapter::mimg2Mat(src);
    return CDetectionWhitePage::isWhitePage_new(mat_src,detectThreshold);
}

//图像对比度亮度
//参数：
//[in/out]src			输入/输出图像
//[in]dContrast			对比度,取值范围[0,200],100不做变化
//[in]nLuminance		亮度,取值范围[0,200],100不做变化
//返回值：
//true: 成功
//false: 失败
bool mcvLuminanceContrast(MImage *src,double dContrast,int nLuminance)
{
    if(!g_init){
        return false;
    }

    if(dContrast == 100 && nLuminance == 100)
    {
        //默认值不做任何处理
        return true;
    }

    Mat mat_src =CAdapter::mimg2Mat(src);

    mat_src.convertTo(mat_src,mat_src.type(),dContrast/100.0,nLuminance-100);

    return true;
}

//色调饱和度
//参数：
//[in] src :输入图像
//[out] dst：输出图像
//[in] color：0—彩色;1—R;2—G;3—B,默认值0
//[in] hue：色彩0~360,默认值180
//[in] saturation：饱和度 0~200,默认值100
//[in] brightness：亮度 0~200,默认值100
//返回值：
//MImage*:调整后图像
MImage * mcvImgAdjust(MImage *src,int color, int hue, int saturation, int brightness)
{
    if(!g_init){
        return 0;
    }

    Mat mat_src = CAdapter::mimg2Mat(src);
    Mat mat_dst =ImgAdjust::HslAdjust(mat_src,color,hue,saturation,brightness);
    MImage *dst = CAdapter::Mat2mimg(mat_dst);
    return dst;
}

//PS效果滤镜
//参数:
//[in] src: 源图像
//[in] nType: 滤镜类型
//     0: 原色(Original)
//     1: 灰色(Gray)
//     2: 黑白(Black & White)
//     3: 油画(Painterly)
//     4: 怀旧(Nostalgic)
//     5: 素描(Sketch)
//     6: 边缘照亮(Glowing Edge)
//     7: 蓝冷(Cold Blue)
//     8: 马赛克(Mosaic)
//     9: 模糊(Blurry)
//     10: 负片(Negative)
//返回值:
//MImage对象指针,如果失败则为0
MImage* mcvPSEffect(MImage* src, int nType)
{
    if(!g_init){
        return 0;
    }

    Mat mat_src = CAdapter::mimg2Mat(src);
    Mat mat_dst(mat_src);
    int n = CAdaptiveThreshold::OptimalThreshold(mat_src);
    switch(nType)
    {
    case 1:
        //灰色(Gray)
        if(mat_src.channels() == 3)
            cvtColor(mat_src, mat_dst, CV_BGR2GRAY);
        else
            mat_dst = mat_src.clone();
        break;
    case 2:
        //黑白(Black & White)
        mat_dst = CAdaptiveThreshold::AdaptiveThreshold(mat_src);
        break;
    case 3:
        //油画(Painterly)
        mat_dst = CFilterEffect::painterly2(mat_src);
        //mat_dst = CFilterEffect::fragmentEffect(mat_src);
        break;
    case 4:
        //怀旧(Nostalgic)nostalgic
        mat_dst = CFilterEffect::nostalgic(mat_src);
        break;
    case 5:
        //素描(Sketch)
        mat_dst = CFilterEffect::sketchEffect(mat_src);
        break;
    case 6:
        //边缘照亮(Glowing Edge)
        mat_dst = CFilterEffect::glowingEdgeEffect(mat_src);
        break;
    case 7:
        //蓝冷(Cold Blue)
        mat_dst = CFilterEffect::shade2Effect(mat_src);
        break;
    case 8:
        //马赛克(Mosaic)
        mat_dst = CFilterEffect::mosaicEffect(mat_src);
        break;
    case 9:
        //模糊(Blurry)
        mat_dst = CFilterEffect::fragmentEffect(mat_src);
        break;
    case 10:
        //负片(Negative)
        cv::bitwise_not(mat_src,mat_dst);
        break;
    }

    MImage* dst = CAdapter::Mat2mimg(mat_dst);
    return dst;
}

//针对图像固定位置PS效果滤镜
//参数:
//[in/out] src: 源图像（也是目标图像）,会对原图进行处理
//[in] nType: 滤镜类型
//     0: 原色(Original)
//     1: 灰色(Gray)
//     2: 黑白(Black & White)
//     3: 油画(Painterly)
//     4: 怀旧(Nostalgic)
//     5: 素描(Sketch)
//     6: 边缘照亮(Glowing Edge)
//     7: 蓝冷(Cold Blue)
//     8: 马赛克(Mosaic)
//     9: 模糊(Blurry)
//     10: 负片(Negative)
//[in] rectROI: 指定位置，只能为0度矩形区域，不能为空，不能宽度/高度为0，不能超出图像范围
//返回值:true—成功，反之亦然
bool mcvPSEffectEx(MImage* src, int nType, MRect rectROI)
{
    if(!g_init){
        return false;
    }

    //裁切出需要处理的图像
    MImage* mImgCut = mcvCut(src,rectROI);
    if(mImgCut == NULL)
    {
        return false;
    }

    //对目前区域进行处理
    MImage* mImgPS = mcvPSEffect(mImgCut, nType);
    mcvReleaseImage(&mImgCut);
    if(mImgPS == NULL)
    {
        return false;
    }

    //对原图处理
    bool bResult = mcvCopyToRect(src, mImgPS, MPoint(rectROI.m_nLeft,rectROI.m_nTop));
    mcvReleaseImage(&mImgPS);
    return bResult;
}

//针对图像多个固定位置PS效果滤镜（主要是马赛克）
//参数:
//[in/out] src: 源图像（也是目标图像）,会对原图进行处理
//[in] nType: 滤镜类型
//     0: 原色(Original)
//     1: 灰色(Gray)
//     2: 黑白(Black & White)
//     3: 油画(Painterly)
//     4: 怀旧(Nostalgic)
//     5: 素描(Sketch)
//     6: 边缘照亮(Glowing Edge)
//     7: 蓝冷(Cold Blue)
//     8: 马赛克(Mosaic)
//     9: 模糊(Blurry)
//     10: 负片(Negative)
//[in]arrRectROI:指定区域列表，本身不能为NULL
//               每一个区域只能为0度矩形区域，不能为空，不能宽度/高度为0，不能超出图像范围
//[in] arrLen: 区域数量
//返回值:true—成功，反之亦然
bool mcvPSEffectEx2(MImage* src, int nType, MRect* arrRectROI, int arrLen)
{
    if(!g_init){
        return false;
    }

    if(arrRectROI == NULL)
    {
        return false;
    }
    for(int n = 0; n < arrLen; n++)
    {
        MRect rectROI = arrRectROI[n];

        if(!mcvPSEffectEx(src, nType, rectROI))
        {
            return false;
        }
    }

    return true;
}

//去噪
//参数:
//[in] src: 源图像
//[in] nMethod: 去噪方式,参数意义如下:
//     0: 中值滤波
//     1: 3*3高斯滤波
//     2: 5*5双边滤波
//     3: 7*7均值滤波
//	   4: 双边滤波
//返回值:
//true: 成功
//false: 失败
bool mcvNoise(MImage *src,int nMethod)
{
    if(!g_init){
        return false;
    }

    Mat mat_src = CAdapter::mimg2Mat(src);
    if(!mat_src.empty())
    {
        Mat mat_src = CAdapter::mimg2Mat(src);
        CNoise::Noise(mat_src,nMethod);
        return true;
    }
    return false;
}

//去噪
//参数:
//[in]src:	输入图像
//[out]dst:	输出图像
//[in]reps: 去噪迭代次数，常常一次去噪操作小孩不理想时可以考虑多次
//[in]kernel: 去噪尺度，必须为奇数，建议3，5，7（值越大针对的噪点尺寸就越大，但是对图像的破化就越大）
//返回值:
//true: 成功
//false: 失败
bool mcvRemoveNoise(MImage* src,int reps,int kernel)
{
	Mat mat_src = CAdapter::mimg2Mat(src);
	CRemoveNoise::removeNoise(mat_src,mat_src,reps,kernel);
	return true;
}

//二值图去杂点
//参数：
//[in/out]src		输入/输出图像
//[in]nMethod		程度(非以下情况按low处理)
//	0				low（默认）
//	1				medium
//	2				high
//返回值：
//	0				正常运行
//	1				不支持的图像尺寸
//	2				不支持的图像类型
//  -1				未能正常结束
int mcvDeletePointInBW(MImage *src,int nMethod)
{
	Mat mat_src = CAdapter::mimg2Mat(src);
	int renFlag =CDeleteNoise_BW::DeNoise(mat_src,nMethod);

	return renFlag;
}

//图像去网纹[仅支持600DPI，不支持类型选择]
//[in/out]		:	输入/输出图像
//[in]nType		:	类型
//	0			:	原图
//	1			:	杂志
//	2			:	报纸
//	3			:	印刷
bool mcvDeScreen(MImage *src,int nType)
{
    if(!g_init){
        return false;
    }

    Mat mat_src = CAdapter::mimg2Mat(src);
    bool bre = CImageEnhancement::DeScreen(mat_src,7,0,0);
    //bool bre = CImageEnhancement::DeScreen(mat_src,nType);
    return bre;
}

//Gama
//[in/out]src		输入/输出图像
//[in]fGama			gama值
bool mcvGamaCorrection(MImage *src,float fGama)
{
    if(!g_init){
        return false;
    }

    Mat mat_src =CAdapter::mimg2Mat(src);
    bool bre = CImageEnhancement::gamaCorrection(mat_src,fGama);

    return bre;
}

//锐化
//参数：
//[in] src :输入图像
//[in] color：0—彩色； 1—R；2—G;3—B;
//[in] hue：色彩0~360
//[in] saturation：饱和度 0~200
//[in] brightness：亮度 0~200
//返回值：
//[out]  dst：输出图像
MImage* mcvUnsharp(MImage *src,float sigma,int amount,int thresholds)
{
    if(!g_init){
        return 0;
    }

    Mat mat_src = CAdapter::mimg2Mat(src);
    Mat mat_dst =ImgAdjust::Unsharp(mat_src,sigma,amount,thresholds);
    MImage *dst = CAdapter::Mat2mimg(mat_dst);
    return dst;
}

//自动调整色阶
//参数: 
//[in] src: 源图像
//返回值:
//MImage对象指针,如果失败则为0
MImage* mcvAdjustLevelAuto(MImage* src)
{
	Mat mat_src = CAdapter::mimg2Mat(src);
	Mat mat_dst;

	bool bRe = CAutoLevel::AdjustLevelAuto(mat_src,mat_dst);

	MImage* mimg = NULL;
	if(bRe == true)
	{
		mimg = CAdapter::Mat2mimg(mat_dst);
	}

	return mimg;

}

//自动色阶
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
MImage* mcvAdjustLevelAutoUnis(MImage *src,int maxValue/* =255 */,int minValue /* =0 */,float fGmax /* =1 */,float fGmin /* =1 */)
{
    if(!g_init){
        return 0;
    }

    Mat mat_src = CAdapter::mimg2Mat(src);

    Mat mat_dst;
    CAutoLevel::AdjustLevelAutoUnis(mat_src,mat_dst,maxValue,minValue,fGmax,fGmin);
    MImage *Mdst = CAdapter::Mat2mimg(mat_dst);

    return Mdst;
}

//彩色平衡
//参数：
//[in] imageSource: 输入图像
//[in]  KR :红色分量比例 0~200
//[in] KG 绿色分量 比例  0~200
//[in] KB： 红色分量比例  0~200
//返回值：
//[out]: dst :输出图像
MImage* mcvColorBlance(MImage *src,int KR, int KG,int KB)
{
    if(!g_init){
        return 0;
    }

    Mat mat_src = CAdapter::mimg2Mat(src);
    Mat mat_dst =ImgAdjust::ColorBlance(mat_src,KR,KG,KB);
    MImage *dst = CAdapter::Mat2mimg(mat_dst);

    return dst;
}

//彩色图像增强(保真)
//[in/out]src:	输入图像
//[in]isAutoDetectWhiteBackGroud:	是否自动检测白色背景(白色背景：实现背景增强否则实现颜色保真),参数如下:
// -1 自动检测
// 0 增亮
// 1 保真
//返回值:
//true: 成功
//false: 失败
bool mcvColorEnhance(MImage *src,int isAutoDetectWhiteBackGroud)
{
    if(!g_init){
        return false;
    }

    Mat mat_src = CAdapter::mimg2Mat(src);
    bool bre = CColorEnhance::shadowEnhance(mat_src,mat_src);

    return bre;
}

//图像去色：
//[in/out]src	：	输入/输出图像
//[in]indexOfHue:	输入颜色范围[0,360]
//[in]scopeRadius:	颜色范围半径[0,180]
//注：限定彩色图像，灰色黑白图像没有效果
//颜色大致范围：360°/0°红、60°黄、120°绿、180°青、240°蓝、300°洋红
void mcvColorDropout(MImage *src,int indexOfHue,int scopeRadius)
{
    if(!g_init){
        return;
    }

    Mat mat_src = CAdapter::mimg2Mat(src);
    ImgAdjust::colorDropout(mat_src,indexOfHue,scopeRadius);

    return;
}

//装订孔填充
//参数：
//[in/out]src:			输入/出图像
//[in]isCircularHole	是否为圆形装订孔:(true:圆形,false：方形)
//[in]isColorFill		是否纯色填充(true:纯色,false:背景色)
//[in]nR				红色分量：颜色分量仅在纯色填充下有效
//[in]nG				绿色分量
//[in]nB				蓝色分量
//返回值:
//true: 成功
//false: 失败
bool mcvPunchHold(MImage *src,bool isCircularHole,
    bool isColorFill ,int nR,int nG,int nB)
{
    if(!g_init){
        return false;
    }

    Mat mat_src = CAdapter::mimg2Mat(src);
    bool bre = CPunchHold::punchHold(mat_src,mat_src,isCircularHole,isColorFill,nR,nG,nB);

    return bre;
}

//边缘填充
//适用于裁切纠偏后的图像
//参数:
//[in/out] src: 源图像/目标图像
//[in] offsetNum: 仿制位置离边界的收缩量（默认4）
//[in] mappingScope: 收缩量的跨度范围，已像素为单位，（默认-1），-1：是按照边缘空隙量无限项边缘内伸缩。
//[in] isColorFill: 是否是纯色填充（默认1），参数如下
//	-1				映射填充
//	0				纯色填充(默认白色)
//	1				自动纯色填充
//[in] nR,nG,nB: 纯色填充的颜色（默认白色）
//返回值:
//true: 成功
//false: 失败
bool mcvFillBorder(MImage *src,int offsetNum,int mappingScope, int isColorFill, int nR, int nG, int nB)
{
    if(!g_init){
        return false;
    }

    Mat mat_src = CAdapter::mimg2Mat(src);

    //old
    //bool bre = CFillBorder::FillBorder2(mat_src,mat_src, offsetNum, mappingScope, isColorFill, cv::Scalar(nB,nG,nR));

    bool isScanner = true;
    if(abs(isColorFill) >=10)
    {
        isColorFill %=10;
        isScanner = false;
    }
    if(isColorFill != -1 &&isColorFill !=0 && isColorFill !=1)
        isColorFill = 1;

    float fRatio = (float)offsetNum/4.0;
    if(isColorFill !=0 && isColorFill !=10)
    {
        nG=nB=nR =-1;
    }

    bool isMapfill =false;
    if(-1 == isColorFill)
        isMapfill = true;


    bool bre = CFillBorder::FillBorder3(mat_src,mat_src,fRatio,isScanner,Scalar(nB,nG,nR),isMapfill,mappingScope);

    return bre;
}

//根据内容倾斜校正,俗称文字纠偏（印刷校正）,算法基于直线检测,返回校正后的图像
//参数:
//[in] src: 源图像
//返回值:
//MImage对象指针,如果失败则为0
MImage* mcvAdjustSkew(MImage* src)
{
    if(!g_init){
        return 0;
    }

    Mat srcMat = CAdapter::mimg2Mat(src);
    //IplImage *iplSrc = &(IplImage)srcMat;
    IplImage iplSrcTmp = (IplImage)srcMat;
    IplImage *iplSrc = &iplSrcTmp;
    float fAngle = CAdjustSkew::AdjustSkew(iplSrc);

    Mat dstMat = Rotate::RotateImage(srcMat,fAngle);
    MImage* mimg = CAdapter::Mat2mimg(dstMat);
    return mimg;
}

//图像反色
//参数：
//[in/out]src		输入/输出图像
//返回值:
//true: 成功
//false: 失败
bool mcvInverse(MImage *src)
{
    if(!g_init){
        return false;
    }

    Mat mat_src = CAdapter::mimg2Mat(src);
    mat_src =~mat_src;
    return true;
}

//图像反转.图像镜像
//参数：
//[in/out]src		输入/输出图像
//[in]nMethod		镜像方式
//	0				x-axis（默认）
//	positive		y-axis
//	negative		both axes
bool mcvFlip(MImage *src,int nMethod/* =0 */)
{
    if(!g_init){
        return false;
    }

    Mat mat_src =CAdapter::mimg2Mat(src);
    flip(mat_src,mat_src,nMethod);

    return true;
}

//图像扩展边界
//参数：
//[in/out]src		输入/输出图像
//[in]nBorderLength	扩展边界的长度
//[in]nBorderType	扩展边界的填充方式
//	0				//costant	!< `iiiiii|abcdefgh|iiiiiii`  with some specified `i`
//  1				//replicate	!< `aaaaaa|abcdefgh|hhhhhhh`
//	2				//reflect	!< `fedcba|abcdefgh|hgfedcb`
//	3				//warp		!< `cdefgh|abcdefgh|abcdefg`
//	4				//reflect_101!< `gfedcb|abcdefgh|gfedcba`
//[in]nR			红色分量
//[in]nG			绿色分量
//[in]nB			蓝色分量
MImage* mcvMakeBorder(MImage *src,MRect nBorderLength /* =0 */,int nBorderType /* =0 */,int nR,int nG,int nB)
{
    if(!g_init){
        return 0;
    }

    Mat mat_src =CAdapter::mimg2Mat(src);
    nBorderType = nBorderType < 0 ?0:nBorderType;
    nBorderType = nBorderType > 4 ?4:nBorderType;
    Mat mat_dst;
    copyMakeBorder(mat_src,mat_dst,nBorderLength.m_nTop,nBorderLength.m_nBotton,nBorderLength.m_nLeft,nBorderLength.m_nRight,nBorderType,Scalar(nB,nG,nR));
    MImage *dst =CAdapter::Mat2mimg(mat_dst);
    return dst;
}

//图像转换为RGB24
MImage* mcvRGB24Style(MImage* src)
{
    if(!g_init){
        return 0;
    }

    Mat mat_src = CMImageDataPool::Access(src->dataIndex);
    Mat tmp;
    if(mat_src.channels() == 1)
    {
        cvtColor(mat_src,tmp,CV_GRAY2RGB);

    }
    else
    {
        cvtColor(mat_src,tmp,CV_BGR2RGB);

    }

    MImage* dst = CAdapter::Mat2mimg(tmp);
    return dst;
}

//获取图像data
//[in]src			图像
//[in]idx			图像索引号
//返回图像类型
uchar* mcvGetImageData(MImage *src)
{
    if(!g_init){
        return 0;
    }

    Mat mat_src = CMImageDataPool::Access(src->dataIndex);
    uchar *data = mat_src.ptr<uchar>(0);

    return data;
}

//获取MImage内存存储类型
//[in]src			输入图像
//返回
//	0				map类型存储
//	1				本地存储
int mcvGetMImageMemType(MImage *src)
{
    if(!g_init){
        return -1;
    }

    if(src->dataIndex ==-1 && src->data != NULL)
        return 1;
    else
        return 0;
}

//[in]src_left				输入书本左页图像
//[in]src_right             输入书本右页图像
//[out]mbokkInfo_left       输出左页上下边界轮廓
//[out]mbookInfo_right      输出右页上下边界轮廓
void mcvBookStretch_Getlandamarks(MImage *src_left, MImage *src_right, MBookInfo *mbookInfo_left, MBookInfo *mbookInfo_right)
{

}

void mcvBookStretch_Getlandamarks_double(MImage *src,MBookInfo *mbookInfo_left,MBookInfo *mbookInfo_right)
{

}

MImage* mcvBookStretch_landamarksStretch_left(MImage *src)
{
    return src;
}

MImage* mcvBookStretch_landamarksStretch_right(MImage *src)
{
    return src;
}


MImage* mcvBookStretch_landamarksStretch_double(MImage *src,MBookInfo *mbookInfo_left,MBookInfo *mbookInfo_right,bool type)
{
    return src;
}


//************************************************************************  
// 函数名称:    mcvBookStretch_CurveShow_double    
// 函数说明:    书本上下轮廓关键点显示  
// 函数参数:    MImage *src    输入原图
// 函数参数:    MBookInfo *mbookIfo    输入上下边界关键点
// 返 回 值:    MImage*   输出显示轮廓线及关键点的图像
//************************************************************************ 
MImage* mcvBookStretch_CurveShow_double(MImage *src,MBookInfo *mbookInfo_left,MBookInfo *mbookInfo_right)
{
    return src;
}

MImage* mcvBookStretch_landamarksStretch(MImage *src_left,MImage *src_right,MBookInfo *mbookInfo_left_old,MBookInfo *mbookInfo_left_new,MBookInfo *mbookInfo_right_old,MBookInfo *mbookInfo_right_new,bool type)
{
    return src_left;
}


MImage* mcvBookStretch_CurveShow(MImage *src,MBookInfo *mbookInfo)
{
    return src;
}

MImage* mcvBookStretch_FingerRemoval(MImage *src_rgb,int type)
{
    return  src_rgb;
}
MImage* mcvBookStretch_Enhance(MImage *src_rgb)
{
    return  src_rgb;
}
MImage* mcvBookStretch_inpaint(MImage *src_rgb,int type)
{
    return src_rgb;
}

//去红色
//[in/out]src		输入/输出图像
bool mcvRemoveRED(MImage*src)
{
	Mat dst= CAdapter::mimg2Mat(src);
	if(dst.channels() !=3)
	return false;
	uchar *dataColorKeep = dst.ptr<uchar>(0);
	for (int idr=0;idr<dst.rows;idr++)
	{
		for (int idc=0;idc<dst.cols;idc++,dataColorKeep+=3)
		{
			if((*(dataColorKeep+2)-max(*(dataColorKeep+1),*dataColorKeep)> max((int)(max(*(dataColorKeep+1),*(dataColorKeep))*0.2),(int)3)&& *(dataColorKeep+2) >30 )
			|| (*(dataColorKeep+2)-max(*(dataColorKeep+1),*dataColorKeep) >11 &&*(dataColorKeep+2)>100)
			)
			{
				*dataColorKeep =255;
				*(dataColorKeep+1) =255;
				*(dataColorKeep+2) =255;
			}
		}
	}

	//return CAdapter::CopyMat2mimg(dst,src);

	//CMImageDataPool::Delete((*src)->dataIndex);
	//delete *src;
	//*src =CAdapter::Mat2mimg(dst);
	return true;
}

//文档优化
//参数: 
//[in] src: 源图像(灰度图片）
//[in] preprocessType
// 0 不处理
// 1 log
// 2 root
// 3 white
// 4 smooth
//返回值:
//MImage对象指针,如果失败则为0
bool mcvBrightBalance(MImage *src,int preprocessType)
{
	Mat srcMat = CAdapter::mimg2Mat(src);
	preprocessType = 5;

	if(!srcMat.empty())
	{
		if (preprocessType == 0)
		{
			IplImage temp = (IplImage)srcMat;
			IplImage *iplSrc = &temp;
			CBrightnessBalance::BrightnessBalance(iplSrc);
		}
		else if (3 == preprocessType || 4 == preprocessType)
		{
			CBrightnessBalance::AdaptiveMakeBeautiful(srcMat,64,preprocessType);
			if(3 == preprocessType)
				CAutoLevel::backGroundColorProcessing(srcMat,srcMat);
			else if(4 == preprocessType)
				CAutoLevel::backGroundColorProcessing(srcMat,srcMat,true);

			
		}
		else if (5 == preprocessType)
		{
			if (srcMat.channels()==1)
				srcMat = CnewColorEnhance::black_whiteEnhance(srcMat,true);
			else
			{
				Mat dst = CnewColorEnhance::black_whiteEnhance(srcMat,true);
				cvtColor(dst,srcMat,CV_GRAY2RGB);

			}

		}
		else if (6 == preprocessType)
		{
			//CTextEnhancement::BrightnessBalance3In1(srcMat,10,true);
			Mat temp = CnewColorEnhance::black_whiteEnhance(srcMat);
			vector<Mat> vMat;
			split(srcMat,vMat);
			vMat[2] = CnewColorEnhance::black_whiteEnhance(vMat[2]);
			Mat mask = vMat[2]<80;
			temp.setTo(0,mask);
			vMat[0] = temp;
			vMat[1] = temp;
			
			merge(vMat,srcMat);
		}

		return true;
	}
	return false;
}

//图像增强三合一：红印，蓝印，黑白
//[in]src:输入图像
//[in]nType:
// 0 黑白化
// 1 红印
// 2 蓝印
//[in]isKeepDetail	是否保留细节
//返回值：输出图像
bool mcvEnhancement3In1(MImage *src,int nType,bool isKeepDetail)
{
	Mat mat_src = CAdapter::mimg2Mat(src);
	//return CTextEnhancement::BrightnessBalance3In1(mat_src,nType,isKeepDetail);
	if(nType !=1 &&nType !=2 &&nType !=10 && nType !=20)
		nType =0;
	Mat temp = CnewColorEnhance::black_whiteEnhance(mat_src.clone());
	if (nType==0 || mat_src.channels()!=3)
	{
		mat_src = temp.clone();
		return true;
	}
	vector<Mat> vMat;
	if (nType==1 || nType==10)
	{
		split(mat_src,vMat);
		
		vMat[2] = CnewColorEnhance::black_whiteEnhance(vMat[2]);
		Mat mask = vMat[2]<80;
		temp.setTo(0,mask);
		vMat[0] = temp;
		vMat[1] = temp;
	}
	else if (nType==2 || nType==20)
	{
		
		split(mat_src,vMat);
		vMat[0] = CnewColorEnhance::black_whiteEnhance(vMat[0]);
		Mat mask = vMat[0]<80;
		temp.setTo(0,mask);
		vMat[2] = temp;
		vMat[1] = temp;
	}
			
	merge(vMat,mat_src);
	return true;
}

//网点增加算法（生成二值化后灰度图8bit）
//Halftone
//Convert to （binary） 8 bit by Halftone
//参数说明:
//input:
//nFilterMatrix:
// 0 - DarkerPhtto
// 1 - DarkerPhoto + Text
// 2 - LighterPhoto
// 3 - LighterPhot + Text
// 4 - ErrorDiffusion
//nEdgeThreshold:边缘增强，取值范围:(0,255]，nEdgeThreshold>0则nFilterThreshold无效
//nFilterThreshold:取值范围:[0,40]，彩色图像判断阈值，nEdgeThreshold= 0 且彩色原图才有效
//返回值:
//二值化后的灰度图
MImage* mcvHalftone(MImage *src, int nColorChannel, int nFilterMatrix, int nEdgeThreshold, int nFilterThreshold)
{
	Mat mat_src = CAdapter::mimg2Mat(src);
	Mat mat_dst = Halftone::ConvertTo1BitHalftone(mat_src,nColorChannel,nFilterMatrix,nEdgeThreshold,nFilterThreshold);
	MImage *dst = CAdapter::Mat2mimg(mat_dst);
	return dst;
}


//绘制水印
//参数
//（1）src：原图：Mat
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
//返回值
//[out]Mat				带水印的图
MImage*  mcvWaterMark2(MImage* src, 
		char * content,
		char *  font,long fontSize,
		long colorR, long colorG, long colorB,
		float transparent, 
		int layout)
{
	Mat mat_src = CAdapter::mimg2Mat(src);
	if(mat_src.empty())
	{
		return src;
	}

	string str = content;

	int srcChannels =mat_src.channels();
	//将图像默认改为彩色图,灰度图后期改回来。
	if (mat_src.channels() == 1)
	{
		cvtColor(mat_src,mat_src,CV_GRAY2BGR);
		srcChannels =1;
	} 
	else
	{
		if(mat_src.channels() ==3)
		{
			srcChannels = 3;
		}
		else
		{
			return src;
		}
	}

#if 1
	
	Rect rrect;
	
	Mat temp;
	if (layout!=2)
	{
		Size dsize;
		if (fontSize==0)
		{
			dsize.width = mat_src.cols*2;
			dsize.height = int(mat_src.cols/float(str.size()))+20;
		}
		else
		{
			dsize.width = 2*fontSize*str.size();
			dsize.height = fontSize+20;
		}
		
		Mat mask = Mat::zeros(dsize,CV_8UC1);

		cv::Point origin; 
		origin.x = 20;//rrect.width/2;
		origin.y = mask.rows-10;
		CvText text(font);
		int type = 0;
		bool underline = false;
		float diaphaneity = 0.5;
		Scalar size;
		if (fontSize==0)
			size = Scalar(dsize.height-20,0.5,0.1,0);
		else
			size = Scalar(fontSize,0.5,0.1,0);

		text.setFont(type,size ,type,diaphaneity);//函数可以修改字体大小等属性，具体请看注释
		
		text.putText(mask,str,origin,cv::Scalar(255));

		mask = mask(Rect(0,0,origin.x,mask.rows));



		mask = mask>0;
		if (mask.cols>mat_src.cols || mask.rows>mat_src.rows)
			fontSize=0;
		
		if (fontSize==0)
		{
			if (true)
				resize(mask,mask,Size(mat_src.cols,int(mat_src.cols*float(mask.rows)/mask.cols)));
			
			rrect.width = mask.cols;
			rrect.height = mask.rows;
			if (layout==0)
			{
				rrect.x = 0;
				rrect.y = 0;
			}
			else if(layout==3)
			{
				rrect.x = 0;
				rrect.y = mat_src.rows-mask.rows;
			}
			else
			{
				rrect.x = 0;
				rrect.y = (mat_src.rows-mask.rows)/2;
			}
		}
		else
		{
			rrect.width = mask.cols;
			rrect.height = mask.rows;
			if (layout==0)
			{
				rrect.x = 0;
				rrect.y = 0;
			}
			else if(layout==3)
			{
				rrect.x = mat_src.cols-mask.cols;
				rrect.y = mat_src.rows-mask.rows;
			}
			else
			{
				rrect.x = (mat_src.cols-mask.cols)/2;
				rrect.y = (mat_src.rows-mask.rows)/2;
			}
		}
		if (mask.rows>mat_src.rows)
		{
			resize(mask,mask,Size(int(mat_src.rows*float(mask.cols)/mask.rows),mat_src.rows));
			rrect.x = (mat_src.cols-mask.cols)/2;
			rrect.y = (mat_src.rows-mask.rows)/2;
			rrect.width = mask.cols;
			rrect.height = mask.rows;
		}
		
		temp = mat_src(rrect).clone();
		temp.setTo(cv::Scalar(colorB, colorG, colorR),mask);


	}
	else if (layout==2)
	{
		Size dsize;
		if (fontSize==0)
		{
			dsize.width = mat_src.cols*2;
			dsize.height = int(mat_src.cols/float(str.size()))+20;
		}
		else
		{
			dsize.width = 2*fontSize*str.size();
			dsize.height = fontSize+20;
		}
		Mat mask = Mat::zeros(dsize,CV_8UC1);


		cv::Point origin; 
		origin.x = 20;
		origin.y = mask.rows-10;
		CvText text(font);
		int type = 0;
		bool underline = false;
		float diaphaneity = 0.5;
		Scalar size;
		if (fontSize==0)
			size = Scalar(dsize.height-20,0.5,0.1,0);
		else
			size = Scalar(fontSize,0.5,0.1,0);
		text.setFont(type,size ,type,diaphaneity);//函数可以修改字体大小等属性，具体请看注释
		
		text.putText(mask,str,origin,cv::Scalar(255));

		mask = mask(Rect(0,0,origin.x,mask.rows));

		MImage *src = CAdapter::Mat2mimg(mask);
		MImage* result = mcvRotateImage(src, -45);
		mask = CAdapter::mimg2Mat(result);
		mcvReleaseImage(&src);
		mcvReleaseImage(&result);
		//delete result;
		//delete src;
		rrect = Rect(0,0,mask.cols,mask.rows);
		if (mask.cols>mat_src.cols || mask.rows>mat_src.rows)
			fontSize=0;

		if (fontSize==0)
		{
			if (abs(float(mat_src.rows)/mask.cols-1)<  abs(float(mat_src.cols)/mask.cols-1))
				dsize = Size(int((float(mat_src.rows)/rrect.height)*rrect.width),mat_src.rows);
			else
				dsize = Size(mat_src.cols,int((float(mat_src.cols)/rrect.width)*rrect.height));
		
			if (dsize.height>mat_src.rows)
			{
				dsize.width = int(dsize.width*mat_src.rows/dsize.height);
				dsize.height = mat_src.rows;
			}
			
			rrect = Rect(0,0,dsize.width,dsize.height);
		
			if (layout==1 || layout==2)
			{
				rrect.x = int((mat_src.cols-dsize.width)/2);
				rrect.y = int((mat_src.rows-dsize.height)/2);
			}
			if (layout==3)
			{
				rrect.x = mat_src.cols-dsize.width;
				rrect.y = mat_src.rows-dsize.height;
			}
			resize(mask,mask,dsize);
		}
		else
		{
			rrect.x = int((mat_src.cols-rrect.width)/2);
			rrect.y = int((mat_src.rows-rrect.height)/2);
		}
		
		temp = mat_src(rrect).clone();
		mask = mask>0;
		temp.setTo(cv::Scalar(colorB, colorG, colorR),mask);
	}
	

	if (transparent==0)
	{
		Mat black = mat_src(rrect);

		addWeighted(black, 0.5, temp, 0.5, 0, black);
		black.copyTo(mat_src(rrect));
	}
	else if(transparent==1)
	{

		temp.copyTo(mat_src(rrect));
	}
#endif
	MImage* mimg = NULL;
	mimg = CAdapter::Mat2mimg(mat_src);

	return mimg;//CAdapter::CopyMat2mimg(mat_src, src);
}


//绘制图片水印
//参数
//（1）src：原图：Mat
//（2）water_src：水印图像：Mat
//（3）pt：水印绘制位置：Point
//（4） transparent：透明度：float
//返回值
//[out]Mat				带水印的图
MImage* mcvWaterMark(MImage* src, MImage* water_src, MPoint pt, float transparent)
{
	Mat mat_src = CAdapter::mimg2Mat(src);
	Mat mat_water_src = CAdapter::mimg2Mat(water_src);
	if(mat_src.empty() || mat_water_src.empty() )
	{
		return src;
	}

	if (mat_src.channels() != mat_water_src.channels())
	{
		if (mat_water_src.channels()==3)
			cvtColor(mat_src,mat_src,CV_GRAY2BGR);
		else
			cvtColor(mat_water_src,mat_water_src,CV_GRAY2BGR);
	}

	if (pt.x==0 || pt.y==0)
	{
		pt.x = mat_src.cols/2;
		pt.y = mat_src.rows/2;
	}

	if (mat_water_src.cols>mat_src.cols)
	{
		resize(mat_water_src, mat_water_src, Size(mat_src.cols,int(mat_water_src.rows*mat_src.cols/mat_water_src.cols)), INTER_CUBIC);
	}
	if (mat_water_src.rows>mat_src.rows)
	{
		resize(mat_water_src, mat_water_src, Size(int(mat_water_src.cols*mat_src.rows/mat_water_src.rows),mat_src.rows), INTER_CUBIC);
	}
	
	int x = pt.x;
	int y = pt.y;
	Rect rrect = Rect(x-mat_water_src.cols/2,y-mat_water_src.rows/2,mat_water_src.cols,mat_water_src.rows);

	if (x<mat_water_src.cols/2 || mat_src.cols-x<mat_water_src.cols/2)
	{
		if (x<mat_water_src.cols/2)
		{
			mat_water_src = mat_water_src(Rect(max(0,int(mat_water_src.cols/2-x)),0,int(mat_water_src.cols/2+x),mat_water_src.rows));
			rrect.x = 0;
			rrect.width = mat_water_src.cols;
		}
		else
		{
			mat_water_src = mat_water_src(Rect(0,0,int(mat_water_src.cols/2+mat_src.cols-x),mat_water_src.rows));
			rrect.width = mat_water_src.cols;
		}
		//float fRiao = 1.0*min(x,mat_src.cols-x)/mat_water_src.cols/2;
		//resize(mat_water_src, mat_water_src, Size(int(mat_water_src.cols*fRiao),int(mat_water_src.rows*fRiao)), INTER_CUBIC);
	}
	if (y<mat_water_src.rows/2 || mat_src.rows-y<mat_water_src.rows/2)
	{
		if (y<mat_water_src.rows/2)
		{	
			mat_water_src = mat_water_src(Rect(0,max(0,int(mat_water_src.rows/2-y)),mat_water_src.cols,int(mat_water_src.rows/2+y)));
			rrect.y = 0;
			rrect.height = mat_water_src.rows;
		}
		else
		{
			mat_water_src = mat_water_src(Rect(0,0,mat_water_src.cols,int(mat_water_src.rows/2+mat_src.rows-y)));
			rrect.height = mat_water_src.rows;
		}
		//float fRiao = 1.0*min(y,mat_src.rows-y)/mat_water_src.rows/2;
		//resize(mat_water_src, mat_water_src, Size(int(mat_water_src.cols*fRiao),int(mat_water_src.rows*fRiao)), INTER_CUBIC);
	}
	
	if (transparent!=0)
	{
		Mat black = mat_src(rrect);
		addWeighted(black, transparent, mat_water_src, 1-transparent, 0, black);
		black.copyTo(mat_src(rrect));
	}
	else
	{

		mat_water_src.copyTo(mat_src(rrect));
	}
	MImage* mimg = NULL;
	mimg = CAdapter::Mat2mimg(mat_src);

	return mimg;//CAdapter::CopyMat2mimg(mat_src, src);
}

//对两张图像进行比对,算法考虑光线的影响,用于防偷换,自动拍摄等
//参数: 
//[in] src1: 源图像
//[in] src2: 源图像
//[in] nThreshold: 阈值（0—255）,据经验取(90—200)
//[in] vcMRect: 提取图像的区域设定
//[in] nMRectCount: 提取图像的区域数量
//返回值:
//true: 成功
//false: 失败
bool  mcvCompare(MImage *src1,MImage *src2,int nThreshold,MRect vcMRect[10],int nMRectCount)
{
	Mat mat_src1 = CAdapter::mimg2Mat(src1);
	Mat mat_src2 = CAdapter::mimg2Mat(src2);
	vector<CvRect> vcRect;

	for(int i=0;i<nMRectCount;i++)
	{  
		CvRect rect1;
		rect1.x=vcMRect[i].m_nLeft;
		rect1.width=vcMRect[i].m_nRight-rect1.x;
		rect1.y=vcMRect[i].m_nTop;
		rect1.height=vcMRect[i].m_nBotton-rect1.y;
		vcRect.push_back(rect1);
	}

	//compare 2 images
	bool isright = CCompare::Compare(mat_src1,mat_src2,nThreshold,vcRect);
	return isright;
}
