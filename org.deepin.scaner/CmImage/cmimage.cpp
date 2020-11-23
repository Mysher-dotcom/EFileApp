#include "cmimage.h"
#include "./ImgProc/AdaptiveThreshold.h"

#include "./ImgProc/Rotate.h"

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
	g_init = true;
    return true;
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
