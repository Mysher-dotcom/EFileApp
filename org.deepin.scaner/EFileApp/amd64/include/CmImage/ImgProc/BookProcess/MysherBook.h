#if defined(__linux__)
#ifndef CMSTR_OCRLIB_LIBRARY_H
#define CMSTR_OCRLIB_LIBRARY_H
#define MYSHERIMAGE_API __attribute__ ((visibility("default")))
#endif
#else
#ifdef LAYOUTREADER_EXPORTS
#define MYSHERIMAGE_API extern "C" __declspec(dllexport)
#else
#define MYSHERIMAGE_API extern "C" __declspec(dllimport)
#endif
#endif

#pragma once
#include "MImage.h"
#include "Adapter.h"
#include "BookProcess.h"
#include "DetectRectByContours.h"
#include "Test_Book.h"

using namespace cv;
using namespace std;
using namespace CTest_Book;
using namespace CBookProcess;


//[in]src_left				输入书本左页图像
//[in]src_right             输入书本右页图像
//[out]mbokkInfo_left       输出左页上下边界轮廓
//[out]mbookInfo_right      输出右页上下边界轮廓
MYSHERIMAGE_API void mcvBookStretch_Getlandamarks(MImage *src_left, MImage *src_right, MBookInfo *mbookInfo_left, MBookInfo *mbookInfo_right);


////合并提取书书本左右页上下边界轮廓函数
//[in]src				输入书本图像(左右页不分割）
//[out]mbokkInfo_left       输出左页上下边界轮廓
//[out]mbookInfo_right      输出右页上下边界轮廓
MYSHERIMAGE_API void mcvBookStretch_Getlandamarks_double(MImage *src, MBookInfo *mbookInfo_left, MBookInfo *mbookInfo_right);


MYSHERIMAGE_API MImage* mcvBookStretch_landamarksStretch_left(MImage *src);


MYSHERIMAGE_API MImage* mcvBookStretch_landamarksStretch_right(MImage *src);


MYSHERIMAGE_API MImage* mcvBookStretch_landamarksStretch_double(MImage *src, MBookInfo *mbookInfo_left, MBookInfo *mbookInfo_right, bool type);


//************************************************************************  
// 函数名称:    mcvBookStretch_CurveShow_double    
// 函数说明:    书本上下轮廓关键点显示  
// 函数参数:    MImage *src    输入原图
// 函数参数:    MBookInfo *mbookIfo    输入上下边界关键点
// 返 回 值:    MImage*   输出显示轮廓线及关键点的图像
//************************************************************************ 
MYSHERIMAGE_API MImage* mcvBookStretch_CurveShow_double(MImage *src, MBookInfo *mbookInfo_left, MBookInfo *mbookInfo_right);


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

MYSHERIMAGE_API MImage* mcvBookStretch_landamarksStretch(MImage *src_left, MImage *src_right, MBookInfo *mbookInfo_left_old, MBookInfo *mbookInfo_left_new, MBookInfo *mbookInfo_right_old, MBookInfo *mbookInfo_right_new, bool type);



//************************************************************************  
// 函数名称:    mcvBookStretch_CurveShow    
// 函数说明:    书本上下轮廓关键点显示  
// 函数参数:    MImage *src    输入原图
// 函数参数:    MBookInfo *mbookIfo    输入上下边界关键点
// 返 回 值:    MImage*   输出显示轮廓线及关键点的图像
//************************************************************************ 
MYSHERIMAGE_API MImage* mcvBookStretch_CurveShow(MImage *src, MBookInfo *mbookInfo);


//*****************************************************
//函数说明：  书本手指检测去除
//MImage *src  输入图像
//int type    书页标记：0为双页，1为左页，2为右页
//返回参数 MImage* 
//*****************************************************
MYSHERIMAGE_API MImage* mcvBookStretch_FingerRemoval(MImage *src_rgb, int type);


//************************************************************************  
// 函数名称:    mcvBookStretch_Enhance  
// 函数说明:    图像灰度对比度增强
// 函数参数:    Mat src    输入图像
// 返 回 值:    Mat  输出增强后灰度图像
//************************************************************************ 
MYSHERIMAGE_API MImage* mcvBookStretch_Enhance(MImage *src_rgb);


//************************************************************************  
// 函数名称:    mcvBookStretch_inpaint
// 函数说明:    曲面展平后图像黑边修复
// 函数参数:    MImage *src_rgb    输入图像
// 函数参数:    int type     type : 1,文档补边 ，0（默认值）,书本补边
// 返 回 值:    MImage
//************************************************************************ 
MYSHERIMAGE_API MImage* mcvBookStretch_inpaint(MImage *src_rgb, int type);

//从源图像中检测出矩形目标（如文档）,俗称裁切纠偏,默认背景为黑色,目标边缘不为黑色,俗称裁切纠偏,算法基于轮廓检测,返回检测出的矩形区域其倾斜角度和4个顶点
//参数: 
//[in] src: 源图像
//[in] rectROI: 关注区域,当其不为0时,只对区域内部分进行计算
//返回值:
MYSHERIMAGE_API MRectRArray mcvDetectRect1(MImage* src, MRect rect = MRect(0, 0, 0, 0));
