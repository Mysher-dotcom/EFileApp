#include "stdafx.h"
#include "ImageDecoder.h"

//����Jpg�ļ�DPI
#if defined(__linux__)
//	#include <sys/io.h>
#else
	#include <io.h>
#endif


//img			ͼ��ָ��
//colorValue	��ɫֵ(char[3])
//pt			��ֹ��(Point[2])
//imgInfo		ͼ����Ϣ(int[3]�ֱ��Ӧ��height,width,channel,lineSize)
void CImageDecoder::drawLineOnImgBuf(char *Img,char *colorValue,Point *pt,int *imgInfo)
{
	//��ȡ��������������
	float difx = pt[1].x -pt[0].x;
	float dify = pt[1].y -pt[0].y;
	float difxSetp =1;
	float difyStep =1;
	//��������
	int nX,nY;
	nX = nY =0;
	//�������
	if(difx ==0 && dify ==0)
		return ;
	//����任��
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
	//��ȡͼ����Ϣ
	int nWidth = imgInfo[1];
	int nHeight = imgInfo[0];
	int nChannel = imgInfo[2];
	int nLineSize = imgInfo[3];
	if(nLineSize <1)
		nLineSize =1;
	int isYU= 1;
	int idx =0;
	//��ȡ�ĵ�ǰ����λ��
	int nXtmp,nYtmp;
	nXtmp = nYtmp =-1;
	switch(nChannel)
	{
		case 2:
			{
				idx =0;
				//��֤row�����ϵ�ֱ����Ϣ
				for(int idr=pt[0].y;idr != pt[1].y;idr+=nY,idx++)
				{
					//�����С
					for(int idIn =-(nLineSize)/2;idIn <nLineSize/2;idIn++ )
					{
						//��ȡ��ǰ����
						nXtmp = idIn+ pt[0].x+(int)(difxSetp*idx);
						nYtmp = idr;
						//���귶Χ�޶�
						if(nXtmp <0 || nYtmp <0 || nXtmp >nWidth-1 || nYtmp >nHeight-1)
							continue;
						//��ɫ����
						*(Img +nYtmp*nWidth*2+nXtmp*nChannel) = colorValue[0];
						*(Img +nYtmp*nWidth*2+nXtmp*nChannel+1) = colorValue[isYU];
						 isYU++;
						 if(isYU >2)
							 isYU=1;
					}
				}
				//��֤col�����ϵ�ֱ����Ϣ
				isYU=1;
				idx =0;
				for(int idc=pt[0].x;idc != pt[1].x;idc+=nX,idx++)
				{
					//�����С
					for(int idIn =-(nLineSize)/2;idIn <nLineSize/2;idIn++ )
					{
						//��ȡ��ǰ����
						nXtmp = idc;
						nYtmp = idIn+ pt[0].y+(int)(difyStep*idx);
						//���귶Χ�޶�
						if(nXtmp <0 || nYtmp <0 || nXtmp >nWidth-1 || nYtmp >nHeight-1)
							continue;
						//��ɫ����
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
				//��֤row�����ϵ�ֱ����Ϣ
				for(int idr=pt[0].y;idr != pt[1].y;idr+=nY,idx++)
				{
					//�����С
					for(int idIn =-(nLineSize)/2;idIn <nLineSize/2;idIn++ )
					{
						//��ȡ��ǰ����
						nXtmp = idIn+pt[0].x+(int)(difxSetp*idx);
						nYtmp = idr;
						//���귶Χ�޶�
						if(nXtmp <0 || nYtmp <0 || nXtmp >nWidth-1 || nYtmp >nHeight-1)
							continue;
						//��ɫ����
						*(Img +nYtmp*nWidth*nChannel+nXtmp*nChannel) = colorValue[0];
						*(Img +nYtmp*nWidth*nChannel+nXtmp*nChannel+1) = colorValue[1];
						*(Img +nYtmp*nWidth*nChannel+nXtmp*nChannel+2) = colorValue[2];
					}
				}
				//��֤col�����ϵ�ֱ����Ϣ
				idx =0;
				for(int idc=pt[0].x;idc != pt[1].x;idc+=nX,idx++)
				{
					//�����С
					for(int idIn =-(nLineSize)/2;idIn <nLineSize/2;idIn++ )
					{
						//��ȡ��ǰ����
						nXtmp = idc;
						nYtmp = idIn+pt[0].y+(int)(difyStep*idx);
						//���귶Χ�޶�
						if(nXtmp <0 || nYtmp <0 || nXtmp >nWidth-1 || nYtmp >nHeight-1)
							continue;
						//��ɫ����
						*(Img + nYtmp*nChannel*nWidth+ nXtmp*nChannel ) = colorValue[0];
						*(Img + nYtmp*nChannel*nWidth+ nXtmp*nChannel +1) = colorValue[1];
						*(Img + nYtmp*nChannel*nWidth+ nXtmp*nChannel +2) = colorValue[2];
					}
				}
			}
			break;

		case 1:
			{
				//��֤row�����ϵ�ֱ����Ϣ
				for(int idr=pt[0].y;idr != pt[1].y;idr+=nY,idx++)
				{
					//�����С
					for(int idIn =-(nLineSize)/2;idIn <nLineSize/2;idIn++ )
					{
						//��ȡ��ǰ����
						nXtmp = idIn +pt[0].x+(int)(difxSetp*idx);
						nYtmp = idr;
						//���귶Χ�޶�
						if(nXtmp <0 || nYtmp <0 || nXtmp >nWidth-1 || nYtmp >nHeight-1)
							continue;
						//��ɫ����
						*(Img +nYtmp*nWidth+nXtmp) = colorValue[0];
					}
				}
				//��֤col�����ϵ�ֱ����Ϣ
				for(int idc=pt[0].x;idc != pt[1].x;idc+=nX,idx++)
				{
					//�����С
					for(int idIn =-(nLineSize)/2;idIn <nLineSize/2;idIn++ )
					{
						//��ȡ��ǰ����
						nXtmp = idc;
						nYtmp = idIn+ pt[0].y+(int)(difyStep*idx);
						//���귶Χ�޶�
						if(nXtmp <0 || nYtmp <0 || nXtmp >nWidth-1 || nYtmp >nHeight-1)
							continue;
						//��ɫ����
						*(Img + nYtmp*nWidth+ nXtmp ) = colorValue[0];
					}
				}
			}
			break;
	}

	return;
}
