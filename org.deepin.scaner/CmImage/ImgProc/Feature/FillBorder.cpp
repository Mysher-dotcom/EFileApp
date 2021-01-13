#include "stdafx.h"
#include "FillBorder.h"
#include "../ColorSpace/AdaptiveThreshold.h"

#define _FillBorder_Log 0

//V1.0.20
bool CFillBorder::FillBorder(Mat src, Mat &dst)
{
	if(!src.data)
	{
		return false;
	}
	Mat gray,bw1,src1;

	src.copyTo (src1);
	dilate(src1,src1,Mat(3,3,CV_8U),Point(-1,-1),2);//erode
	Mat srcnew=Mat::zeros(src.rows+2,src.cols+2,src.type());

	Mat imageROI = srcnew(cv::Rect(1,1,src.cols,src.rows));
	src1.copyTo(imageROI);
	srcnew.copyTo(dst);
	if(dst.channels()==3)
	{
		cvtColor(dst,gray,CV_RGB2GRAY);
	}
	else
	{
		dst.copyTo(gray);
	}
    //IplImage* asrc=&(IplImage)gray;
    IplImage asrcTmp = (IplImage)gray;
    IplImage* asrc= &asrcTmp;

	int threshold1 = CAdaptiveThreshold::OptimalThreshold(asrc);
	
	if(threshold1>180)
	{
		threshold1=threshold1-150;
	}
	else if(threshold1<=130&&threshold1>0)
	{
		threshold1=threshold1-90;
	}
	else
	{
		threshold1=threshold1-100;
	}
	while(threshold1<=0)
	{
		threshold1=threshold1+10;
	}
	threshold(gray,bw1,threshold1,255,CV_THRESH_BINARY);

	dst =bw1;
	return true;
	
	vector<vector<Point> > contours;
	vector<vector<Point> > resultcontours;
	findContours(bw1,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
	// findContours(bw1,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
	// draw
	int num=0;
	int index=0;
	for(int i=0;i<contours.size();i++)
	{
		if(num<contours[i].size())
		{
			num=contours[i].size();
			index=i;
		}
	}
	resultcontours.push_back (contours[index]);
	Rect ccomp;
	Mat result(dst.size(),CV_8U,Scalar(0));
	Mat result1(dst.size(),CV_8U,Scalar(255));

	drawContours(result,resultcontours,-1,Scalar(255),1);
	drawContours(result1, resultcontours, 0, Scalar(0), CV_FILLED);
	dst = result1;
	return true;
	// floodFill(result, Point(1,1), Scalar(255,255,255), &ccomp, Scalar(255,255,255),Scalar(255,255,255));
	/* namedWindow("contours",CV_WINDOW_NORMAL);
	imshow("contours",result);*/

	// Mat result2(dst.size(),CV_8U,Scalar(0));
	result= result1+result;
	/* namedWindow("contours1",CV_WINDOW_NORMAL);
	imshow("contours1",result1);
	cv::waitKey ();*/
	vector<Mat> result3;
	result3.push_back (result);
	result3.push_back (result);
	result3.push_back (result);
	if(dst.channels()==3)
	{
	merge(result3,dst);
	}
	else
	{
		result.copyTo(dst);
	}
	dilate(dst,dst,Mat(5,3,CV_8U),Point(-1,-1),2);//erode
	erode(dst,dst,Mat(2,3,CV_8U),Point(-1,-1),2);//erode

	src.copyTo(imageROI);
	dst=dst+srcnew;
	return true;
}

//排序
bool cmp(int x1,int x2)
{
	if(x1 < x2)
		return true;
	else
		return false;
}

//x方向排序(升序)
bool cmpX(Point pt1,Point pt2)
{
	if(pt1.x < pt2.x)
		return true;
	else if(pt1.x == pt2.x && pt1.y < pt2.y)
		return true;
	else 
		return false;
}

//x方向排序
bool cmpX1(Point pt1,Point pt2)
{
	if(pt1.x < pt2.x)
		return true;
	else if(pt1.x == pt2.x && pt1.y > pt2.y)
		return true;
	else 
		return false;
}

//y方向排序（升序）
bool cmpY(Point pt1,Point pt2)
{
	if(pt1.y <pt2.y)
		return true;
	else if(pt1.y == pt2.y && pt1.x < pt2.x)
		return true;
	else
		return false;
}

//y方向排序
bool cmpY1(Point pt1,Point pt2)
{
	if(pt1.y <pt2.y)
		return true;
	else if(pt1.y == pt2.y && pt1.x > pt2.x)
		return true;
	else
		return false;
}

//找寻背景色
bool CFillBorder::findMainColor(vector<int> vr,vector<int> vg,vector<int> vb,Scalar &color)	
{
	sort(vr.begin(),vr.end());//,cmp);
	sort(vg.begin(),vg.end());//,cmp);
	sort(vb.begin(),vb.end());////,cmp);
	int maxN,idx,tmop,r,g,b,idNum;
	maxN = idx = tmop =r=g=b=idNum=0;
	int idr,idg,idb;
	idr =idg =idb =0;
	for (vector<int>::iterator itr =vr.begin();itr!=vr.end();itr++,idNum++)
	{
		int tmp =*itr;
		if(tmp%10 ==0)
		{
			if(tmop>maxN)
			{
				maxN =tmop;
				idx =*itr;
				tmop=0;
				idr = idNum;
			}
		}else
			tmop++;
	}
	r =idx;
	maxN = idx = tmop =idNum =0;
	for (vector<int>::iterator itr =vg.begin();itr!=vg.end();itr++,idNum++)
	{
		int tmp =*itr;
		if(tmp%10 ==0)
		{
			if(tmop>maxN)
			{
				maxN =tmop;
				idx =*itr;
				tmop=0;
				idg = idNum;
			}
		}else
			tmop++;
	}
	g=idx;
	maxN = idx = tmop =idNum=0;
	for (vector<int>::iterator itr =vb.begin();itr!=vb.end();itr++,idNum++)
	{
		int tmp =*itr;
		if(tmp%10 ==0)
		{
			if(tmop>maxN)
			{
				maxN =tmop;
				idx =*itr;
				tmop=0;
				idb = idNum;
			}
		}else
			tmop++;
	}
	b=idx;
	
	idx = max(idb,max(idr,idg));//(idr +idg +idb)/3;

	float changeRatio = 1;
	if(idx <vb.size() && idx <vg.size() &&idx < vb.size())
		color = Scalar(vb[idx]*changeRatio,vg[idx]*changeRatio,vr[idx]*changeRatio);
	else
		color =Scalar(b*changeRatio,g*changeRatio,r*changeRatio);

	return true;
}

//补边
#include <numeric>
#include <algorithm>
using namespace std;
bool CFillBorder::FillBorder2(const Mat &src, Mat &dstR,int offsetNum /* =2 */,int mappingScope /* =-1 */,int isColorFill,Scalar color /* =Scalar(255,255,255) */)
{
	bool isScanner = true;
	if(abs(isColorFill) >=10)
	{
		isColorFill %=10;
		isScanner = false;
	}
	if(isColorFill != -1 &&isColorFill !=0 && isColorFill !=1)
		isColorFill = 1;

	int increaseBorNum =10;
	//对图像补位
	Mat dst = Mat::zeros(src.rows+2*increaseBorNum,src.cols+2*increaseBorNum,src.type());
	Mat dstROI = dst(cv::Rect(increaseBorNum,increaseBorNum,src.cols,src.rows));		
	src.copyTo(dstROI);
	//图像轮廓
	RotatedRect rMaxRect;//轮廓最小外接矩形	
	Mat temp;
	vector<Point> areaMaxPoint = CFillBorder::findMaxContour(dst,temp,rMaxRect,isScanner);

	if(0 == areaMaxPoint.size())
		return false;
	
	Point minY,minX,maxY,maxX;
	vector<int> vleftUpPt,vRightUpPt,vleftDownPt,vRightDownPt;
	vector<int> vInt;int idt =0;
	for (vector<Point>::iterator itr = areaMaxPoint.begin();itr!=areaMaxPoint.end();itr++,idt++)
	{
		vInt.push_back(idt);
		//左上角点
		int ix = (int)sqrt(float(itr->x*itr->x +itr->y*itr->y));
		vleftUpPt.push_back(ix);

		//右上角点
		ix = (int)sqrt(float((src.cols - itr->x)*(src.cols - itr->x) + itr->y *itr->y));
		vRightUpPt.push_back(ix);

		//左下角点
		ix =(int)sqrt(float(itr->x*itr->x + (src.rows-itr->y)*(src.rows-itr->y)));
		vleftDownPt.push_back(ix);

		//右下角点
		ix =(int)sqrt(float((src.cols -itr->x)*(src.cols -itr->x) +(src.rows-itr->y)*(src.rows-itr->y)));
		vRightDownPt.push_back(ix);
	}
	//排序
	vector<int>::iterator itrUPlmin = min_element(vleftUpPt.begin(),vleftUpPt.end());
	vector<int>::iterator itrUPRmin = min_element(vRightUpPt.begin(),vRightUpPt.end());
	vector<int>::iterator itrLOWlmin =min_element(vleftDownPt.begin(),vleftDownPt.end());
	vector<int>::iterator itrLOWRmin =min_element(vRightDownPt.begin(),vRightDownPt.end());
	//求出四个顶点
	minX = areaMaxPoint[distance(vleftUpPt.begin(),itrUPlmin)];
	maxX = areaMaxPoint[distance(vRightUpPt.begin(),itrUPRmin)];
	minY = areaMaxPoint[distance(vleftDownPt.begin(),itrLOWlmin)];
	maxY = areaMaxPoint[distance(vRightDownPt.begin(),itrLOWRmin)];
	//四条边
	vector<Point> vpXlow;
	vector<Point> vpXup;
	vector<Point> vpYlow;
	vector<Point> vpYup;
	bool isFirLine,isSecLine,isThrLine,isFourLine;
	isFirLine = isSecLine = isThrLine = isFourLine = true;
	vector<Point> vPStartTmp;
	for (vector<Point>::iterator itr =areaMaxPoint.begin();itr!=areaMaxPoint.end();itr++)
	{
		//末边
		if((itr->x != minX.x || itr->y != minX.y) && isFirLine)
		{vPStartTmp.push_back(*itr);continue;}
		isFirLine =false;
		//左边
		if((itr->x !=minY.x || itr->y != minY.y) &&isSecLine)
		{vpXlow.push_back(*itr);continue;}
		else
		isSecLine =false;
		//下边
		if((itr->x !=maxY.x || itr->y !=maxY.y) && isThrLine)
		{vpYup.push_back(*itr);continue;}
		isThrLine =false;
		//右边
		if((itr->x !=maxX.x || itr->y !=maxX.y) && isFourLine)
		{vpXup.push_back(*itr);continue;}
		isFourLine =false;

		vpYlow.push_back(*itr);
	}
	for(vector<Point>::iterator itr =vPStartTmp.begin();itr!=vPStartTmp.end();itr++)
		vpYlow.push_back(*itr);

	vpXlow.push_back(minY);vpYup.push_back(maxY);vpXup.push_back(maxX);vpXlow.push_back(minX);
	//////////////////////////////////////////////////////////////////////////补边
	
	Point ptSt,ptEnd;
	//上边
	if(vpYlow.size() >0)
	{
		ptSt =vpYlow[0];
		ptEnd =vpYlow[vpYlow.size()-1];
	}

	if(ptEnd.x >9)
		for(int idx =1;idx<=ptEnd.x-9;idx++)
			vpYlow.push_back(Point(ptEnd.x-idx,ptEnd.y));

	if(dst.cols - ptSt.x > 9)
		for(int idx =1;idx <= dst.cols - ptSt.x -8;idx++)
			vpYlow.push_back(Point(ptSt.x+idx,ptSt.y));
	//下边
	if(vpYup.size()>0)
	{
		ptSt = vpYup[0];
		ptEnd = vpYup[vpYup.size()-1];
	}

	if(ptSt.x >9)
		for(int idx =0;idx<=ptSt.x-9;idx++)
			vpYup.insert(vpYup.begin(),Point(ptSt.x-idx,ptSt.y+1));

	if(dst.cols - ptEnd.x > 9)
		for(int idx =1;idx <= dst.cols - ptEnd.x-8;idx++)
			vpYup.push_back(Point(ptEnd.x+idx,ptEnd.y));

	//排序
	sort(vpXlow.begin(),vpXlow.end(),cmpY);
	sort(vpXup.begin(),vpXup.end(),cmpY);
	sort(vpYlow.begin(),vpYlow.end(),cmpX);
	sort(vpYup.begin(),vpYup.end(),cmpX);
	//对四个便进行修复V1.0.10
	CFillBorder::fourLineRepair(vpXlow,vpXup,vpYlow,vpYup,dst.cols);

	//////////////////////////////////////////取背景色进行填充//////////////////////////////////////////
	Scalar colorUp,colorLow,colorLeft,colorRight;
	vector<int> vr,vg,vb;
	int num=0;
	bool limitLoc = false;
	int nScopeTmp = 15;

	for(vector<Point>::iterator itr =vpXlow.begin();itr!=vpXlow.end();itr++)
	{
		if((itr->y > src.rows*1/6 && itr->y <src.rows*2/6)||!limitLoc)
		{
			for(int idc=0;idc<src.channels();idc++)
			{
				int nX = itr->x+nScopeTmp;
				nX = nX >src.cols-1 ? src.cols-1:nX;

				if(idc ==0)
					vr.push_back(dst.ptr<uchar>(itr->y)[(nX)*src.channels()+idc]);
				else if(idc ==1)
					vg.push_back(dst.ptr<uchar>(itr->y)[(nX)*src.channels()+idc]);
				else
					vb.push_back(dst.ptr<uchar>(itr->y)[(nX)*src.channels()+idc]);
			}

			num++;
		}
	}
	CFillBorder::findMainColor(vr,vg,vb,colorLeft);
	//左边
	num =0;
	vr.clear();vg.clear();vb.clear();
	for(vector<Point>::iterator itr =vpXup.begin();itr!=vpXup.end();itr++)
	{
		if((itr->y > src.rows/4 &&itr->y <src.rows*3/4)||!limitLoc)
		{
			for(int idc=0;idc<src.channels();idc++)
			{
				int nX =itr->x-nScopeTmp;
				nX = nX <0?0:nX;

				if(idc ==0)
					vr.push_back(dst.ptr<uchar>(itr->y)[(nX)*src.channels()+idc]);
				else if(idc ==1)
					vg.push_back(dst.ptr<uchar>(itr->y)[(nX)*src.channels()+idc]);
				else
					vb.push_back(dst.ptr<uchar>(itr->y)[(nX)*src.channels()+idc]);
			}

			num++;
		}
	}
	sort(vr.begin(),vr.end(),cmp);
	sort(vg.begin(),vg.end(),cmp);
	sort(vb.begin(),vb.end(),cmp);
	CFillBorder::findMainColor(vr,vg,vb,colorRight);
	//下边
	num =0;
	vr.clear();vg.clear();vb.clear();
	for(vector<Point>::iterator itr =vpYlow.begin()+vpYlow.size()/3;itr!=vpYlow.end()-vpYlow.size()/3;itr++)
	{
		if((itr->x > src.cols/4 && itr->x < src.cols*3/4)||!limitLoc)
		{
			for(int idc=0;idc<src.channels();idc++)
			{
				int nY = itr->y+nScopeTmp;
				nY = nY > src.rows -1? src.rows-1 :nY;

				if(idc ==0)
					vr.push_back(dst.ptr<uchar>(nY)[(itr->x)*src.channels()+idc]);
				else if(idc ==1)
					vg.push_back(dst.ptr<uchar>(nY)[(itr->x)*src.channels()+idc]);
				else
					vb.push_back(dst.ptr<uchar>(nY)[(itr->x)*src.channels()+idc]);
			}

			num++;
		}
	}
	sort(vr.begin(),vr.end(),cmp);
	sort(vg.begin(),vg.end(),cmp);
	sort(vb.begin(),vb.end(),cmp);
	CFillBorder::findMainColor(vr,vg,vb,colorLow);
	//上边
	num =0;
	vr.clear();vg.clear();vb.clear();
	for(vector<Point>::iterator itr =vpYup.begin()+vpYup.size()/3;itr!=vpYup.end()-vpYup.size()/3;itr++)
	{
		if((itr->x > src.cols/4 && itr->x < src.cols*3/4)||!limitLoc)
		{
			for(int idc=0;idc<src.channels();idc++)
			{
				int nY = itr->y - nScopeTmp;
				nY = nY < 0? 0:nY;

				if(idc ==0)
					vr.push_back(dst.ptr<uchar>(nY)[(itr->x)*src.channels()+idc]);
				else if(idc ==1)
					vg.push_back(dst.ptr<uchar>(nY)[(itr->x)*src.channels()+idc]);
				else
					vb.push_back(dst.ptr<uchar>(nY)[(itr->x)*src.channels()+idc]);
			}
			num++;
		}
	}
	sort(vr.begin(),vr.end(),cmp);
	sort(vg.begin(),vg.end(),cmp);
	sort(vb.begin(),vb.end(),cmp);
	CFillBorder::findMainColor(vr,vg,vb,colorUp);

	if(true)
	{
	if(vpYup.size() >10) CFillBorder::findSingleLine(vpYup,true);
	if(vpYlow.size()>10) CFillBorder::findSingleLine(vpYlow,true);

	if(vpXup.size() >10)
	{
		CFillBorder::correctCurve(vpXup,1);
		CFillBorder::findSingleLine(vpXup);
	}
	if(vpXlow.size() >10) CFillBorder::findSingleLine(vpXlow);
	}
	//外扩后增加5，离得近则不用补边
	increaseBorNum -= 7;
	/*********************************************图像补边******************************************************************************/
	if(isColorFill ==1)
		color = colorLeft;
	for(vector<Point>::iterator itr=vpXlow.begin();itr!=vpXlow.end();itr++)//左边
	{
		if(itr->x <=increaseBorNum)
			continue;
		int idScope =0;
		bool isClockWise =false;
		int locNum = itr->x + offsetNum;
		if(locNum >= dst.cols)
		{
			locNum = dst.cols-1;
			offsetNum = dst.cols -1 -offsetNum;
			offsetNum = offsetNum >0? offsetNum:0;
		}

		for( int idx =itr->x +offsetNum;idx >=0;idx--,idScope++)
		{
			if(!isClockWise)
				locNum++;
			else
				locNum--;

			for(int idc =0; idc <dst.channels();idc++)
			{
				uchar* dataR = &dst.ptr<uchar>(itr->y)[(int)(itr->x+offsetNum-idx)*dst.channels()+idc];

				if(isColorFill == -1  )
				{
					if(mappingScope < 1)
						*dataR = dst.ptr<uchar>(itr->y)[(int)(itr->x+offsetNum+idx)*dst.channels()+idc];
					else
					{
						if(idScope > mappingScope)
						{idScope = idScope - mappingScope;isClockWise= !isClockWise;}

						if(!isClockWise)
						{
							*dataR = dst.ptr<uchar>(itr->y)[(int)(locNum)*dst.channels()+idc];
						}
						else
						{
							*dataR = dst.ptr<uchar>(itr->y)[(int)(locNum)*dst.channels()+idc];
						}
					}
				}
				else 
					if(dst.channels() ==3)
						if(idc ==0)
							*dataR = color[2];
						else if(idc ==1)
							*dataR =color[1];
						else
							*dataR =color[0];
					else
						*dataR =color[2];
			}//end for(int idc=0;idc<dst.channels();idc++)
		}//end for(int idx)
	}//end for(itr)
	if(isColorFill ==1)
		color =colorRight;
	for(vector<Point>::iterator itr=vpXup.begin();itr!=vpXup.end();itr++)//右边
	{
		if(itr->x >=dst.cols -increaseBorNum-1)
			continue;
		int idScope =0;
		offsetNum = offsetNum > itr->x ?itr->x:offsetNum;
		for( int idx = 0 ;idx <  dst.cols -itr->x+offsetNum ;idx++,idScope++)
		{
			for(int idc =0;idc <dst.channels();idc++)
			{
				uchar* dataR = &dst.ptr<uchar>(itr->y)[(itr->x-offsetNum+idx)*dst.channels() +idc];
				if( isColorFill ==-1 )
					if(mappingScope <1)
						*dataR = dst.ptr<uchar>(itr->y)[(itr->x-offsetNum-idx)*dst.channels() +idc];
					else
					{
						if(idScope >mappingScope)
							idScope = idScope -mappingScope;

						int idxTmp = itr->x - offsetNum -idScope;
						idxTmp = idxTmp <0? 0:idxTmp;
						*dataR = dst.ptr<uchar>(itr->y)[(idxTmp)*dst.channels() +idc];
					}
				else
					if(dst.channels() ==3)
						if(idc ==0)
							*dataR = color[2];
						else if(idc ==1)
							*dataR =color[1];
						else
							*dataR =color[0];
					else
						//*dataR = (color[0]+color[1]+color[2])/3;
						*dataR =color[2];
			}
		}
	}
	if(isColorFill ==1)
		color =colorLow;
	for(vector<Point>::iterator itr=vpYlow.begin();itr!=vpYlow.end();itr++) //上边
	{
		//边界位置不做处理
		if(itr->y <=increaseBorNum)
			continue;
		int idy = 0;
		int idScope =0;
		//限制向内偏移范围
		offsetNum = (offsetNum+itr->y)>=dst.rows-1?(dst.rows -itr->y):offsetNum;

		for( int idx = itr->y +offsetNum ;idx > 0;idx--,idy++,idScope++)
		{
			for(int idc =0;idc<dst.channels();idc++)
			{
				uchar* dataR = &dst.ptr<uchar>(idx)[int(itr->x)*dst.channels()+idc];
				if( isColorFill == -1 )
					if(mappingScope <1 )
						*dataR = dst.ptr<uchar>(itr->y+offsetNum+idy)[int(itr->x)*dst.channels()+idc];
					else
					{
						if(idScope >mappingScope)
							idScope = idScope -mappingScope;
						int idxTmp = itr->y +offsetNum +idScope;
						idxTmp = idxTmp >=dst.rows ?dst.rows-1:idxTmp;

						*dataR = dst.ptr<uchar>(idxTmp)[int(itr->x)*dst.channels()+idc];
					}
				else
					if(dst.channels() ==3)
						if(idc ==0)
							*dataR = color[2];
						else if(idc ==1)
							*dataR =color[1];
						else
							*dataR =color[0];
					else
						//*dataR = (color[0]+color[1]+color[2])/3;
						*dataR = color[2];
			}
		}
	}
	if(isColorFill ==1)
		color =colorUp;
	for(vector<Point>::iterator itr=vpYup.begin();itr!=vpYup.end();itr++)//下边
	{
		//边界位置不做处理
		if(itr->y >= dst.rows -increaseBorNum-1)
			continue;
		int idy = 0;
		int idScope =0;
		offsetNum = (itr->y -offsetNum)<0?itr->y:offsetNum;

		for( int idx = itr->y -offsetNum ;idx <dst.rows;idx++,idy++,idScope++)
		{
			for(int idc =0;idc<dst.channels();idc++)
			{
				uchar* dataR = &dst.ptr<uchar>(idx)[int(itr->x)*dst.channels()+idc];
				if( isColorFill == -1 )
					if(mappingScope <1)
					{
						int idxTmp = itr->y -offsetNum -idy;
						idxTmp = idxTmp <0 ? 0:idxTmp;
						*dataR = dst.ptr<uchar>(idxTmp)[int(itr->x)*dst.channels()+idc];
					}
					else
					{
						if(idScope > mappingScope)
							idScope = idScope -mappingScope;
						int idxTmp = itr->y - offsetNum -idScope;
						idxTmp = idxTmp <0?0:idxTmp;
						*dataR = dst.ptr<uchar>(idxTmp)[int(itr->x)*dst.channels()+idc];
					}
				else
					if(dst.channels() ==3)
						if(idc ==0)
							*dataR = color[2];
						else if(idc ==1)
							*dataR =color[1];
						else
							*dataR =color[0];
					else
						//*dataR = (color[0]+color[1]+color[2])/3;
						*dataR = color[2];
			}
		}
	}
	//裁切
	Mat dstROI1 = dst(cv::Rect(10,10,src.cols,src.rows));	
	dstROI1.copyTo(dstR);

	return true;
}
///////////////////////////补边 V1.0.14///////////////////////////////////////////////////////////////
//对模板图进行更改补充（temp暂未使用）
//[in/out]src	输入/输出图像
//[in]nType		
//0上边
//1下边
//2左边
//3右边
void fillVoidInMask(Mat &src,int nType =0)
{
	return ;
	//图像信息
	uchar *data;
	bool isStart = false;
	//转置
	switch(nType)
	{
	case 0:
		{
			transpose(src, src);
			flip(src, src, 0);
		}
		break;

	case 1:
		{
			transpose(src, src);
			flip(src, src, 1);
		}
		break;

	case 3:
		{
			flip(src, src, 1);
		}
		break;
	}

	data = src.ptr<uchar>(0);
	for(int idr=0;idr<src.rows;idr++)
	{
		isStart = false;
		for(int idc=0;idc<src.cols;idc++,data++)
		{
			if(*data == 255)
				isStart = true;

			if(isStart)
				*data = 255;
		}
	}

	//还原
	switch(nType)
	{
	case 0:
		{
			transpose(src, src);
			flip(src, src, 1);
		}
		break;

	case 1:
		{
			transpose(src, src);
			flip(src, src, 0);
		}
		break;

	case 3:
		{
			flip(src, src, 1);
		}
		break;
	}

}

//图像填补
//[in/out]src	输入/输出图像
//[in]binMask	二值模板图
//vlineAll		每一段的轮廓
//nStep			每段轮廓的长度
//vMaxIdx		每段轮廓距离边缘最大长度(功能待完成)
//nSize			形态学尺寸
//color			纯色填充颜色

void fillEdgeCore(Mat &matRoi,const Mat &rectMatMask,const Mat &matRoiSampling,Scalar color,bool isMapFill,int mappingScope,int nType)
{
#if _FillBorder_Log
	g_log->Log("fillEdgeCore-start");
#endif
	//相邻背景色
	int pre_meanR,pre_meanG,pre_meanB;
	pre_meanR = pre_meanG = pre_meanB =-1;

	//模板图像
	Mat rectMat = matRoi.clone();
	Mat hsiMat;
	
	cvtColor(rectMat,hsiMat,CV_BGR2HSV);

	//g_log->Log("fill 122");
	if(!isMapFill)
	{
		uchar *data = hsiMat.ptr<uchar>(0);
		const uchar *dataMask = rectMatMask.ptr<uchar>(0);
		const uchar *dataSampling = matRoiSampling.ptr<uchar>(0);

		//计算背景色
		int meanR,meanG,meanB,idxNum;
		meanR=meanG=meanB=idxNum=0;

		//g_log->Log("fill 123");
		int rArr[26]={0};
		int gArr[26]={0};
		int bArr[26]={0};

		if(color[0]==-1 || color[1] ==-1 || color[2] == -1)
		{
			if(rectMat.channels() ==3)
			{
				for(int idr=0;idr <rectMat.rows;idr++)
				{
					for(int idc=0;idc <rectMat.cols;idc++,data+=rectMat.channels(),dataMask++,dataSampling++)
					{
						if(*dataMask ==255 || *dataSampling == 255)
						{
							rArr[*(data)/10]++;
							gArr[*(data+1)/10]++;
							bArr[*(data+2)/10]++;
						}
					}
				}
			}//end channel
			else
			{
				for(int idr=0;idr <rectMat.rows;idr++)
				{
					for(int idc=0;idc <rectMat.cols;idc++,data+=rectMat.channels(),dataMask++)
					{
						if(*dataMask ==255 || *dataSampling == 255)
						{
							rArr[*data/10]++;
						}
					}
				}
			}//end channel
			//g_log->Log("fill 124");
			if(-1 ==pre_meanR || -1 == pre_meanG || -1== pre_meanB)
			{
				pre_meanR = distance(rArr,max_element(rArr,rArr+26))*10;
				pre_meanG = distance(gArr,max_element(gArr,gArr+26))*10;
				pre_meanB = distance(bArr,max_element(bArr,bArr+26))*10;
			}
			//g_log->Log("fill 15");
			meanR = (distance(rArr,max_element(rArr,rArr+26))*10 + pre_meanR*3)/4;
			meanG = (distance(gArr,max_element(gArr,gArr+26))*10 + pre_meanG*3)/4;
			meanB = (distance(bArr,max_element(bArr,bArr+26))*10 + pre_meanB*3)/4;
			//g_log->Log("fill 126");
			Mat zerosMat=Mat(1,1,CV_8UC3);
			data = zerosMat.ptr<uchar>(0);
			*data = meanR;
			*(data+1) = meanG;
			*(data+2) = meanB;
			cvtColor(zerosMat,zerosMat,CV_HSV2BGR);
			data = zerosMat.ptr<uchar>(0);
			meanR = *data;
			meanG = *(data+1);
			meanB = *(data+2);

		}//end color
		else//纯色填充
		{
			meanR = color[2];
			meanG = color[1];
			meanB = color[0];
		}
		//g_log->Log("fill 127");
		//指针重新指向
		data = rectMat.ptr<uchar>(0);
		dataMask = rectMatMask.ptr<uchar>(0);

		//g_log->Log("fill 128");
		//颜色补边
		if(rectMat.channels() ==3)
			for(int idr=0;idr <rectMat.rows;idr++)
			{
				for(int idc=0;idc <rectMat.cols;idc++,data+=rectMat.channels(),dataMask+=rectMatMask.channels())
				{
					if( *dataMask == 0)
					{
						*data = meanR;
						*(data+1) = meanG;
						*(data+2) = meanB;
					}
				}
			}
		else
		{
			meanR =(meanR+meanG+meanB)/3;
			for(int idr=0;idr <rectMat.rows;idr++)
			{
				for(int idc=0;idc <rectMat.cols;idc++,data+=rectMat.channels(),dataMask++)
				{
					if( *dataMask == 0)
						*data = meanR;
				}
			}
		}
	}//上面为颜色填充
	else
	{
		Mat maskTmp = rectMatMask.clone();

		//映射填充
		switch(nType)
		{
		case 0://逆时针
			{
				transpose(rectMat, rectMat); 
				flip(rectMat,rectMat,0);

				transpose(maskTmp, maskTmp); 
				flip(maskTmp,maskTmp,0);
			}
			break;

		case 1://顺时针
			{
				transpose(rectMat, rectMat); 
				flip(rectMat,rectMat,1);

				transpose(maskTmp, maskTmp); 
				flip(maskTmp,maskTmp,1);
			}
			break;

		case -2:
			{
				flip(rectMat,rectMat,0);
				flip(maskTmp,maskTmp,0);
			}
			break;

		case -3:
			{
				flip(rectMat,rectMat,-1);
				flip(maskTmp,maskTmp,-1);
			}
			break;

		case -4:
			{
				flip(rectMat,rectMat,1);
				flip(maskTmp,maskTmp,1);
			}
			break;

		case 3:
			{
				flip(rectMat,rectMat,1);
				flip(maskTmp,maskTmp,1);
			}
			break;
		}//end 旋转

		uchar *data;
		uchar *dataMask;
		int non_useNum=0;
		for (int idr=0;idr < rectMat.rows;idr++)
		{
			data = rectMat.ptr<uchar>(idr);
			dataMask = maskTmp.ptr<uchar>(idr);
			int nIdc =-1;
			uchar *dataR =0;
			for (int idc=0; idc < rectMat.cols ; idc++,dataMask++,data+=rectMat.channels())
			{
				if(*dataMask > 128)
				{
					dataR = data;
					nIdc =idc-1;
					break;
				}
			}

			if(nIdc ==-1)
				if(idr == non_useNum)
					non_useNum++;
			
			bool isRight = false;
			int nGap = rectMat.cols - nIdc;
			if(mappingScope >0)
				nGap = mappingScope > nGap?nGap:mappingScope;

			if(rectMat.channels() ==3 &&nIdc != -1)
			{
				for (int idx=0;idx<=nIdc;idx++,data-=3)
				{
					if( nGap >0 )
					{
						if( idx%nGap ==0)
							isRight = !isRight;

						*data = *dataR;
						*(data+1) =*(dataR+1);
						*(data+2) =*(dataR+2);
						
						if(isRight)
							dataR +=3;
						else
							dataR -=3;
					}
				}//彩色图
			}
			else if(nIdc != -1)
			{
				for (int idx=0;idx<=nIdc;idx++,data--)
				{
					if( nGap >0 )
					{
						if( idx%nGap ==0)
							isRight = !isRight;

						*data = *dataR;
						if(isRight)
							dataR ++;
						else
							dataR --;
					}
				}
			}//灰度图
		}

		int nNext = non_useNum;
		
		//上边点
		if(( nType <0)&& non_useNum >0)
		{
			bool  isRight =false;
			for (int idr=0;idr<non_useNum;idr++)
			{
				uchar *data = rectMat.ptr<uchar>(non_useNum-idr-1);
				int nGap = rectMat.rows - non_useNum;
				if(mappingScope >0)
					nGap = mappingScope > nGap?nGap:mappingScope;

				if( nGap !=0 && idr%nGap ==0)
				{
					isRight = !isRight;
				}

				uchar *dataNext = rectMat.ptr<uchar>(nNext);
				
				for(int idc=0;idc<rectMat.cols;idc++)
				{
					for (int idch=0;idch<rectMat.channels();idch++,data++,dataNext++)
						*data = *dataNext;
				}
				if(isRight)
					nNext++;
				else
					nNext--;
			}
		}

		//反向旋转
		switch(nType)
		{
		case 0://逆时针
			{
				transpose(rectMat, rectMat); 
				flip(rectMat,rectMat,1);

				transpose(maskTmp, maskTmp); 
				flip(maskTmp,maskTmp,1);
			}
			break;

		case 1://顺时针
			{
				transpose(rectMat, rectMat); 
				flip(rectMat,rectMat,0);

				transpose(maskTmp, maskTmp); 
				flip(maskTmp,maskTmp,0);
			}
			break;

		case -2:
			{
				flip(rectMat,rectMat,0);
				flip(maskTmp,maskTmp,0);
			}
			break;

		case -3:
			{
				flip(rectMat,rectMat,-1);
				flip(maskTmp,maskTmp,-1);
			}
			break;

		case -4:
			{
				flip(rectMat,rectMat,1);
				flip(maskTmp,maskTmp,1);
			}
			break;

		case 3:
			{
				flip(rectMat,rectMat,1);
				flip(maskTmp,maskTmp,1);
			}
			break;
		}//end 旋转

	}
	//g_log->Log("fill 128");
	//感兴趣区域替换
	rectMat.copyTo(matRoi);
#if _FillBorder_Log
	g_log->Log("fillEdgeCore-end");
#endif
}
void fillEdge(Mat &src,Mat &binMask,vector<vector<Point> > vlineAll,int nStep,vector<vector<int> > vMaxIdx,int nSize=3,Scalar color = Scalar(-1,-1,-1),int nX=0,int nY=0,bool isMapFill =false,int mappingScope=-1)
{
#if _FillBorder_Log
	g_log->Log("fillEdge-start");
#endif
	vector<Point> vline;
	//对模板进行释放外扩
	Mat kernel;
	if(nSize >1)
	{
#if _FillBorder_Log
	g_log->Log("fillEdge-getStructuringElement-start");
#endif

		kernel=getStructuringElement(MORPH_RECT,Size(nSize,nSize));

#if _FillBorder_Log
	g_log->Log("fillEdge-getStructuringElement-end");
#endif
	}

	//方向类型
	int nType =0;
	//判断同临近矩形的大小区别，剔除异常矩形
	Rect rectPrev =Rect(0,0,0,0);

	//g_log->Log("fill 1");
	for(vector<vector<Point> >::iterator itr = vlineAll.begin(); itr != vlineAll.end(); itr++)
	{
 		vline = *itr;
		if( vline.size() <4)
			continue;

		//在矩形上的两个点
		Point pt1,pt2;
		pt1.x = min(vline[0].x ,src.cols - vline[0].x -1);
		pt1.y = min(vline[0].y ,src.rows - vline[0].y -1);

		pt2.x = min(vline[vline.size()-1].x ,src.cols - vline[vline.size()-1].x -1);
		pt2.y = min(vline[vline.size()-1].y ,src.rows - vline[vline.size()-1].y -1);
		//g_log->Log("fill 10");
		bool isVertical1,isVertical2;
		isVertical1 =isVertical2 =true;

		if(pt1.x -nX < pt1.y -nY)
			isVertical1 = false;
		if(pt2.x -nX< pt2.y -nY)
			isVertical2 = false;

		//分类
		Rect rect=Rect(0,0,0,0);
		//四个角
		if(
			(isVertical1 && !isVertical2)
			||
			(!isVertical1 && isVertical2)
			)
		{
			//左侧上下两角
			if(pt1.x==vline[0].x || pt2.x == vline[vline.size()-1].x)
			{
				if(pt1.y == vline[0].y && pt2.y == vline[vline.size()-1].y)//左上角
				{
					rect =Rect(0,0,pt1.x+pt2.x+1,pt1.y+pt2.y+1);
					nType =-1;
				}
				else//左下角
				{
					rect =Rect(0,min(vline[0].y,vline[vline.size()-1].y),pt1.x+pt2.x+1,src.rows -min(vline[0].y,vline[vline.size()-1].y)-1);
					nType = -2;
				}
			}
			else //右侧上下两角
			{
				if(pt1.y == vline[0].y || pt2.y == vline[vline.size()-1].y)//右上角点
				{
					rect =Rect(min(vline[0].x,vline[vline.size()-1].x),0,src.cols-1-min(vline[0].x,vline[vline.size()-1].x),pt1.y+pt2.y+1);
					nType =-4;
				}
				else  //右下角点
				{
					rect = Rect(min(vline[0].x,vline[vline.size()-1].x)-max(pt1.x,pt2.x),min(vline[0].y,vline[vline.size()-1].y)-max(pt1.y,pt2.y),src.cols-1-min(vline[0].x,vline[vline.size()-1].x)+max(pt1.x,pt2.x),src.rows-1-min(vline[0].y,vline[vline.size()-1].y)+max(pt1.y,pt2.y));
					nType = -3;
				}
			}
		}
		else if( !isVertical1 && !isVertical2) //左右侧边
		{
			if(pt1.x == vline[0].x)//左边
			{
				rect = Rect(0,min(vline[0].y,vline[vline.size()-1].y),pt1.x+pt2.x,abs(vline[0].y-vline[vline.size()-1].y)+1);
				
				if(nType !=2 )
				{
					//0->2
					if(nType ==0)
					{
						rect.width += rect.x;
						rect.height += rect.y;
						rect.x =0;
						rect.y =0;
					}

					//1->2
					if(1 == nType)
					{
						rect.width += rect.x;
						rect.x =0;
						rect.height = src.rows -1 -rect.y;
						rect.y =0;
					}
				}//corner

				nType=2;
			}
			else //右边
			{
				rect = Rect(vline[0].x-pt2.x,min(vline[0].y,vline[vline.size()-1].y),pt1.x+pt2.x,max(abs(vline[0].y-vline[vline.size()-1].y)+1,src.rows-1-min(vline[0].y,vline[vline.size()-1].y)));
				
				if(nType !=3)
				{
					//0->3
					if(0== nType)
					{
						rect.width = src.cols - 1 -rect.x;
						rect.height +=rect.y;
						rect.y =0;
					}

					//1->3
					if(1 == nType)
					{
						rect.width = src.cols - 1 -rect.x;
						rect.height += src.rows -1 -rect.y;
					}
				}//corner

				nType =3;
			}
		}
		else if(isVertical1 && isVertical1) //上下侧边
		{
			if(pt1.y == vline[0].y) //上边
			{
				rect = Rect(min(vline[0].x,vline[vline.size()-1].x),0,abs(vline[0].x-vline[vline.size()-1].x)+1,pt1.y+pt2.y);
				
				if(nType !=0 )
				{
					//3->0
					if(nType ==3)
					{
						rect.width = src.cols - 1 -rect.x;
					}

					//2->0
					if(2 == nType)
					{
						rect.width += rect.x;
						rect.x =0;
					}
				}//corner
				
				nType = 0;
			}
			else //下边
			{
				rect = Rect(min(vline[0].x,vline[vline.size()-1].x),min(vline[0].y,vline[vline.size()-1].y) - max(pt2.y,pt1.y),abs(vline[0].x-vline[vline.size()-1].x)+1,src.rows-1-min(vline[0].y,vline[vline.size()-1].y) + max(pt2.y,pt1.y));
				
				if(nType !=1)
				{
					//2->1
					if(2== nType)
					{
						rect.width += rect.x;
						rect.x =0;
					}

					//3->1
					if(3 == nType)
					{
						rect.width = src.cols - 1 -rect.x;
						rect.height += src.rows -1 -rect.y;
					}
				}//corner
				
				nType =1;
			}
		}
		//g_log->Log("fill 11");
   		rect.height *= 1.2;
		rect.width *= 1.2;
		//范围进行限制
		rect.x  = rect.x <0 ? 0:(rect.x > src.cols-1?src.cols-1:rect.x);
		rect.y  = rect.y <0 ? 0:(rect.y > src.rows-1?src.rows-1:rect.y);
		
		rect.height  = rect.y + rect.height > src.rows  ? src.rows - rect.y :rect.height;
		rect.width = rect.x +rect.width >src.cols  ? src.cols  - rect.x :rect.width;

		if(rect.height < 1 || rect.width <1 )
			continue;

		if(itr == vlineAll.begin())
			rectPrev = rect;

		//g_log->Log("fill 12");
		if(rect.width >0 &&rect.height >0)
		{
			//提取感兴趣区域
			Mat matRoi = src(rect);
			Mat matRoiMask = binMask(rect).clone();
			Mat matRoiSampling =  CAdaptiveThreshold::AdaptiveThreshold(matRoi);

#if _FillBorder_Log
			g_log->Log("fillEdge-fillEdgeCore-start");
#endif				
			//孔洞填充
			fillEdgeCore(matRoi,matRoiMask,matRoiSampling,color,isMapFill,mappingScope,nType);

#if _FillBorder_Log
			g_log->Log("fillEdge-fillEdgeCore-end");
#endif
		}
		//g_log->Log("fill 13");
		//前一步骤的矩形信息
		rectPrev = rect;
	}//for loop

#if _FillBorder_Log
	g_log->Log("fillEdge-end");
#endif
}

//剔除异常的轮廓,将二值图中异常的内嵌凹陷进行剔除
//思路：
//从四个边的方向，向内收缩，遇到的第一个非0点即为边界，内嵌的不考虑
void eliminateAbnormalContour(Mat &src)
{
	uchar *data  = src.ptr<uchar>(0);
	Mat src_copy = src.clone();
	transpose(src_copy, src_copy);
	flip(src_copy, src_copy, 1);

	for(int idr=0;idr < src.rows;idr++)
	{
		int idxStart =-1;
		int idxEnd =-1;
		for (int idc=0;idc <src.cols;idc++,data++)
		{
			if(*data !=0)
			{
				//右侧终止点
				idxEnd = idc;
				//左侧起始点
				if(idxStart == -1)
					idxStart = idc;
			}
		}
		if(idxStart != -1 && idxEnd != -1)
			line(src,Point(idxStart,idr),Point(idxEnd,idr),Scalar(255,255,255),1);
	}

	//2
	uchar *data_copy  = src_copy.ptr<uchar>(0);

	for(int idr=0;idr < src_copy.rows;idr++)
	{
		int idxStart =-1;
		int idxEnd =-1;
		for (int idc=0;idc <src_copy.cols;idc++,data_copy++)
		{
			if(*data_copy !=0)
			{
				//右侧终止点
				idxEnd = idc;
				//左侧起始点
				if(idxStart == -1)
					idxStart = idc;
			}
		}
		if(idxStart != -1 && idxEnd != -1)
			line(src_copy,Point(idxStart,idr),Point(idxEnd,idr),Scalar(255,255,255),1);
	}

	transpose(src_copy, src_copy);
	flip(src_copy, src_copy, 0);
	
	data = src.ptr<uchar>(0);
	data_copy = src_copy.ptr<uchar>(0);
	for(int idr=0;idr<src.rows;idr++)
		for (int idc=0;idc<src.cols;idc++,data_copy++,data++)
			if(*data_copy == 0)
				*data = 0;
}


///////////////////////////补边 V1.0.14////////////////////////////////////////////////////////////////
//修正曲线
bool CFillBorder::correctCurve(vector<Point> &vPoint,int isXsort)
{
	switch(isXsort)
	{
	case 0:
		sort(vPoint.begin(),vPoint.end(),cmpX);
	break;

	case 2:
		sort(vPoint.begin(),vPoint.end(),cmpX1);
	break;

	case 1:
		sort(vPoint.begin(),vPoint.end(),cmpY1);
	break;

	case 3:
		sort(vPoint.begin(),vPoint.end(),cmpY);
	break;

	default:
		return false;
	}

	//新的边界
	vector<Point> vp;
	vector<Point>::iterator itrNext;

	for (vector<Point>::iterator itr = vPoint.begin();itr!=vPoint.end()-1;itr++)
	{
		itrNext = itr+1;
		//上下两边
		if(isXsort == 0 || isXsort ==2)
		{
			if(itrNext->x - itr->x <=1)
				vp.push_back(*itr);
			else
			{
				//y方向的变化
				float difY = (float)(itrNext->y -itr->y)/(float)(itrNext->x - itr->x);
				for(int idx =0;idx<(itrNext->x -itr->x);idx++)
				{
					vp.push_back(Point(itr->x+idx,itr->y+idx*difY));
				}
			}
		}
		else
		{
			if(itrNext->y -itr->y <=1)
				vp.push_back(*itr);
			else
			{
				//y方向的变化
				float difX = (float)(itrNext->x -itr->x)/(float)(itrNext->y - itr->y);
				for(int idx =0;idx<(itrNext->y -itr->y);idx++)
				{
					vp.push_back(Point(itr->x+idx*difX,itr->y+idx));
				}
			}

		}

	}
	//只取单像素点
	int idNum =0;
	switch(isXsort)
	{
	case 0:
		{
			sort(vp.begin(),vp.end(),cmpX);
			idNum =vp[0].x;
			break;
		}


	case 2:
		{
			sort(vp.begin(),vp.end(),cmpX1);
			idNum =vp[0].x;
			break;
		}

	case 1:
		{
			sort(vp.begin(),vp.end(),cmpY1);
			idNum =vp[0].y;
			break;
		}
	case 3:
		{
			sort(vPoint.begin(),vPoint.end(),cmpY);
			idNum =vp[0].y;
			break;
		}
	}

	//返回边界
	vector<Point> reVP;
	reVP.push_back(vp[0]);
	for (vector<Point>::iterator itr = vp.begin()+1;itr!=vp.end()-1;itr++)
	{
		if(isXsort == 0 || isXsort ==2)
		{
			if(itr->x-idNum ==1 )
				reVP.push_back(*itr);
			else
				continue;
			idNum++;
		}
		else
		{
			if(itr->y-idNum ==1 )
				reVP.push_back(*itr);
			else
				continue;
			idNum++;
		}
	}
	vPoint.clear();
	vPoint =reVP;

	return true;
}

//求出最大轮廓
vector<Point> CFillBorder::findMaxContour(Mat dstIn,Mat &dst,RotatedRect &rMaxRect,bool isScanner)
{
	isScanner = false;
	int nThreshold =30;
	if(isScanner)
		nThreshold =10;

	int nThresholdTmp = 108;

	dst = dstIn.clone();
	uchar *data = dst.ptr<uchar>(0);
	if(dst.channels() ==3)
	{
		for (int idr=0;idr<dst.rows;idr++)
		{
			for (int idc=0;idc<dst.cols;idc++,data+=3)
			{ 
				if((
					abs(*data -*(data+1))<nThreshold && 
					abs(*data -*(data+2))<nThreshold &&
					abs(*(data+1) -*(data+2))<nThreshold &&
					*data <nThresholdTmp && *(data+1)<nThresholdTmp && *(data+2)<nThresholdTmp
					)
					||
					(
					abs(*data -*(data+1))<nThreshold*2 && 
					abs(*data -*(data+2))<nThreshold*2 &&
					abs(*(data+1) -*(data+2))<nThreshold*2 &&
					*data <nThresholdTmp/2 && *(data+1)<nThresholdTmp/2 && *(data+2)<nThresholdTmp/2
					)
					||
					(
					abs(*data -*(data+1))<nThreshold*3 && 
					abs(*data -*(data+2))<nThreshold*3 &&
					abs(*(data+1) -*(data+2))<nThreshold*3 &&
					*data <nThresholdTmp/3 && *(data+1)<nThresholdTmp/3 && *(data+2)<nThresholdTmp/3
					)
					)
				{*data =0;*(data+1)=0;*(data+2)=0;}
				else
				{*data =255;*(data+1)=255;*(data+2)=255;}
			}//end for(cols)
		}//end for(rows)
		cvtColor(dst,dst,CV_RGB2GRAY);
	}//end if(彩色图像)
	else
	{
		//dst = CAdaptiveThreshold::AdaptiveThreshold(dst);
	}
	threshold(dst,dst,128,255,CV_THRESH_BINARY);

#if 1
	int nMax = 13;
	Mat element = getStructuringElement(MORPH_RECT,Size(nMax,nMax));
	morphologyEx(dst,dst, MORPH_CLOSE, element);  
#endif

	//测试12.10
	eliminateAbnormalContour(dst);

	//最大轮廓
	vector<Point> areaMatvPoint;
	//红色通道
	Mat dstTemp = dst.clone();
	vector<vector<Point> > vvPoint;
	findContours(dstTemp,vvPoint,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
	dstTemp.release();
	//遍历图像的轮廓
	int rectMaxArea =0;
	int rectArea =0;
	RotatedRect rrectMax;
	//求出最大轮廓
	for (vector<vector<Point> >::iterator itr=vvPoint.begin();itr!=vvPoint.end();itr++)
	{
		RotatedRect rrect =minAreaRect(*itr);
		rectArea =rrect.size.area();
		if( rectArea >rectMaxArea )
		{
			rMaxRect =rrect;
			rectMaxArea = rectArea;
			areaMatvPoint.clear();
			areaMatvPoint = *itr;
			rrectMax = rrect;
		}
	}

	return areaMatvPoint;
}

//凹陷补全
bool CFillBorder::findSingleLine(vector<Point> &singleline,bool isXway)
{
	if(singleline.size() <10)
		return false;

	vector<Point> vline = singleline;
	vector<Point> vBreak;
	
	vector<int> vBreakNum;
	int idxNum =0;

	if(isXway)
	{
		sort(vline.begin(),vline.end(),cmpX);
		vector<Point>::iterator itrNext ;
		for (vector<Point>::iterator itr = vline.begin(); itr != vline.end()-1;itr++)
		{
			itrNext = itr+1;
			idxNum++;
			if(abs(itrNext->y - itr->y) >1)
				if(vBreak.size() ==0)
				{vBreak.push_back(*itr);vBreakNum.push_back(idxNum);idxNum=0;}
				else
				{vBreak.push_back(*itrNext);vBreakNum.push_back(idxNum);idxNum=0;}
		}
		//设置
		vector<int>::iterator itrInt =vBreakNum.begin();
		if(vBreak.size() >2)
		for (vector<Point>::iterator itr = vBreak.begin();itr!=vBreak.end()-1;)
		{
			itrNext = itr+1;
			if(itrNext->x -itr->x ==1)
			{itr = vBreak.erase(itr); itrInt =vBreakNum.erase(itrInt);}
			else
			{itr++;itrInt++;}
		}

		//填补
		itrInt = vBreakNum.begin();
		vector<int>::iterator itrIntNext;
		if(vBreak.size() >1)
			for (vector<Point>::iterator itr =vBreak.begin() ;itr!=vBreak.end()-1;itr++)
			{
				itrNext = itr+1;
				itrIntNext =itrInt +1;

				if(itrNext->x- itr->x >20 && (float)(*itrIntNext -*itrInt) /(float)(itrNext->x-itr->x) >3 )
				{
					float dif = (float)(itrNext->y -itr->y)/float(abs(itrNext->x - itr->x));
					int id =0;
					for (vector<Point>::iterator itrIn = vline.begin();itrIn != vline.end(); itrIn++)
					{
						if(itrIn->x > itr->x && itrIn->x < itrNext->x)
						{
							id++;
							itrIn->y = itr->y + id*dif;
						}
					}
				}
			}
		else
			return true;
	}
	else
	{
		sort(vline.begin(),vline.end(),cmpY);
		vector<Point>::iterator itrNext;
		for (vector<Point>::iterator itr = vline.begin(); itr != vline.end()-1;itr++)
		{
			itrNext = itr+1;
			if(abs(itrNext->x - itr->x) >1)
				if(vBreak.size() ==0)
				{vBreak.push_back(*itr);vBreakNum.push_back(idxNum);idxNum=0;}
				else
				{vBreak.push_back(*itrNext);vBreakNum.push_back(idxNum);idxNum=0;}
		}

		//设置
		vector<int>::iterator itrInt =vBreakNum.begin();
		if(vBreak.size() >2)
			for (vector<Point>::iterator itr = vBreak.begin();itr!=vBreak.end()-1;)
			{
				itrNext = itr+1;
				if(itrNext->y -itr->y ==1)
				{itr = vBreak.erase(itr); itrInt =vBreakNum.erase(itrInt);}
				else
				{itr++;itrInt++;}
			}

			//填补
			itrInt = vBreakNum.begin();
			vector<int>::iterator itrIntNext;
			if(vBreak.size() >1)
				for (vector<Point>::iterator itr =vBreak.begin() ;itr!=vBreak.end()-1;itr++)
				{
					itrNext = itr+1;
					itrIntNext =itrInt +1;

					if(itrNext->y- itr->y >20 && (float)(*itrIntNext -*itrInt) /(float)(itrNext->x-itr->x) >3 )
					{
						float dif = (float)(itrNext->x -itr->x)/float(abs(itrNext->y - itr->y));
						int id =0;
						for (vector<Point>::iterator itrIn = vline.begin();itrIn != vline.end(); itrIn++)
						{
							if(itrIn->y > itr->y && itrIn->y < itrNext->y)
							{
								id++;
								itrIn->x = itr->x + id*dif;
							}
						}
					}
				}
			else
				return true;
	}

	singleline.clear();
	singleline = vline;

	return true;
}

//找出轮廓，去手指版
vector<Point> CFillBorder::findMaxContour(Mat src)
{
	//预处理
	Mat gray;
	if (src.channels() ==1)
		gray =  src.clone();
	else
		cvtColor(src,gray,CV_RGB2GRAY);

	//去噪
	GaussianBlur(gray,gray,Size(5,5),2);

	//二值化
    //IplImage *ipl1= &IplImage(gray);
    IplImage ipl1Tmp = (IplImage)gray;
    IplImage* ipl1= &ipl1Tmp;
	IplImage *ipl2 =cvCreateImage(cvGetSize(ipl1),ipl1->depth,1);
	CAdaptiveThreshold::AdaptiveThreshold(ipl1,ipl2);
	gray =cvarrToMat(ipl2,true);
	cvReleaseImage(&ipl2);

	//求出轮廓
	Mat dst = gray.clone();
	vector<vector<Point> > vvPoint;
	findContours(dst,vvPoint,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
	//遍历图像的轮廓
	int rectMaxArea =0;
	int rectArea =0;
	//最大轮廓
	vector<Point> areaMatvPoint;
	//求出最大轮廓
	for (vector<vector<Point> >::iterator itr=vvPoint.begin();itr!=vvPoint.end();itr++)
	{
		RotatedRect rrect =minAreaRect(*itr);
		rectArea =rrect.size.area();
		if( rectArea >rectMaxArea )
		{
			rectMaxArea = rectArea;
			areaMatvPoint.clear();
			areaMatvPoint = *itr;
		}
	}

	return areaMatvPoint;
}

//去手指
bool CFillBorder::FillFinger(Mat src,Mat &dst,int offsetNum,int stretchNum)
{
	if(src.channels() != 3)
		return false;
	//求出图像的轮廓
	vector<Point> areaPoint = CFillBorder::findMaxContour(src);
	dst =src.clone();
	//求出异常轮廓区域
	vector<vector<Point> >  vvPoint;
	vector<Point> vPoint;
	bool isNear =false;
	for (vector<Point>::iterator itr = areaPoint.begin();itr!=areaPoint.end();itr++)
	{
		if(itr->x <10 || itr->x >src.cols -10 || itr->y <10 || itr->y >src.rows -10 )
			isNear =true;
		else
			isNear =false; 
		
		if(!isNear)
			vPoint.push_back(*itr);
		else
		{
			if(vPoint.size() >20)
			{
				vvPoint.push_back(vPoint);
				vPoint.clear();
			}
			else
				vPoint.clear();
		}
	}
	//通过肤色检测判断是否为手指异常轮廓，并填充轮廓
	dst = src.clone();
	vector<bool> vIsFinger;
	for (vector<vector<Point> >::iterator itr =vvPoint.begin();itr!=vvPoint.end();itr++)
	{
		vPoint.clear();
		vPoint = *itr;
		int whichSide=0;
		//检测肤色
		if(CFillBorder::detectSkin(src,vPoint,whichSide))
		{
			//根据类型填充手指区域
			CFillBorder::FillFingerContour(dst,vPoint,whichSide,offsetNum,stretchNum);
		}
	
	}

	return true;
}

//检测手指
bool CFillBorder::detectSkin(Mat src,vector<Point> vp,int &whichSide)
{
	return  true;
	int maxX,maxY,minX,minY;
	maxX =minX =vp[0].x;
	maxY =minY =vp[0].y;
	for (vector<Point>::iterator itr =vp.begin();itr!=vp.end();itr++)
	{
		if(itr->x >maxX)
			maxX =itr->x;
		
		if(itr->x <minX)
			minX =itr->x;

		if(itr->y >maxY)
			maxY =itr->y;

		if(itr->y <minY)
			minY =itr->y;
	}
 	float fR1 = 1- (float)maxX /(float)src.cols;
	float fR2 = (float)minX /(float)src.cols;
	float fR3 = 1- (float)maxY /(float)src.rows;
	float fR4 = (float)minY /(float)src.rows;
	
	if(fR1 <= fR2 && fR1<=fR3 &&fR1<=fR4)
		whichSide =2;
	if(fR2 <= fR1 && fR2<=fR3 &&fR2<=fR4)
		whichSide =0;
	if(fR3 <= fR1 && fR3<=fR2 &&fR3<=fR4)
		whichSide =1;
	if(fR4 <= fR1 && fR4<=fR3 &&fR4<=fR2)
		whichSide =3;

	float skinNum=0;
	float regionNum =(float)(maxX-minX)*(maxY-minY);
	for (int idr =minY;idr <maxY;idr++)
	{
		for(int idc =minX;idc<maxX;idc++)
		{
			int dataB =src.ptr<uchar>(idr)[idc*src.channels()];
			int dataG =src.ptr<uchar>(idr)[idc*src.channels()+1];
			int dataR =src.ptr<uchar>(idr)[idc*src.channels()+2];

			if(dataR > 35 && dataG > 20 && dataB > 15 && dataR > dataB && dataR > dataG && abs(dataR - dataG) > 10)
			{
				skinNum++;
			}
		}
	}
	//计算区域的肤色比
	if((skinNum / regionNum >0.2)||(skinNum>3000 &&skinNum/regionNum > 0.03))
		return true;
	else
		return false;
}

//填充手指区域
bool CFillBorder::FillFingerContour(Mat &src,vector<Point> vp,int whichSide,int offsetNum,int stretchNum)
{
	vector<Point> vline =vp;
	vector<Point> vSingle;
	vector<Point>::iterator itrNext;
	vector<int> vr,vg,vb;
	int offsetColorNum =2;

	switch(whichSide)
	{
	//右边
	case 2:
		{
			sort(vline.begin(),vline.end(),cmpY1);
			for (vector<Point>::iterator itr =vline.begin();itr!=vline.end();itr++)
			{
				itrNext = itr+1;
				if(itr->y ==itrNext->y)
					continue;
				else
					vSingle.push_back(*itr);
			}

			//延伸轮廓
			CFillBorder::changeContour(src,vSingle,offsetNum,stretchNum);
			
			//找背景色
			for (vector<Point>::iterator itr =vSingle.begin();itr!=vSingle.end();itr++)
			{
				for(int id=1;id <=10;id++)
				{
					vr.push_back(src.ptr<uchar>(itr->y)[(itr->x-offsetColorNum-id)*src.channels()+2]);
					vg.push_back(src.ptr<uchar>(itr->y)[(itr->x-offsetColorNum-id)*src.channels()+1]);
					vb.push_back(src.ptr<uchar>(itr->y)[(itr->x-offsetColorNum-id)*src.channels()]);
				}
			}
			Scalar color;
			//CFillBorder::findMainColor(vr,vg,vb,color);
			CFillBorder::meanBackGroundColor(vr,vg,vb,color);
			//背景色填充
			for (vector<Point>::iterator itr =vSingle.begin();itr!=vSingle.end();itr++)
			{
				for(int idx= itr->x-offsetNum;idx<src.cols;idx++)
				{
					uchar *dataR =&src.ptr<uchar>(itr->y)[idx*src.channels()+2];
					*dataR = color[2];

					uchar *dataG =&src.ptr<uchar>(itr->y)[idx*src.channels()+1];
					*dataG = color[1];

					uchar *dataB =&src.ptr<uchar>(itr->y)[idx*src.channels()];
					*dataB = color[0];
				}
			}	
		}
	break;

	//上边
	case 3:
	{
		sort(vline.begin(),vline.end(),cmpX);
		for (vector<Point>::iterator itr =vline.begin();itr!=vline.end();itr++)
		{
			itrNext = itr+1;
			if(itr->x ==itrNext->x)
				continue;
			else
				vSingle.push_back(*itr);
		}
		CFillBorder::changeContour(src,vSingle,offsetNum,stretchNum,true);
		
		for (vector<Point>::iterator itr =vSingle.begin();itr!=vSingle.end();itr++)
		{
			int idy =itr->x +10;
			idy = idy < src.rows?idy :src.rows;
			for(int id=1;id <=10;id++)
			{
				vr.push_back(src.ptr<uchar>(itr->y+id+offsetColorNum)[itr->x*src.channels() +2]);
				vg.push_back(src.ptr<uchar>(itr->y+id+offsetColorNum)[itr->x*src.channels() +1]);
				vb.push_back(src.ptr<uchar>(itr->y+id+offsetColorNum)[itr->x*src.channels()]);
			}
		}
		Scalar color;
		//CFillBorder::findMainColor(vr,vg,vb,color);
		CFillBorder::meanBackGroundColor(vr,vg,vb,color);
		for (vector<Point>::iterator itr =vSingle.begin();itr!=vSingle.end();itr++)
		{
			for(int idy=itr->y+10;idy>=0;idy--)
			{
				uchar *dataR =&src.ptr<uchar>(idy)[itr->x*src.channels()+2];
				*dataR = color[2]*0.9;

				uchar *dataG =&src.ptr<uchar>(idy)[itr->x*src.channels()+1];
				*dataG = color[1];

				uchar *dataB =&src.ptr<uchar>(idy)[itr->x*src.channels()];
				*dataB = color[0];
			}
		}	

	}//case 3
	break;

	//下边
	case 1:
		{
			sort(vline.begin(),vline.end(),cmpX1);
			for (vector<Point>::iterator itr =vline.begin();itr!=vline.end();itr++)
			{
				itrNext = itr+1;
				if(itr->x ==itrNext->x)
					continue;
				else
					vSingle.push_back(*itr);
			}
			CFillBorder::changeContour(src,vSingle,offsetNum,stretchNum,true);
			for (vector<Point>::iterator itr =vSingle.begin();itr!=vSingle.end();itr++)
			{
				for(int id=1;id <=10;id++)
				{
					vr.push_back(src.ptr<uchar>(itr->y-id-offsetColorNum)[itr->x*src.channels() +2]);
					vg.push_back(src.ptr<uchar>(itr->y-id-offsetColorNum)[itr->x*src.channels() +1]);
					vb.push_back(src.ptr<uchar>(itr->y-id-offsetColorNum)[itr->x*src.channels()]);
				}
			}
			Scalar color;
			//CFillBorder::findMainColor(vr,vg,vb,color);
			CFillBorder::meanBackGroundColor(vr,vg,vb,color);
			for (vector<Point>::iterator itr =vSingle.begin();itr!=vSingle.end();itr++)
			{
				for(int idy=itr->y-10;idy<src.rows;idy++)
				{
					uchar *dataR =&src.ptr<uchar>(idy)[itr->x*src.channels()+2];
					*dataR = color[2]*0.9;

					uchar *dataG =&src.ptr<uchar>(idy)[itr->x*src.channels()+1];
					*dataG = color[1];

					uchar *dataB =&src.ptr<uchar>(idy)[itr->x*src.channels()];
					*dataB = color[0];
				}
			}	

		}// case 4
		break;

	//左边
	case 0:
		{
			sort(vline.begin(),vline.end(),cmpY);
			for (vector<Point>::iterator itr =vline.begin();itr!=vline.end();itr++)
			{
				itrNext = itr+1;
				if(itr->y ==itrNext->y)
					continue;
				else
					vSingle.push_back(*itr);
			}

			CFillBorder::changeContour(src,vSingle,offsetNum,stretchNum);

			for (vector<Point>::iterator itr =vSingle.begin();itr!=vSingle.end();itr++)
			{
				int idx =itr->x +10;
				idx = idx < src.cols?idx :src.cols;
				for(int id=1;id <=10;id++)
				{
					vr.push_back(src.ptr<uchar>(itr->y)[(itr->x+id+offsetColorNum)*src.channels()+2]);
					vg.push_back(src.ptr<uchar>(itr->y)[(itr->x+id+offsetColorNum)*src.channels()+1]);
					vb.push_back(src.ptr<uchar>(itr->y)[(itr->x+id+offsetColorNum)*src.channels()]);
				}
			}
			Scalar color;
			//CFillBorder::findMainColor(vr,vg,vb,color);
			CFillBorder::meanBackGroundColor(vr,vg,vb,color);
			for (vector<Point>::iterator itr =vSingle.begin();itr!=vSingle.end();itr++)
			{
				for(int idx=itr->x+10;idx>=0;idx--)
				{
					uchar *dataR =&src.ptr<uchar>(itr->y)[idx*src.channels()+2];
					*dataR = color[2]*0.9;

					uchar *dataG =&src.ptr<uchar>(itr->y)[idx*src.channels()+1];
					*dataG = color[1];

					uchar *dataB =&src.ptr<uchar>(itr->y)[idx*src.channels()];
					*dataB = color[0];
				}
			}	
		}// case 0
		break;

	}//switch
	return true;
}

//延伸轮廓
bool CFillBorder::changeContour(Mat src,vector<Point> &vSingle,int offsetNum,int stretchNum,bool isXway)
{
	//拉伸起止位置
	int iSt,iEnd;
	Point ptSt,ptEnd;
	ptSt = vSingle[0];ptEnd= vSingle[vSingle.size()-1];
	if(!isXway)
	{
		iSt = vSingle[0].y -stretchNum <0 ? vSingle[0].y:stretchNum;
		iEnd = vSingle[vSingle.size()-1].y+stretchNum >= src.rows ?src.rows -ptEnd.y-1:stretchNum;
		//起始位置拉伸
		for(int idx =1;idx<=iSt;idx++)
			vSingle.insert(vSingle.begin(),Point(ptSt.x,ptSt.y-idx));
		//结束位置拉伸
		for(int idx =1;idx<iEnd;idx++)
			vSingle.push_back(Point(ptEnd.x,ptEnd.y+idx));
	}
	else
	{		//拉伸起止位置
		iSt = vSingle[0].x -stretchNum <0 ? vSingle[0].x:stretchNum;
		iEnd = vSingle[vSingle.size()-1].x+stretchNum >= src.cols ?src.cols -ptEnd.x-1:stretchNum;
		//起始位置拉伸
		for(int idx =1;idx<=iSt;idx++)
			vSingle.insert(vSingle.begin(),Point(ptSt.x-idx,ptSt.y));
		//结束位置拉伸
		for(int idx =1;idx<iEnd;idx++)
			vSingle.push_back(Point(ptEnd.x+idx,ptEnd.y));
	}

	return true;
}

//平均背景色
bool CFillBorder::meanBackGroundColor(vector<int> vr,vector<int> vg,vector<int> vb,Scalar &color)
{
	int num = accumulate(vr.begin(),vr.end(),0);
	
	float meanR =(float)num/float(vr.size());
	color[2] =meanR;
	num =accumulate(vg.begin(),vg.end(),0);
	meanR =(float)num/float(vg.size());
	color[1] =meanR;
	num =accumulate(vb.begin(),vb.end(),0);
	meanR =(float)num/float(vb.size());
	color[0] =meanR;

	return true;
}

//链接断点
bool CFillBorder::megerline(vector<Point> &vPoint)
{
	return true;
	vector<vector<Point> > vvPoint;
	vector<Point> vPtmp;
	vector<Point> vLastPt;
	vector<Point>::iterator itrNext;

	for (vector<Point>::iterator itr = vPoint.begin();itr!=vPoint.end()-1;itr++)
	{
		itrNext = itr+1;

		if(abs(itrNext->y -itr->y)>1)
		{
			if(vPtmp.size() >5)
				vvPoint.push_back(vPtmp);
			vPtmp.clear();
		}
			else
			vPtmp.push_back(*itr);
	}
	vLastPt =vPtmp;
	vPoint.clear();
	vPtmp.clear();
	//链接点
	for (vector<vector<Point> >::iterator itr =vvPoint.begin();itr!=vvPoint.end();itr++)
	{
		vPtmp =*itr;
		if(vPoint.size()==0)
			vPoint.insert(vPoint.begin(),vPtmp.begin(),vPtmp.end());
		else
		{
			Point ptEnd = vPoint[vPoint.size()-1];
			Point ptSt = vPtmp[0];

			if(ptSt.x - ptEnd.x >1 )
			{
				for(int idx =1;idx <ptSt.x -ptEnd.x;idx++)
				{
					vPoint.push_back(Point(ptEnd.x+idx,ptEnd.y));
				}
			}
			vPoint.insert(vPoint.end(),vPtmp.begin(),vPtmp.end());
		}
	}
	//链接最后一组点
	Point ptEnd =vPoint[vPoint.size()-1];
	Point ptSt = vLastPt[0];
	if(ptSt.x - ptEnd.x >1 )
	{
		for(int idx =1;idx <ptSt.x -ptEnd.x;idx++)
		{
			vPoint.push_back(Point(ptEnd.x+idx,ptSt.y));
		}
	}
	vPoint.insert(vPoint.end(),vLastPt.begin(),vLastPt.end());

	return true;
}

//顶角缺陷的填补
void CFillBorder::fourLineRepair(vector<Point> &vpXlow,vector<Point> &vpXup,vector<Point> &vpYlow,vector<Point> &vpYup,int imgCols)
{
	int nMeanValue =0;
	int nMeanValueY =0;
	for(vector<Point>::iterator itr = vpXlow.begin(); itr != vpXlow.end();itr++)
		nMeanValue += (*itr).x;
	for(vector<Point>::iterator itr =vpXup.begin();itr != vpXup.end();itr++)
		nMeanValueY +=(*itr).x;

	nMeanValue /=vpXlow.size();
	nMeanValueY /=vpXup.size();
	//四个顶点多余
	vector<Point> vpTmp,vpTmpDown;
	vector<Point>::iterator itrLoc,itrLocDown;
	for(vector<Point>::iterator itr =vpXlow.begin();itr != vpXlow.end();itr++)
	{
		if(itr->x > nMeanValue)
		{
			vpTmp.push_back(*itr);
			itrLoc = itr;
		}
		else
			break;
	}

	if(vpTmp.size() !=0)
	{
		vpXlow.erase(vpXlow.begin(),itrLoc);

		Point pt = vpTmp[vpTmp.size()-1];

		if(pt.x >1)
			for(int idx=1;idx<pt.x;idx++)
				vpTmp.push_back(Point(pt.x-idx,pt.y));

		for(vector<Point>::iterator itr = vpTmp.begin(); itr != vpTmp.end(); itr++)
			vpYlow.push_back(*itr);
	}

	for(vector<Point>::iterator itr = vpXlow.end()-1;itr != vpXlow.begin()-1;itr--)
	{
		if(itr->x > nMeanValue)
		{
			vpTmpDown.push_back(*itr);
			itrLocDown = itr;
		}
		else
			break;
	}

	if(vpTmpDown.size() != 0)
	{
		vpXlow.erase(itrLocDown,vpXlow.end()-1);

		Point pt = vpTmpDown[vpTmpDown.size()-1];

		if(pt.x >1)
			for(int idx=1;idx<pt.x;idx++)
				vpTmpDown.push_back(Point(pt.x-idx,pt.y));

		for(vector<Point>::iterator itr = vpTmpDown.begin(); itr != vpTmpDown.end(); itr++)
			vpYup.push_back(*itr);
	}

	////////////////////////////////////////////////////////////////////////
	//四个顶点多余
	vpTmp.clear();
	vpTmpDown.clear();
	for(vector<Point>::iterator itr =vpXup.begin();itr != vpXup.end();itr++)
	{
		if(itr->x < nMeanValueY)
		{
			vpTmp.push_back(*itr);
			itrLoc = itr;
		}
		else
			break;
	}

	if(vpTmp.size() !=0)
	{
		vpXup.erase(vpXup.begin(),itrLoc);

		Point pt = vpTmp[vpTmp.size()-1];

		if(pt.x >1)
			for(int idx=1;idx<imgCols-nMeanValueY-1;idx++)
				vpTmp.push_back(Point(pt.x+idx,pt.y));

		for(vector<Point>::iterator itr = vpTmp.begin(); itr != vpTmp.end(); itr++)
			vpYlow.push_back(*itr);
	}

	for(vector<Point>::iterator itr = vpXup.end()-1;itr != vpXup.begin()-1;itr--)
	{
		if(itr->x < nMeanValueY)
		{
			vpTmpDown.push_back(*itr);
			itrLocDown = itr;
		}
		else
			break;
	}

	if(vpTmpDown.size() != 0)
	{
		vpXup.erase(itrLocDown,vpXup.end()-1);

		Point pt = vpTmpDown[vpTmpDown.size()-1];

		if(pt.x >1)
			for(int idx=1;idx<imgCols-nMeanValueY-1;idx++)
				vpTmpDown.push_back(Point(pt.x+idx,pt.y));

		for(vector<Point>::iterator itr = vpTmpDown.begin(); itr != vpTmpDown.end(); itr++)
			vpYup.push_back(*itr);
	}

	return ;
}

//mcvFileBoder主体
//最近维护：Max 20190319
bool CFillBorder::FillBorder3(const Mat &srcIn, Mat &dstR,float fRatio,bool isScanner,Scalar color,bool isMapFill,int mappingScope)
{
#if _FillBorder_Log
	g_log->Log("FillBorder3-start");
#endif
	int increaseBorNum =10;

	Mat src;
	if(1 == srcIn.channels())
	{
		vector<Mat> vMat;
		vMat.push_back(srcIn);
		vMat.push_back(srcIn);
		vMat.push_back(srcIn);
		merge(vMat,src);
	}
	else
		src = srcIn.clone();

	//对图像补位
	Mat dst = Mat::zeros(src.rows+2*increaseBorNum,src.cols+2*increaseBorNum,src.type());
	Mat dstROI = dst(cv::Rect(increaseBorNum,increaseBorNum,src.cols,src.rows));		
	src.copyTo(dstROI);
	
	//图像轮廓
	RotatedRect rMaxRect;//轮廓最小外接矩形	
	Mat binMask;

#if _FillBorder_Log
	g_log->Log("FillBorder3-findMaxContour-start");
#endif
	
	vector<Point> areaMaxPoint = CFillBorder::findMaxContour(dst,binMask,rMaxRect,isScanner);

#if _FillBorder_Log
	g_log->Log("FillBorder3-findMaxContour-end");
#endif

	//返回异常
	if(0 == areaMaxPoint.size())
		return false;

	Rect rect = boundingRect(areaMaxPoint);

	int difNum = 80;
	//计算局部长度
	int nStep = areaMaxPoint.size()/difNum;

	//轮廓分段
	vector<vector<Point> > vlineAll;
	vector<Point> vline;
	int idx =0;
	
	vector<vector<int> > vMaxIdx;
	vector<int> vInt;
	int nMaxX,nMinX,nMaxY,nMinY;

	for(vector<Point>::iterator itr = areaMaxPoint.begin(); itr != areaMaxPoint.end(); itr++,idx++)
	{
		if(vline.size() ==0)
		{
			vline.push_back(*itr);
			nMaxX = vline[0].x;
			nMinX = vline[0].x;

			nMaxY = vline[0].y;
			nMinY = vline[0].y;
		}
		else
		{
			vline.push_back(*itr);
			if(itr->x>nMaxX)
				nMaxX = itr->x;

			if(itr->x <nMinX)
				nMinX = itr->x;

			if(itr->y >nMaxY)
				nMaxY = itr->y;

			if(itr->y <nMinY)
				nMinY = itr->y;
		}

		//分段
		if(idx% nStep ==0 && idx !=0)
		{
			vlineAll.push_back(vline);
			vline.clear();

			vInt.push_back(nMinX);
			vInt.push_back(nMaxX);
			vInt.push_back(nMinY);
			vInt.push_back(nMaxY);
			vMaxIdx.push_back(vInt);
			vInt.clear();
		}

		//末段
		if(itr == areaMaxPoint.end()-1 && vline.size() != 0)
		{
			vlineAll.push_back(vline);
			vline.clear();

			vInt.push_back(nMinX);
			vInt.push_back(nMaxX);
			vInt.push_back(nMinY);
			vInt.push_back(nMaxY);
			vMaxIdx.push_back(vInt);
			vInt.clear();
		}
	}
	//生成二值图模板
	int nLineW = fRatio < 0 ? 0: (int)fRatio;
	
	//如果轮廓贴近边缘，则不进行处理
	for(int idx=0;idx <areaMaxPoint.size()-1;idx++)
	//	if(areaMaxPoint[idx].x > 9 && 
	//		areaMaxPoint[idx].y > 9 && 
	//		areaMaxPoint[idx].x <src.cols -10&&
	//		areaMaxPoint[idx].y <src.rows -10
	//		)
		circle(binMask,areaMaxPoint[idx],nLineW,Scalar(0,0,0),-1);

#if _FillBorder_Log
	g_log->Log("FillBorder3-fillEdge-start");
#endif

	//边缘填补
	fillEdge(dst,binMask,vlineAll,nStep,vMaxIdx,4*(nStep/difNum)+1,color,rect.x,rect.y,isMapFill,mappingScope);	

#if _FillBorder_Log
	g_log->Log("FillBorder3-fillEdge-end");
#endif
	

	//原先图
	Mat oriMat = dst(Rect(increaseBorNum,increaseBorNum,src.cols,src.rows));

	if(1 == srcIn.channels())
		cvtColor(oriMat,oriMat,CV_BGR2GRAY);
	oriMat.copyTo(dstR);

#if _FillBorder_Log
	g_log->Log("FillBorder3-end");
#endif
	return true;
}
