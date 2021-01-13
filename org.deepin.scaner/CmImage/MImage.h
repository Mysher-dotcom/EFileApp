//THIS FILE IS PART OF MysherImage PROJECT
//MysherImage is a digital image processing library, by the BeiJing Myhser Technology Co.Ltd. independent research and development.
//MImage.h - The data structure declarations of the MysherImage library
//
// MysherImage V6.3.0.1
// Copyright (C) 2016-2026 Beijing Mysher Technology CO., LTD All rights reserved
// Use in commercial applications requires written permission
// This software is provided "as is", with no warranty.
#pragma once
#include <string.h>
#include <vector>
using namespace std;
class MImage
{
public:
	int width;
	int height;
	int channel;
	int widthStep;
	char *data;
	long dataIndex;
	int xDPI;
	int yDPI;

	MImage()
	{
		this->width = 0;
		this->height = 0;
		this->channel = 0;
		this->widthStep = 0;
		this->data = 0;
		this->dataIndex = -1;
		xDPI = 96;
        yDPI = 96;
	}//MImage()
	~MImage()
	{
		if(this->data!=0)
		{
			delete this->data;
			this->data = 0;
		}
	}//end  ~MImage()
};

class MPoint
{
public:
	int x;
	int y;
	MPoint()
	{
		this->x = 0;
		this->y = 0;
	}
	MPoint(int nX, int nY)
	{
		this->x = nX;
		this->y = nY;
	}	
};

class MPointF
{
public:
	float x;
	float y;
	MPointF()
	{
		this->x = 0;
		this-> y = 0;
	}
};

class MRect
{
public:
	int m_nLeft;
	int m_nTop;
	int m_nRight;
	int m_nBotton;

    MRect(int nLeft,int nTop,int nRight,int nBottom)
	{
		this->m_nLeft = nLeft;
		this->m_nRight=nRight;
		this->m_nTop=nTop;
        this->m_nBotton=nBottom;
	}
	MRect()
	{
		this->m_nLeft = 0;
		this->m_nRight= 0;
		this->m_nTop= 0;
		this->m_nBotton= 0;
	}
};

class MRectF
{
public:
	float m_nLeft;
	float m_nTop;
	float m_nRight;
	float m_nBotton;

	MRectF(float nLeft,float nTop,float nRight,float nBorrom)
	{
		this->m_nLeft = nLeft;
		this->m_nRight=nRight;
		this->m_nTop=nTop;
		this->m_nBotton=nBorrom;
	}
	MRectF()
	{
		this->m_nLeft = 0;
		this->m_nRight= 0;
		this->m_nTop= 0;
		this->m_nBotton= 0;
	}
	MRectF(MRectF &rectF)
	{
		this->m_nLeft = rectF.m_nLeft;
		this->m_nRight= rectF.m_nRight;
		this->m_nTop= rectF.m_nTop;
		this->m_nBotton= rectF.m_nBotton;
	}
};

class MRectFArray
{
public:
	MRectF m_rectF[100];
	int m_nCount;
	MRectFArray()
	{
		this->m_nCount = 0;
	}
	void Add(MRectF rectF)
	{
		this->m_rectF[this->m_nCount] = MRectF(rectF);
		this->m_nCount ++;
	}
};

class MRectR
{
public:
	MPoint m_pt[4];
	float m_fAngle;
	MRectR()
	{
		for(int n =0; n < 4; n++)
		{
			this->m_pt[n] = MPoint(0,0);
		}
		this->m_fAngle = 0;
	}//end MRectR()
	MRectR(MPoint pt[4], float fAngle )
	{
		for(int n =0; n < 4; n++)
		{
			this->m_pt[n] = pt[n];
		}
		this->m_fAngle = fAngle;
	}//end MRectR(MPoint pt[4], float fAngle )
};

class MRectRArray
{
public:
	MRectR m_rectR[100];
	int m_nCount;
	MRectRArray()
	{
		this->m_nCount = 0;
	}
};

class MColor
{
public:
	int m_nR;
	int m_nG;
	int m_nB;
	MColor(int nR, int nG, int nB)
	{
		this->m_nR = nR;
		this->m_nG = nG;
		this->m_nB = nB;
	}
	MColor()
	{
		this->m_nR = 0;
		this->m_nG = 0;
		this->m_nB = 0;
	}
};

class MCharPTArr
{
public:
	char m_ChPt[10][256];
	int m_nCount;
	MCharPTArr()
	{
		this->m_nCount = 0;
		::memset(this->m_ChPt,0,sizeof(this->m_ChPt));
	}
	bool Add(char* chPt)
	{
		if(chPt!= NULL)    
		{
            memset(this->m_ChPt[this->m_nCount],0,256);
			::memcpy(this->m_ChPt[this->m_nCount],chPt,256);
			this->m_nCount ++; 
			return true;
		}
		else
		{
			return false;
		}
	}
};

class MBookInfo
{
public:
	MPoint _arrPT_Top[5000];
	MPoint _arrPT_Bottom[5000];
	MPoint _arrPTcontrol_Top[30];
	MPoint _arrPTcontrol_Bottom[30];
	int _arrPT_Top_Count;
	int _arrPT_TopControl_Count;
	int _arrPT_Bottom_Count;
	int _arrPT_BottomControl_Count;
	MBookInfo()
	{
		this->_arrPT_Bottom_Count=0;
		this->_arrPT_Top_Count =0;
		this->_arrPT_BottomControl_Count=0;
		this->_arrPT_TopControl_Count=0;
	}
	MBookInfo& operator =(MBookInfo mbook1)
	{
		::memcpy(this->_arrPT_Top,mbook1._arrPT_Top,sizeof(this->_arrPT_Top));
		::memcpy(this->_arrPT_Bottom,mbook1._arrPT_Bottom,sizeof(this->_arrPT_Bottom));
		::memcpy(this->_arrPTcontrol_Top,mbook1._arrPTcontrol_Top,sizeof(this->_arrPTcontrol_Top));
		::memcpy(this->_arrPTcontrol_Bottom,mbook1._arrPTcontrol_Bottom,sizeof(this->_arrPTcontrol_Bottom));

		this->_arrPT_Top_Count = mbook1._arrPT_Top_Count;
		this->_arrPT_TopControl_Count = mbook1._arrPT_TopControl_Count;
		this->_arrPT_Bottom_Count =mbook1._arrPT_Bottom_Count;
		this->_arrPT_BottomControl_Count =mbook1._arrPT_BottomControl_Count;

		return *this;
	}
};
