#include "stdafx.h"
#include "DetectionWhitePage.h"
#include "../ColorSpace/AdaptiveThreshold.h"
#include "AutoLevel.h"
//V0.0.3
//V0.0.4 2020-8-14 Tim.liu 增加新版空白页检测

//新版检测空白页
bool CDetectionWhitePage::isWhitePage_new(Mat src,int detectThreshold)
{
	Mat dst;
	if(min(src.rows ,src.cols) >600)
	{
		float fratio = 600.0/(min(src.rows,src.cols));
		resize(src,dst,Size(src.cols*fratio,src.rows*fratio));
	}
	else
		dst =src.clone();

	//预处理
	Mat mask;
	if (CDetectionWhitePage::grad(dst,mask))
	{
		std::vector<std::vector<cv::Point>> contours;
		cv::findContours(mask.clone(), contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
		contours.erase(std::remove_if(contours.begin(), contours.end(), 
			[](const std::vector<cv::Point>& c){return cv::contourArea(c) < 10; }), contours.end());

		// 显示图像并保存
		Mat temp = Mat::zeros(mask.size(),CV_8UC1); 
		cv::drawContours(temp, contours, -1, cv::Scalar(255), -1);
		mask = temp.clone();

		rectangle(mask, Rect(0, 0, src.cols - 1, src.rows - 1), Scalar(255), 2);
		Point vpoint(0,0);
		Mat bw = CDetectionWhitePage::bwlabel_mask(mask, vpoint);
		int scoreNum = mean(mask)[0]*mask.rows*mask.cols/255;
		//cout<<"count=:"<<scoreNum<<endl;
		if(scoreNum >detectThreshold)
			return false;
	}
	return true;
}

//检测空白页
bool CDetectionWhitePage::isWhitePage(Mat src,int detectThreshold)
{
	Mat dst;
	if(src.rows *src.cols >3800000)
	{
		float fratio = 3800000.0/(float(src.rows*src.cols));
		resize(src,dst,Size(src.cols*fratio,src.rows*fratio));
	}
	else
		dst =src.clone();

	//预处理
	CAutoLevel::backGroundColorProcessing(dst,dst);
	//求解轮廓
	vector<vector<Point> > vvPoint = CDetectionWhitePage::findImageContours(dst);

	int scoreNum = CDetectionWhitePage::calcuateConditionWeight(vvPoint);
	
	if(scoreNum >detectThreshold)
		return false;
	return true;
}

//求解图像轮廓
vector<vector<Point> > CDetectionWhitePage::findImageContours(Mat src)
{
	//预处理
	Mat gray;
	if (src.channels() ==1)
		gray =  src.clone();
	else
		cvtColor(src,gray,CV_RGB2GRAY);
	//二值化
    //IplImage *ipl1= &IplImage(gray);
    IplImage ipl1Tmp = IplImage(gray);
    IplImage *ipl1 = &ipl1Tmp;
	IplImage *ipl2 =cvCreateImage(cvGetSize(ipl1),ipl1->depth,1);
	CAdaptiveThreshold::AdaptiveThreshold(ipl1,ipl2);
	cvXorS(ipl2,cvScalarAll(255),ipl2);
	gray =cvarrToMat(ipl2,true);
	cvReleaseImage(&ipl2);
	//求出轮廓
	Mat dst = gray.clone();
	medianBlur(dst,dst,5);

	vector<vector<Point> > vvPoint;
	findContours(dst,vvPoint,CV_RETR_CCOMP,CV_CHAIN_APPROX_NONE);
	//返回图像轮廓
	return vvPoint;
}

//根据轮廓元素个数进行排序
bool cmpPoint(vector<Point> vp1,vector<Point> vp2)
{
	if(vp1.size() > vp2.size())
		return true;
	else
		return false;
}

//计算权重分
int CDetectionWhitePage::calcuateConditionWeight(vector<vector<Point> > vvPoint,int leastNumofVector /* =10 */)
{
	vector<vector<Point> > vvPointQualified;
	int scoreNum =0;
	//比例分
	int scoreNum_NumRatio =30;
	int scoreNum_Distribution =40;
	int scoreNum_SpecialCase =30;
	//【Part1.符合条件轮廓部分】
	for (vector<vector<Point> >::iterator itr = vvPoint.begin(); itr != vvPoint.end() ; itr++)
	{
		if( itr->size() >leastNumofVector )
			vvPointQualified.push_back(*itr);
	}
	if(vvPointQualified.size() ==0)
		return 0;
	float cfRatio = (float)vvPointQualified.size() /(float)vvPoint.size();
	if( cfRatio >= 0.7 &&  vvPointQualified.size() > 100)
		scoreNum_NumRatio =30;
	else
		if(vvPointQualified.size() <100)
			scoreNum_NumRatio = int(30.0 * cfRatio * 1.4)*0.5;
		else
			scoreNum_NumRatio = int(30.0 * cfRatio * 1.4)*2;

	if(vvPointQualified.size() ==0)
		return 0;
	//【Part2.轮廓分布权重】
	sort(vvPointQualified.begin(),vvPointQualified.end(),cmpPoint);
	vector<Point> vpMostElements = vvPointQualified[0];
	
	float fArea = (float)contourArea(vpMostElements);
	
	float inFarea =0.0;
	for (vector<vector<Point> >::iterator itr = vvPointQualified.begin(); itr!= vvPointQualified.end() ; itr++)
	{
		if(itr->size() < vpMostElements.size()/10)
			inFarea += contourArea(*itr);
	}

	cfRatio = inFarea /fArea*30;
	if(cfRatio >= 0.7 )
		scoreNum_Distribution =40;
	else
		scoreNum_Distribution = int(40.0 * cfRatio * 1.4);
	//【Part3.特殊情况的权重部分】待写
	
	scoreNum = scoreNum_Distribution+scoreNum_NumRatio+scoreNum_SpecialCase;
	return scoreNum;
}


//************************************************************************  
// 函数名称:    bwlabel_mask   
// 函数说明:    二值图像指定连通域搜索保留
// 函数参数:    Mat bw    输入二值图像
// 函数参数:    Point vpoint    输入指定坐标点
// 返 回 值:    Mat  输出图像
//************************************************************************ 
Mat CDetectionWhitePage::bwlabel_mask(Mat &bw, Point vpoint)
{
	int width = bw.cols;
	int height = bw.rows;
	int p = 0;
	vector<Point> vPoint;
	Point p2t;
	Mat parent = Mat::zeros(height, width, CV_8UC1);
	vPoint.clear();
	int j = vpoint.x;
	int i = vpoint.y;
	if (bw.ptr<uchar>(i)[j] != 0)
	{
		p2t.x = j;
		p2t.y = i;
		vPoint.push_back(p2t);
		p = p + 1;
		parent.ptr<uchar>(i)[j] = 255;
		bw.ptr<uchar>(i)[j] = 0;
		int num = 0;
		while (num < p)
		{
			for (int y = vPoint[num].y - 1; y <= vPoint[num].y + 1; y++)
			{
				if (y<0 || y>height - 1)
					continue;
				for (int x = vPoint[num].x - 1; x <= vPoint[num].x + 1; x++)
				{
					if (x<0 || x>width - 1)
						continue;
					if (bw.ptr<uchar>(y)[x] != 0)
					{
						p2t.x = x;
						p2t.y = y;
						vPoint.push_back(p2t);
						parent.ptr<uchar>(y)[x] = 255;
						bw.ptr<uchar>(y)[x] = 0;
						p = p + 1;
					}
				}
			}
			num = num + 1;
		}
	}
	return parent;
}

bool CDetectionWhitePage::grad(Mat src ,Mat &mask)
{
	if (src.empty())
		return false;

	Mat gray = src.clone();
	if (src.channels()==3)
	{
		//vector<cv::Mat> rgbChannels(3); 
		//split(src, rgbChannels);
		//gray = rgbChannels[0].mul(0.34)+rgbChannels[1].mul(0.33)+rgbChannels[2].mul(0.33);
		//addWeighted(rgbChannels[0], 0.33, rgbChannels[1], 0.33, 0, gray);
		cvtColor(src,gray, CV_BGR2GRAY);
	}
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	Mat grad;
	if (1)
	{
		Mat abs_grad_x, abs_grad_y;
		/// 求 X方向梯度
		//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
		Sobel(gray, grad, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
		convertScaleAbs(grad, abs_grad_x);

		/// 求Y方向梯度
		//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
		Sobel(gray, grad, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
		convertScaleAbs(grad, abs_grad_y);

		/// 合并梯度(近似)
		addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);
	}
	double aaa = mean(grad)[0];
	double avg = (mean(grad)[0]*2>40?mean(grad)[0]*2:40);
	mask = grad>avg;
	return true;
}
