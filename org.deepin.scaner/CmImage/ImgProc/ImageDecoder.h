#pragma once
//V0.0.1 Bicycle 2016年11月29日15:33:40 
//支持设置jpeg,tiff压缩方式，DPI；png的压缩；及其它类型的保存
//#include "tiff.h"
#include "stdafx.h"
#include <stdint.h>
class CImageDecoder
{
private:
	
public:
	//YUY2图像上绘线
	//img			图像指针
	//colorValue	颜色值(char[3])单通道，双通道，后面的颜色值无关紧要
	//pt			起止点(Point[2])
	//imgInfo		图像信息(int[3]分别对应：height,width,channel,lineSize)
	static void drawLineOnImgBuf(char *Img,char *colorValue,Point *pt,int *imgInfo);

	
};

