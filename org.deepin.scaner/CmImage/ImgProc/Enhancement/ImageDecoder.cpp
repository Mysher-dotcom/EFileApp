#include "stdafx.h"
#include "ImageDecoder.h"

//更改Jpg文件DPI
#if defined(__linux__)
//	#include <sys/io.h>
#else
	#include <io.h>
#endif


//img			图像指针
//colorValue	颜色值(char[3])
//pt			起止点(Point[2])
//imgInfo		图像信息(int[3]分别对应：height,width,channel,lineSize)
void CImageDecoder::drawLineOnImgBuf(char *Img,char *colorValue,Point *pt,int *imgInfo)
{
	//获取个方向增长趋势
	float difx = pt[1].x -pt[0].x;
	float dify = pt[1].y -pt[0].y;
	float difxSetp =1;
	float difyStep =1;
	//增长趋势
	int nX,nY;
	nX = nY =0;
	//错误情况
	if(difx ==0 && dify ==0)
		return ;
	//求出变换率
	if(dify !=0)
	{
		difxSetp = difx/abs(dify);
		nY = dify/abs(dify);
	}
	if(difx !=0)
	{
		difyStep = dify/abs(difx);
		nX = difx/abs(difx);
	}
	//获取图像信息
	int nWidth = imgInfo[1];
	int nHeight = imgInfo[0];
	int nChannel = imgInfo[2];
	int nLineSize = imgInfo[3];
	if(nLineSize <1)
		nLineSize =1;
	int isYU= 1;
	int idx =0;
	//获取的当前坐标位置
	int nXtmp,nYtmp;
	nXtmp = nYtmp =-1;
	switch(nChannel)
	{
		case 2:
			{
				idx =0;
				//保证row方向上的直线信息
				for(int idr=pt[0].y;idr != pt[1].y;idr+=nY,idx++)
				{
					//字体大小
					for(int idIn =-(nLineSize)/2;idIn <nLineSize/2;idIn++ )
					{
						//获取当前坐标
						nXtmp = idIn+ pt[0].x+(int)(difxSetp*idx);
						nYtmp = idr;
						//坐标范围限定
						if(nXtmp <0 || nYtmp <0 || nXtmp >nWidth-1 || nYtmp >nHeight-1)
							continue;
						//颜色覆盖
						*(Img +nYtmp*nWidth*2+nXtmp*nChannel) = colorValue[0];
						*(Img +nYtmp*nWidth*2+nXtmp*nChannel+1) = colorValue[isYU];
						 isYU++;
						 if(isYU >2)
							 isYU=1;
					}
				}
				//保证col方向上的直线信息
				isYU=1;
				idx =0;
				for(int idc=pt[0].x;idc != pt[1].x;idc+=nX,idx++)
				{
					//字体大小
					for(int idIn =-(nLineSize)/2;idIn <nLineSize/2;idIn++ )
					{
						//获取当前坐标
						nXtmp = idc;
						nYtmp = idIn+ pt[0].y+(int)(difyStep*idx);
						//坐标范围限定
						if(nXtmp <0 || nYtmp <0 || nXtmp >nWidth-1 || nYtmp >nHeight-1)
							continue;
						//颜色覆盖
						*(Img + nYtmp*2*nWidth+ nXtmp*nChannel ) = colorValue[0];
						*(Img + nYtmp*2*nWidth+ nXtmp*nChannel +1) = colorValue[isYU];
						isYU++;
						 if(isYU >2)
							 isYU=1;
					}
				}
			}
			break;

		case 3:
			{
				idx =0;
				//保证row方向上的直线信息
				for(int idr=pt[0].y;idr != pt[1].y;idr+=nY,idx++)
				{
					//字体大小
					for(int idIn =-(nLineSize)/2;idIn <nLineSize/2;idIn++ )
					{
						//获取当前坐标
						nXtmp = idIn+pt[0].x+(int)(difxSetp*idx);
						nYtmp = idr;
						//坐标范围限定
						if(nXtmp <0 || nYtmp <0 || nXtmp >nWidth-1 || nYtmp >nHeight-1)
							continue;
						//颜色覆盖
						*(Img +nYtmp*nWidth*nChannel+nXtmp*nChannel) = colorValue[0];
						*(Img +nYtmp*nWidth*nChannel+nXtmp*nChannel+1) = colorValue[1];
						*(Img +nYtmp*nWidth*nChannel+nXtmp*nChannel+2) = colorValue[2];
					}
				}
				//保证col方向上的直线信息
				idx =0;
				for(int idc=pt[0].x;idc != pt[1].x;idc+=nX,idx++)
				{
					//字体大小
					for(int idIn =-(nLineSize)/2;idIn <nLineSize/2;idIn++ )
					{
						//获取当前坐标
						nXtmp = idc;
						nYtmp = idIn+pt[0].y+(int)(difyStep*idx);
						//坐标范围限定
						if(nXtmp <0 || nYtmp <0 || nXtmp >nWidth-1 || nYtmp >nHeight-1)
							continue;
						//颜色覆盖
						*(Img + nYtmp*nChannel*nWidth+ nXtmp*nChannel ) = colorValue[0];
						*(Img + nYtmp*nChannel*nWidth+ nXtmp*nChannel +1) = colorValue[1];
						*(Img + nYtmp*nChannel*nWidth+ nXtmp*nChannel +2) = colorValue[2];
					}
				}
			}
			break;

		case 1:
			{
				//保证row方向上的直线信息
				for(int idr=pt[0].y;idr != pt[1].y;idr+=nY,idx++)
				{
					//字体大小
					for(int idIn =-(nLineSize)/2;idIn <nLineSize/2;idIn++ )
					{
						//获取当前坐标
						nXtmp = idIn +pt[0].x+(int)(difxSetp*idx);
						nYtmp = idr;
						//坐标范围限定
						if(nXtmp <0 || nYtmp <0 || nXtmp >nWidth-1 || nYtmp >nHeight-1)
							continue;
						//颜色覆盖
						*(Img +nYtmp*nWidth+nXtmp) = colorValue[0];
					}
				}
				//保证col方向上的直线信息
				for(int idc=pt[0].x;idc != pt[1].x;idc+=nX,idx++)
				{
					//字体大小
					for(int idIn =-(nLineSize)/2;idIn <nLineSize/2;idIn++ )
					{
						//获取当前坐标
						nXtmp = idc;
						nYtmp = idIn+ pt[0].y+(int)(difyStep*idx);
						//坐标范围限定
						if(nXtmp <0 || nYtmp <0 || nXtmp >nWidth-1 || nYtmp >nHeight-1)
							continue;
						//颜色覆盖
						*(Img + nYtmp*nWidth+ nXtmp ) = colorValue[0];
					}
				}
			}
			break;
	}

	return;
}
