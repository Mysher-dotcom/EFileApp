#pragma once
#include "stdafx.h"
//多图裁切
//V1.0.0 Leao完成版本
//Max 2016-10-10 18:51:31

//V1.0.1 Bicycle 修改不能裁切，多裁切BUG 2017年8月1日12:00:57
#define  MAXBOXCOUNT 20

class CCropRectByCV
{
public:
	CCropRectByCV(void);
	~CCropRectByCV(void);

private:
	//图像宽高
	int m_nWidth;
	int m_nHeight;
	//参考图像缩放比例
	float m_fScale;
	//轮廓
	CvSeq * m_pContours;
	int m_nContoursCount;
	//Box
	CvBox2D m_pBoxes[MAXBOXCOUNT];
	int m_nBoxCount;
	//Box四个点
	CvPoint m_pPoints[MAXBOXCOUNT*4];
	float m_angle[MAXBOXCOUNT];
	float m_aer[MAXBOXCOUNT];

	//关注区域
	CvRect m_rect;

	//判断box是否在boxes之内 ，返回index
    bool isInTheBoxes(CvBox2D * boxes,int count,CvBox2D box,int & index);
	//过滤boxes
	int FilterBoxes(CvBox2D * srcBoxes,int srcCount);
	//获取最大的轮廓
	void GetMaxAreaContours(CvSeq * srcContours,int desCount);
	//计算轮廓
	CvSeq * GetContoursFromImage(IplImage* src,float scale,int & count);
	//计算box
	void CaculateBoxForContours(CvSeq * srcContours,int srcCount);
	//计算box四个点
	void CaculatePoint();

	//合并矩形
	void CombineBoxes();
	
public:
	void SetEffectRect(CvRect rc);
	void Process(IplImage* src,float scale);
	void GetBoxes(CvPoint * pointes[],float * m_angle[],int & boxesCount);
};

