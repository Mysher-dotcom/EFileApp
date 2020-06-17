#pragma once
#include "stdafx.h"
//书本曲面检测算法
//Tim.liu 2018-09-11 14:20  V0.0.1
//2018-9-30 14:55 V0.0.6 曲面展平修改
//2018-10-9 14:33 V0.0.8 边线矫正改为书脊线矫正
//2018-10-9 16:56 V0.0.9 书本算法函数从BookStretch转移到BookProcess
//2018-10-11 14:13 V0.0.10 修改边线倾斜矫正为水平中线垂直矫正，修改无目标时算法崩溃
//2018-10-12 9:58 V0.0.11  在V0.0.9上修改手指去除算法
//2018-10-12 10:42 V0.0.12 添加边线倾斜矫正函数KeepParaell
//2018-10-12 18:26 V0.0.13 修改边线倾斜矫正为书脊线与文本端线相结合的矫正方式
//2018-10-16 9:28 V0.0.14 精细V0.0.13的倾斜矫正方式
//2018-10-16 17:4 V0.0.15 精细V0.0.14的倾斜矫正方式
//2018-10-18 10:6 V0.0.16 算法速度优化（手指去除，曲面展平）
//2018-10-19 11:15 V0.0.17 曲面展平扭曲修改
//2018-10-19 14:33 V0.0.18 边线倾斜矫正修改，展平后下部多出一部分BUG修改
//2018-10-23 11:04 V0.0.19 书本黑边去除算法修改
//2018-10-23 11:04 V0.0.20 展平后笔画变粗现象修改
//2018-10-24 9:39 V0.0.21 展平后右页笔画加粗问题修改
//2018-10-24 15:20 V0.0.22 补边算法速度优化
//2018-10-25 9:52 V0.0.23 手指去除，部分去除不了问题修改
//2018-10-29 16:07 V0.0.24 展平后字符拉伸问题修改
//2018-10-31 16:00 V0.0.25 曲面展平扭曲修改
//2018-11-1 17:46 V0.0.26 在v0.0.23基础上修改书脊线检测，改善曲面展平效果
//2018-11-02 17:35 V0.0.27 修改关键点提取均匀度，修改字符展平后字符拉升
//2018-11-06 18:12 V0.0.28 展平字符均匀度，展平宽度，按压变形展平效果改善修改
//2018-11-08 15:32 V0.0.30 展平变形修改
//2018-11-08 18:10 V0.0.31 自适应辅助展平线寻找回退到10-25（V0.0.23)
//2018-11-09 17:14 V0.0.32 书本矩形裁切出1X1的图像后补边算法崩溃修改
//2018-11-13 10:14 V0.0.33 展平扭曲修改（线性变化权重误差计算），倾斜适应
//2018-11-13 16:00 V0.0.34 展平扭曲修改(二次方变化权重误差计算），倾斜适应
//2018-11-14 11:00 V0.0.35 编辑关键点后展平算法崩溃修改
//2018-11-15 17:46 V0.0.36 融合v0.0.26与v0.0.35展平算法，修改编辑页面曲面检测没有关键点时调用曲面展平算法崩溃
//2018-11-20 11:50 V0.0.37 添加合并双页处理函数
//2018-11-21 18:08 V0.0.38 demo添加书本处理菜单项
//2018-11-26 17:00 V0.0.39 修改解决彩色封面、黑白内容书本去手指
//2018-11-27 17:34 V0.0.40 修改解决彩色书本边缘纯色去手指
//2018-11-30 12:14 V0.0.41 修改宽高比较大书本压窄，小、薄书本扭曲，刚性较小书本文本行不直,算法速度优化
//2018-11-30 16:04 V0.0.42 彩色图像补边算法修改
//2018-12-4 11:38 V0.0.43 彩色书本,黑白书本修改为分开补边
//2018-12-5 13:38 V0.0.44 补边算法彩色书页判断实现
//2018-12-6 12:22 V0.0.45 彩色书页补边算法修改
//2018-12-6 17:32 V0.0.46 展平宽度适应调整
//2018-12-10 17:53 V0.0.47 展平宽度适应调整
//2018-12-11 15:13 V0.0.48 展平宽度适应调整
//2018-12-11 17:53 V0.0.49 关键点检测崩溃边界约束条件修改
//2019-5-12 21:53 V0.0.50 手指去除优化
//2019-5-13 15:53 V0.0.51 曲面检测修改
//2019-5-13 16:59 V0.0.52 添加二值图像突出部分去除
//2019-5-27 16:29 V0.0.53 修改IRIS书本扫描崩溃
//2019-5-28 9:23 V0.0.54 对IRIS崩溃问题、修正为左右页不同处理
//2019-9-5 16:44 V0.0.55 针对修改书本展平问题；
//2020-2-25 16:44 V0.0.56 针对修改书本展平变现问题；
//2020-3-25 15:44 V0.0.57 修改书本补边算法效果；
class CBookProcess
{
//方法
///////////////////////////////////////////////////////////////////
public:
	//去除二值图像边缘的突出部
	//uthreshold、vthreshold分别表示突出部的宽度阈值和高度阈值
	//type代表突出部的颜色，0表示黑色，1代表白色 
	static void delete_jut(Mat& src, Mat& dst, int uthreshold, int vthreshold, int type);

	//************************************************************************  
	// 函数名称:    object_detection  
	// 函数说明:    图像预处理（裁切原图像，加快算法速度）
	// 函数参数:    Mat src    输入原图像
	// 函数参数:    Mat& dst   裁切后图像
	// 函数参数:    int type   0：为左页，1：为右页
	// 返 回 值:	point	   裁切图像左上角在原图中的坐标；
	//************************************************************************ 
	static Point object_detection(Mat src,Mat& dst,int type);

	//************************************************************************  
	// 函数名称:    bwlabel_mask   
	// 函数说明:    二值图像指定连通域搜索保留
	// 函数参数:    Mat bw    输入二值图像
	// 函数参数:    Point vpoint    输入指定坐标点
	// 返 回 值:    Mat  输出图像
	//************************************************************************ 
	static Mat  bwlabel_mask(Mat bw,Point vpoint);

	//************************************************************************  
	// 函数名称:    YCrCb_detect   
	// 函数说明:    YCrCb颜色空间皮肤检测按压手指检测
	// 函数参数:    Mat &src  输入原图图像
	// 返 回 值:    Mat  输出皮肤检测图像
	//************************************************************************ 
	static Mat YCrCb_detect(Mat &src);

	//************************************************************************  
	// 函数名称:    detection  
	// 函数说明:    阈值按压手指检测
	// 函数参数:    Mat &src  输入原图图像
	// 返 回 值:    Mat  输出皮肤检测图像
	//************************************************************************ 
	static Mat detection(Mat src);

	//************************************************************************  
	// 函数名称:    mcvBookStretch_Enhance  
	// 函数说明:    图像灰度对比度增强
	// 函数参数:    Mat src    输入图像
	// 返 回 值:    Mat  输出增强后灰度图像
	//************************************************************************ 
	static Mat  Enhance(Mat src,int type);

	//************************************************************************  
	// 函数名称:    BookStretch_inpaint
	// 函数说明:    曲面展平后图像黑边修复
	// 函数参数:    Mat src     输入图像
	// 返 回 值:    Mat
	//************************************************************************ 
	static Mat BookStretch_inpaint(Mat src,int type =0);

	//************************************************************************ 
	//自适应梯度二值化算法
	//[in]src				输入图像
	//[out]dst				输出二值图像
	//************************************************************************ 
	static Mat GradientAdaptiveThreshold(Mat src,float threshold);

	//************************************************************************ 
	//直方图自适应二值化算法
	//[in]src				输入图像
	//[out]dst				输出二值图像
	//************************************************************************ 
	static Mat HistAdaptiveThreshold(Mat src,bool type);

	//*************************************************************
	//描述：
	//书页二值化（阈值二值化+彩色目标二值化）
	//输入：
	//Mat src：原图
	//输出：
	//Mat ：二值图像
	//*************************************************************
	static Mat im2bw(Mat src);

	//************************************************************************  
	// 函数名称:    bwlabel   
	// 函数说明:    二值图像小连通域删除
	// 函数参数:    Mat bw    输入二值图像
	// 函数参数:    int threshold 删除连通域阈值
	// 返 回 值:    Mat  输出图像
	//************************************************************************ 
	static Mat  bwlabel(Mat bw,int threshold);

	//**************************************************************
	//合并左右页曲面检测
	//曲面检测，检测书本的上下边缘轮廓，后续算法会基于轮廓进行展平
	//输入：
	//Mat src:书本原图
	//输出：
	//vector<vector<Point2f>> &left_edges:书本左页上下边缘，其中包含两个轮廓，
	//（1）第一个Vector<Point>是上边缘
	//（2）第二个Vector<Point>是下边缘
	//vector<vector<Point2f>> &rightt_edges:书本右页上下边缘，其中包含两个轮廓，
	//（1）第一个Vector<Point>是上边缘
	//（2）第二个Vector<Point>是下边缘
	//int :返回参数（-1：失败，0：左页成功，1：右页成功）
	//***************************************************************
	static Mat DetectEdges_Double(Mat src ,vector<vector<Point2f>>& left_edges ,vector<vector<Point2f>>& right_edges);

	static vector<vector<Point>> GetBookEdgesFromContour_double(vector<Point> vPTT,Point point3,Point point4,float cols, float rows);

	//**************************************************************
	//1.曲面检测
	//曲面检测，检测书本的上下边缘轮廓，后续算法会基于轮廓进行展平
	//输入：
	//Mat src:书本原图
	//int pageType：书本页类型，0-左页，1-右页
	//输出：
	//vector<Vector<Point>>:书本上下边缘，其中包含两个轮廓，
	//（1）第一个Vector<Point>是上边缘
	//（2）第二个Vector<Point>是下边缘
	//***************************************************************
	static vector<vector<Point2f>> DetectEdges(Mat src,int pageType);

	//*************************************************************
	//描述：
	//从获取边缘关键点集合获取上下边缘轮廓集合

	//输入：
	//vector<vector<Point>> keyPoints：上下边缘关键点集合;

	//输出：
	//vector<Vector<Point>>:上下边缘轮廓集合
	//******************************************************************
	static vector<vector<Point2f>> GetEdgesFromKeyPoints(vector<vector<Point2f>> keyPoints);

	//**************************************************************
	//2.提取上下边缘关键点集合
	//从上下边缘轮廓集合获取上下边缘关键点集合，关键点有数量限制，用于在UI上操作
	//输入：
	//vector<vector<Point>>：上下边缘轮廓集合
	//int：关键点数量
	//输出：
	//vector<Vector<Point>>:上下边缘关键点集合，点集中点的数量等于第二个参数【int：关键点数量】
	//**************************************************************
	static vector<vector<Point2f>> GetKeyPointsFromEdges(vector<vector<Point2f>> edges,int keyPointsCount ,bool state = false);

	//*************************************************************
	//3.曲面展平
	//根据上下边缘关键点集合曲面展平
	//输入：
	//Mat src：原图
	//vector<vector<Point>> keyPoints：上下边缘轮廓集合，关键点
	//输出：
	//Mat dst：处理后图像
	//**************************************************************
	static Mat Flatting(Mat src, vector<vector<Point2f>> keyPoints);

	//*************************************************************
	//4.手指去除
	//输入：
	//Mat src：展平后原图
	//int pageType：书本页类型，0-左页，1-右页
	//输出：
	//Mat dst：处理后图像
	//*************************************************************
	static Mat FingerHidding(Mat src, int pageType);


	//**************************************************************
	//5.1 绘制曲面检测
	//根据曲面检测和上下边缘关键点提取结果进行绘制，用于测试
	//输入：
	//Mat src：原图（进行曲面检测的原图）
	//vector<vector<Point>> edges：
	//vector<vector<Point>> keyPoints
	//**************************************************************
	static Mat DrawEdges(Mat src, vector<vector<Point2f>> edges, vector<vector<Point2f>> keyPoints);

	
	//*************************************************************
	//描述：
	//手指检测
	//输入：
	//Mat src：展平后原图
	//输出：
	//Mat mask：手指目标掩模图像
	//*************************************************************
	static Mat FingerDetection(Mat src ,int pageType = 0);

	//*************************************************************
	//描述：
	//手指掩模修正
	//输入：
	//Mat src：展平后原图
	//Mat mask：手指目标掩模图像
	//输出：
	//Mat mask：手指目标掩模图像
	//*************************************************************
	static Mat FingerMaskCorrect(Mat src , Mat mask , int pageType,int& out_x);

	//*************************************************************
	//描述：
	//手指区域填充
	//输入：
	//Mat src：展平后原图
	//Mat mask：手指目标掩模图像
	//输出：
	//Mat dst：统填充后图像
	//*************************************************************
	static Mat FingerFill(Mat src , Mat mask , int pageType = 0);


	//*************************************************************
	//描述：
	//书页彩色判断
	//输入：
	//Mat src：展平后原图
	//Mat Fibger_mask：手指目标掩模图像
	//输出：
	//Mat mask：书页彩色区域二值图像
	//*************************************************************
	static bool ColorIdentification(Mat src,Mat Finger_mask,Mat &mask,int out_x);

	//*************************************************************
	//描述：
	//书页全页彩色判断
	//输入：
	//Mat src：展平后原图
	//输出：
	//bool
	//*************************************************************
	static bool ColorIdentification_all(Mat src);

	//**************************************************************
	//描述：
	//二值化
	//输入：
	//Mat src:书本原图
	//输出：
	//Mat：二值图像
	//***************************************************************
	static Mat Threshold(Mat src);
private:
	//**************************************************************
	//描述：
	//插值的目的是通过缩小图像，提升整体算法速度，如果存在缩小则在整体算法返回时需要完成坐标系转换
	//输入：
	//Mat src:书本原图
	//输出：
	//Mat：处理过的图像
	//Scale：缩放比例，可以为1（证明没有插值）
	//***************************************************************
	static Mat Resize(Mat src,double &scale);

	

	//**************************************************************
	//描述：
	//书本边缘提取，指获取书本上下边缘的轮廓集合
	//输入：
	//Mat bw:二值图像
	//输出：
	//vector<vector<Point>> edges:书本上下边缘轮廓，可以为空，当输入图像不合法时为空
	//***************************************************************
	static vector<vector<Point>> GetBookEdges(Mat bw);

	//**************************************************************
	//描述：
	//轮廓提取，提取最大符合条件的轮廓
	//输入：
	//Mat bw:二值化图像
	//输出：
	//vector<Point> 轮廓点集
	//***************************************************************
	static vector<Point> GetMaxBookContour(Mat src);

	//**************************************************************
	//描述：
	//判断书本外围廓是否合法，如果不合法则返回
	//输入：
	//vector<Point> 轮廓点集
	//输出：
	//vector<Point> 轮廓点集,可以为空
	//***************************************************************
	static vector<Point> CheckBookContour(vector<Point> contour);

	//**************************************************************
	//描述：
	//书本上下边缘轮廓提取。
	//输入：
	//vector<Point> contour,书本轮廓点集
	//输出：
	//vector<vector<Point>> edges:书本上下边缘轮廓，可以为空，当输入图像不合法时为空
	//***************************************************************
	static vector<vector<Point>> GetBookEdgesFromContour(Mat src ,vector<Point> contour);

	//**************************************************************
	//描述：
	//判断书本上下边缘轮廓是否合法。
	//输入：
	//vector<vector<Point>> edges:书本上下边缘轮廓，可以为空，当输入图像不合法时为空
	//输出：
	//vector<vector<Point>> edges:书本上下边缘轮廓，可以为空，当输入图像不合法时为空
	//***************************************************************
	static vector<vector<Point>> CheckBookEdges(vector<vector<Point>> edges);

	//**************************************************************
	//描述：
	//校正上下边缘轮廓，用于
	//（1）保证上下点对齐
	//（2）去除对于部分，由于厚度导致
	//输入：
	//vector<vector<Point>> edges:书本上下边缘轮廓，可以为空，当输入图像不合法时为空
	//输出：
	//vector<vector<Point>> edges:书本上下边缘轮廓，可以为空，当输入图像不合法时为空
	//***************************************************************
	static vector<vector<Point>> AdjusBookEdges(Mat src,vector<vector<Point>> edges,bool type = true);


	//************************************************************************  
	// 函数名称:    KeepParallel 
	// 函数说明:	书本边线顶点矫正
	// 函数参数:    vector<vector<Point>>& edgePoint  上下边界轮廓坐标
	// 函数参数:    int row  图像高度
	// 函数参数:    int x_max 边线的最大可能范围（约束参数）
	// 返 回 值:    vector<vector<Point>>
	//************************************************************************ 
	static vector<vector<Point>> KeepParallel(vector<vector<Point>> edgePoint, Mat src);

	//************************************************************************  
	// 函数名称:    Midline_detection  
	// 函数说明:	书本书脊顶点检测
	// 函数参数:    vector<vector<Point>>& edgePoint  上下边界轮廓坐标
	// 函数参数:    vector<int>& index 返回中线上下顶点在轮廓坐标向量中的下标
	// 函数参数:    int row  图像高度
	// 函数参数:    int col  图像宽度
	// 返 回 值:    bool
	//************************************************************************ 
	static bool Midline_detection(vector<vector<Point>> edgePoint,vector<int>& index , Mat src);

	static bool Midline_detection_1(vector<vector<Point>> edgePoint,vector<int>& index , Mat src);

	//************************************************************************  
	// 函数名称:	 Vertex_Correction
	// 函数说明:    书本边线顶点矫正
	// 函数参数:	mat src				  原图像
	//				vector<Point> areaMatvPoint  书本轮廓点
	//				Point2f point         初始顶点
	//				Point2f &point_news   新顶点
	//				int ind1			  新顶点搜索起始位置
	//				int ind2			  新顶点搜索终止位置
	//				int type			  矫正的顶点0为上顶点，1为下顶点
	// 返 回 值:   int
	//************************************************************************ 
	static int Vertex_Correction(vector<Point> areaMatvPoint,Point2f point,Point2f &point_new, int ind1,int type,int col ,int row);
	
	//************************************************************************  
	// 函数名称:	 Curve_Smooth 
	// 函数说明:    书本上下曲线轮廓平滑
	// 函数参数:	mat src				  原图像
	//				vector<Point>& vPB    下轮廓
	//				vector<Point>& vPT	  上轮廓
	// 返 回 值:    void
	//************************************************************************ 
	static void Curve_Smooth(Mat src,vector<Point>& vPB,vector<Point>& vPT, int type = 0);
	static void Curve_Smooth_1(Mat src,vector<Point>& vPB,vector<Point>& vPT, int type = 0);

	//************************************************************************  
	// 函数名称:    bwlabel_min_x   
	// 函数说明:    二值图像指定连通域左边界x坐标值（使用区域增长算法计算）
	// 函数参数:    Mat bw    输入二值图像
	// 函数参数:    Point vpoint    输入指定坐标点
	// 返 回 值:    Point
	//************************************************************************ 
	static Point  bwlabel_min_x(Mat bw,Point vpoint);

	//************************************************************************  
	// 函数名称:    GradientAdaptiveThreshold_Rect  
	// 函数说明:    区域自适应梯度二值化边缘检测
	// 函数参数:    Mat src    输入图像
	// 函数参数:    Point point_1   指定区域左上角坐标
	// 函数参数:    Point point_2   指定区域右下角坐标
	// 函数参数:    float threshold   自适应梯度放大倍数
	// 返 回 值:    Mat 
	//************************************************************************ 
	static Mat GradientAdaptiveThreshold_Rect(Mat src,Point point_1,Point point_2,float threshold);

	/*
	//************************************************************************  
	// 函数名称:    bwlabel_mask   
	// 函数说明:    二值图像指定连通域搜索保留
	// 函数参数:    Mat bw    输入二值图像
	// 函数参数:    Point vpoint    输入指定坐标点
	// 返 回 值:    Mat  输出图像
	//************************************************************************ 
	static Mat  bwlabel_mask(Mat bw,Point vpoint);*/

	

	//二值图像空洞填充
	static void fillHole(const Mat srcBw, Mat &dstBw);

	//************************************************************************  
	// 函数名称:    IM_illumination_processing          
	// 函数说明:    灰度图像光照不均匀处理 
	// 函数参数:    Mat & src_img   输入图像  
	// 函数参数:    Mat & dst_img   输出图像  
	//************************************************************************  
	static void IM_illumination_processing (const Mat src, Mat &dst_img);


	//************************************************************************  
	// 函数名称:    ACE_Enhance          
	// 函数说明:    单通道增强  
	// 函数参数:    Mat & src_img   输入图像  
	// 函数参数:    Mat & dst_img   输出图像  
	// 函数参数:    unsigned int half_winSize   增强窗口的半窗大小  
	// 函数参数:    double Max_Q            最大Q值  
	// 返 回 值:    bool  
	//************************************************************************  
	static bool  ACE_Enhance(Mat& src_img, Mat& dst_img, unsigned int half_winSize, double Max_Q);

	//************************************************************************  
	// 函数名称:    Gamma_Enhance
	// 函数说明:    伽马变换图像增强
	// 函数参数:    Mat src     输入图像
	// 返 回 值:    Mat
	//************************************************************************ 
	static Mat Gamma_Enhance(Mat src);

	//************************************************************************  
	// 函数名称:    multiScaleSharpen
	// 函数说明:    多尺度对比度提升算法
	// 函数参数:    Mat src     输入图像
	// 函数参数:    int Radius     尺度参数
	// 返 回 值:    Mat
	//************************************************************************ 
	static Mat multiScaleSharpen(Mat Src, int Radius);

	//************************************************************************  
	// 函数名称:    BrightnessBalance
	// 函数说明:    亮度平衡算法
	// 函数参数:    Mat src     输入图像
	// 返 回 值:    Mat
	//************************************************************************ 
	static Mat BrightnessBalance(Mat src);

	//************************************************************************  
	// 函数名称:    mcvBookStretch_colorEnhance 
	// 函数说明:    彩色图像增强(保真)
	// 函数参数:    Mat src     输入图像
	// 函数参数:    Mat &src    输出图像
	// 返 回 值:    bool
	//************************************************************************ 
	static bool  colorEnhance(Mat src,Mat &dst,float mean_pix,float minp,float maxp);

	
};