#pragma once
//V0.0.1 Bicycle 2016年11月30日15:16:23
//V1.0.0 Bic 2018年9月10日 修改崩溃BUG，原因：在数据转换的时候将原图尺寸改动，最终结果未进行补位。
//实现绘制图像水印
#include "stdafx.h"
#include "MImage.h"

class CWaterMark
{
public:
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
	static Mat WaterMark2(Mat src, 
		char * content = "Water Mark",
		char *  font = "Arial",long fontSize = 72,
		long colorR = 255, 
		long colorG = 0, 
		long colorB = 0,
		float transparent = 255, 
		int layout = 0);

	static Mat WaterMark(Mat image, 
		char* msg = "Water Mark Test", 
		int x = 0, 
		int y = 0, 
		long fontSize = 72, 
		char* font = "Arial", 
		bool IsItalic = false, 
		bool IsUnderline = false, 
		bool IsWeight = false,
		float angle = 0, 
		float transparent = 255, 
		long colorR = 255, 
		long colorG = 0, 
		long colorB = 0,
		float fStringRatio =0,
		float fStringRatioH =0);

		//创建Font
		//参数说明：
		//（1）font：字体：string
		//（2）size：表示字体高度：int
		//（3）IsItalic：是否斜体：bool
		//（4）IsUnderline：是否下划线：bool
		//（5）IsWeight：是否粗体：bool
		//static Gdiplus::Font CreateFont(char*  font, int size, bool IsItalic, bool IsUnderline, bool IsWeight);
};

