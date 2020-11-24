#pragma once
#include "stdafx.h"
//类版本号V0.0.9

//文字增强 Bicycle  Time：2017年4月17日13:45:32   版本号V0.0.2
//De-screen	Bicyle	Time:2017年9月21日14:23:00	  版本号V0.1.1
//调节图像Gama值 Bicycle	Time:2017年4月24日10:58:16  版本号:V0.0.1
//图像滤色	Bicycle	Time:2017年4月26日14:05:43	  版本号V0.0.1
//文字增强/图像背景平滑，去除/自动色阶 Bicycle  Time：7/18  版本号V0.0.1

class CImageEnhancement
{
public:
	//文字增强，背景平滑/滤除，自动色阶 V0.0.1
	//[in]src			输入图像
	//[out]dst			输出图像
	//[in]nType
	//	1				文字增强
	//	2				背景平滑
	//	3				背景滤除
	//	4				自动色阶
	//注：出现其它数字默认转换为0,不做处理
	//[in]maxValue		亮度最大值
	//[in]minValue		亮度最小值
	//--- maxValue,minValue--- 有负值则参数失效
	//[in]fGmax			最大增益(0.3,1.5]
	//[in]fGmin			最小增益[0,0.3]
	//--- fGmax,fGmin---为1则对应的参数失效	
	static bool imageStretch(Mat src,Mat &dst,uint nType=0,int maxValue =255,int minValue =0,float fGmax =1,float fGmin=1);

	//TextEnhancement
	//[in]src			输入图像
	//[in/out]dst		输出图像
	static bool textEnhancement(Mat src,Mat &dst,uchar *checkTable = NULL);

	//TextEnhancement 
	//参数同上
	static bool textEnhancementTmp(Mat src,Mat &dst,uchar *checkTable = NULL);

	//De-screen
	//[in/out]			输入/输出图像
	//[in]nType			类型
	//	0				原图
	//	1				杂志
	//	2				报纸
	//	3				印刷
	static bool DeScreen(Mat &src,uint nType=0);

	//De-screen
	//[in/out]src		输入/输出图像
	//[in]nSize			高斯核尺寸
	static bool DeScreen(Mat &src,int nSzie =7,int Xsigma=0,int Ysigma=0);

	//Gama
	//[in/out]src		输入/输出图像
	//[in]fGama			gama值
	static bool gamaCorrection(Mat &src,float fGama=1.0);

	//图像滤色
	//[in/out]src			输入/输出图像
	//[in]removeColorType	
	//	0					红色
	//	1					绿色
	//	2					蓝色
	static bool removeColor(Mat &src,int removeColorType =0);

private:
	//自动色阶查找表
	static bool autoLevelUnisLookTable(vector<Mat> vectorOfChannelImage,int *unisLookTable,int maxValue =255,int minValue =0,float fGmax =1,float fGmin=1);

	//背景平滑/去除查找表
	static bool backGroundLookupTable(bool isBackGroundColorSmooth,int *backGroundLookupTable_r,int *backGroundLookupTable_g,int *backgroundValueTable_b,int threshold_r,int threshold_g,int threshold_b);
};

