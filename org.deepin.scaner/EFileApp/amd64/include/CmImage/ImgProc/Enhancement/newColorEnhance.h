#pragma once
#include "stdafx.h"
//��ɫ��ǿ�㷨
//Tim.liu 2019-12-26 10:20  V0.0.1
//Tim.liu 2020-2-6 17:00  V0.0.2 �Ż��鱾�ڰ��ĵ�ֱ�Ӻ��ߣ��Ż����ղ����ȳ�����Ӱ��
//Tim.liu 2020-2-7 14:22  V0.0.3 �Ż����ղ����ȳ�����Ӱ��
//Tim.liu 2020-2-21 14:22  V0.0.4 ������ɫ��ǿ�㷨��
//Tim.liu 2020-3-9 9:32  V0.0.5 �ĵ��Ż��㷨���£�
//Tim.liu 2020-3-12 15:15  V0.0.6 �ĵ��Ż��㷨�������ף�����ͼ����������


class CnewColorEnhance
{
//����
///////////////////////////////////////////////////////////////////
public:

	//************************************************************************  
	// ��������:    black_whiteEnhance
	// ����˵��:    �ڰ��ĵ���ǿ�㷨
	// ��������:    Mat src    ����RGBԭͼ��
	// �� �� ֵ:	Mat	   �����ͼ��
	//************************************************************************ 
	static  Mat black_whiteEnhance(cv::Mat src, bool type = false);

	//************************************************************************  
	//Adaptive Contrast Enhancement������Ӧ�Աȶ���ǿ��ACE��
	//�������ܣ���ȡͼ��ľֲ���ֵ��ֲ���׼���ͼ
	//�������ƣ�adaptContrastEnhancement
	//����������Mat &scr������ͼ��Ϊ��ͨ��RGBͼ��
	//����������Mat &dst����ǿ������ͼ��Ϊ��ͨ��RGBͼ��
	//����������float fratioTop��������
	//����������float fratioBot��������
	//�������ͣ�bool
	//************************************************************************  
	static bool photoshopEnhancement(Mat &src, float fratioTop = 0.05 ,float fratioBot = 0.05);

	static vector<float> findThreshold(Mat src,int thresh);

	//************************************************************************  
	// ��������:    Color_HomoFilter 
	// ����˵��:    ̬ͬ�˲���ɫ��ǿ�㷨
	// ��������:    Mat src    ����RGBԭͼ��
	// �� �� ֵ:	Mat	   �����ͼ��
	//************************************************************************ 
	static Mat Color_HomoFilter(cv::Mat src);

	static void ALTMRetinex(const Mat src, Mat &dst, bool LocalAdaptation = false, bool ContrastCorrect = true);

	 //�������ܣ���ȡͼ��ľֲ���ֵ��ֲ���׼���ͼ
	//�������ƣ�adaptContrastEnhancement
	//����������Mat &scr������ͼ��Ϊ��ͨ��RGBͼ��
	//����������Mat &dst����ǿ������ͼ��Ϊ��ͨ��RGBͼ��
	//����������int winSize���ֲ���ֵ�Ĵ��ڴ�С��ӦΪ������
	//����������int maxCg����ǿ���ȵ����ޣ�
	//�������ͣ�bool
	static bool adaptContrastEnhancement(Mat &scr, Mat &dst, int winSize,int maxCg);

	static void OnYcbcrY(Mat &workImg);
private:
	//************************************************************************  
	// ��������:    HomoFilter 
	// ����˵��:    ̬ͬ�˲��㷨
	// ��������:    Mat src    ���뵥ͨ��ͼ��
	// �� �� ֵ:	Mat	   �����ͼ��
	//************************************************************************ 
	static Mat HomoFilter(cv::Mat src);

	 //�����˲���
	static Mat guidedFilter(cv::Mat& I, cv::Mat& p, int r, float eps);

	static bool getVarianceMean(Mat src, Mat &localMeansMatrix, Mat &localVarianceMatrix, int winSize);

	static int ImageStretchByHistogram(IplImage *src1,IplImage *dst1);

	//����ͼ��sobel�ݶ�
	//[in]src			����ͼ��
	static Mat im2bw(Mat src,Mat &mean_1);

	static int unevenLightCompensate(Mat &image, int blockSize);//ͼ�����Ⱦ���

	static bool imsubtract(Mat src1, Mat &src2);

	static void  fillHole(Mat srcBw, Mat &dstBw);
};