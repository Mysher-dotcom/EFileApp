#include "stdafx.h"
#include "ColorEnhance.h"
//V0.0.4

//Hue色调调节
bool CColorEnhance::adjustHue(const Mat &src,Mat &dst,int adjustScope /* =0 */)
{
	if(src.channels() !=3)
		return false;

	Mat mat_hsb;

	cvtColor(src,mat_hsb,CV_RGB2HSV);
	vector<Mat> vMat;
	split(mat_hsb,vMat);

	adjustScope = adjustScope <-100 ?-100:adjustScope;
	adjustScope = adjustScope >100 ?100:adjustScope;

	float tmp = (float)(adjustScope +100.0)*float(255.0/200.0); 

	uchar *data = vMat[0].ptr<uchar>(0);
	for (int idx=0;idx<vMat[0].rows*vMat[0].cols*vMat[0].channels();idx++,data++)
		*data += (int)(tmp+0.5); 

	merge(vMat,dst);
	cvtColor(dst,dst,CV_HSV2RGB);

	return true;
}

//彩色图像增强(保真)
bool CColorEnhance::colorEnhance(Mat src,Mat &dst,int isAutoDetectWhiteBackGroud)
{
	Mat originalMat =src.clone();

	vector<Mat> vMat;
	//判断是否是彩色
	//保真图组
	Mat imgR,imgG,ImgB;
	//增亮图组
	Mat imgR1,imgG1,imgB1;
	if(src.channels()==3)
	{
		split(src,vMat);
		//保真图组
		imgR = vMat[2];
		imgG = vMat[1];
		ImgB = vMat[0];
		//增量图组
		imgR1 = vMat[2];
		imgG1 = vMat[1];
		imgB1 = vMat[0];
	}else
	{
		imgR = src.clone();
		imgR1 = src.clone();
	}
	//对图像进行增强
	//sigmoid函数256个对应的数值
	float sigmoid256Fidelity[256]={0};//保真
	float sigmoid256Blast[256]={0};//增亮
	for(float idx =0;idx<256;idx++)
	{
		sigmoid256Fidelity[(int)idx] = 255.0/(1.0+pow((float)2.718,(float)(-1.0*(idx -255.0/2.0)/25.0)));
		//sigmoid256Blast[(int)idx] = 255.0/(1.0+pow((float)2.3,(float)(-1.0*(idx -255.0/3.0)/20.0)));
		sigmoid256Blast[(int)idx] = 255.0 /(1.0+pow((float)2.5,(float)(-1.0*(idx-255.0/2.5)/23.0)));
	}
	//计算图像中白色区域占的比例
	int whiteNum =0;
	int zerosNum =0;
	//彩色图像
	uchar *dataR = imgR.ptr<uchar>(0);
	uchar *dataR1 = imgR1.ptr<uchar>(0);
	if(src.channels() ==3)
	{
		//保真
		uchar *dataG = imgG.ptr<uchar>(0);
		uchar *dataB = ImgB.ptr<uchar>(0);
		//增亮
		uchar *dataG1 =imgG1.ptr<uchar>(0);
		uchar *dataB1 =imgB1.ptr<uchar>(0);

		for (int idr =0;idr <src.rows;idr++)
		{
			for(int idc =0;idc <src.cols;idc++)
			{
				//计算白色/黑色区域个数
				if(*dataR <20 && *dataG <20 && *dataB <20)
					zerosNum++;
				if(*dataR >60 && *dataG >60 && *dataB>60&&abs(*dataB-*dataG)<20&&abs(*dataB-*dataR)<20&&abs(*dataG-*dataR)<20)
					whiteNum++;
				//彩色保真增强
				*dataR++ =sigmoid256Fidelity[*dataR];
				*dataG++ =sigmoid256Fidelity[*dataG];
				*dataB++ =sigmoid256Fidelity[*dataB];

				//彩色亮度增强
				*dataR1++ = sigmoid256Blast[*dataR1];
				*dataG1++ = sigmoid256Blast[*dataG1];
				*dataB1++ = sigmoid256Blast[*dataB1];
			}
		}
		//计算白色区域比例
		float whiteRatio = (float)whiteNum/(float)(ImgB.rows*ImgB.cols-zerosNum);
		//清空图像组
		vMat.clear();
		if( (whiteRatio <0.2 && isAutoDetectWhiteBackGroud ==-1 ) || isAutoDetectWhiteBackGroud ==1)
		{	
			vMat.push_back(ImgB);
			vMat.push_back(imgG);
			vMat.push_back(imgR);
			merge(vMat,dst);
			//加权相加
			addWeighted(originalMat,0.8,dst,0.2,0,dst);
		}
		else if( (whiteRatio >=0.2 && isAutoDetectWhiteBackGroud ==-1 )|| isAutoDetectWhiteBackGroud ==0 )
		{
			vMat.push_back(imgB1);
			vMat.push_back(imgG1);
			vMat.push_back(imgR1);
			merge(vMat,dst);
			addWeighted(originalMat,0.7,dst,0.5,0,dst);
		}
	}
	else
	{
		for(int idr =0;idr <src.rows;idr++)
			for(int idc =0;idc<src.cols;idc++)
			{
				*dataR++ = sigmoid256Fidelity[*dataR];
				*dataR1++ =sigmoid256Blast[*dataR1];
				//计算黑白区域
				if(*dataR <20 )
					zerosNum++;
				if(*dataR >60 )
					whiteNum++;
			}
			//计算白色区域所占比例
			float whiteRatio = (float)whiteNum/(float)(ImgB.rows*ImgB.cols-zerosNum);
			if((whiteRatio <0.2 && isAutoDetectWhiteBackGroud ==-1 ) || isAutoDetectWhiteBackGroud ==1)
				dst = imgR;
			else if((whiteRatio >=0.2 && isAutoDetectWhiteBackGroud ==-1 )|| isAutoDetectWhiteBackGroud ==0)
				dst = imgR1;

	}
	return true;
}

//图像阴影增强
bool CColorEnhance::shadowEnhance(const Mat &src,Mat &reDst)
{
	Mat dst = src.clone();
	if( 3 != dst.channels())
		return false;

	Mat mask,mask1;
	CColorEnhance::findExtremeImg(dst,mask,mask1,false);

	return CColorEnhance::colorEnhanceWithShadow(reDst,mask,mask1);
}

//////////////////////////////私有成员////////////////////////////////////
//寻找图像暗/亮通道
bool CColorEnhance::findExtremeImg(const Mat &src,Mat &dst,Mat &dst1,bool isDarkChannel /* = true */)
{
	if(src.channels() !=3)
		return false;

	const uchar *data =src.ptr<uchar>(0);
	dst =Mat::zeros(src.rows,src.cols,CV_8UC1);
	uchar *dataDst =dst.ptr<uchar>(0);
	if(isDarkChannel)
		for (int idr =0;idr<src.rows;idr++)
		{
			for (int idc =0;idc<src.cols;idc++,data +=3,dataDst++)
			{
				*dataDst = min(min(*(data),*(data+1)),*(data+2));
			}//end for(cols)
		}//end for(rows)
	else
		for (int idr =0;idr<src.rows;idr++)
		{
			for (int idc =0;idc<src.cols;idc++,data +=3,dataDst++)
			{
				*dataDst = max(max(*(data),*(data+1)),*(data+2));
			}//end for(cols)
		}//end for(rows)

	int kSize = min(dst.rows,dst.cols)/40;
	kSize = kSize > 10 ? 10:kSize;
	kSize =kSize*2+1;
	//medianBlur(dst,dst1,kSize);
	blur(dst,dst1,Size(kSize,kSize));
#if 1
	int kSize1 = min(dst1.rows,dst1.cols)/30;
	kSize1 = kSize1 >10 ?10:kSize1;
	kSize1 = kSize1*2+1;
	Mat element = getStructuringElement(MORPH_ERODE, Size(kSize,kSize));
	//进行腐蚀操作
	dilate(dst1,dst1,element);
	erode(dst1,dst1,element);
	//medianBlur(dst1,dst1,kSize);
	blur(dst,dst1,Size(kSize,kSize));
#else
	cvtColor(src,dst1,CV_RGB2GRAY);
#endif
	return true;
}

#include <math.h>
//图像增强
bool CColorEnhance::colorEnhanceWithShadow(Mat &src,Mat mask,Mat mask1)
{
	float sigmoid256Blast[256]={0};//增亮
	float checkBook[256];
	
	for (int idx=0;idx<256;idx++)
	{
		checkBook[idx] = pow(float(255.0-idx)/(float)255.0,float(4))*3.0 +0.9;
		sigmoid256Blast[idx] = 255.0 /(1.0+pow((float)2.5,(float)(-1.0*((float)idx-255.0/2.5)/25.0)));
	}

	if(src.channels() !=3)
		return false;

	uchar *data = src.ptr<uchar>(0);
	uchar *dataMask = mask.ptr<uchar>(0);
	uchar *dataMask1 =mask1.ptr<uchar>(0);

	for(int idr =0;idr<src.rows;idr++)
		for(int idc=0;idc<src.cols;idc++,dataMask++,dataMask1++)
			for(int idx=0;idx<src.channels();idx++,data++)
			{
#if 0
				*data =saturate_cast<uchar>(sigmoid256Blast[int(saturate_cast<uchar>(*data * checkBook[*dataMask1<140?*dataMask1:*dataMask]))]);
#else
				*data = sigmoid256Blast[int(saturate_cast<uchar>(*data*checkBook[*dataMask1<140?*dataMask1:*dataMask]))];
#endif
			}
	return true;
}

//图像黑白化
Mat CColorEnhance::whiteAndBlack(const Mat &src,const int kernerl_size/* =61 */)
{
	Mat gray = Mat::zeros(src.rows,src.cols,CV_8UC1);;
	if(3 == src.channels())
		{
			const uchar *data =src.ptr<uchar>(0);
			uchar *dataDst =gray.ptr<uchar>(0);
			for (int idr =0;idr<src.rows;idr++)
			{
				for (int idc =0;idc<src.cols;idc++,data +=3,dataDst++)
				{
					*dataDst = min(min(*(data),*(data+1)),*(data+2));
				}//end for(cols)
			}//end for(rows)
		}
	else
		gray = src.clone();


	Mat grayFilter;
	Mat kernel_image = Mat::ones(kernerl_size,kernerl_size,CV_32FC1);
	kernel_image /= kernerl_size*kernerl_size*1.01;
	filter2D(gray,grayFilter,gray.depth(),kernel_image);
	gray =255 + gray -grayFilter;

	return gray;
}
