#include "stdafx.h"
#include "../Transform/Rotate.h"
#include "DetectRectByContours.h"
#include "../ColorSpace/AdaptiveThreshold.h"

//V1.0.15 Bic 10.15
//#include "LogWriter.h"
//CLogWriter *g_log = CLogWriter::GetLogWriter();
#define  CV_WRITE_LOG 0
CvSeq* MergerCvSeq(CvSeq * c,CvSeq * c2,CvMemStorage *storage)
{
	CvSeq* allpointsSeq = cvCreateSeq(CV_SEQ_KIND_GENERIC|CV_32SC2, sizeof(CvContour), sizeof(CvPoint), storage);
	CvSeqReader reader;
	CvPoint pt = cvPoint(0,0);
	cvStartReadSeq(c,&reader);  
	for(int i = 0;i<c->total;i++){
	  CV_READ_SEQ_ELEM(pt,reader);
	  cvSeqPush(allpointsSeq,&pt);
	}
	cvStartReadSeq(c2,&reader);
	for(int i = 0;i<c2->total;i++){
	 CV_READ_SEQ_ELEM(pt,reader);
	 cvSeqPush(allpointsSeq,&pt);
	}
	return allpointsSeq;
}

//在银行卡合并时通过是否平行判断
//参数说明：
//c：银行卡大部分图像
//c2：银行卡小部分图像
//返回值：
//true：需要合并
//false：不需要合并
bool IsNeedMerger4CardByParallel(CvSeq * c,CvSeq * c2)
{
	if(c == 0 || c2 == 0)
	{
		return false;
	}
	CvBox2D boxC = cvMinAreaRect2(c); //通过轮廓获取最小外接矩形
	RotatedRect rrC = RotatedRect(boxC); //获取外接矩形RotatedRect对象
	Point2f ptC[4];
	rrC.points(ptC);

	CvBox2D boxC2 = cvMinAreaRect2(c2); //通过轮廓获取最小外接矩形
	RotatedRect rrC2 = RotatedRect(boxC2); //获取外接矩形RotatedRect对象
	Point2f ptC2[4];
	rrC2.points(ptC2);

	//分别计算两个部件不平行的两条线角度
	float f0C = atan((float)(ptC[0].y - ptC[1].y)/(ptC[0].x - ptC[1].x))*180/CV_PI;
	float f1C = atan((float)(ptC[1].y - ptC[2].y)/(ptC[1].x - ptC[2].x))*180/CV_PI;
	float f0C2 = atan((float)(ptC2[0].y - ptC2[1].y)/(ptC2[0].x - ptC2[1].x))*180/CV_PI;
	float f1C2 = atan((float)(ptC2[1].y - ptC2[2].y)/(ptC2[1].x - ptC2[2].x))*180/CV_PI;
	double dDegree = 2;
	//通过平行判断
	if(abs(f0C - f0C2) < dDegree && abs(f1C - f1C2) < dDegree)
	{
		return true;
	}

	return false;
}

//判断是否为合适的轮廓
//1.轮廓面积不能太小（至少占总面积1/40）
//2.保证轮廓中心不在图像边界
//3.通过面积比值（真实面积/最小外接矩形面积）判断是否为矩形
//参数:
//c:待查轮廓
//nSizeTh:轮廓面积阈值
//vcRadioSizeTh:面积阈值，通过面积比值（真实面积/最小外接矩形面积）判断是否为矩形
//大阈值:普通轮廓
//小阈值:超出图像轮廓（因为超出图像，所以面积比值需要较小）
//vcBordePT:底板边缘左上角，右下角
//返回值:
bool IsGoodContour(CvSeq * c,int nSizeTh,vector<float> vcRadioSizeTh,vector<CvPoint> vcBordePT)
{
	CvBox2D boxC = cvMinAreaRect2(c); //通过轮廓获取最小外接矩形
	int nArea = (int)(abs(cvContourArea(c)) + 0.5); //轮廓真实面积
	int nSize = boxC.size.width * boxC.size.height; //最小外接矩形面积
	RotatedRect RR_test = RotatedRect(boxC); //获取外接矩形RotatedRect对象
	Point2f pt_test[4];
	RR_test.points(pt_test);
	float fRadioSize_Th0 = vcRadioSizeTh[0];
	float fRadioSize_Th1 = vcRadioSizeTh[1];
	CvPoint tpl = vcBordePT[0];//底板边缘左上角
	CvPoint lpr = vcBordePT[1];//底板边缘右下角
	//轮廓面积不能太小（至少占总面积1/40）
	if(nSize>nSizeTh)
	{
		//保证轮廓中心不在图像边界
#if 0
		if(tpl.x<boxC.center.x&&boxC.center.x<lpr.x&&tpl.y<boxC.center.y&&boxC.center.y<lpr.y)
		{
			float fRadioSize = (float)nArea / nSize;
			float fRadioSize_Th = 1;
			Point2f pt_center = RR_test.center;
			if(pt_test[0].x<tpl.x||pt_test[0].y<tpl.y||
				pt_test[1].x<tpl.x||pt_test[1].y<tpl.y||
				pt_test[2].x<tpl.x||pt_test[2].y<tpl.y||
				pt_test[3].x<tpl.x||pt_test[3].y<tpl.y||
				pt_test[0].x>lpr.x||pt_test[0].y>lpr.y||
				pt_test[1].x>lpr.x||pt_test[1].y>lpr.y||
				pt_test[2].x>lpr.x||pt_test[2].y>lpr.y||
				pt_test[3].x>lpr.x||pt_test[3].y>lpr.y
				)
			{
				fRadioSize_Th = fRadioSize_Th1;
			}
			else
			{
				fRadioSize_Th = fRadioSize_Th0;
			}
			//通过面积比值（真实面积/最小外接矩形面积）判断是否为矩形
			if (fRadioSize > fRadioSize_Th)
			{
				return true;
			}
		}
#else
		return true;
#endif
	}//if(nSize>nSizeTh)

	return false;
}

//DetectRect中筛选最终的选取轮廓(pMax)
void filtratePMax(CvSeq** pMax, float & angle, CvPoint* pt)
{
	CvSeq * pMaxCopy = *pMax;
	CvSeq *pTmp = NULL;

	int nMaxArea = 0;
#if 1
	int idx =0;
	int nMaxIdx =-1;
	for(CvSeq * cptr = pMaxCopy; cptr != NULL ; cptr = cptr->h_next,idx++)
	{
		CvBox2D boxC = cvMinAreaRect2(cptr);

		double nAreaTmp =fabs(cvContourArea(cptr));
		//int nAreaTmp = boxC.size.height*boxC.size.width;
		if(nAreaTmp > nMaxArea)
		{
			nMaxArea = nAreaTmp;
			nMaxIdx = idx;
			pTmp = cptr;
		}
	}


#else
	int idx =0;
	for(;pMaxCopy;pMaxCopy->h_next,idx++)
	{
		CvBox2D boxC = cvMinAreaRect2(pMaxCopy);

		int nAreaTmp = boxC.size.height*boxC.size.width;
		if(nAreaTmp > nMaxArea)
		{
			nMaxArea = nAreaTmp;
			cvSeqRemove(*pMax,idx);
		}
	}

#endif

	*pMax = pTmp;
	return ;
}

//内部-寻找目标轮廓
CvSeq* CDetectRectByContours::findObjectContour(IplImage *bw,float fAreaRatio,int nSizeTh,vector<float> vcRadioSizeTh,vector<CvPoint> vcBordePT,CvMemStorage *pStorage,Rect crect)
{
	CvSeq *pContour = NULL;
	CvSeq *pMax = NULL;
	// 执行条件   
	if (bw)
	{
		//查找所有轮廓   
		pStorage = cvCreateMemStorage(0);
		cvFindContours(bw, pStorage, &pContour, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		
		//遍历轮廓,找出适合的最大轮廓
		for (CvSeq * c = pContour; c != NULL; c = c->h_next)
		{
			if(IsGoodContour(c,nSizeTh,vcRadioSizeTh,vcBordePT))
			{
				CvBox2D boxC = cvMinAreaRect2(c);
				double dcontourArea = cvContourArea(c);
				if(dcontourArea/double(boxC.size.width*boxC.size.height) < fAreaRatio)
					continue;

				if((boxC.center.x < crect.x -crect.width/2 || boxC.center.x >crect.x +crect.width*2 )&&crect.width >0 &&crect.height >0)
					continue;

				if (pMax == NULL)
				{
					pMax = c;
				}
				else
				{
					CvBox2D boxMax = cvMinAreaRect2(pMax);
					
					if (boxMax.size.width * boxMax.size.height < boxC.size.width * boxC.size.height)
					{
						pMax = c;
					}
				}
			}
		}//end for

		//调用轮廓合并函数
		CDetectRectByContours::contourMerge(pContour,pMax,pStorage,bw->width,bw->height);
	}
	
	return pMax;
}
#if 1
vector<Point> CDetectRectByContours::findObjectContour1(const Mat &bw,float fAreaRatio,int nSizeTh,vector<float> vcRadioSizeTh,vector<Point> vcBordePT,Rect crect)
{
	vector<Point> pMax;
	vector<vector<Point> > vvContoursPass;
	// 执行条件   
	if ( bw.data != NULL)
	{
		//查找所有轮廓   
		std::vector<std::vector<cv::Point> > vvContours;
		findContours(bw,vvContours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);

		//遍历轮廓,找出适合的最大轮廓
		for (vector<vector<Point> >::iterator itr = vvContours.begin(); itr != vvContours.end();itr++)
		{
			if(itr->size() <10)
				continue;

			vvContoursPass.push_back(*itr);
			RotatedRect rrect = minAreaRect(*itr);
			double dRectArea = rrect.size.height*rrect.size.width;
			if(dRectArea > nSizeTh)
			{
				double dArea = contourArea(*itr);

				if(dArea/dRectArea <fAreaRatio)
					continue;

				if((rrect.center.x < crect.x + crect.width/8 || rrect.center.x > crect.x + crect.width*7/8 )&&crect.width >0 &&crect.height >0)
					continue;

				if(pMax.size() ==0)
				{
					pMax = *itr;
				}
				else
				{
					RotatedRect rrectMax = minAreaRect(pMax);

					if(rrectMax.size.width*rrectMax.size.height < dRectArea)
						pMax =*itr;
				}
			}
		}

		//调用轮廓合并函数
		CDetectRectByContours::contourMerge(vvContoursPass,pMax,bw.cols,bw.rows);
	}

	return pMax;
}
#endif
//计算矩形的旋转角度，四个点坐标
//[in]pMax			输入的图像轮廓
//[in/out]angle		矩形的旋转角度
//[in/out]pt		返回的矩形四个点坐标
//[in]nTeshold		缩放比例（模板缩放比例）
//[in]fRadio		缩放比例(整图缩放比例)
//[in]dif_edge1，dif_edge2，dif_edge3，dif_edge4	新旧方案四个边之间的距离差
bool calcPara(CvSeq* pMax,float & angle, CvPoint2D32f* pt,int nTreshold,float fRadio,int dif_edge1,int dif_edge2,int dif_edge3,int dif_edge4)
{
	if (pMax != NULL)
	{
		CvBox2D box;
		CvBox2D box_temp;
		box_temp = cvMinAreaRect2(pMax);
		cv::RotatedRect rotateRect1=cv::RotatedRect(box_temp);

		for (CvSeq * c = pMax; c != NULL; c = c->h_next)
		{
			box = cvMinAreaRect2(c);
			if(abs(box.angle -angle)>85)
			{
				int nTmp = box.size.width;
				box.size.width = box.size.height;
				box.size.height = nTmp;
			}
			box.angle  = angle;

			cv::RotatedRect rotateRect = cv::RotatedRect(box);
			
			//根据新旧四个变之间的距离差，对矩形的四个边进行纠正
			if(dif_edge1/nTreshold < -4)
			{
				rotateRect.center.x -= dif_edge1/2;
				rotateRect.size.width -= dif_edge1;
			}

			
			if( dif_edge2/nTreshold > 4)
			{
				rotateRect.center.x -=dif_edge2/2;
				rotateRect.size.width += dif_edge2;
			}

			
			if( dif_edge3/nTreshold < -4)
			{
				rotateRect.center.y -=dif_edge3/2;
				rotateRect.size.height -=dif_edge3;
			}

			
			if(dif_edge4/nTreshold > 4)
			{
				rotateRect.center.y -=dif_edge4/2;
				rotateRect.size.height +=dif_edge4;
			}
			
			cv::Point2f pt2f[4];
			rotateRect.points(pt2f);
			for (int i = 0; i<4; i++)
			{
				pt[i].x = (pt2f[i].x);
				pt[i].y = (pt2f[i].y);
			}
			angle = box.angle;

			break;
		}//end for
	}//end if(pMax != NULL)
	return true;
}
bool calcPara(vector<Point> pMax,float & angle, Point2f* pt,float nTreshold,float fRadio,float dif_edge1,float dif_edge2,float dif_edge3,float dif_edge4,vector<int> edgeDif,int halfBorder)
{
	if (pMax.size() != 0)
	{
		RotatedRect box;
		RotatedRect box_temp;
		box_temp = minAreaRect(pMax);
		cv::RotatedRect rotateRect1= box_temp;


			box = minAreaRect(pMax);
			if(abs(box.angle -angle)>85)
			{
				int nTmp = box.size.width;
				box.size.width = box.size.height;
				box.size.height = nTmp;
			}
			box.angle  = angle;

			cv::RotatedRect rotateRect = box;

			//根据新旧四个变之间的距离差，对矩形的四个边进行纠正
			//if(dif_edge1/nTreshold < -4 )
			if( abs(dif_edge1/nTreshold) > 4 )
			{
				rotateRect.center.x -= dif_edge1/2;
				rotateRect.size.width -= dif_edge1;
			}


			//if( dif_edge2/nTreshold > 4 )
			if( abs(dif_edge2/nTreshold) > 4 )
			{
				rotateRect.center.x -=dif_edge2/2;
				rotateRect.size.width += dif_edge2;
			}


			//if( dif_edge3/nTreshold < -4  )
			if( abs(dif_edge3/nTreshold) > 4  )
			{
				rotateRect.center.y -=dif_edge3/2;
				rotateRect.size.height -=dif_edge3;
			}


			//if(dif_edge4/nTreshold > 4 )
			if( abs(dif_edge4/nTreshold )>4)
			{
				rotateRect.center.y -=dif_edge4/2;
				rotateRect.size.height +=dif_edge4;
			}

			//进行贴边拟合
			//for(int xt=0;xt <edgeDif.size();xt++)
			//	edgeDif[xt] =0;

			rotateRect.center.x = (rotateRect.center.x - halfBorder)/fRadio;	
			rotateRect.size.width /=fRadio;
			rotateRect.center.y = (rotateRect.center.y - halfBorder)/fRadio;
			rotateRect.size.height /= fRadio;
			
			rotateRect.center.x += edgeDif[0] -edgeDif[2];
			rotateRect.size.width -= (edgeDif[0] +edgeDif[2]);
			rotateRect.center.y += (edgeDif[1] - edgeDif[3]);
			rotateRect.size.height -=(edgeDif[1] +edgeDif[3]);


			cv::Point2f pt2f[4];
			rotateRect.points(pt2f);
			for (int i = 0; i<4; i++)
			{
				pt[i].x = (pt2f[i].x);
				pt[i].y = (pt2f[i].y);
			}
			angle = box.angle;
		}//end for
	return true;
}
//裁切纠偏
//pt 数组CvPoint pt[4]
//Max，2016-06-01 12:15:34
bool CDetectRectByContours::DetectRect(IplImage *src0, float & angle, CvPoint* pt, Rect rect)
{
	bool isOriginalRect =false;
	//鲁棒性检测
	if(rect.x < 0 || rect.y < 0 || rect.width < 1 || rect.height < 1 ||
		rect.x > src0->width || rect.y > src0->height || rect.width > src0->width || rect.height > src0->height )
	{
		rect.x = 0;
		rect.y = 0;
		rect.width = src0->width ;
		rect.height = src0->height;

		isOriginalRect =true;
	}

	//异常返回
	if(src0->width <1 || src0->height <1)
		return false;

	//g_log->Log("DetectRect begin1");

	float fAreaRatio = 0.4;
	//首先对图像进行缩放
	int nTargetW = 640;
	float fRadio = 1.0;
	IplImage *resize;
	if( max(src0->width,src0->height) > nTargetW)
	{
		int nDstW,nDstH;
		if(src0->width > src0->height)
		{
			nDstW = nTargetW;
			fRadio = (float)nDstW / src0->width;
			nDstH = src0->height * fRadio;
		}
		else
		{
			nDstH = nTargetW;
			fRadio = (float)nDstH/src0->height;
			nDstW = src0->width*fRadio;
		}
		
		//缩放
		resize= cvCreateImage(cvSize(nDstW, nDstH), src0->depth, src0->nChannels);
		cvResize(src0, resize,CV_INTER_AREA);

		if (isOriginalRect)
		{
			rect.x =0;
			rect.y =0;
			rect.width =resize->width;
			rect.height = resize->height;
		}
		else
		{
			rect.x *= fRadio;
			rect.y *= fRadio;
			rect.width *= fRadio;
			rect.height *= fRadio;
		}
	}
	else
	{
		 resize=cvCloneImage(src0);
	}
	
	if(!isOriginalRect)
	{
		//出界异常修复
		rect.x  = rect.x >resize->width-1?resize->width-1:rect.x;
		rect.y = rect.y>resize->height-1?resize->height-1:rect.y;
		rect.width = rect.x+rect.width >rect.width-1?rect.width-1-rect.x:rect.width;
		rect.height =rect.y+rect.height>rect.height-1?rect.height-1-rect.y:rect.height;
	}
	
	//对制定区域Rect进行裁切纠偏
	IplImage* src = 0;
	if(rect.width > 0 && rect.height > 0 && !isOriginalRect)
	{
		cvSetImageROI(resize,rect);
		src = cvCreateImage(cvSize(rect.width,rect.height),resize->depth,resize->nChannels);
		cvCopy(resize,src);
		cvResetImageROI(resize);
	}
	else
	{
		src = cvCloneImage(resize);
	}
	cvReleaseImage(&resize);

	//g_log->Log("DetectRect begin3");

	//补黑边，因为部分图像不存在黑边
	const int halfBorder = 20;
	IplImage *makeBorder = cvCreateImage(cvSize(cvGetSize(src).width+halfBorder*2,cvGetSize(src).height+halfBorder*2),src->depth,src->nChannels);
	CvPoint pt1;
	pt1.x = halfBorder;
	pt1.y = halfBorder;
	cvCopyMakeBorder(src,makeBorder,pt1,IPL_BORDER_CONSTANT,cvScalarAll(0));

	//IplImage* dst = NULL;
	if (makeBorder == NULL || pt == NULL)
	{
		cvReleaseImage(&makeBorder);
		return false;
	}
	cvReleaseImage(&src);

	//清空
	for (int i = 0; i < 4; i++)
	{
		pt[i].x = 0;
		pt[i].y = 0;
	}

	//g_log->Log("DetectRect Image Preprocess");
	int nDepth = makeBorder->depth;
	//2.计算阈值
	//2.1.用于是否为判断轮廓底板边缘
	float sjbl=0.15;
	CvPoint tpl;
	tpl.x = makeBorder->width*sjbl;
	tpl.y = makeBorder->height*sjbl;//底板边缘左上角
	CvPoint lpr;
	lpr.x = makeBorder->width-makeBorder->width*sjbl;
	lpr.y = makeBorder->height-makeBorder->height*sjbl;//底板边缘右下角
	vector<CvPoint> vcBordePT;
	vcBordePT.push_back(tpl);
	vcBordePT.push_back(lpr);
	//2.2.通过轮廓至少大于图像总面积1/100,(过滤面积较小的干扰)
	float fSizeTh = 1.0/100.0;
	int nSizeTh = makeBorder->width*makeBorder->height*fSizeTh;
	//2.3.通过轮廓，真实面积/最小外接矩形面积<阈值，判断是否为矩形（排除凹字形凸字形）
	float fRadioSize_Th0 = 0.8;
	float fRadioSize_Th1 = 0.5;
	float fRadioSize_Th2 = 0.3;
	vector<float> vcRadioSizeTh;
	vcRadioSizeTh.push_back(fRadioSize_Th0);
	vcRadioSizeTh.push_back(fRadioSize_Th1);

	//旧版
	IplImage *bwOri = cvCreateImage(cvSize(makeBorder->width, makeBorder->height), nDepth, 1);
	CAdaptiveThreshold::AdaptiveThreshold(makeBorder,bwOri);

	CvMemStorage *pStorageOri = NULL;
	CvSeq *pMaxOri = CDetectRectByContours::findObjectContour(bwOri,fAreaRatio,nSizeTh,vcRadioSizeTh,vcBordePT,pStorageOri);
	//异常判断	
	if(pMaxOri == NULL)
	{
		cvReleaseMemStorage(&pStorageOri);
		pStorageOri = NULL;
		cvReleaseImage(&bwOri);

		return false;
	}

	Rect cRect = cvBoundingRect(pMaxOri);
	CvBox2D box_tempOri = cvMinAreaRect2(pMaxOri);

	//4.二值化(使用二值化，则无法处理黑色目标，除非考虑反色）
	CvMemStorage *pStorage = NULL;
	IplImage *bw = cvCreateImage(cvSize(makeBorder->width, makeBorder->height), nDepth, 1);
	float fNewChangeSize = 4;
	CDetectRectByContours::reshapeBW(makeBorder,bw,nDepth,fNewChangeSize);
	CvSeq *pMax = CDetectRectByContours::findObjectContour(bw,fAreaRatio,nSizeTh,vcRadioSizeTh,vcBordePT,pStorage,cRect);
	cvReleaseImage(&bw);
	CvBox2D box_temp;
	if(pMax != NULL)
		box_temp = cvMinAreaRect2(pMax);
	else
		box_temp = box_tempOri;
	
	//g_log->Log("DetectRect find pMax&pMaxOri");
	//计算量
	double fRatio = max(cos(double(box_temp.angle/180.0)),sin(double(box_temp.angle/180.0)));

 	if( abs(box_temp.angle - box_tempOri.angle) >85 || (abs(box_temp.angle) >85 &&abs(box_tempOri.angle)>85))
	{
		int nTmp = box_temp.size.width;
		box_temp.size.width = box_temp.size.height;
		box_temp.size.height =nTmp;

		if(abs(box_temp.angle) >85 &&abs(box_tempOri.angle)>85)
		{
			nTmp = box_tempOri.size.width;
			box_tempOri.size.width = box_tempOri.size.height;
			box_tempOri.size.height =nTmp;
		}

		box_tempOri.angle +=90;
	}
	//替换原图尺寸
	angle = box_tempOri.angle;

	//计算偏移量
	int dif_edge1 = box_tempOri.center.x + box_tempOri.size.width/2*fRatio -(box_temp.center.x+box_temp.size.width/2*fRatio);
	int dif_edge2 = box_tempOri.center.x - box_tempOri.size.width/2*fRatio -(box_temp.center.x-box_temp.size.width/2*fRatio);
	int dif_edge3 = box_tempOri.center.y + box_tempOri.size.height/2*fRatio -(box_temp.center.y+box_temp.size.height/2*fRatio);
	int dif_edge4 = box_tempOri.center.y - box_tempOri.size.height/2*fRatio -(box_temp.center.y-box_temp.size.height/2*fRatio);

	//释放图像
 	cvReleaseImage(&bwOri);
	cvReleaseImage(&makeBorder);

	int numberOfAdoptEdge =0;
	//统计符合边缘个数
	if(abs(dif_edge1/fNewChangeSize) < 4)
	{
		numberOfAdoptEdge ++;
	}

	if(abs(dif_edge2/fNewChangeSize) < 4)
	{
		numberOfAdoptEdge ++;
	}

	if(abs(dif_edge3/fNewChangeSize) < 4)
	{
		numberOfAdoptEdge ++;
	}

	if(abs(dif_edge4/fNewChangeSize) < 4)
	{
		numberOfAdoptEdge ++;
	}
	

	CvPoint2D32f pt2f[4];
 	if(numberOfAdoptEdge ==3 )
	{
		//求外接矩形点
		if(!calcPara(pMaxOri,angle,pt2f,fNewChangeSize,fRadio,dif_edge1,dif_edge2,dif_edge3,dif_edge4))
		{
			cvReleaseMemStorage(&pStorage);
			pStorage = NULL;
			cvReleaseMemStorage(&pStorageOri);
			pStorageOri = NULL;

			return false;
		}
	}
	else
	{
		//求外接矩形点
		if(!calcPara(pMaxOri,angle,pt2f,fNewChangeSize,fRadio,dif_edge1,dif_edge2,dif_edge3,dif_edge4))
		{
			cvReleaseMemStorage(&pStorage);
			pStorage = NULL;
			cvReleaseMemStorage(&pStorageOri);
			pStorageOri = NULL;

			//g_log->Log("DetectRect HalfWay Out(calcPare == false)");
			return false;
		}
	}

	//g_log->Log("DetectRect Calc Rect Info");
	//坐标系转换
      for (int i=0; i<4; i++)
	{
		//缩放补边
		pt[i].x = int((pt2f[i].x  -halfBorder)/ fRadio);
		pt[i].y = int((pt2f[i].y  -halfBorder)/ fRadio);
		//关注区域
		if(rect.width >0 && rect.height >0)
		{
			pt[i].x += rect.x;
			pt[i].y += rect.y;
		}
		
		pt[i].x = pt[i].x < 0? 0:(pt[i].x > src0->width-1 ? src0->width-1:pt[i].x);
		pt[i].y = pt[i].y < 0? 0:(pt[i].y > src0->height-1 ? src0->height-1:pt[i].y);
	}
	
	//释放资源
	cvReleaseMemStorage(&pStorage);
	pStorage = NULL;
	cvReleaseMemStorage(&pStorageOri);
	pStorageOri = NULL;

	//g_log->Log("DetectRect End");
	return true; 
}

//Mat 对应版本
#if 1
int calMaxIdx(vector<int> vDif1,vector<int> vDif2,vector<int> vDif3)
{
	int nMaxIdx =-1;
	for (int idx =0;idx < vDif1.size(); idx++)
	{
		if(vDif1[idx] >60 || vDif2[idx] >60 || vDif3[idx] >60)
		{
			if(nMaxIdx > idx || nMaxIdx ==-1)
				nMaxIdx =idx;
		}
	}

	if(nMaxIdx == -1)
		nMaxIdx =0;

	return nMaxIdx+1;
}

vector<int> calcEdgeDif(const Mat &src0,RotatedRect box_tempOri,int halfBorder,float fRadio)
{
	//四边的贴合值
	vector<int> edgeDif;
	//获取四个点坐标
	Point2f pt[4];
	box_tempOri.points(pt);
	//还原真实
	int idx_leftDw = 0;
	int nMaxGap =-1;
	for(int i=0;i <4;i++)
	{
		pt[i].x = (pt[i].x  -halfBorder)/ fRadio;
		pt[i].y = (pt[i].y  -halfBorder)/ fRadio;

		pt[i].x =pt[i].x <0 ?0 :(pt[i].x > src0.cols-1? src0.cols-1:pt[i].x);
		pt[i].y =pt[i].y <0 ?0 :(pt[i].y > src0.rows-1? src0.rows-1:pt[i].y);

		//保留左下角点位置
		if(-1 == nMaxGap || pt[i].x < nMaxGap)
		{
			nMaxGap = pt[i].x;
			idx_leftDw = i;
		}
	}
	//范围限制
	int idx_leftDw_pre = idx_leftDw -1;
	idx_leftDw_pre = idx_leftDw_pre <0 ?3:idx_leftDw_pre;
	int idx_leftDw_next = idx_leftDw+1;
	idx_leftDw_next = idx_leftDw_next >3? 0:idx_leftDw_next;

	if(pt[idx_leftDw_pre].x < pt[idx_leftDw_next].x)
	{
		if(pt[idx_leftDw_pre].y > pt[idx_leftDw].y)
			idx_leftDw = idx_leftDw_pre;
	}
	else
	{
		if(pt[idx_leftDw_next].y > pt[idx_leftDw].y)
			idx_leftDw = idx_leftDw_next;
	}
	//中间三个点
	Point ptMid1,ptMid2,ptMid3;
	for (int idxStep=0;idxStep<4;idxStep++)
	{
		int idx= (idx_leftDw +idxStep)%4;

		int idxNext1 = (idx+1)%4;
		int idxNext2 = (idx+2)%4;
		int idxNext3 = (idx+3)%4;

		float difX = (pt[idxNext1].x - pt[idx].x)/4.0;
		float difY = (pt[idxNext1].y - pt[idx].y)/4.0;
		
		//1
		ptMid1.x = pt[idx].x + difX;
		ptMid1.y = pt[idx].y + difY;
		//2
		ptMid2.x = pt[idx].x + difX*2;
		ptMid2.y = pt[idx].y + difY*2;
		//3
		ptMid3.x = pt[idx].x + difX*3;
		ptMid3.y = pt[idx].y + difY*3;
	  

		float flen = sqrt((pt[idxNext2].x - pt[idxNext1].x)*(pt[idxNext2].x - pt[idxNext1].x)+
							(pt[idxNext2].y - pt[idxNext1].y)*(pt[idxNext2].y - pt[idxNext1].y));

 		difX = (pt[idxNext2].x - pt[idxNext1].x)/flen;
		difY = (pt[idxNext2].y - pt[idxNext1].y)/flen;
	
		
		vector<int> vDif1,vDif2,vDif3;
		const uchar *data = src0.ptr<uchar>(0);
		
		for (float id=0;id <2.0/fRadio ;id+=0.9)
		{
			int nIdx = int(ptMid1.y + id *difY )*src0.cols*src0.channels() + int(ptMid1.x + id*difX)*src0.channels();
			int nIdxNext = int(ptMid1.y + (id+1) *difY )*src0.cols*src0.channels() + int(ptMid1.x + (id+1)*difX)*src0.channels();
			vDif1.push_back(max(*(data+nIdxNext),max(*(data+nIdxNext+2),*(data+nIdxNext+1))) - max(*(data+nIdx),max(*(data+nIdx+2),*(data+nIdx+1))));

			nIdx = int(ptMid2.y + id *difY )*src0.cols*src0.channels() + int(ptMid2.x + id*difX)*src0.channels();
			nIdxNext = int(ptMid2.y + (id+1) *difY )*src0.cols*src0.channels() + int(ptMid2.x + (id+1)*difX)*src0.channels();
			vDif2.push_back(max(*(data+nIdxNext),max(*(data+nIdxNext+2),*(data+nIdxNext+1))) - max(*(data+nIdx),max(*(data+nIdx+2),*(data+nIdx+1))));

			nIdx = int(ptMid3.y + id *difY )*src0.cols*src0.channels() + int(ptMid3.x + id*difX)*src0.channels();
			nIdxNext = int(ptMid3.y + (id+1) *difY )*src0.cols*src0.channels() + int(ptMid3.x + (id+1)*difX)*src0.channels();
			vDif3.push_back(max(*(data+nIdxNext),max(*(data+nIdxNext+2),*(data+nIdxNext+1))) - max(*(data+nIdx),max(*(data+nIdx+2),*(data+nIdx+1))));
		}

		edgeDif.push_back(calMaxIdx(vDif1,vDif2,vDif3));
	}
	
	return edgeDif;
}

#include <time.h>
bool CDetectRectByContours::DetectRect(const Mat &src0,float & angle,Point *pt/* =NULL */,Rect rect /* =Rect */)
{
	bool isOriginalRect =false;
	//鲁棒性检测
	if(rect.x < 0 || rect.y < 0 || rect.width < 1 || rect.height < 1 ||
		rect.x > src0.cols || rect.y > src0.rows || rect.width > src0.cols || rect.height > src0.rows )
	{
		rect.x = 0;
		rect.y = 0;
		rect.width = src0.cols ;
		rect.height = src0.rows;

		isOriginalRect =true;
	}


	Rect rectOri = rect;
	//异常返回
	if(src0.cols <1 || src0.rows <1)
		return false;

	float fAreaRatio = 0.4;
	//首先对图像进行缩放
	int nTargetW = 800;
	float fRadio = 1.0;
	Mat resizeImg;
	if( max(src0.cols,src0.rows) > nTargetW)
	{
		int nDstW,nDstH;
		if(src0.cols> src0.rows)
		{
			nDstW = nTargetW;
			fRadio = (float)nDstW / src0.cols;
			nDstH = src0.rows * fRadio;
		}
		else
		{
			nDstH = nTargetW;
			fRadio = (float)nDstH/src0.rows;
			nDstW = src0.cols*fRadio;
		}

		//缩放
		resize(src0,resizeImg,Size(nDstW,nDstH));

		if (isOriginalRect)
		{
			rect.x =0;
			rect.y =0;
			rect.width =resizeImg.cols;
			rect.height = resizeImg.rows;
		}
		else
		{
			rect.x *= fRadio;
			rect.y *= fRadio;
			rect.width *= fRadio;
			rect.height *= fRadio;
		}
	}
	else
	{
		resizeImg=src0.clone();
	}


	if(!isOriginalRect)
	{
		//出界异常修复
		rect.x  = rect.x >resizeImg.cols-1?resizeImg.cols-1:rect.x;
		rect.y = rect.y>resizeImg.rows-1?resizeImg.rows-1:rect.y;
		rect.width = rect.x+rect.width >resizeImg.cols-1?resizeImg.cols-1-rect.x:rect.width;
		rect.height =rect.y+rect.height>resizeImg.rows-1?resizeImg.rows-1-rect.y:rect.height;
	}

	//对制定区域Rect进行裁切纠偏
	Mat src;
	if(rect.width > 0 && rect.height > 0 && !isOriginalRect)
	{
		src= resizeImg(rect).clone();
	}
	else
	{
		src = resizeImg.clone();
	}
	resizeImg.release();

	//补黑边，因为部分图像不存在黑边
	//不能补黑边（为了解决新版FW导致四周亮误判问题，但是带来问题无法解决3边，2边裁切问题）
	const int halfBorder = 0;
	Mat makeBorder;
	copyMakeBorder(src,makeBorder,halfBorder,halfBorder,halfBorder,halfBorder,BORDER_CONSTANT,Scalar(0,0,0));

	//IplImage* dst = NULL;
	if (makeBorder.data == NULL || pt == NULL)
	{
		makeBorder.release();
		return false;
	}
	src.release();

	//清空
	for (int i = 0; i < 4; i++)
	{
		pt[i].x = 0;
		pt[i].y = 0;
	}

	int nDepth = makeBorder.depth();
	//2.计算阈值
	//2.1.用于是否为判断轮廓底板边缘
	float sjbl=0.15;
	Point tpl = Point(makeBorder.cols*sjbl,makeBorder.rows*sjbl);//底板边缘左上角
	Point lpr = Point(makeBorder.cols-makeBorder.cols*sjbl,makeBorder.rows-makeBorder.rows*sjbl);//底板边缘右下角
	vector<Point> vcBordePT;
	vcBordePT.push_back(tpl);
	vcBordePT.push_back(lpr);
	//2.2.通过轮廓至少大于图像总面积1/100,(过滤面积较小的干扰)
	float fSizeTh = 1.0/100.0;
	int nSizeTh = makeBorder.cols*makeBorder.rows*fSizeTh;
	//2.3.通过轮廓，真实面积/最小外接矩形面积<阈值，判断是否为矩形（排除凹字形凸字形）
	float fRadioSize_Th0 = 0.8;
	float fRadioSize_Th1 = 0.5;
	float fRadioSize_Th2 = 0.3;
	vector<float> vcRadioSizeTh;
	vcRadioSizeTh.push_back(fRadioSize_Th0);
	vcRadioSizeTh.push_back(fRadioSize_Th1);

	//旧版
	Mat bwOri = CAdaptiveThreshold::AdaptiveThreshold(makeBorder);

	vector<Point> pMaxOri = CDetectRectByContours::findObjectContour1(bwOri,fAreaRatio,nSizeTh,vcRadioSizeTh,vcBordePT,rect);
	//异常判断	
	if(pMaxOri.size() == 0)
	{
		bwOri.release();

		return false;
	}

	RotatedRect box_tempOri = minAreaRect(pMaxOri);
	Rect cRect = box_tempOri.boundingRect();


	//4.二值化(使用二值化，则无法处理黑色目标，除非考虑反色）
	float fNewChangeSize = 4;
	Mat bwPre = CDetectRectByContours::reshapeBW(makeBorder,nDepth,fNewChangeSize);

	Mat bw;
	resize(bwPre,bw,Size(makeBorder.cols,makeBorder.rows));


	vector<Point> pMax = CDetectRectByContours::findObjectContour1(bw,fAreaRatio,nSizeTh,vcRadioSizeTh,vcBordePT,cRect);
	bw.release();
	
	RotatedRect box_temp;
	if(pMax.size() != 0)
		box_temp = minAreaRect(pMax);
	else
		box_temp = box_tempOri;

	//计算量
	double fRatio = max(cos(double(box_temp.angle/180.0)),sin(double(box_temp.angle/180.0)));

#if 0
	if( abs(box_temp.angle - box_tempOri.angle) >85 || (abs(box_temp.angle) >85 &&abs(box_tempOri.angle)>85))
	{
		int nTmp = box_temp.size.width;
		box_temp.size.width = box_temp.size.height;
		box_temp.size.height =nTmp;

		if(abs(box_temp.angle) >85 &&abs(box_tempOri.angle)>85)
		{
			nTmp = box_tempOri.size.width;
			box_tempOri.size.width = box_tempOri.size.height;
			box_tempOri.size.height =nTmp;
		}

		box_tempOri.angle +=90;
	}
	//替换原图尺寸
	angle = box_tempOri.angle;
#else
	if( abs(box_temp.angle - box_tempOri.angle) > 85)
	{
		int nTmp = box_tempOri.size.width;
		box_tempOri.size.width = box_tempOri.size.height;
		box_tempOri.size.height =nTmp;

		box_tempOri.angle +=90;
	}
	angle = box_tempOri.angle;
#endif


	//计算偏移量
	float dif_edge1 = box_tempOri.center.x + box_tempOri.size.width/2.0*fRatio -(box_temp.center.x+box_temp.size.width/2.0*fRatio);
	float dif_edge2 = box_tempOri.center.x - box_tempOri.size.width/2.0*fRatio -(box_temp.center.x-box_temp.size.width/2.0*fRatio);
	float dif_edge3 = box_tempOri.center.y + box_tempOri.size.height/2.0*fRatio -(box_temp.center.y+box_temp.size.height/2.0*fRatio);
	float dif_edge4 = box_tempOri.center.y - box_tempOri.size.height/2.0*fRatio -(box_temp.center.y-box_temp.size.height/2.0*fRatio);

	//计算贴合值
	Mat src0Ori = src0(rectOri).clone();
	vector<int> edgeDif = calcEdgeDif(src0Ori, box_tempOri, halfBorder, fRadio);
	src0Ori.release();

	//释放图像
	bwOri.release();
	makeBorder.release();

	int numberOfAdoptEdge =0;
	//统计符合边缘个数
	if(abs(dif_edge1/fNewChangeSize) < 4)
		numberOfAdoptEdge ++;
	else
		dif_edge1 =0;

	if(abs(dif_edge2/fNewChangeSize) < 4)
		numberOfAdoptEdge ++;
	else
		dif_edge2 =0;

	if(abs(dif_edge3/fNewChangeSize) < 4)
		numberOfAdoptEdge ++;
	else
		dif_edge3 =0;

	if(abs(dif_edge4/fNewChangeSize) < 4)
		numberOfAdoptEdge ++;
	else
		dif_edge4 =0;

	Point2f pt2f[4];
	if(numberOfAdoptEdge ==3 )
	{
		//求外接矩形点
		if(!calcPara(pMaxOri,angle,pt2f,fNewChangeSize,fRadio,dif_edge1,dif_edge2,dif_edge3,dif_edge4,edgeDif,halfBorder))
		{
			return false;
		}
	}
	else 
	{
		//求外接矩形点
 		if(!calcPara(pMaxOri,angle,pt2f,fNewChangeSize,fRadio,dif_edge1,dif_edge2,dif_edge3,dif_edge4,edgeDif,halfBorder))
		{
			return false;
		}
	}

	//坐标系转换
	for (int i=0; i<4; i++)
	{
		float difGap = 1.0/fRadio;
		//缩放补边
		pt[i].x = (int)pt2f[i].x;
		pt[i].y = (int)pt2f[i].y;
		//关注区域
		if(rect.width >0 && rect.height >0)
		{
			pt[i].x += rectOri.x+0.5;
			pt[i].y += rectOri.y+0.5;
		}

		pt[i].x = pt[i].x < 0? 0:(pt[i].x > src0.cols-1 ? src0.cols-1:pt[i].x);
		pt[i].y = pt[i].y < 0? 0:(pt[i].y > src0.rows-1 ? src0.rows-1:pt[i].y);
	}
	
	return true; 
}
#endif

//裁切纠偏
//参数说明:
//src：源图像
//rect：关注区域
//返回值说明:
//裁切后图像
//Max，2016-10-12 15:22:30
IplImage* CDetectRectByContours::DoDetectRect(IplImage *src, Rect rect)
{
	float fAngle;
	CvPoint cvpt[4];
	bool isTrue = CDetectRectByContours::DetectRect(src,fAngle,cvpt,rect);
	if(isTrue)
	{
		Mat matSrc;
		if(src != NULL)
		{
			matSrc = cv::cvarrToMat(src,true);  
			Mat matDst = Rotate::RotateCut(matSrc, cvpt);
            //IplImage *iplDst0 =  &(IplImage)matDst;
            IplImage iplDst0Tmp =  (IplImage)matDst;
            IplImage *iplDst0 =  &iplDst0Tmp;
			IplImage *iplDst = ::cvCloneImage(iplDst0);
			return iplDst;
		}
		
	}//end if(iplDetect != NULL)
	return NULL;
}

//裁切纠偏
//函数不涉及具体裁切纠偏算法,调用DetectRect获取参数,对src进行裁切纠偏获取subImage
//参数说明:
//[in]src：源图像
//[in]rect：关注区域
//返回值说明:
//裁切后图像
Mat CDetectRectByContours::DoDetectRect(Mat src, Rect rect)
{
	Mat dst;
	if(!src.data)
	{
		return dst;
	}

    //IplImage *iplSrc =  &(IplImage)src;
    IplImage iplSrcTmp =  (IplImage)src;
    IplImage *iplSrc =  &iplSrcTmp;
	IplImage *iplDst = CDetectRectByContours::DoDetectRect(iplSrc,rect);
	if(iplDst != NULL)
	{
		dst = cv::cvarrToMat(iplDst,true);  
		cvReleaseImage(&iplDst);
	}

	return dst;
}

//轮廓补充操作
//[in]makeBorder:读入图
//[in/out]bw:返回修正二值图
void CDetectRectByContours::reshapeBW(IplImage *makeBorder,IplImage *bw,int nDepth,float fRatio)
{
	int nWidth = makeBorder->width;
	int nHeight = makeBorder->height;
	IplImage *makeBorderTmp = cvCreateImage(cvSize(nWidth/fRatio,nHeight/fRatio),makeBorder->depth,makeBorder->nChannels);
	cvResize(makeBorder,makeBorderTmp);
	
	//二值化
	IplImage *bwTmp = cvCreateImage(cvSize(makeBorderTmp->width, makeBorderTmp->height), nDepth, 1);
	CAdaptiveThreshold::AdaptiveThreshold(makeBorderTmp, bwTmp);
	cvReleaseImage(&makeBorderTmp);

	//保留二值化的边缘信息
	IplImage *bwEdgeSave = cvCreateImage(cvSize(bwTmp->width,bwTmp->height),nDepth,1);
	cvCopy(bwTmp,bwEdgeSave);

	//解决边缘出现亮斑的情况
	Mat img = cvarrToMat(bwTmp); 
	Mat element3(3,3,CV_8U,cv::Scalar(1));
	Mat img_open;
	cv::morphologyEx(img, img_open,cv::MORPH_OPEN,element3);
	img.release();
	//imwrite("C:\\Users\\Xue.Bicycle\\Desktop\\bbbbMid0.jpg",img_open);
	//解决图像内部出现不连续块的状况
	Mat element17(17,17,CV_8U,cv::Scalar(1));
	Mat img_close;
	cv::morphologyEx(img_open, img_close,cv::MORPH_CLOSE,element17);
	img_open.release();
	
	cvReleaseImage(&bwTmp);
	bwTmp = cvCreateImage(cvSize( img_close.cols , img_close.rows), nDepth, 1);
    //IplImage* pBinary = &IplImage(img_close);
    IplImage pBinaryTmp =  (IplImage)img_close;
    IplImage *pBinary =  &pBinaryTmp;
	bwTmp = cvCloneImage(pBinary);

	//cvSaveImage("C:\\Users\\Xue.Bicycle\\Desktop\\bbbbMid.jpg",bwTmp);
	//将预处理图像的中间部分进行替换
	IplConvKernel* kernel_tmp = cvCreateStructuringElementEx(3, 3, 2,2, CV_SHAPE_RECT);
	//腐蚀
	IplImage *bwEdgeErodePre = cvCreateImage(cvSize(bwTmp->width,bwTmp->height),nDepth,1);
	cvErode(bwEdgeSave,bwEdgeErodePre,kernel_tmp,1);
	cvReleaseImage(&bwEdgeSave);

	IplImage *bwEdgeErode = cvCreateImage(cvSize(bwTmp->width,bwTmp->height),nDepth,1);
	cvDilate(bwEdgeErodePre,bwEdgeErode,kernel_tmp,1);
	cvReleaseImage(&bwEdgeErodePre);

	//遍历图像
	uchar *data = (uchar*)bwTmp->imageData;
	uchar *dataEdge = (uchar*)bwEdgeErode->imageData;
	int step = bwTmp->widthStep / sizeof(uchar);

	int nCh = bwTmp->nChannels;
	nCh = bwEdgeErode->nChannels;

	for(int idr=0;idr<bwTmp->height;idr++)
	{
		for(int idc=0;idc<bwTmp->width;idc++)
		{
			if(
				(
				(idr<bwTmp->height*0.1
				||idr >bwTmp->height*0.9
				||idc <bwTmp->width*0.1
				||idc >bwTmp->width*0.9
				)
				&& dataEdge[idr*step + idc] <128
				)
				||//上下中间限定
				(
					(
					idr <bwTmp->height*0.05
					||
					idr >bwTmp->height*0.95
					)
					&&
					(
					idc <bwTmp->width *0.2
					||
					idc >bwTmp->width *0.8
					)
				)
				||//左右中间限定
				(
					(
					idc <bwTmp->width*0.05
					||
					idc >bwTmp->width*0.95
					)
					&&
					(
					idr <bwTmp->height *0.2
					||
					idr >bwTmp->height *0.8
					)
				)
				)
			{
				data[idr*step+idc] = 0;
			}
		}
	}

	//cvSaveImage("C:\\Users\\Xue.Bicycle\\Desktop\\bbbbEnd.jpg",bwTmp);
	//还原尺寸
	cvResize(bwTmp,bw);

	//释放图像
	cvReleaseImage(&bwEdgeErode);
	cvReleaseImage(&bwTmp);
}
Mat CDetectRectByContours::reshapeBW(Mat makeBorder,int nDepth,float fRatio)
{
	int nWidth = makeBorder.cols;
	int nHeight = makeBorder.rows;

	Mat makeBorderTmp;
	resize(makeBorder,makeBorderTmp,Size(nWidth/fRatio,nHeight/fRatio));

	//二值化
	Mat bwTmp =CAdaptiveThreshold::AdaptiveThreshold(makeBorderTmp);
	makeBorderTmp.release();

	//保留二值化的边缘信息
	Mat bwEdgeSave = bwTmp.clone();

	//解决边缘出现亮斑的情况
	Mat img = bwTmp.clone(); 
	Mat element3(3,3,CV_8U,cv::Scalar(1));
	Mat img_open;
	cv::morphologyEx(img, img_open,cv::MORPH_OPEN,element3);
	img.release();

	//解决图像内部出现不连续块的状况
	Mat element17(17,17,CV_8U,cv::Scalar(1));
	Mat img_close;
	cv::morphologyEx(img_open, img_close,cv::MORPH_CLOSE,element17);
	img_open.release();

	bwTmp = img_close.clone();
	img_close.release();
	//将预处理图像的中间部分进行替换
	Mat kernel_tmp = getStructuringElement(CV_SHAPE_RECT,Size(3,3),Point(2,2));
	//腐蚀
	Mat bwEdgeErodePre;
	erode(bwEdgeSave,bwEdgeErodePre,kernel_tmp);
	bwEdgeSave.release();

	Mat bwEdgeErode;
	dilate(bwEdgeErodePre,bwEdgeErode,kernel_tmp);
	bwEdgeErodePre.release();

	//遍历图像
	uchar *data = bwTmp.ptr<uchar>(0);
	uchar *dataEdge = bwEdgeErode.ptr<uchar>(0);

	int step = bwTmp.rows;
	int nCh = bwTmp.channels();
	nCh = bwEdgeErode.channels();

	for(int idr=0;idr<bwTmp.rows;idr++)
	{
		for(int idc=0;idc<bwTmp.cols;idc++,dataEdge++,data++)
		{
			if(
				(
				(idr<bwTmp.rows*0.1
				||idr >bwTmp.rows*0.9
				||idc <bwTmp.cols*0.1
				||idc >bwTmp.cols*0.9
				)
				&& *dataEdge <128
				)
				||//上下中间限定
				(
				(
				idr <bwTmp.rows*0.05
				||
				idr >bwTmp.rows*0.95
				)
				&&
				(
				idc <bwTmp.cols *0.2
				||
				idc >bwTmp.cols *0.8
				)
				)
				||//左右中间限定
				(
				(
				idc <bwTmp.cols*0.05
				||
				idc >bwTmp.cols*0.95
				)
				&&
				(
				idr <bwTmp.rows *0.2
				||
				idr >bwTmp.rows *0.8
				)
				)
				)
			{
				*data =0;
			}
		}
	}
	return bwTmp;
}
//轮廓合并操作
//[in]pContour:轮廓集合
//[in/out]pMax:最大轮廓
//[in]pStorage:存储内存
//[in]nW:图像宽度
//[in]nH:图像高度
void CDetectRectByContours::contourMerge(CvSeq *pContour,CvSeq *pMax,CvMemStorage *pStorage,int nW,int nH)
{
		if(pMax != 0)
		{
			CvBox2D boxMax = cvMinAreaRect2(pMax);
			int nHeightMax = boxMax.size.height*0.8;
			int nHeightMin = boxMax.size.height*0.1;
			int nWidthMax = boxMax.size.width*0.8;
			int nWidthMin = boxMax.size.width*0.1;
			cv::Point2f pt2f[4];
			int nMinDisFormBor = 50;

			for (CvSeq * c = pContour; c != NULL; c = c->h_next)
			{
				CvBox2D boxC = cvMinAreaRect2(c);
				cv::RotatedRect rotateRect = cv::RotatedRect(boxC);
				rotateRect.points(pt2f);

				if(boxMax.size.width == boxC.size.width && 
					boxMax.size.height == boxC.size.height &&
					boxMax.center.x == boxC.center.x &&
					boxMax.center.y == boxC.center.y)
				{
					continue;
				}
				else
				{
					int nMinV = boxMax.size.width < boxMax.size.height ? boxMax.size.width:boxMax.size.height;
					int nDistance = sqrt(pow((boxMax.center.x - boxC.center.x),2)+pow((boxMax.center.y - boxC.center.y),2));


					double dcontourArea = cvContourArea(c);
					if(
						(
						dcontourArea > nW*nH/400
						&&
						nDistance < nMinV*1.5 + max(boxMax.size.height,boxMax.size.width)/2
						&&
						pt2f[0].x >nMinDisFormBor && pt2f[1].x >nMinDisFormBor && pt2f[2].x >nMinDisFormBor && pt2f[3].x >nMinDisFormBor
						&&
						pt2f[0].y >nMinDisFormBor && pt2f[1].y >nMinDisFormBor && pt2f[2].y >nMinDisFormBor && pt2f[3].y >nMinDisFormBor
						&&
						pt2f[0].x <nW-nMinDisFormBor && pt2f[1].x <nW-nMinDisFormBor && pt2f[2].x <nW-nMinDisFormBor && pt2f[3].x <nW-nMinDisFormBor
						&&
						pt2f[0].y <nH -nMinDisFormBor && pt2f[1].y <nH -nMinDisFormBor && pt2f[2].y <nH -nMinDisFormBor && pt2f[3].y <nH -nMinDisFormBor
						||
						dcontourArea >nW*nH/nMinDisFormBor
						)
						)
					{
						//轮廓合并
						pMax =  MergerCvSeq(pMax,c,pStorage);
					}//是否合并的条件
				}
			}//for (CvSeq * c = pContour; c != NULL; c = c->h_next)
		}//if(pMax != 0)

}
#if 1
void CDetectRectByContours::contourMerge(vector<vector<Point> > pContour,vector<Point> &pMax,int nW,int nH)
{
	if(pMax.size() != 0)
	{
		RotatedRect boxMax = minAreaRect(pMax);
		//CvBox2D boxMax = cvMinAreaRect2(pMax);
		int nHeightMax = boxMax.size.height*0.8;
		int nHeightMin = boxMax.size.height*0.1;
		int nWidthMax = boxMax.size.width*0.8;
		int nWidthMin = boxMax.size.width*0.1;
		cv::Point2f pt2f[4];
		int nMinDisFormBor = 50;

		for (vector<vector<Point> >::iterator itr = pContour.begin(); itr != pContour.end() ; itr++)
		{
			RotatedRect rotateRect = minAreaRect(*itr);
			rotateRect.points(pt2f);
			

			if(boxMax.size.width == rotateRect.size.width && 
				boxMax.size.height == rotateRect.size.height &&
				boxMax.center.x == rotateRect.center.x &&
				boxMax.center.y == rotateRect.center.y)
			{
				continue;
			}
			else
			{
				int nMinV = boxMax.size.width < boxMax.size.height ? boxMax.size.width:boxMax.size.height;
				int nDistance = sqrt(pow((boxMax.center.x - rotateRect.center.x),2)+pow((boxMax.center.y - rotateRect.center.y),2));


				double dcontourArea = contourArea(*itr);
				if(
					(
					dcontourArea > nW*nH/400
					&&
					nDistance < nMinV*1.5 + max(boxMax.size.height,boxMax.size.width)/2
					&&
					pt2f[0].x >nMinDisFormBor && pt2f[1].x >nMinDisFormBor && pt2f[2].x >nMinDisFormBor && pt2f[3].x >nMinDisFormBor
					&&
					pt2f[0].y >nMinDisFormBor && pt2f[1].y >nMinDisFormBor && pt2f[2].y >nMinDisFormBor && pt2f[3].y >nMinDisFormBor
					&&
					pt2f[0].x <nW-nMinDisFormBor && pt2f[1].x <nW-nMinDisFormBor && pt2f[2].x <nW-nMinDisFormBor && pt2f[3].x <nW-nMinDisFormBor
					&&
					pt2f[0].y <nH -nMinDisFormBor && pt2f[1].y <nH -nMinDisFormBor && pt2f[2].y <nH -nMinDisFormBor && pt2f[3].y <nH -nMinDisFormBor
					||
					dcontourArea >nW*nH/nMinDisFormBor
					)
					)
				{
					//轮廓合并
					for(int idx =0;idx <(*itr).size();idx++)
						pMax.push_back((*itr)[idx]);
				}//是否合并的条件
			}
		}
	}//if(pMax != 0)

}
#endif
