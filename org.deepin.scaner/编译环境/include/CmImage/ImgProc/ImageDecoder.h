#pragma once
//V0.0.1 Bicycle 2016��11��29��15:33:40 
//֧������jpeg,tiffѹ����ʽ��DPI��png��ѹ�������������͵ı���
//#include "tiff.h"
#include "stdafx.h"
#include <stdint.h>
class CImageDecoder
{
private:
	
public:
	//YUY2ͼ���ϻ���
	//img			ͼ��ָ��
	//colorValue	��ɫֵ(char[3])��ͨ����˫ͨ�����������ɫֵ�޹ؽ�Ҫ
	//pt			��ֹ��(Point[2])
	//imgInfo		ͼ����Ϣ(int[3]�ֱ��Ӧ��height,width,channel,lineSize)
	static void drawLineOnImgBuf(char *Img,char *colorValue,Point *pt,int *imgInfo);

	
};

