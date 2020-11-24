#include "stdafx.h"
#include "MergerImage.h"


//合并图像
//参数:
//vcImg			:	图像列表
//bVertical		:	是否垂直
//bFrame		:	是否对每个图像在合并后加边框
IplImage * CMergerImage::MergerImg(vector<IplImage*> vcImg, bool bVertical, bool bFrame)
{
	IplImage* dst = NULL;
	int pixel = 2;

	if(vcImg.size() > 0)
	{
		int nTotalWidth = 0;
		int nTotalHeight = 0;
		for(int n = 0; n < vcImg.size(); n++)
		{
			if(vcImg[n] != NULL)
			{
				
				//可能数据有误
				//Max 2016-09-06 17:07:46
				IplImage* tmp = vcImg[n];
				if(bVertical == true)
				{
					if(nTotalWidth < tmp->width)
					{
						nTotalWidth = tmp->width;
					}
					nTotalHeight += tmp->height + pixel;
				}
				else
				{
					if(nTotalHeight < tmp->height)
					{
						nTotalHeight = tmp->height;
					}
					nTotalWidth += tmp->width + pixel;
				}
			}
		}//end for

		dst = cvCreateImage(cvSize(nTotalWidth, nTotalHeight),8,3);
		//设置白色
		cvSet(dst, CV_RGB(255,255,255), NULL); 
		int nTotalX = 0;
		int nTotalY = 0;
		for(int n = 0; n < vcImg.size(); n++)
		{
			IplImage* tmp = vcImg[n];
			if(tmp != NULL)
			{
				cvSetImageROI(dst,cvRect(nTotalX,nTotalY,tmp->width,tmp->height));
				IplImage *tmpRGB = cvCreateImage(cvGetSize(tmp),8,3);
				if(tmp->nChannels == 1)
				{
					cvCvtColor(tmp,tmpRGB,CV_GRAY2BGR);
				}
				else
				{
					cvCopy(tmp,tmpRGB);
				}
			
				//绘制边框
				if(bFrame)
				{
					cvRectangle(tmpRGB, cvPoint(1, 1), cvPoint(tmpRGB->width-2, tmpRGB->height-2), CV_RGB(0,0,255), 1);
				}
				cvCopy(tmpRGB,dst);
				cvReleaseImage(&tmpRGB);
				cvResetImageROI(dst);
				if(bVertical)
				{
					nTotalY += tmp->height + pixel;
				}
				else
				{
					nTotalX += tmp->width + pixel;
				}
			}
		}//end for

	}//end if(vcImg.size() > 0)

	return dst;
}

IplImage * CMergerImage::MergerImg(IplImage* src, IplImage*  src1, bool bVertical, bool bFrame)
{
	vector<IplImage*> vcImg;
	vcImg.push_back(src);
	vcImg.push_back(src1);

	return CMergerImage::MergerImg(vcImg, bVertical, bFrame);
}
