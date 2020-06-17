#pragma once
//通过直方图拉伸进行图像增强
//Max 2016-08-22 16:22:12 V0.0.1
//Bicycle 2017年5月16日15:42:43  V0.0.2 自动色阶紫光版本
//Bicycle 2017年5月17日 V0.0.3 背景色平滑/背景色变白
//Bicycle 2017年6月12日 V0.0.7 紫光要求更改	

//默认最高值
#define _MX_MAXVALUE 255
#define _MX_AUTOLEVEL_HIGHMIN 127
#define _MX_AUTOLEVEL_LOWMAX 100

#define _MX_HIGH_STARTER 50//寻找高点起始值
#define _MX_IGNORELOW_HIGHVALUE 100//当高点达到值时忽略低点

//检测是否溢出
static int CheckValue(int value)  
{  
	return value <= 0? 0 : value >= _MX_MAXVALUE? _MX_MAXVALUE : value;  
}  

class CAutoLevel
{
public:
	//自动调整色阶
	static bool AdjustLevelAuto(Mat src,Mat &dst);

	//手动调整色阶
	static bool AdjustLevelCustom(Mat src,Mat &dst);

	//拉伸直方图算法
	//Max 2013-5-6 16:16:03
    static bool StrechHistogram(unsigned char* pSrc,int nSrcWidth,int nSrcHeight,int nSrcBpp,int nSrcStep,
		int nLow,int nHigh,int nMin =0,int nMax = 255);

	//自动色阶 V0.0.2 bicycle Time:2017年5月16日15:46:20
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
	static bool AdjustLevelAutoUnis(Mat src,Mat &dst,int maxValue =255,int minValue =0,float fGmax =1,float fGmin=1);

	//背景色平滑 V0.0.3 bicycle Time:2017年5月17日14:09:13
	//[in]src						输入图像
	//[out]dst						输出图像
	//[in]isBackGroundColorSmooth	背景色平滑/变白(默认：变白(false));
	//返回值：
	//true:成功，false：失败
	//注：函数只支持彩色图像！
	static bool backGroundColorProcessing(Mat src,Mat &dst,bool isBackGroundColorSmooth = false); 

	//文档增强（Document Enhancement）
	//参数
	//[in]src		:	源图像
	//[out]dst		:	目标图像
	//[in]nBoxSize	:	窗口尺寸
	//					（1） > 0，按窗口对每个区域图像分别进行处理
	//					（2） <=0，对全图进行处理
	//[in]nMinTh	:	最低阈值（直接采用此阈值）
	//[in]nMaxTh	:	算法会计算最优化阈值，如果阈值大于nMaxTh，则使其等于nMaxTh
	//返回值
	//true	:	操作成功
	//false	:	操作失败
	static bool DocumentEnhancement(Mat src,Mat &dst, int nBoxSize = 256,int nMinTh = 50, int nMaxTh = 200);

	//自动调整色阶,考虑蒙版
	//参数
	//[in]src		:	源图像
	//[out]dst		:	目标图像
	//[in]nMinTh	:	最低阈值（直接采用此阈值）
	//[in]nMaxTh	:	算法会计算最优化阈值，如果阈值大于nMaxTh，则使其等于nMaxTh
	//[in]mask		:	蒙板（文档化使用）
	//返回值
	//true	:	操作成功
	//false	:	操作失败
	static bool AdjustLevelAuto2(Mat src,Mat &dst,int nMinTh = 50, int nMaxTh = 200,Mat mask = Mat());

	//手动调整色阶
	//弹出窗口进行色阶调整以便于观察（基于直方图非线性增强）
	//参数
	//[in]src		:	源图像
	//返回值
	//true	:	操作成功
	//false	:	操作失败
	//static bool AdjustLevelCustom(Mat src,Mat &dst);

	//直方图非线性增强算法，考虑蒙板
	//参数:
	//pSrc:			源图像
	//nSrcWidth:	源图像宽度
	//nSrcHeight:	源图像高度
	//nSrcBpp:		源图像位数（8灰度，24彩色）
	//nSrcStep:		源图像每行内存长度
	//nLow:			直方图增强低阈值（直方图非线性增强原理：将高阈值和低阈值之间波形拉伸至整个直方图）
	//nHigh:		直方图增强高阈值（直方图非线性增强原理：将高阈值和低阈值之间波形拉伸至整个直方图）
	//nMin:			直方图低阈值最低值（低阈值低于此强制等于此）
	//nMax:			直方图高阈值最高值（高阈值高于此强制等于此）
	//pMask:		直方图增强蒙板（对于蒙板中卫0时按正常高低阈值增强，如果等于1则按增补阈值增强）
	//nMaskStep:	直方图增强蒙板每行内存长度
	//nMaskHigh:	直方图增强高阈值增补值（当蒙板等于1时，按此增补值增强）
	//返回值:
	//false:		操作失败
	//true:			操作成功
    static bool StrechHistogram2(unsigned char* pSrc,int nSrcWidth,int nSrcHeight,int nSrcBpp,int nSrcStep,
        int nLow,int nHigh,int nMin =0,int nMax = 255, unsigned char* pMask = NULL,int nMaskStep = 0,int nMaskHigh =30);

	//寻找图像中的最亮点 Bicycle V0.0.2 2017.5.12
	//[in]src			灰度图
	//[out]maxValue		最大值
	//[out]minValue		最小值
	static bool findMaxValueofImage(Mat src,int &maxValue,int &minValue,float numOfMax=0.9667,float numOfMin =0.03333);

private:
	//判断图像是否需要裁切[背景平滑内嵌函数]
	//[in]src			输入图像
	//返回值：
	//true,需要裁切false，不需要裁切
	static bool isNeedCut(Mat src);
};

