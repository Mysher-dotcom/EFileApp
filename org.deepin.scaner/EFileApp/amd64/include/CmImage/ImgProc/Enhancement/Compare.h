#pragma once
#include "stdafx.h"
//ͼ���͵��
//V0.0.1 ʵ�ֹ���
//Jessie 2016-10-20 16:19:54
//V0.0.3 
//1�����AutoLeavel,������ջ���Ӱ��������������
//2����һ��ȥ����ֵ����������������̬ѧ�����������˲���
//Jessie 2016-10-19 14:34:16
//Mat retinex_result(Mat gray);
class CCompare
{
public:
static bool Compare(Mat Src1,Mat Src2,int nThreshold,vector<CvRect> rect);

};