/*
 * Hue Saturation Lightness Adjust
 *
 * Author: JoStudio
 */
//V0.1.0  修改颜色空间的转换模式BGR2HSL  Time:2018年1月4日14:02:07
//V0.1.2  查找表方案
//V0.1.3  优化运行速度   Time：2018年1月18日16:57:11
//V0.1.4  饱和度无效果BUG更正  Time:2018年1月29日11:34:15
#ifndef OPENCV2_PS_HSL_HPP_
#define OPENCV2_PS_HSL_HPP_
#include "stdafx.h"
//#include "opencv2/core.hpp"
using namespace cv;

namespace cv {
/**
 * Class of HSL parameters for one channel
 */
class HSLChannel {
public:
	int hue;          //色度调整值，    数据范围:  [-180, 180]
	int saturation;   //饱和度调整值	数据范围:  [-100, 100]
	int brightness;   //明度调整值，    数据范围:  [-100, 100]

	int   colorIndex; //color index: 0 = RED, 1 = YELLOW, 2 = GREEN
	float left_left;  //hue range left-left
	float left;       //hue range left
	float right;	  //hue range right
	float right_right;//hue range right-right
	bool defined;     

	HSLChannel();
	virtual ~HSLChannel();

	void calcDefined();
	void setColorIndex(int index);
	void adjust(int h, float *delta_hsb);
};

/**
 * Class of HSL
 */
class HSL {
public:
	HSL();
	virtual ~HSL();

	HSLChannel channels[7];

	bool adjustByOpencv(Mat &src);
};

} /* namespace cv */

#endif /* OPENCV2_PS_HSL_HPP_ */
