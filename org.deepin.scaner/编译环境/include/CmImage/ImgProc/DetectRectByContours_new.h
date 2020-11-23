 #pragma once
 #include "stdafx.h"

class CDetectRectByContours_new
{
public:
	//���о�ƫ
	//����˵��:
	//[in]src��Դͼ��
	//[out]angle����б�Ƕ�
	//[out]pt�� ����CvPoint pt[4]
	//[in]rect����ע����
	//����ֵ˵��:
	//��ͼ���ϱ�ʶ�������
	static bool DetectRect(Mat src, float & angle, Point* pt, Rect rect = Rect(0,0,0,0));

	static bool DetectRect_scanner(Mat src, float & angle, Point* pt, Rect rect = Rect(0,0,0,0));

	static bool DetectRect_Book(Mat src, float & angle, Point* pt, Rect rect = Rect(0,0,0,0));

	static bool DetectRect_Multi(Mat src, MRectRArray &mRectArray, Rect rect = Rect(0,0,0,0));

	static bool DetectRect_seg(Mat &src, Point* pt);

	static Mat Resize(Mat src, float &fRiao);

	static Mat im2bw(Mat src);

	static bool grad(Mat &src ,Mat &mask);
private:
	static bool findMaxConyours(Mat bw, vector<Point2f> &PT4, int type=0);

	static bool findMaxConyours_scanner(Mat bw, Mat bw1, vector<Point2f> &PT4);

	//ȥ����ֵͼ���Ե��ͻ����
	//uthreshold��vthreshold�ֱ��ʾͻ�����Ŀ����ֵ�͸߶���ֵ
	//type����ͻ��������ɫ��0��ʾ��ɫ��1�����ɫ 
	static void delete_jut(Mat& src, Mat& dst, int uthreshold, int vthreshold, int type);

	static void  fillHole(Mat srcBw, Mat &dstBw);
};