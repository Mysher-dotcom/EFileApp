#include "stdafx.h"
#include "PunchHold.h"
#include "../ColorSpace/AdaptiveThreshold.h"
//V0.0.3  Time:2017年8月9日

//去除装订孔
//[in]src				输入图像
//[out]dst				输出图像
//[in]isCircularHole	是否为圆形装订孔
//[in]isColorFill		是否纯色填充(true:纯色,false:背景色)
//[in]nR				红色分量：颜色分量仅在纯色填充下有效
//[in]nG				绿色分量
//[in]nB				蓝色分量
bool CPunchHold::punchHold(Mat srcTmp,Mat &dst,bool isCircularHole,bool isColorFill,int nR,int nG,int nB)
{
	Mat src =srcTmp.clone();
	int minSideNum = src.cols;
	float ratioN = 1000.0 / (float)minSideNum;
	//缩放
	float increaseN= (float)minSideNum /1000.0;
	if(ratioN >1)
	{
		ratioN =1;
		increaseN =1;
	}
	resize(src,src,Size(0,0),ratioN,ratioN);
	//对缩放图进行轮廓求解
	vector<vector<Point> > vvPoint;
	vvPoint = CPunchHold::calcuateContours(src,isCircularHole);
	vector<cirCtours> vCircle;
	//筛选轮廓
	vCircle =CPunchHold::filtrateContours(vvPoint,isCircularHole,src.cols,src.rows);
	if(vCircle.size() <1)
		return false;
	//轮廓归类
	CPunchHold::filtrateContoursByClassify(vCircle);
	//轮廓分类
	CPunchHold::newSplit(src,vCircle);

	//填充孔径
	sipltAndRepair(dst,vCircle,increaseN,isCircularHole,isColorFill);

	return true;
}

/*******************************以下为私有量**********************************/
//排序
bool cmpCircle(cirCtours cc1,cirCtours cc2)
{
	if( cc1.circleArea > cc2.circleArea)
		return true;
	else
		return false;
}

//四个方向排序
bool cmpCircleX(cirCtours cc1,cirCtours cc2)
{
	if( cc1.pt2f.x > cc2.pt2f.x)
		return true;
	else
		return false;
}

//排序
bool cmpCircleY(cirCtours cc1,cirCtours cc2)
{
	if( cc1.pt2f.y > cc2.pt2f.y)
		return true;
	else
		return false;
}

bool cmpI(int i1,int i2)
{
	return i1 < i2;
}

//计算轮廓
//[in]src				输入图像
//[in]isCircularHole	是否为圆形装订孔:(true:圆形,false:方形)
vector<vector<Point> > CPunchHold::calcuateContours(Mat src,bool isCircularHole)
{
	double valuTable[256]={0};
	//预处理
	Mat gray(src.rows,src.cols,CV_8UC1);
	if (src.channels() ==1)
		gray =  src.clone();
	else
	{
		//cvtColor(src,gray,CV_RGB2GRAY);
		uchar *data = gray.ptr<uchar>(0);
		uchar *dataOri =src.ptr<uchar>(0);
		for(int idx=0;idx<src.rows*src.cols;idx++,data++,dataOri+=3)
		{
			valuTable[*data =max(max(*(dataOri),*(dataOri+1)),*(dataOri+2))]++;
		}
	}
	//二值化
    //IplImage *ipl1= &IplImage(gray);
    IplImage ipl1Tmp = IplImage(gray);
    IplImage *ipl1= &ipl1Tmp;
	IplImage *ipl2 =cvCreateImage(cvGetSize(ipl1),ipl1->depth,1);
	
	int nThreshold = CAdaptiveThreshold::OptimalThreshold(ipl1,1);
	nThreshold = nThreshold >100?100:nThreshold;
	cvThreshold(ipl1,ipl2,nThreshold,255,THRESH_BINARY);
	
	IplConvKernel* kernel;

	kernel =cvCreateStructuringElementEx(7,7,5,5,CV_SHAPE_ELLIPSE);
	cvXorS(ipl2,cvScalarAll(255),ipl2);
	//图像形态学处理
	cvErode(ipl2,ipl2,kernel);
	cvDilate(ipl2,ipl2,kernel);
	cvReleaseStructuringElement(&kernel);
	//转换类型	
	gray = cvarrToMat(ipl2,true);
	cvReleaseImage(&ipl2);

	//求出轮廓
	Mat dst = gray.clone();
	vector<vector<Point> > vvPoint;

	//imwrite("C:\\Users\\Xue.Bicycle\\Desktop\\tt.bmp",dst);
	findContours(dst,vvPoint,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);

	return vvPoint;
}

//删除不符合轮廓
//[in/out]vvPoint		输入/输出轮廓
//[in]isCircularHole	是否为圆形装订孔:(true:圆形,false:方形)
//[in]iwidth			图像宽
//[in]iheight			图像高
//返回值
//vector<circleCtours>	装订孔信息
vector<cirCtours> CPunchHold::filtrateContours(vector<vector<Point> > &vvPoint,bool isCircularHole ,
	int iwidth,int iheight)
{
	vector<vector<Point> > vvPointCicle = vvPoint;
	//根据最小外接圆同轮廓的面积比判断是否为圆孔
	vector<cirCtours> vCContours;
	vector<cirCtours> vCContours_candidate;
	for (vector<vector<Point> >::iterator itr =vvPointCicle.begin();itr!=vvPointCicle.end();itr++)
	{
		vector<Point> vp =*itr;
		//剔除异常
		if(vp.size() <10)
			continue;
		Point2f pt2;
		float rf;
		minEnclosingCircle(vp,pt2,rf);//轮廓最小外接圆
		RotatedRect rrect = minAreaRect(vp);//轮廓最小外接矩形

		//外接圆面积
		float circleArea = 3.14 *rf*rf;
		//矩形面积
		float rectArea =rrect.size.area();
		//轮廓面积
		float contourAreaNum =fabs(contourArea(vp,false));
		//剔除异常
		if(contourAreaNum <10)
			continue;
		//装订孔分类
		float ratioR,ratioR1;
		cirCtours cctours;
		if(!isCircularHole)
		{
			ratioR= rectArea /contourAreaNum;
			ratioR1 =circleArea /contourAreaNum;
			cctours.pt2f =rrect.center;
			cctours.circleR =rrect.size.width;
			cctours.circleR1 =rrect.size.height;
			cctours.circleArea =rrect.size.area();
			cctours.vpoint =vp;
		}
		else
		{
			ratioR = circleArea /contourAreaNum;
			ratioR1 =rectArea /contourAreaNum;
			cctours.pt2f =pt2;//圆中心
			cctours.circleR =rf;//圆半径
			cctours.circleR1 =rf;
			cctours.circleArea = contourAreaNum;
			cctours.vpoint =vp;
		}

		//判断是否为装订空
		if((ratioR >=1 && ratioR <=1.3 )&& 
			(vp[0].x <iwidth/7 ||vp[0].x >iwidth *6/7 ||vp[0].y <iheight/7 ||vp[0].y >iheight *6/7 )&&
			vp.size() >5  )
		{vCContours.push_back(cctours);}
		else if( ratioR >=1&& ratioR <2 )
			vCContours_candidate.push_back(cctours);
	}
	if(vCContours.size() <1)
		return vCContours;

	//排序
	sort(vCContours.begin(),vCContours.end(),cmpCircle);
	//根据尺寸筛选
	if((float)vCContours[0].vpoint.size()/(float)vCContours[vCContours.size()-1].vpoint.size()>2)
	{
		int pointNum = vCContours[vCContours.size()/2].circleArea;
		//去头
		for(vector<cirCtours>::iterator itr =vCContours.begin();itr != vCContours.begin()+vCContours.size()/2;itr++)
		{
			if((float)itr->circleArea/(float)pointNum >2 )
				vCContours.erase(itr);
			else
				break;
		}
		//摘尾
		for (vector<cirCtours>::iterator itr =vCContours.end()-1;itr != vCContours.begin()+vCContours.size()/2;itr--)
		{
			if((float)pointNum/(float)itr->circleArea>2)
				vCContours.erase(itr);
			else
				break;
		}
	}

	if(vCContours_candidate.size() <1)
		return vCContours;

	//筛选装订孔的位置
	vector<cirCtours> cCirTm = vCContours;
	sort(cCirTm.begin(),cCirTm.end(),cmpCircleX);
	int minX =cCirTm[0].pt2f.x;
	int maxX =cCirTm[cCirTm.size()-1].pt2f.x;
	sort(cCirTm.begin(),cCirTm.end(),cmpCircleY);
	int minY =cCirTm[0].pt2f.y;
	int maxY =cCirTm[cCirTm.size()-1].pt2f.y;

	sort(vCContours_candidate.begin(),vCContours_candidate.end(),cmpCircle);

	vector<cirCtours> vcir;
	for (vector<cirCtours>::iterator itr = vCContours_candidate.begin();itr!=vCContours_candidate.end();itr++)
	{
		cirCtours cctous =*itr;
		bool Con1 = cctous.circleArea > vCContours[vCContours.size()-1].circleArea*0.5;
		bool Con2 = cctous.circleArea <vCContours[0].circleArea*2;
		if((cctous.circleArea > vCContours[vCContours.size()-1].circleArea*0.5 && cctous.circleArea <vCContours[0].circleArea*2)&&
			(abs(cctous.pt2f.x-minX)<vCContours[0].circleR ||
			abs(cctous.pt2f.x-maxX)<vCContours[0].circleR  ||
			abs(cctous.pt2f.y-maxY)<vCContours[0].circleR  ||
			abs(cctous.pt2f.y-minY)<vCContours[0].circleR 
			)
			)
		{
			vcir.push_back(*itr);
		}
	}
	vCContours.insert(vCContours.end(),vcir.begin(),vcir.end());
	return vCContours;
}

//分割图
//[in/out]				输入/输出图像
//[in]vCircle			装订空信息
//[in]increaseN			缩放比例
//[in]isCircularHole	是否为圆形装订孔:(true:圆形,false:方形)
bool CPunchHold::sipltAndRepair(Mat &src,vector<cirCtours> vCircle,float increaseN,bool isCircularHole,bool isColorFill)
{
	int increaseBorderNum=min(src.cols,src.rows) *0.023;

	for(vector<cirCtours>::iterator itr =vCircle.begin(); itr !=vCircle.end();itr++)
	{
		cirCtours ccTous = *itr;
		float fratio =0.5;
		int upLeft_pointX = ccTous.pt2f.x*increaseN-ccTous.circleR*increaseN*fratio -increaseBorderNum<0?0:ccTous.pt2f.x*increaseN-ccTous.circleR*increaseN*fratio -increaseBorderNum;
		int upLeft_pointY = ccTous.pt2f.y*increaseN-ccTous.circleR1*increaseN*fratio-increaseBorderNum<0?0:ccTous.pt2f.y*increaseN-ccTous.circleR1*increaseN*fratio-increaseBorderNum;
		int sizeWidth = max(ccTous.circleR,ccTous.circleR1)*increaseN +2*increaseBorderNum ;
	
		//越界问题
		if(upLeft_pointX + sizeWidth >src.cols)
			sizeWidth = src.cols -upLeft_pointX;
		if(upLeft_pointY +sizeWidth >src.rows)
			sizeWidth = src.rows -upLeft_pointY;
		//裁切装订空位置
		Mat dst = src(Rect(upLeft_pointX,upLeft_pointY,sizeWidth,sizeWidth));
		Mat dstResize;
		if(!isColorFill)
			resize(dst,dstResize,Size(20,20));
			//dstResize =dst;
		else
			dstResize = dst;
		Mat dstMask =dstResize.clone();
		
		//制作模板
		if(dstMask.channels() ==3)
			cvtColor(dstMask,dstMask,CV_RGB2GRAY);

		int originalH = dstMask.rows;
		resize(dstMask,dstMask,Size(100,100));
		adaptiveThreshold(dstMask,dstMask,255,CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY,85,0);
		dstMask =~dstMask;

		//纯色填补
  		if(isColorFill)
		{
			Mat dstMaskColor;
			//图像形态学
			int nSize = 5;//2*(int)(dstMask.rows/20)+1;
			Mat kernel ;
			if(isCircularHole)
				kernel= getStructuringElement(MORPH_ELLIPSE,Size(nSize,nSize));
			else
				kernel=getStructuringElement(MORPH_RECT,Size(nSize,nSize));
			
			erode(dstMask,dstMask,kernel,Point(-1,-1),3);
			dilate(dstMask,dstMask,kernel,Point(-1,-1),4);
			resize(dstMask,dstMask,Size(originalH,originalH));

			vector<Mat> vMat;
			for (int idx=0;idx<dst.channels();idx++)
				vMat.push_back(dstMask);
			merge(vMat,dstMaskColor);
			addWeighted(dst,1,dstMaskColor,1,0,dst,dst.type());

			continue;
		}

		//图像修复
		Mat kernel ;
		if(isCircularHole)
			kernel= getStructuringElement(MORPH_ELLIPSE,Size(5,5));
		else
			kernel =getStructuringElement(MORPH_RECT,Size(5,5));

		//namedWindow("1",0);
		//imshow("1",dstMask);
		//waitKey();

		dilate(dstMask,dstMask,kernel,Point(-1,-1),2);

		//namedWindow("2",0);
		//imshow("2",dstMask);
		//waitKey();

		resize(dstMask,dstMask,Size(originalH,originalH));
		threshold(dstMask,dstMask,128,255,CV_THRESH_BINARY);
		kernel.release();
		inpaint(dstResize,dstMask,dstResize,20,INPAINT_TELEA);
		//填补区域替换
		Mat inpaintDst;
		resize(dstResize,inpaintDst,Size(sizeWidth,sizeWidth));
		resize(dstMask,dstMask,Size(sizeWidth,sizeWidth));

		Mat dstTmp = dst.clone();
		uchar *dataArea = dstTmp.ptr<uchar>(0);
		uchar *dataResize =inpaintDst.ptr<uchar>(0);
		uchar *dataMask = dstMask.ptr<uchar>(0);

		for(int idx=0;idx<dst.rows*dst.cols;idx++,dataArea+=dst.channels(),dataResize+=dstResize.channels(),dataMask+=dstMask.channels())
		{
			if(*dataMask >128)
			{
				if(dst.channels() ==3)
				{
					*dataArea = *dataResize;
					*(dataArea+1)=*(dataResize+1);
					*(dataArea+2)=*(dataResize+2);
				}
				else
					*dataArea = *dataResize;
			}
		}
		resize(dstTmp,dst,Size(sizeWidth,sizeWidth));

		//释放
		dstMask.release();
		dstResize.release();
	}
	return true;
}

//轮廓归类,不符合的剔除
//vCCtours				输入/输出装订孔信息
bool CPunchHold::filtrateContoursByClassify(vector<cirCtours> &vCCtours)
{
	if(vCCtours.size() <3)
		return false;

	vector<cirCtours> vCCtoursRL;//right-left way
	vector<int> vCCtoursRL_itr;
	vector<cirCtours> vCCtoursUL;//up-low way
	vector<int> vCCtoursUL_itr;
	vector<vector<cirCtours> > vvCCtours;
	for(vector<cirCtours>::iterator itr =vCCtours.begin();itr!=vCCtours.end();itr++)
	{
		if(itr->vpoint.size() <1)
			continue;
		//left-right way
		if( vCCtoursRL.size() <1 && vCCtoursUL.size()<1 )
		{
			vCCtoursRL.push_back(*itr);
			vCCtoursUL.push_back(*itr);
			itr->vpoint.clear();
		}

		//遍历同维度
		int idx =0;
		for (vector<cirCtours>::iterator itrIn =vCCtours.begin();itrIn!=vCCtours.end();itrIn++,idx++)
		{
			if(itrIn->vpoint.size() <1)
				continue;
			float thresholdNum =20;
			//right-left way
			if( abs(vCCtoursRL[0].pt2f.x - itrIn->pt2f.x) <= min(itrIn->circleR,thresholdNum))
			{vCCtoursRL.push_back(*itrIn);vCCtoursRL_itr.push_back(idx);}

			//up-low way
			if( abs(vCCtoursUL[0].pt2f.y - itrIn->pt2f.y) <= min(itrIn->circleR,thresholdNum))
			{vCCtoursUL.push_back(*itrIn);vCCtoursUL_itr.push_back(idx);}

		}

		if(vCCtoursRL.size()>1 || vCCtoursUL.size() >1 )
		{
			if(vCCtoursRL.size() > vCCtoursUL.size())
			{
				vvCCtours.push_back(vCCtoursRL);
				for(vector<int>::iterator itrIn_delete_circleContours =vCCtoursRL_itr.begin();itrIn_delete_circleContours !=vCCtoursRL_itr.end();itrIn_delete_circleContours++)
					vCCtours[*itrIn_delete_circleContours].vpoint.clear();
			}else
			{
				vvCCtours.push_back(vCCtoursUL);
				for(vector<int>::iterator itrIn_delete_circleContours =vCCtoursUL_itr.begin();itrIn_delete_circleContours !=vCCtoursUL_itr.end();itrIn_delete_circleContours++)
					vCCtours[*itrIn_delete_circleContours].vpoint.clear();
			}
		}
		vCCtoursRL_itr.clear();
		vCCtoursUL_itr.clear();
		vCCtoursRL.clear();
		vCCtoursUL.clear();
	}// end for
	vCCtoursUL.clear();
	//重新分配矩形
	vCCtours.clear();
	for (vector<vector<cirCtours> >::iterator itr = vvCCtours.begin();itr!=vvCCtours.end();itr++)
	{
		vCCtoursUL =*itr;
		vCCtours.insert(vCCtours.end(),vCCtoursUL.begin(),vCCtoursUL.end());
		vCCtoursUL.clear();
	}

	return true;
}

/***********************新的筛选方案*********************************/
bool cmpCtoursX(cirCtours cc1,cirCtours cc2)
	{return cc1.pt2f.x < cc2.pt2f.x;}

bool cmpCtoursY(cirCtours cc1,cirCtours cc2)
	{return cc1.pt2f.y < cc2.pt2f.y;}

bool CPunchHold::newSplit(const Mat &dst,vector<cirCtours> &vCCtours)
{
	vector<cirCtours> vCCtoursCopy = vCCtours;
	
	sort(vCCtours.begin(),vCCtours.end(),cmpCtoursX);
	sort(vCCtoursCopy.begin(),vCCtoursCopy.end(),cmpCtoursY);
	//X
	vector<cirCtours> vCirCtoursXMin;
	vector<cirCtours> vCirCtoursXMax;
	for(vector<cirCtours>::iterator itr=vCCtours.begin();itr != vCCtours.end();itr++)
	{
		if(itr->pt2f.x <dst.cols/10 && itr->pt2f.x >10)
			vCirCtoursXMin.push_back(*itr);
		else if(itr->pt2f.x >dst.cols*9/10 && itr->pt2f.x <dst.cols-10 )
			vCirCtoursXMax.push_back(*itr);
	}
	//Y
	vector<cirCtours> vCirCtoursYMin;
	vector<cirCtours> vCirCtoursYMax;
	vector<cirCtours> vCirCtoursTmp;
	for(vector<cirCtours>::iterator itr=vCCtoursCopy.begin();itr!=vCCtoursCopy.end();itr++)
	{
		if(itr->pt2f.x >=dst.cols/10 &&itr->pt2f.x <=dst.cols*9/10)
		{
			if(itr->pt2f.y <dst.rows/5 && itr->pt2f.y >5)
				vCirCtoursYMin.push_back(*itr);
			else if(itr->pt2f.y >dst.rows*4/5 && itr->pt2f.y <dst.rows-10)
				vCirCtoursYMax.push_back(*itr);
		}
		else 
		{
			if(itr->pt2f.y <dst.rows/5 && itr->pt2f.y>5)
				vCirCtoursTmp.push_back(*itr);
			else if(itr->pt2f.y >dst.rows*4/5 && itr->pt2f.y <dst.rows-10)
				vCirCtoursTmp.push_back(*itr);
		}
	}

	int nDif = min(dst.cols,dst.rows)/20;
	CPunchHold::isBindingHole(dst,nDif,vCirCtoursXMin);
	CPunchHold::isBindingHole(dst,nDif,vCirCtoursXMax);
	CPunchHold::isBindingHole(dst,nDif,vCirCtoursYMin,false);
	CPunchHold::isBindingHole(dst,nDif,vCirCtoursYMax,false);

	vCCtours.clear();
	if(vCirCtoursXMin.size() >1)
		vCCtours.insert(vCCtours.begin(),vCirCtoursXMin.begin(),vCirCtoursXMin.end());
	if(vCirCtoursXMax.size() >1)
		vCCtours.insert(vCCtours.begin(),vCirCtoursXMax.begin(),vCirCtoursXMax.end());
	if(vCirCtoursYMin.size() >1)
		vCCtours.insert(vCCtours.begin(),vCirCtoursYMin.begin(),vCirCtoursYMin.end());
	if(vCirCtoursYMax.size() >1)
		vCCtours.insert(vCCtours.begin(),vCirCtoursYMax.begin(),vCirCtoursYMax.end());

	if(vCirCtoursYMax.size() >1 || vCirCtoursYMin.size() >1)
	{
		//vCCtours.insert(vCCtours.begin(),vCirCtoursTmp.begin(),vCirCtoursTmp.end());
		for(vector<cirCtours>::iterator itr = vCirCtoursTmp.begin();itr!=vCirCtoursTmp.end();itr++)
		{
			if(vCirCtoursYMax.size() >1)
				if( abs(vCirCtoursYMax[0].pt2f.y - itr->pt2f.y) <vCirCtoursYMax[0].circleR  )
					vCCtours.push_back(*itr);

			if(vCirCtoursYMin.size() >1)
				if(abs(vCirCtoursYMin[0].pt2f.y - itr->pt2f.y)<vCirCtoursYMin[0].circleR)
					vCCtours.push_back(*itr);
		}
	}
	return true;
}

bool CPunchHold::isBindingHole(const Mat &dst,int nDif,vector<cirCtours> &vCirCtours,bool isX)
{
	if(vCirCtours.size() <=1)
		 return false;

	vector<vector<cirCtours> > vvCirCtours;
	vector<cirCtours> vCirTmp;

	vCirTmp.push_back(vCirCtours[0]);

	for(vector<cirCtours>::iterator itr =vCirCtours.begin()+1;itr !=vCirCtours.end();itr++)
	{
		if( (abs(vCirTmp[vCirTmp.size()-1].pt2f.x - itr->pt2f.x)<vCirTmp[0].circleR && isX ) || (!isX && abs(vCirTmp[vCirTmp.size()-1].pt2f.y - itr->pt2f.y) < vCirTmp[0].circleR ))
			vCirTmp.push_back(*itr);
		else
		{
			vvCirCtours.push_back(vCirTmp);
			vCirTmp.clear();
			vCirTmp.push_back(*itr);
		}
	}
	vvCirCtours.push_back(vCirTmp);
	
	vCirCtours.clear();
	vCirCtours = vvCirCtours[0];
	for(int idx=1;idx <vvCirCtours.size();idx++)
	{
		if( (vvCirCtours[idx]).size() > vCirCtours.size())
		{vCirCtours.clear();vCirCtours = vvCirCtours[idx];}
	}

	if(isX)
		sort(vCirCtours.begin(),vCirCtours.end(),cmpCtoursY);
	else
		sort(vCirCtours.begin(),vCirCtours.end(),cmpCtoursX);	

	if(( abs((vCirCtours[0].pt2f.x + vCirCtours[vCirCtours.size()-1].pt2f.x )/2 - dst.cols/2) < nDif  && !isX ) 
		||
		( abs((vCirCtours[0].pt2f.y + vCirCtours[vCirCtours.size()-1].pt2f.y )/2 - dst.rows/2) < nDif  && isX )
		||vCirCtours.size() >3
		)
		;
	else
		vCirCtours.clear();

	return true;
}
