/*
 * Hue Saturation Lightness Adjust
 *
 * Author: JoStudio
 */
//V0.1.0  修改颜色空间的转换模式BGR2HSL  Time:2018年1月4日14:02:07
//V0.1.2  查找表方案
//V0.1.3  优化运行速度   Time：2018年1月18日16:57:11
//V0.1.4  饱和度无效果BUG更正  Time:2018年1月29日11:34:15
#include "stdafx.h"
#include "HSL.hpp"
#include "ColorSpace.hpp"
#include <math.h>

#define CLIP_RANGE(value, min, max)  ( (value) > (max) ? (max) : (((value) < (min)) ? (min) : (value)) )
#define COLOR_RANGE(value)  CLIP_RANGE(value, 0, 255)

namespace cv {

HSLChannel::HSLChannel()
{
	hue = 0;
	saturation = 0;
	brightness = 0;
	defined = false;

	setColorIndex(0);
}

HSLChannel::~HSLChannel()
{

}

void HSLChannel::setColorIndex(int index)
{
	int data[][4] = {
			{  0,   0, 360, 360},
			{315, 345,  15,  45},
			{ 15,  45,  75, 105},
			{ 75, 105, 135, 165},
			{135, 165, 195, 225},
			{195, 225, 255, 285},
			{255, 285, 315, 345}
	};

	if (index < 0 ) index = 0;
	if (index > 6 ) index = 6;

	colorIndex = index;
	left_left = data[index][0];
	left      = data[index][1];
	right     = data[index][2];
	right_right = data[index][3];
}

void HSLChannel::calcDefined()
{
	if ( hue != 0 || saturation != 0 || brightness != 0 ) {
		defined = true;
		return;
	}
	defined = false;
}

void HSLChannel::adjust(int h, float *delta_hsb)
{
	if (colorIndex == 0 ) {
		delta_hsb[0] += hue;
		delta_hsb[1] += saturation;
		delta_hsb[2] += brightness;
		return;
	}

	if ( left < right ) {
		if ( h >= left_left && h <= right_right ) {
			if ( h >=left && h <= right) {
				delta_hsb[0] += hue;
				delta_hsb[1] += saturation;
				delta_hsb[2] += brightness;
				return;
			}

			if ( h >=left_left && h <= left && left > left_left) {
				delta_hsb[0] += hue * (h - left_left) / (left - left_left) ;
				delta_hsb[1] += saturation * (h - left_left) / (left - left_left);
				delta_hsb[2] += brightness * (h - left_left) / (left - left_left);
				return;
			}

			if ( h >=right && h <= right_right && right_right > right) {
				delta_hsb[0] += hue * (right_right - h) / (right_right - right);
				delta_hsb[1] += saturation * (right_right - h) / (right_right - right);
				delta_hsb[2] += brightness * (right_right - h) / (right_right - right);
				return;
			}
		}

	}
	else 
	{
		if ( h >=left && h <= 360 ) {
			delta_hsb[0] += hue;
			delta_hsb[1] += saturation;
			delta_hsb[2] += brightness;
			return;
		}

		if ( h >=0 && h <= right ) {
			delta_hsb[0] += hue;
			delta_hsb[1] += saturation;
			delta_hsb[2] += brightness;
			return;
		}

		if ( h >=left_left && h <= left && left > left_left) {
			delta_hsb[0] += hue * (h - left_left) / (left - left_left);
			delta_hsb[1] += saturation * (h - left_left) / (left - left_left);
			delta_hsb[2] += brightness * (h - left_left) / (left - left_left);
			return;
		}

		if ( h >=right && h <= right_right && right_right > right) {
			delta_hsb[0] += hue * (right_right - h) / (right_right - right);
			delta_hsb[1] += saturation * (right_right - h) / (right_right - right);
			delta_hsb[2] += brightness * (right_right - h) / (right_right - right);
			return;
		}
	}
}


//----------------------------------------------------------
//HSL class

HSL::HSL()
{
	for (int i = 0; i < 7; i++)
		channels[i].setColorIndex(i);
}

HSL::~HSL()
{
}

bool HSL::adjustByOpencv(Mat &dst)
{
	if(dst.empty() || dst.channels() !=3)
		return false;

	//Mat dst;
	//颜色空间转换
	cvtColor(dst,dst,CV_BGR2HLS);
	//calculate defined
	for (int i = 0; i < 7; i++)
		channels[i].calcDefined();
	
	uchar *data = dst.ptr<uchar>(0);
	float delta_hsb[3];
	float hsb[3];
	//遍历图像域
	for (int idr=0;idr<dst.rows;idr++)
	{
		for (int idc=0;idc<dst.cols;idc++,data+=3)
		{
			hsb[0]=(float)*data;
			hsb[1]=(float)*(data+2);
			hsb[2]=(float)*(data+1);
			
			//幅值范围缩放
			hsb[0] *= 2.0;
			hsb[1] /= 255.0;
			hsb[2] /= 255.0;

			delta_hsb[0] = delta_hsb[1] = delta_hsb[2] = 0.0;
			for (int i = 0; i < 4; i++) 
			{//只用到4个通道的V0.1.1 
				if ( channels[i].defined ) 
					channels[i].adjust(hsb[0], delta_hsb);
			}

			//adjust hue
			hsb[0] = int(hsb[0] + delta_hsb[0]) % 360;
			if ( hsb[0] <  0 ) hsb[0] += 360;

			//adjust satureation
			delta_hsb[1] = CLIP_RANGE(delta_hsb[1], -100, 100);
			if(delta_hsb[1]<0)
			{
				hsb[1] = hsb[1]*(1.0+delta_hsb[1]/100.0);
			}else
			{
				hsb[1] = hsb[1] +(1.0 -hsb[1])*delta_hsb[1]/100.0;
				hsb[2] = hsb[2] + 0.1*( 1.0 - hsb[2] ) * delta_hsb[1] / 100.0;
			}

			//adjust brightness
			delta_hsb[2] = CLIP_RANGE(delta_hsb[2], -100, 100);

			if(delta_hsb[1] >=0 || delta_hsb[2]>=0)
			{
				if(delta_hsb[1] >=0 && delta_hsb[2] >=0)
				{
					hsb[1] = hsb[1] + ( 1.0 - hsb[1] ) * delta_hsb[1] / 200.0 -0.3*hsb[1]*delta_hsb[2]/200.0; //saturation increase
					hsb[2] = hsb[2] + ( 1.0 - hsb[2] )/200.0*(delta_hsb[2]+0.1*delta_hsb[1]); //brightness increase
				}
				else
				{
					if(delta_hsb[1] >=0)
					{
						hsb[1] = hsb[1] + ( 1.0 - hsb[1]) * delta_hsb[1] / 100.0; //saturation increase
						//hsb[2] = hsb[2] + 0.1*( 1.0 - hsb[2]) * delta_hsb[1] / 100.0; //brightness increase
						hsb[2] = hsb[2] + ( 1.0 - hsb[2] )/200.0*(delta_hsb[2]+0.1*delta_hsb[1]); //brightness increase
						hsb[2] = hsb[2]<0?0:hsb[2];
					}
					else
					{
						hsb[2] = hsb[2] + ( 1. - hsb[2] ) * delta_hsb[2] / 100.0; //brightness increase
						hsb[1] = hsb[1] - 0.3*hsb[1]  * delta_hsb[2] / 100.0; //saturation decrease
					}
				}
			}
			else if(delta_hsb[1] < 0)
				hsb[1] = hsb[1] * (1.0 + delta_hsb[1] / 100.0);
			else if( delta_hsb[2] < 0)
				hsb[2] = hsb[2] * (1.0 + delta_hsb[2] / 100.0);

			*data = hsb[0]/2.0;
			*(data+1) =hsb[2]*255.0;
			*(data+2) =hsb[1]*255.0;
		}
	}
	cvtColor(dst,dst,CV_HLS2BGR);
	
	return true;
}

} /* namespace cv */
