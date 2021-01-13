#include "stdafx.h"
#include "Noise.h"
//图像去噪：
/*参数设置 0..................中值滤波；
           1.....................3*3高斯
           2......................5*5高斯
           3......................7*7高斯
           4......................双边滤波*/
//Jessie 2016-06-21 13:48:52

bool CNoise::Noise(Mat &src,int nMethod)
{
	if(src.empty())
	{
		return false;
	}

	switch(nMethod) 
	{
	case 0:
		//中值滤波
		medianBlur(src,src,3);
		break;
	case 1:
		//高斯滤波
		GaussianBlur(src,src,Size(3,3),0,0); 
		break;
	case 2:
		//高斯滤波
		GaussianBlur(src,src,Size(5,5),0,0); 
		break;
	case 3:
		//高斯滤波
		GaussianBlur(src,src,Size(7,7),0,0); 
		break;
	case 4:
		//双边滤波
		int nvalue=25;
		Mat dst;
		bilateralFilter (src,dst,nvalue,nvalue*2,nvalue/2);
		dst.copyTo(src);
		break;
	};
	return true;
}
