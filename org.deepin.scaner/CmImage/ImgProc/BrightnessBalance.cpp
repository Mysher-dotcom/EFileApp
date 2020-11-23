#include "stdafx.h"
#include "BrightnessBalance.h"
//V1.0.4 封装整理 10.25

CBrightnessBalance::CBrightnessBalance(void)
{
}


CBrightnessBalance::~CBrightnessBalance(void)
{
}

/*************************************全局资源*************************************/
//线程资源
//HANDLE Thread_r=NULL;
//HANDLE Thread_g=NULL;
//HANDLE Thread_b=NULL;		
//线程的独立资源
IplConvKernel* element_r=NULL;
IplConvKernel* element_g=NULL;
IplConvKernel* element_b=NULL;
//高斯核
CvMat kernel_r;
CvMat kernel_g;
CvMat kernel_b;
CvMat kernel_image;
//中间变量
IplImage * temp_r=NULL;
IplImage * temp_g=NULL;
IplImage * temp_b=NULL;		
//分通道图像
IplImage * pImgr=NULL;
IplImage * pImgg=NULL;
IplImage * pImgb=NULL;		
/*************************************全局资源*************************************/
/*************************************全局变量*************************************/
//线程挂起信号量
bool CalcOver_r=false;
bool CalcOver_g=false;
bool CalcOver_b=false;
bool isDoc=false;				//决定文档模式还是非文档模式，缺省=false
int T_bottom=-1;int T_top=-1;	//直方图参数
/*************************************全局变量*************************************/
/*************************************Run*************************************/
/*************************************Run*************************************/

//计算图像中的亮点位置(private)
//[in]src			输入图像
//[in]fratioTop		比例
Point CBrightnessBalance::findMaxThreshold1(const Mat &src,float fratioTop /* = 0.3 */,float fratioBot /* = 0.3*/)
{
	Point pt;
	pt.x = 128;
	pt.y = 128;
	int valueArr[256]={0};

	const uchar *data = src.ptr<uchar>(0);

	for (int idr=0;idr < src.rows;idr++)
	{
		for (int idc=0;idc <src.cols;idc++,data++)
		{
			valueArr[*data]++;
		}
	}

	long lSum =0;
	int nThres = src.rows*src.cols*fratioTop;
	for (int idx=255;idx > 0;idx--)
	{
		lSum += valueArr[idx];
		if(lSum >nThres)
		{
			pt.x = idx;
			break;
		}
	}

	lSum =0;
	nThres = src.rows*src.cols*fratioBot;
	for (int idx=0;idx < 255;idx++)
	{
		lSum += valueArr[idx];
		if(lSum >nThres)
		{
			pt.y = idx;
			break;
		}
	}

	return pt;
}
