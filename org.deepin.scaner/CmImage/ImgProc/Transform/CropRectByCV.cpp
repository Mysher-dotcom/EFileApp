#include "stdafx.h"
#include "CropRectByCV.h"
#include "../ColorSpace/AdaptiveThreshold.h"
//V1.0.1

CCropRectByCV::CCropRectByCV(void)
{
	this->m_nBoxCount = 0;
	this->m_nContoursCount = 0;
	this->m_pContours = NULL;
	
}

CCropRectByCV::~CCropRectByCV(void)
{
	if (m_nContoursCount != 0)
	{
		cvReleaseMemStorage(&m_pContours->storage);
	}
}

bool CCropRectByCV::isInTheBoxes(CvBox2D * boxes,int count,CvBox2D box,int & index)
{
	int i,j;
	CvMat* pmatContour = cvCreateMat(1,4,CV_32FC2);
	CvPoint2D32f srcPoints[4];

	CvMat* pmatContourCheck = cvCreateMat(1,4,CV_32FC2);
	CvPoint2D32f checkPoints[4];
	cvBoxPoints(box,checkPoints);
	for(j = 0;j<4;j++)
	{		
		CvScalar vertex = cvScalar(checkPoints[j].x, checkPoints[j].y);
		cvSet1D(pmatContourCheck,j,vertex);
	}
	for(i = 0;i<count;i++)
	{
		cvBoxPoints(boxes[i],srcPoints);
		for(j = 0;j<4;j++)
		{		
			CvScalar vertex = cvScalar(srcPoints[j].x, srcPoints[j].y);
			cvSet1D(pmatContour,j,vertex);
		}
		
		if (cvPointPolygonTest(pmatContour,box.center,1) > 0 ||
			cvPointPolygonTest(pmatContourCheck,boxes[i].center,1) > 0 )
		{
			index = i;
            return true;
		}
	}
    return false;
}

CvSeq * CCropRectByCV::GetContoursFromImage(IplImage* src,float scale,int & count)
{
	this->m_nWidth = src->width;
	this->m_nHeight = src->height;
	this->m_fScale = scale;

	count = 0;
	IplImage * srcImage;
	IplImage * image;
	//LoadImage
	srcImage = cvCloneImage(src);

	image = cvCreateImage(cvSize((int)(src->width * scale),int(src->height * scale)),IPL_DEPTH_8U,src->nChannels);
	//缩小图像
	cvResize(srcImage,image,CV_INTER_NN);

	//模糊图像
	cvSmooth(image,image,CV_MEDIAN,7);
	//创建一个新的灰度图
	IplImage * pGrayImage = cvCreateImage(cvGetSize(image),IPL_DEPTH_8U,1);
	if (src->nChannels != 1)
	{
		cvCvtColor(image,pGrayImage,CV_BGR2GRAY);
	}
	else
	{
		pGrayImage->imageData = image->imageData;
	}
	//求边界
#if 0
	cvCanny(pGrayImage,pGrayImage,20,40);//只支持灰度
	cvDilate(pGrayImage,pGrayImage);
	cvErode(pGrayImage,pGrayImage);
#else
	//CAdaptiveThreshold::AdaptiveThreshold(pGrayImage,pGrayImage);
	////cvSmooth(pGrayImage,pGrayImage,CV_MEDIAN,3,3,0,0);
	//cvDilate(pGrayImage,pGrayImage);
	//cvErode(pGrayImage,pGrayImage);
	int nThreshold = CAdaptiveThreshold::OptimalThreshold(pGrayImage);
	cvThreshold(pGrayImage,pGrayImage,nThreshold,255,THRESH_BINARY);
	cvSmooth(pGrayImage,pGrayImage,CV_MEDIAN,7,7);
	int nRow = 2*(min(pGrayImage->width,pGrayImage->height)/40)+1;
//	IplConvKernel* kernel = cvCreateStructuringElementEx(7,7,4,4,CV_SHAPE_RECT);

	//cvDilate(pGrayImage,pGrayImage,kernel);
	//cvErode(pGrayImage,pGrayImage,kernel);
#endif
	//获取轮廓
	CvMemStorage * pStorage = cvCreateMemStorage();

	if (m_rect.width > 0 && m_rect.height > 0 )
	{
		m_rect.x *= scale;
		m_rect.y *= scale;
		m_rect.width *= scale;
		m_rect.height *= scale;
		cvSetImageROI(pGrayImage,m_rect);
	}

	count = cvFindContours(pGrayImage,pStorage,&this->m_pContours,sizeof(CvContour),
		CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);//只支持黑白
#if 0
	//cvDrawContours(srcImage,pStorage,CvScalar(255,255,255),CvScalar(255,255,255),-1,CV_FILLED);
	int test = pGrayImage->nChannels;
	cvDrawContours(pGrayImage,this->m_pContours,CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 2, CV_FILLED, 8, cvPoint(0, 0)); 

	/*cvReleaseMemStorage(&pStorage);
	test =pGrayImage->nChannels;
	count = cvFindContours(pGrayImage,pStorage,&this->m_pContours,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
*/
#endif
#if 0
	//显示图像
	cvNamedWindow("fillImage",0);
	cvShowImage("fillImage",pGrayImage);
	waitKey();
#endif
	this->m_nContoursCount = count;
	//释放Image
	cvReleaseImage(&srcImage);
	cvReleaseImage(&image);
	cvReleaseImage(&pGrayImage);
	return m_pContours;
}

void CCropRectByCV::CaculateBoxForContours(CvSeq * srcContours,int srcCount)
{
	int count = 0;
	int effectSize = (int)(this->m_nWidth * this->m_fScale * 
		this->m_nHeight * m_fScale / MAXBOXCOUNT / 10);
	int n = 0;
	CvBox2D box;
	
	for (CvSeq * c = srcContours ; 
		c != NULL &&  n < srcCount && count < MAXBOXCOUNT;
		c= c->h_next)
	{
		double arc = fabs(cvContourArea(c,CV_WHOLE_SEQ));
		if (arc > effectSize)
		{
			box = cvMinAreaRect2(c);
			m_pBoxes[count] = box;
			count ++;
		}
		else
		{
			//cvSeqRemove(srcContours,n);
		}
		n ++;
	}
	this->m_nBoxCount = count;
}

void CCropRectByCV::CaculatePoint()
{
	CvPoint2D32f srcPoints[4];
	for (int i = 0;i<m_nBoxCount;i++)
	{
		float tempAngle = m_pBoxes[i].angle;
		if (tempAngle < - 45)
		{
			tempAngle = 90 + tempAngle;
		}
		if (tempAngle > 45)
		{
			tempAngle = - 90 + tempAngle;
		}
		m_angle[i] = tempAngle;
		cvBoxPoints(m_pBoxes[i],srcPoints);
		
		m_aer[i] = m_pBoxes[i].size.width * m_pBoxes[i].size.height;

		for(int j = 0;j<4;j++)
		{
			this->m_pPoints[i * 4 + j].x = (int)(srcPoints[j].x + 0.5 + m_rect.x) / this->m_fScale;
			this->m_pPoints[i * 4 + j].y = (int)(srcPoints[j].y + 0.5 + m_rect.y) / this->m_fScale;
		}
	}
}

int CCropRectByCV::FilterBoxes(CvBox2D * srcBoxes,int srcCount)
{
	int n = 0;
	int effectCount = 0;
	int effectWidth = 150;
	int effectHeight = 150;
	CvBox2D * desBoxes = new CvBox2D[srcCount];
	for (n = 0;n<srcCount;n++)
	{
		if (srcBoxes[n].size.height > effectHeight && srcBoxes[n].size.width > effectWidth)
		{
			desBoxes[effectCount] = srcBoxes[n];
		}
	}
	for (n = 0;n<effectCount;n++)
	{
		srcBoxes[n] = desBoxes[n];
	}
	delete desBoxes;
	return effectCount;
}

void CCropRectByCV::GetBoxes(CvPoint * pointes[],float * m_angle[],int & boxesCount)
{
	* pointes = (CvPoint *)this->m_pPoints;
	* m_angle = (float *)this->m_angle;
	boxesCount = this->m_nBoxCount;
}

void CCropRectByCV::Process(IplImage* src,float scale)
{
	int count = 0;
	this->GetContoursFromImage(src,scale,count);
	this->CaculateBoxForContours(this->m_pContours,this->m_nContoursCount);
	this->CaculatePoint();
	//this->CombineBoxes();
}

void CCropRectByCV::SetEffectRect(CvRect rc)
{
	
	m_rect = rc;
	
}

void CCropRectByCV::CombineBoxes()
{
	CvPoint points[8];
	float angle;
	//获取最大区域Box
	int maxAre = 0;
	int maxAreIndex = -1;
	for (int i = 0 ; i < m_nBoxCount ; i ++)
	{
		if (m_aer[i] > maxAre)
		{
			maxAreIndex = i;
			maxAre = m_aer[i];
		}
	}
	if (maxAreIndex < 0 || maxAreIndex >= m_nBoxCount)
	{
		return;
	}

	//保存最大区域Box
	for (int i = 0 ;i < 4; i ++)
	{
		points[i].x = m_pPoints[maxAreIndex * 4 + i].x;
		points[i].y = m_pPoints[maxAreIndex * 4 + i].y;
	}
	angle = m_angle[maxAreIndex];

	//找到需要合并的Box ,角度相同
	int combineBoxIndex = -1;
	for (int i = 0 ;i < m_nBoxCount ; i ++)
	{
		if (i == maxAreIndex)
		{
			continue;
		}
		if (abs(m_angle[i] - angle) < 2)
		{
			combineBoxIndex = i;
			break;
		}
	}

	if (combineBoxIndex < 0 || combineBoxIndex >= m_nBoxCount
		|| combineBoxIndex == maxAreIndex)
	{
		for (int i = 0 ;i < 4; i ++)
		{
			m_pPoints[i].x = points[i].x;
			m_pPoints[i].y = points[i].y;
		}
		m_nBoxCount = 1;
		return;
	}
	for (int i = 0 ;i < 4; i ++)
	{
		points[ 4 + i].x = m_pPoints[combineBoxIndex * 4 + i].x;
		points[4 + i].y = m_pPoints[combineBoxIndex * 4 + i].y;
	}

	//合并Box
	CvMemStorage* st1 = cvCreateMemStorage(0);
	CvSeq* sq = cvCreateSeq(CV_SEQ_KIND_GENERIC  | CV_32SC2, sizeof(CvContour), sizeof(CvPoint), st1);
	for (int i = 0; i < 8; ++i)
	{
		cvSeqPush(sq, &points[i]);
	}
	
	CvBox2D box = cvMinAreaRect2(sq);
	CvPoint2D32f srcPoints[4];
	cvBoxPoints(box,srcPoints);
	cvReleaseMemStorage(&st1);
	for (int i = 0 ;i < 4; i ++)
	{
		m_pPoints[i].x = srcPoints[i].x;
		m_pPoints[i].y = srcPoints[i].y;
	}
	m_nBoxCount = 1;
}
