#pragma once
//V0.0.3	Bicycle   Time：2017年8月9日
 #include "stdafx.h"

//轮廓信息集合
class cirCtours
{
public:
	vector<Point> vpoint;
	Point2f pt2f;//圆中心
	float circleR;//半径
	float circleR1;
	float circleArea;
};

class CPunchHold
{
public:
	//去除装订孔
	//[in]src				输入图像
	//[out]dst				输出图像
	//[in]isCircularHole	是否为圆形装订孔:(true:圆形,false:方形)
	//[in]isColorFill		是否纯色填充(true:纯色,false:背景色)
	//[in]nR				红色分量：颜色分量仅在纯色填充下有效
	//[in]nG				绿色分量
	//[in]nB				蓝色分量
	static bool punchHold(Mat src,Mat &dst,bool isCircularHole =true,
		bool isColorFill =true,int nR=255,int nG =255,int nB =255);


	//计算轮廓
	//[in]src				输入图像
	//[in]isCircularHole	是否为圆形装订孔:
	static vector<vector<Point> > calcuateContours(Mat src,bool isCircularHole=true); 

private:
	//删除不符合轮廓
	//[in/out]vvPoint		输入/输出轮廓
	//[in]isCircularHole	是否为圆形装订孔:
	//[in]iwidth			图像宽
	//[in]iheight			图像高
	//返回值
	//vector<circleCtours>	装订孔信息
	static vector<cirCtours> filtrateContours(vector<vector<Point> > &vvPoint,bool isCircularHole ,
		int iwidth,int iheight);
	
	//轮廓归类,不符合的剔除
	//vCCtours				输入/输出装订孔信息
	static bool filtrateContoursByClassify(vector<cirCtours> &vCCtours);

	//分割图
	//[in/out]				输入/输出图像
	//[in]vCircle			装订空信息
	//[in]increaseN			缩放比例
	//[in]isCircularHole	是否为圆形装订孔:
	//[in]isColorFill		是否纯色填充
	static bool sipltAndRepair(Mat &src,vector<cirCtours> vCircle,float increaseN,bool isCircularHole =true,bool isColorFill =false);

	//筛选
	//[in]dst				输入图像
	//[in]vCCtours			输入装订孔集合
	//[out]true:成功，false：失败
	static bool newSplit(const Mat &dst,vector<cirCtours> &vCCtours);

	//装订孔滤除
	//[in]vCirCtours		输入装订孔集合
	//[out]true:成功，false：失败
	static bool isBindingHole(const Mat &dst,int nDif,vector<cirCtours> &vCirCtours,bool isX =true);
};

