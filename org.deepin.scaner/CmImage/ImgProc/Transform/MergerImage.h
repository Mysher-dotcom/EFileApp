#pragma once
//对图像进行合并，切分等工作
//Max 2016-09-08 10:17:08
class CMergerImage
{
public:
	//合并图像
	//参数:
	//vcImg			:	图像列表
	//bVertical		:	是否垂直
	//bFrame		:	是否对每个图像在合并后加边框
	static IplImage * MergerImg(vector<IplImage*> vcImg, bool bVertical = true, bool bFrame = false);
	static IplImage * MergerImg(IplImage* src, IplImage*  src1, bool bVertical = true, bool bFrame = false);
};

