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

    static Mat Resize(Mat src, float &fRiao);

	static Mat im2bw(Mat src);

    static bool grad(Mat &src ,Mat &mask);
private:
    static bool findMaxConyours(Mat bw, vector<Point2f> &PT4, int type=0);
};
