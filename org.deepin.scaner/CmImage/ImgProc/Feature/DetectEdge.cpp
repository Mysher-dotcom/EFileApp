#include "stdafx.h"
#include "DetectEdge.h"
#include "../ColorSpace/AdaptiveThreshold.h"
//边缘检测
//参数：
//[in]Src		：	源图像
//[in]nMethod	：	边缘检测方式， 具体如下
//		0		：	X方向Sobel;
//		1		：	Y方向Sobel;
//		2		：	整体方向Sobel;	
//		3		：	Laplace变换;
//		4		：	Canny检测(固定参数);
//返回值：
//无效Mat		：	失败
//Mat			：	边缘检测图像
Mat CDetectEdge::DetectEdge(Mat src,int nType)
{
	Mat dst;
	switch(nType) 
	{
	case 0:                  
		if(!src.empty())
		{
			//dstr.copyTo(dst);
			Mat grad_x;
			Mat abs_grad_x;

			//求 X方向梯度
			Sobel( src, grad_x, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT );
			convertScaleAbs( grad_x, dst );

		}
		return dst;

	case 1:
		if(!src.empty())
		{
			Mat  grad_y;
			Mat abs_grad_y;

			//求Y方向梯度
			Sobel( src, grad_y, CV_16S, 0, 1, 3, 1, 1, BORDER_DEFAULT );
			convertScaleAbs( grad_y, dst );
		}
		return dst;
	case 2:
		//	Mat dst;
		if(!src.empty())
		{
			Mat grad_x, grad_y;
			Mat abs_grad_x, abs_grad_y;


			//求 X方向梯度
			Sobel( src, grad_x, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT );
			convertScaleAbs( grad_x, abs_grad_x );

			//求Y方向梯度
			Sobel( src, grad_y, CV_16S, 0, 1, 3, 1, 1, BORDER_DEFAULT );
			convertScaleAbs( grad_y, abs_grad_y );

			//合并梯度(近似)
			addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, dst );
		}
		return dst;
	case 3:
		//Mat dst;
		if(!src.empty())
		{
			Mat src_gray,dst_gray, abs_dst;

			//使用高斯滤波消除噪声
			GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );

			//转换为灰度图
			cvtColor( src, src_gray, COLOR_RGB2GRAY );

			//使用Laplace函数
			Laplacian( src_gray, dst_gray, CV_16S, 3, 1, 0, BORDER_DEFAULT );

			//计算绝对值，并将结果转换成8位
			convertScaleAbs( dst_gray, dst );
		}
		return dst;
	case 4:
		//Mat dst;
		if(!src.empty())
		{
			Mat srcImage1=src.clone();
			Mat dstImage,edge,grayImage;

			// 【1】创建与src同类型和大小的矩阵(dst)
			dst.create( srcImage1.size(), srcImage1.type() );

			// 【2】将原图像转换为灰度图像
			if(srcImage1.channels() == 3)
			{
				cvtColor( srcImage1, grayImage, COLOR_BGR2GRAY );
			}
			else
			{
				grayImage = srcImage1.clone();
			}


			// 【3】先用使用 3x3内核来降噪
			blur( grayImage, edge, Size(3,3) );

			// 【4】运行Canny算子
			Canny( edge, dst, 50, 150,3 );

			//【5】将dst内的所有元素设置为0 
			//dst = Scalar::all(0);

			///【6】使用Canny算子输出的边缘图g_cannyDetectedEdges作为掩码，来将原图g_srcImage拷到目标图g_dstImage中
			//srcImage1.copyTo( dst, edge);
		}
		return dst;

	default:
		if(src.empty())
		{
			dst = src.clone();
		}
		return dst;

	};//
}

//边缘检测基于，高斯核函数
//参数：
//[in]src		：	源图像
//[in]nSize		：	高斯核尺寸
//返回值：
//无效Mat		：	失败
//Mat			：	边缘检测图像
Mat CDetectEdge::DetectEdge_Gaussion(Mat src,int nSize)
{
	cv::Mat dst;

	if(!src.data)
	{
		//操作失败，返回空mat
		return dst;
	}

	//灰度化
	Mat gray;
	if(src.channels() == 3)
	{
		cvtColor(src,gray, CV_BGR2GRAY);  
	}
	else
	{
		gray = src.clone();
	}

	//使用均值核平滑图像
	Mat kernel = Mat_<float>(nSize,nSize) ;
	float fSqual = 1.0f / (nSize * nSize);
	for (int y = 0;	y < nSize; y++)
	{
		for (int x = 0;	x < nSize; x++)
		{
			  kernel.at<float>(y,x) = fSqual;
		}
	}
	Mat matFilter;
	cv::filter2D(gray, matFilter, src.depth(), kernel); 

	//通过和原图相减以及反色，生成需要的边缘提取图像
	//黑色表示边缘，白色表示背景
	Mat grad = 255 - (matFilter - gray);

	//计算阈值
	int nThreshold = CAdaptiveThreshold::OptimalThreshold(grad,0);

	//二值化
	cv::threshold(grad,dst,nThreshold,255,0);

	//开运算
	cv::erode(dst,dst,0);
	//cv::dilate(dst,dst,0);

	return dst;
}
