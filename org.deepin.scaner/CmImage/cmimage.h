#ifndef CMIMAGE_GLOBAL_H
#define CMIMAGE_GLOBAL_H


#include "stdafx.h"


#include <iostream>
using namespace std;

extern "C"
{
    //初始化
    //参数:
    //无
    //返回值:
    //true: 成功
    //false: 失败
    bool mcvInit();

    //获取版本号
    string mcvGetVersion();

    //加载图像
    //参数:
    //无
    //[in] chImgPath: 源图像路径
    //返回值:
    //MImage对象指针,如果失败则为0
    MImage* mcvLoadImage(char* path,long w_Dest=0, long h_Dest=0);

    //克隆MImage对象指针（深拷贝）,对象内存与src无关
    //参数:
    //[in] src: 源图像指针
    //返回值:
    //MImage对象指针,如果失败则为0
    MImage* mcvClone(MImage* src);

    //将copy复制到src指定位置
    //参数:
    //[in/out] dst: 目标图像
    //[in] src: 源图像指针
    //[in] pt: 复制位置
    //返回值:
    //true: 成功
    //false: 失败
    bool mcvCopyToRect(MImage* src, MImage* copy, MPoint pt);

	//从内存创建MImage对象指针,对象内存与data无关
	//参数:
	//[in] width: 源图像宽度
	//[in] height: 源图像高度
	//[in] channel: 源图像颜色通道数量,支持1,3（默认位深度为8）
	//[in] data: 源图像内存
	//[in] bFlip: 是否需要对内存做Flip
	//返回值:
	//MImage对象指针,如果失败则为0
	MImage* mcvCreateImageFromArray(int width, int height, int channel, char* data, bool bFlip=false);

    //释放图像,所有MImage对象指针必须通过此函数释放否则内存泄漏
    //参数使用源图像指针的指针,如此可以将用户声明的Mimage对象指针置为0,调用ReleaseImage后,可以通过"if(pt == 0)"判断是否已经释放内存
    //参数:
    //[in] src: 源图像指针的指针
    //返回值:
    //无
    void mcvReleaseImage(MImage** src);
    void mcvReleaseImage1(MImage* src);


    //弹出一个可以调节尺寸的窗口显示Mimage对象,窗口为模态,意味着需要将窗口关闭才可以进行之后的操作
    //参数:
    //[in] src: 源图像
    //返回值:
    //无
    void mcvShowImage(MImage* src);

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
    bool mcvSaveImage(char* sz, MImage* src,int xDPI = -1,int yDPI = -1,int jpgQuanlity = 50, bool bBW = false);

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
    bool mcvDrawLine(MImage* src, MPoint pt0, MPoint pt1, MColor color, int thickness = 10);

	//图像上绘线
	//img			图像指针
	//colorValue	颜色值(char[3])单通道，双通道，后面的颜色值无关紧要
	//pt			起止点(Point[2])
	//imgInfo		图像信息(int[3]分别对应：height,width,channel,line size)
	void mcvDrawLineOnImageBuffer(char *Img,char *colorValue,MPoint *pt,int *imgInfo);

    //绘制任意角度矩形
    //参数:
    //[in] src: 源图像
    //[in] rectR: 带角度矩形
    //[in] color: 绘制颜色
    //[in] thickness: 线宽度
    //返回值:
    //true: 成功
    //false: 失败
    bool mcvDrawRectR(MImage* src, MRectR rectR, MColor color, int thickness = 10);

    //将源对象灰度化,返回目标对象内存与源对象独立
    //参数:
    //[in] src: 源图像
    //返回值:
    //MImage对象指针,如果失败则为0
    MImage* mcvGrayStyle(MImage *src);

    //二值化（图像为单通道,每个通道为1个字节,即常见灰度格式）
    //参数:
    //[in] src: 源图像
    //[in] nThreshold: 二值化阈值（0-255）
    //返回值:
    //MImage对象指针,如果失败则为0
    MImage* mcvThreshold(MImage* src, int nThreshold = 128);

    //局部自适应最佳阈值二值化（图像为单通道,每个通道为1个字节,即常见灰度格式）
    //参数:
    //[in] src: 源图像
    //返回值:
    //MImage对象指针,如果失败则为0
    MImage* mcvAdaptiveThreshold(MImage* src);

	//自适应最佳阈值二值化(OprnCV基于高斯边缘)（图像为单通道,每个通道为1个字节,即常见灰度格式）
	//参数: 
	//[in] src: 源图像
	//返回值:
	//MImage对象指针,如果失败则为0
	MImage* mcvAdaptiveThreshold2(MImage* src);

	//Bernsen阈值二值化（图像为单通道,每个通道为1个字节,即常见灰度格式）,采用改进Bernsen算法,常用于光线不好,导致图像边缘模糊的情况
	//参数: 
	//[in] src: 源图像
	//返回值:
	//MImage对象指针,如果失败则为0
	MImage* mcvBernsenThreshold(MImage* src);

	//局部自适应最佳阈值二值化（图像为单通道,每个通道为1个字节,即常见灰度格式）
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
	MImage* mcvAdaptiveThresholdEx(MImage* src,int nMethod=0,int nBoxSize=64);

	//最佳阈值二值化（图像为单通道,每个通道为1个字节,即常见灰度格式）
	//参数: 
	//[in] src: 源图像
	//返回值:
	//MImage对象指针,如果失败则为0
	MImage* mcvOptimalThreshold(MImage* src);

	//自适应梯度二值化算法
	//[in]src			输入图像
	//[out]dst			输出图像
	MImage* mcvGradientAdaptiveThreshold(MImage *src,float threshold);

	//直方图自适应二值化算法
	//[in]src				输入图像
	//[out]dst				输出图像
	MImage* mcvHistAdaptiveThreshold(MImage *src);

    //从源图像中检测出矩形目标（如文档）,俗称裁切纠偏,默认背景为黑色,目标边缘不为黑色,俗称裁切纠偏,算法基于轮廓检测,返回检测出的矩形区域其倾斜角度和4个顶点
    //参数:
    //[in] src: 源图像
    //[out] fAngle: 倾斜偏移角度,单位为°
    //[out] ptMPoint: 检测矩形区域的4个顶点
    //[in] rectROI: 关注区域,当其不为0时,只对区域内部分进行计算
    //返回值:
    //true: 成功
    //false: 失败
    bool mcvDetectRect(MImage *src,float &fAngle, MPoint *ptMPoint, MRect rect = MRect(0,0,0,0));
	bool mcvDetectRect_scanner(MImage *src,float &fAngle, MPoint *ptMPoint, MRect rect = MRect(0,0,0,0));
    MRectRArray mcvDetectRect1(MImage* src, MRect rect = MRect(0,0,0,0));

    //从源图像中检测出多个矩形目标（如文档）,俗称多图裁切,默认背景为黑色,目标边缘不为黑色,俗称裁切纠偏,算法基于轮廓检测,返回检测出的矩形区域其倾斜角度和4个顶点
    //参数:
    //[in] src: 源图像
    //[in] rectROI: 关注区域,当其不为0时,只对区域内部分进行计算
    //返回值:
    //检测出的矩形区域列表,为0时可能意味失败,具体见MRectRArray定义
    MRectRArray mcvDetectRectMulti(MImage* src, MRect rect = MRect(0,0,0,0));

    //新版多图裁切（支持扫描仪）
    //参数:
    //[in] src: 源图像
    //[in] rectROI: 关注区域,当其不为0时,只对区域内部分进行计算
    //返回值:
    //检测出的矩形区域列表,为0时可能意味失败,具体见MRectRArray定义
    MRectRArray mcvDetectRectMultiForScanner(MImage *src1,MRect rect = MRect(0,0,0,0));
    MRectRArray mcvDetectRectMultiForScannerBeta(MImage *src1,MRect rect = MRect(0,0,0,0));

    //检测空白页
    //参数：
    //[in]src				输入图像
    //[in]detectThreshold	判断阈值
    //返回值：
    //true: 成功
    //false: 失败
    bool mcvDetectBlankPage(MImage *src,int detectThreshold = 60);

    //图像对比度亮度
    //参数：
    //[in/out]src			输入/输出图像
    //[in]dContrast			对比度,取值范围[0,200],100不做变化
    //[in]nLuminance		亮度,取值范围[0,200],100不做变化
    //返回值：
    //true: 成功
    //false: 失败
    bool mcvLuminanceContrast(MImage *src,double dContrast=100,int nLuminance=100);

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
    MImage * mcvImgAdjust(MImage *src,int color =0, int hue =180, int saturation =100, int brightness= 100);

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
    MImage* mcvPSEffect(MImage* src, int nType);

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
    bool mcvPSEffectEx(MImage* src, int nType, MRect rectROI);

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
    bool mcvPSEffectEx2(MImage* src, int nType, MRect* arrRectROI, int arrLen);

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
    bool mcvNoise(MImage *src,int nMethod);

	//去噪,针对大的脏污类噪点
	//参数:
	//[in/out]src:	输入/出图像
	//[out]dst:	输出图像
	//[in]reps: 去噪迭代次数，常常一次去噪操作小孩不理想时可以考虑多次
	//[in]kernel: 去噪尺度，必须为奇数，建议3，5，7（值越大针对的噪点尺寸就越大，但是对图像的破化就越大）
	//返回值:
	//true: 成功
	//false: 失败
	bool mcvRemoveNoise(MImage* src,int reps =1,int kernel =3);

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
	int mcvDeletePointInBW(MImage *src,int nMethod=0);

    //图像去网纹
    //[in/out]		:	输入/输出图像
    //[in]nType		:	类型
    //	0			:	原图
    //	1			:	杂志
    //	2			:	报纸
    //	3			:	印刷
    //返回值：
    //true: 成功
    //false: 失败
    bool mcvDeScreen(MImage *src,int nType =0);

    //Gama
    //[in/out]src		     输入/输出图像
    //[in]fGama			gama值
    // gama值(0,1]，图像变亮
    // gama值(1,无穷) ，图像变暗
    //返回值：
    //true: 成功
    //false: 失败
    bool mcvGamaCorrection(MImage *src,float fGama=1.0);

    //锐化
    //参数：
    //[in] src :输入图像
    //[in] sigma：取>0,默认3.0;
    //[in] nAmount：取值>0，默认200，越大效果越明显
    //[in] thresholds：0~255,一般取1
    //返回值：
    //MImage*:调整后图像
    //jessie 2017-08-03 10:15:47
    MImage* mcvUnsharp(MImage *src,float sigma = 3.0,int amount=200,int thresholds=1);

    //背景色平滑
    //参数：
    //[in/out]src					输入/输出图像
    //[in]isBackgroundColorWhite	是否为背景色平滑(true:平滑,false:白色)
    //返回值：
    //true: 成功
    //false: 失败
    bool mcvBackGroundSmooth(MImage *src,bool isBackgroundColorSmooth =true);

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
    MImage* mcvImageStretch(MImage *src,int nType=0,int maxValue =255,int minValue =0,float fGmax =1,float fGmin=1);

	//自动调整色阶
	//参数: 
	//[in] src: 源图像
	//返回值:
	//MImage对象指针,如果失败则为0
	MImage* mcvAdjustLevelAuto(MImage* src);

    //自动色阶
    //用于扫描仪
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
    //MImage*:调整后图像
    //注：因为后四个参数有功能重叠部分，如果同时存在。先maxValue,minValue，后fGmax,fGmin。
    MImage* mcvAdjustLevelAutoUnis(MImage *src,int maxValue=255,int minValue =0,float fGmax =1,float fGmin =1);

    //彩色平衡
    //参数：
    //[in] imageSource: 输入图像
    //[in]  KR :红色分量比例 0~200
    //[in] KG 绿色分量 比例  0~200
    //[in] KB： 红色分量比例  0~200
    //返回值：
    //[out]: dst :输出图像
    MImage* mcvColorBlance(MImage *src,int KR = 100, int KG= 100,int KB= 100);

    //彩色图像增强(保真)
    //参数:
    //[in/out]src:	输入图像
    //[in]isAutoDetectWhiteBackGroud:	是否自动检测白色背景(白色背景：实现背景增强否则实现颜色保真),参数如下:
    // -1 自动检测
    // 0 增亮
    // 1 保真
    //返回值:
    //true: 成功
    //false: 失败
    bool mcvColorEnhance(MImage *src,int isAutoDetectWhiteBackGroud = -1);

    //图像去色：
    //[in/out]src	：	输入/输出图像
    //[in]indexOfHue:	输入颜色范围[0,360]
    //[in]scopeRadius:	颜色范围半径[0,180]
    //注：限定彩色图像，灰色黑白图像没有效果
    //颜色大致范围：360°/0°红、60°黄、120°绿、180°青、240°蓝、300°洋红
    void mcvColorDropout(MImage *src,int indexOfHue=0,int scopeRadius=60);

    //装订孔填充
    //参数：
    //[in/out]src:			输入/出图像
    //[in]isCircularHole	是否为圆形装订孔:(true:圆形,false:方形)
    //[in]isColorFill		是否纯色填充(true:纯色,false:背景色)
    //	1					圆形
    //	2					方形
    //[in]nR				红色分量：颜色分量仅在纯色填充下有效
    //[in]nG				绿色分量：颜色分量仅在纯色填充下有效
    //[in]nB				蓝色分量：颜色分量仅在纯色填充下有效
    //返回值:
    //true: 成功
    bool mcvPunchHold(MImage *src,bool isCircularHole =true,
        bool isColorFill =true,int nR=255,int nG =255,int nB =255);

    //边缘填充
    //适用于裁切纠偏后的图像
    //参数:
    //[in/out] src: 源图像/目标图像
    //[in] offsetNum: 仿制位置离边界的收缩量（默认4）
    //[in] mappingScope: 收缩量的跨度范围，已像素为单位，（默认-1），-1：是按照边缘空隙量无限项边缘内伸缩。(只针对映射填充)
    //[in] isColorFill: 是否是纯色填充（默认1），参数如下
    //	-1				映射填充
    //	0				纯色填充(默认白色)
    //	1				自动纯色填充
    //[in] nR,nG,nB: 纯色填充的颜色（默认白色）
    //返回值:
    //true: 成功
    //false: 失败
    bool mcvFillBorder(MImage *src,int offsetNum = 4,int mappingScope = -1,int isColorFill = -1, int nR = 255, int nG = 255, int nB = 255);

    //根据内容倾斜校正,俗称文字纠偏（印刷校正）,算法基于直线检测,返回校正后的图像
    //参数:
    //[in] src: 源图像
    //返回值:
    //MImage对象指针,如果失败则为0
    MImage* mcvAdjustSkew(MImage* src);

    //图像反色
    //参数：
    //[in/out]src		输入/输出图像
    //返回值:
    //true: 成功
    //false: 失败
    bool mcvInverse(MImage *src);

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
    //[in] fRadio: 输出图像宽大或缩小比例,nWidth和nHeight为0时生效
    //返回值:
    //MImage对象指针,如果失败则为0
    MImage* mcvResize(MImage *src,int nWidth,int nHeight,float fRadio= 1,int nType=1);

    //旋转(改变尺寸)
    //参数:
    //[in] src: 源图像
    //[in] fAngle: 旋转角度,单位为°
    //返回值:
    //MImage对象指针,如果失败则为0
    MImage* mcvRotateImage(MImage* src, float angle);

    //旋转(保持尺寸)
    //参数:
    //[in] src: 源图像
    //[in] fAngle: 旋转角度,单位为°
    //返回值:
    //MImage对象指针,如果失败则为0
    MImage* mcvRotateImageKeepSize(MImage* src, float angle);

    //逆时针,旋转90°
    //参数:
    //[in] src: 源图像
    //[in] fAngle: 旋转角度,单位为°
    //返回值:
    //MImage对象指针,如果失败则为0
    MImage* mcvRotateImageLeft(MImage* src);

    //图像合并
    //参数:
    //[in]Img1			:	输入图像1
    //[in]Img2			：  输入图像2
    //[in]bVertical		:	是否垂直
    //[in]bFrame		:	是否对每个图像在合并后加边框
    //输出：
    //[out]MImage*		：	输出图像
    MImage* mcvMergerImage(MImage *Img1,MImage *Img2,bool bVertical = true, bool bFrame = false);

    //图像裁切
    //参数:
    //[in] src: 源图像
    //[in] mRect: 表示包含裁切图像的左右上下位置四个参数的矩阵
    //返回值:
    //MImage对象指针,如果失败则为0
    MImage* mcvCut(MImage* src,MRect mrect);

    //图像裁切(任意角度)
    //参数:
    //[in] src: 源图像
    //[in] mRect: 表示包含裁切图像的左右上下位置四个参数的矩阵
    //返回值:
    //MImage对象指针,如果失败则为0
    MImage* mcvCutR(MImage* src,MRectR mrect);
    MImage* mcvCutR2(MImage* src,MRectR *mrect);

    //图像反转.图像镜像
    //参数：
    //[in/out]src		输入/输出图像
    //[in]nMethod		镜像方式
    //	0				Flip（默认）
    //	1				Mirror
    //返回值：
    //true: 成功
    //false: 失败
    bool mcvFlip(MImage *src,int nMethod=0);

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
    //返回值：
    //MImage*：		扩展边界后图
    MImage* mcvMakeBorder(MImage *src,MRect nBorderLength ,int nBorderType =0,int nR=255,int nG=255,int nB =255);


    //图像转换为RGB24
    //注意：MImage按BGR格式读取图像，RGB只用于与其他图像类进行转换
    MImage* mcvRGB24Style(MImage* src);

    //获取图像data
    //因为采用统一管理内存的方式，因此MImage无法再获取内存，需要使用mcvGetImageData
    //[in]src			图像
    //返回图像内存指针，无需也不能自行释放
    unsigned char* mcvGetImageData(MImage *src);

    //获取图像为宽度
    int mcvGetWidthStep(MImage *src);

    //获取MImage内存存储类型
    //[in]src			输入图像
    //返回
    //0:统一存储
    //1:本地存储
    int	mcvGetMImageMemType(MImage *src);




	//[in]src_left				输入书本左页图像
	//[in]src_right             输入书本右页图像
	//[out]mbokkInfo_left       输出左页上下边界轮廓
	//[out]mbookInfo_right      输出右页上下边界轮廓
	void mcvBookStretch_Getlandamarks(MImage *src_left, MImage *src_right, MBookInfo *mbookInfo_left, MBookInfo *mbookInfo_right);


	////合并提取书书本左右页上下边界轮廓函数
	//[in]src				输入书本图像(左右页不分割）
	//[out]mbokkInfo_left       输出左页上下边界轮廓
	//[out]mbookInfo_right      输出右页上下边界轮廓
	void mcvBookStretch_Getlandamarks_double(MImage *src, MBookInfo *mbookInfo_left, MBookInfo *mbookInfo_right);


	MImage* mcvBookStretch_landamarksStretch_left(MImage *src);


	MImage* mcvBookStretch_landamarksStretch_right(MImage *src);


	MImage* mcvBookStretch_landamarksStretch_double(MImage *src, MBookInfo *mbookInfo_left, MBookInfo *mbookInfo_right, bool type);


	//************************************************************************  
	// 函数名称:    mcvBookStretch_CurveShow_double    
	// 函数说明:    书本上下轮廓关键点显示  
	// 函数参数:    MImage *src    输入原图
	// 函数参数:    MBookInfo *mbookIfo    输入上下边界关键点
	// 返 回 值:    MImage*   输出显示轮廓线及关键点的图像
	//************************************************************************ 
	MImage* mcvBookStretch_CurveShow_double(MImage *src, MBookInfo *mbookInfo_left, MBookInfo *mbookInfo_right);


	//书本左右页展平合并函数
	//当两张图像上下边界轮廓都正确，输出展平拼接后图像
	//当只有一张图像轮廓正确，输出正确的那张图像展平后图像
	//当轮廓都不正确时，输出0
	//[in]src_left				输入书本左页图像
	//[in]src_right             输入书本右页图像
	//[in]mbokkInfo_left_old       输入左页上下边界原轮廓
	//[in]mbokkInfo_left_new     输入左页上下边界拖动后轮廓
	//[in]mbookInfo_right_old      输入右页上下边界原轮廓
	//[in]mbookInfo_right_new      输入右页上下边界拖动后轮廓
	//[in]type              是否去手指标识（true:为去手指,false：为不去手指）
	//[out]                    输出展平后图像

	MImage* mcvBookStretch_landamarksStretch(MImage *src_left, MImage *src_right, MBookInfo *mbookInfo_left_old, MBookInfo *mbookInfo_left_new, MBookInfo *mbookInfo_right_old, MBookInfo *mbookInfo_right_new, bool type);



	//************************************************************************  
	// 函数名称:    mcvBookStretch_CurveShow    
	// 函数说明:    书本上下轮廓关键点显示  
	// 函数参数:    MImage *src    输入原图
	// 函数参数:    MBookInfo *mbookIfo    输入上下边界关键点
	// 返 回 值:    MImage*   输出显示轮廓线及关键点的图像
	//************************************************************************ 
	MImage* mcvBookStretch_CurveShow(MImage *src, MBookInfo *mbookInfo);


	//*****************************************************
	//函数说明：  书本手指检测去除
	//MImage *src  输入图像
	//int type    书页标记：0为双页，1为左页，2为右页
	//返回参数 MImage* 
	//*****************************************************
	MImage* mcvBookStretch_FingerRemoval(MImage *src_rgb, int type);


	//************************************************************************  
	// 函数名称:    mcvBookStretch_Enhance  
	// 函数说明:    图像灰度对比度增强
	// 函数参数:    Mat src    输入图像
	// 返 回 值:    Mat  输出增强后灰度图像
	//************************************************************************ 
	MImage* mcvBookStretch_Enhance(MImage *src_rgb);


	//************************************************************************  
	// 函数名称:    mcvBookStretch_inpaint
	// 函数说明:    曲面展平后图像黑边修复
	// 函数参数:    MImage *src_rgb    输入图像
	// 函数参数:    int type     type : 1,文档补边 ，0（默认值）,书本补边
	// 返 回 值:    MImage
	//************************************************************************ 
	MImage* mcvBookStretch_inpaint(MImage *src_rgb, int type);

	//去红色
	//[in/out]src		输入/输出图像
	//返回值：
	//true: 成功
	//false: 失败
	bool mcvRemoveRED(MImage*src);

	//文档优化
	//参数: 
	//[in] src: 源图像
	//[in] preprocessType
	// 0 不处理
	// 1 log
	// 2 root
	//返回值:
	//true: 成功
	//false: 失败
	bool mcvBrightBalance(MImage *src,int preprocessType =0);


	//图像增强三合一：红印，蓝印，黑白
	//[in]src:输入图像
	//[in]nType:
	//	0				图像黑白[默认]
	//	1				红印(红色增强拉伸)
	//	10				红印(黑白+红印)
	//	2				蓝印(蓝色增强拉伸)
	//	20				蓝印(黑白+蓝印)
	//[in]isKeepDetail	是否保留细节
	//返回值：输出图像
	bool mcvEnhancement3In1(MImage *src,int nType,bool isKeepDetail =true);

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
	MImage* mcvHalftone(MImage *src, int nColorChannel = 3, int nFilterMatrix =0, int nEdgeThreshold = 70, int nFilterThreshold = 40);


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
	MImage* mcvWaterMark2(MImage* src, 
			char * content = "Water Mark",
			char *  font = "Arial",long fontSize = 72,
			long colorR = 255, 
			long colorG = 0, 
			long colorB = 0,
			float transparent = 1, 
			int layout = 0);

	//绘制图片水印
	//参数
	//（1）src：原图：Mat
	//（2）water_src：水印图像：Mat
	//（3）pt：水印绘制位置：Point
	//（4） transparent：透明度：float
	//返回值
	//[out]Mat				带水印的图
	MImage* mcvWaterMark(MImage* src, MImage* water_src, MPoint pt, float transparent = 0);


	//对两张图像进行比对,算法考虑光线的影响,用于防偷换,自动拍摄等
	//参数: 
	//[in] src1: 源图像
	//[in] src2: 源图像
	//[in] nThreshold: 阈值（0—255）,据经验取(90—200)
	//[in] vcMRect: 提取图像的区域设定
	//[in] nMRectCount: 提取图像的区域数量
	//返回值:
	//true: 相同
	//false: 不同
	bool mcvCompare(MImage *src1,MImage *src2,int nThreshold,MRect vcMRect[10],int nMRectCount);
}
#endif // CMIMAGE_GLOBAL_H
