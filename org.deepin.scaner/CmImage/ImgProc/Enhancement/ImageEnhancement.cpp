#include "stdafx.h"
#include "AutoLevel.h"
#include "ImageEnhancement.h"
#include "../ColorSpace/AdaptiveThreshold.h"

//图像增强类   版本号：V0.0.8

bool cmpInt(int &i1,int &i2)
{
	return i1 < i2;
}

//文字增强，背景平滑/滤除，自动色阶 V0.0.1
//[in]src			输入图像
//[out]dst			输出图像
//[in]nType
//	1				文字增强
//	2				背景平滑
//	3				背景滤除
//	4				自动色阶
//注：出现其它数字默认转换为0,不做处理
//[in]maxValue		亮度最大值
//[in]minValue		亮度最小值
//--- maxValue,minValue--- 有负值则参数失效
//[in]fGmax			最大增益(0.3,1.5]
//[in]fGmin			最小增益[0,0.3]
//--- fGmax,fGmin---为1则对应的参数失效
bool CImageEnhancement::imageStretch(Mat src,Mat &dst,uint nType/* =0 */,int maxValue /* =255 */,int minValue /* =0 */,float fGmaxIn /* =1 */,float fGminIn/* =1 */)
{
	dst = src.clone();
	vector<Mat> vectorOfChannelImage;
	//split colorful image
	if(src.channels() == 3)
		split(src,vectorOfChannelImage);
	else
		vectorOfChannelImage.push_back(src);
	
	//lookupTable
	int keepValueTable_r[256];
	int keepValueTable_g[256];
	int keepValueTable_b[256];
	int autoLevelValueLookupTableResult[256];
	bool isAutoLevel =false;
	int textEnhancementValueLookupTable[256];
	bool isTextEnhancement =false;

	int backgroundValueTable_R[256];
	int backgroundValueTable_G[256];
	int	backgroundValueTable_B[256];
	bool isBackGroundRemove =false;
	bool isBackGroundSmooth =false;

	//背景平滑/滤除
	const float fRatio=0.9;
	int backgroundColorThreshold_r=-1;
	int backgroundColorThreshold_g=-1;
	int	backgroundColorThreshold_b=-1;

	//初始化
	for(int idx=0;idx<256;idx++)
	{
		keepValueTable_r[idx]=idx;
		keepValueTable_g[idx]=idx;
		keepValueTable_b[idx]=idx;

		autoLevelValueLookupTableResult[idx]=idx;
		textEnhancementValueLookupTable[idx]=idx;
		
		backgroundValueTable_R[idx]=idx;
		backgroundValueTable_G[idx]=idx;
		backgroundValueTable_B[idx]=idx;
	}

	//nType
	vector<int> vInt;
	while(nType)
	{
		vInt.push_back(nType%10);
		nType = nType/10;
	}

	sort(vInt.begin(),vInt.end());//,cmpInt);
	if(vInt.size()<1 ||(vInt.size()==1 &&vInt[0]==0))
		return false;

	//createLookupTable
	for(vector<int>::iterator itr = vInt.begin(); itr!=vInt.end();itr++)
	{
		switch(*itr)
		{
		case 4:
			{
				CImageEnhancement::autoLevelUnisLookTable(vectorOfChannelImage,autoLevelValueLookupTableResult,maxValue,minValue,fGmaxIn,fGminIn);
				isAutoLevel = true;

				for(int idx=0;idx<256;idx++)
					{
						keepValueTable_r[idx]=autoLevelValueLookupTableResult[keepValueTable_r[idx]];
						keepValueTable_g[idx]=autoLevelValueLookupTableResult[keepValueTable_g[idx]];
						keepValueTable_b[idx]=autoLevelValueLookupTableResult[keepValueTable_b[idx]];
					}
				break;
			}
		
		case 1:
			{
				Mat gray;
				if(3 == src.channels())
					cvtColor(src,gray,CV_RGB2GRAY);
				else
					gray = src;
				uchar* dataGray = gray.ptr<uchar>(0);
				vector<int>	vImgValue;
				for(int idx=0;idx<gray.rows*gray.cols;idx++,dataGray++)
					vImgValue.push_back(*dataGray);

				sort(vImgValue.begin(),vImgValue.end());//,cmpInt);
				float fLimitValue = (float)vImgValue[vImgValue.size()/10*8];

				for (int idx =0;idx <256;idx++)
					textEnhancementValueLookupTable[idx] = saturate_cast<uchar>(fLimitValue/(1.0+pow((float)10.0,(float)(-1.0*((float)idx -fLimitValue/1.1)/25.5)))+fLimitValue/8.5);
				isTextEnhancement =true;

				for(int idx=0;idx<256;idx++)
				{
					if(idx < fLimitValue)
					{
						keepValueTable_r[idx]=textEnhancementValueLookupTable[keepValueTable_r[idx]];
						keepValueTable_g[idx]=textEnhancementValueLookupTable[keepValueTable_g[idx]];
						keepValueTable_b[idx]=textEnhancementValueLookupTable[keepValueTable_b[idx]];
					}else
					{
						keepValueTable_r[idx]=idx;
						keepValueTable_g[idx]=idx;
						keepValueTable_b[idx]=idx;
					}
				}
				break;
			}

		case 2:
		case 3:
		{
			if(backgroundColorThreshold_r ==-1)
            {
                IplImage tmp = IplImage(vectorOfChannelImage[0]);
                IplImage *ipl = &tmp;
                backgroundColorThreshold_r = (float)CAdaptiveThreshold::OptimalThreshold(ipl)*fRatio;
            }
			if(backgroundColorThreshold_g ==-1 && vectorOfChannelImage.size()==3)
            {
                IplImage tmp = IplImage(vectorOfChannelImage[1]);
                IplImage *ipl = &tmp;
                backgroundColorThreshold_g = (float)CAdaptiveThreshold::OptimalThreshold(ipl)*fRatio;
            }
			if(backgroundColorThreshold_b ==-1 && vectorOfChannelImage.size() ==3)
            {
                IplImage tmp = IplImage(vectorOfChannelImage[2]);
                IplImage *ipl = &tmp;
                backgroundColorThreshold_b = (float)CAdaptiveThreshold::OptimalThreshold(ipl)*fRatio;
            }
			if( (*itr)==2 )
				isBackGroundSmooth =true;
			else
				isBackGroundRemove =true;
			
			CImageEnhancement::backGroundLookupTable(isBackGroundSmooth,backgroundValueTable_R,backgroundValueTable_G,backgroundValueTable_B,
															backgroundColorThreshold_r,backgroundColorThreshold_g,backgroundColorThreshold_b);

			for(int idx=0;idx<256;idx++)
				{
					keepValueTable_r[idx]=backgroundValueTable_R[keepValueTable_r[idx]];
					keepValueTable_g[idx]=backgroundValueTable_G[keepValueTable_g[idx]];
					keepValueTable_b[idx]=backgroundValueTable_B[keepValueTable_b[idx]];
				}
			break;
		}
		
		default:
				break;
		}
	}

	if(!isAutoLevel&&!isTextEnhancement&&!isBackGroundSmooth&&!isBackGroundRemove)
		return false;

	Mat srcChannelRed,srcChannelBlue,srcChannelGreen;
	srcChannelRed = vectorOfChannelImage[0];
	if(vectorOfChannelImage.size() ==3)
	{
		srcChannelGreen =vectorOfChannelImage[1];
		srcChannelBlue =vectorOfChannelImage[2];
	}

	uchar *dataR = srcChannelRed.ptr<uchar>(0);
	uchar *dataG =dataR;
	uchar *dataB = dataR; 
	if(vectorOfChannelImage.size() ==3)
	{
		dataG = srcChannelGreen.ptr<uchar>(0);
		dataB = srcChannelBlue.ptr<uchar>(0);
	}

	for (int idx=0;idx<srcChannelRed.rows*srcChannelRed.cols;idx++,dataR++,dataG++,dataB++)
		if(*dataR >backgroundColorThreshold_r*fRatio && ((*dataG >backgroundColorThreshold_g*fRatio && *dataB >backgroundColorThreshold_b*fRatio)||vectorOfChannelImage.size() ==1 ) )
		{
			*dataR = keepValueTable_r[*dataR];
			*dataG = keepValueTable_g[*dataG];
			*dataB = keepValueTable_b[*dataB];
		}
	
	vectorOfChannelImage[0]=srcChannelRed;
	if(vectorOfChannelImage.size() ==3)
	{
		vectorOfChannelImage[1]=srcChannelGreen;
		vectorOfChannelImage[2]=srcChannelBlue;
		merge(vectorOfChannelImage,dst);
	}
	else
		dst = srcChannelRed;
	return true;
}
 
//自动色阶查找表
bool CImageEnhancement::autoLevelUnisLookTable(vector<Mat> vectorOfChannelImage,int *newImageValueLookupTableResult,int maxValue /* =255 */,int minValue /* =0 */,float fGmaxIn /* =1 */,float fGminIn/* =1 */)
{
	float fGmax,fGmin;
	fGmax = fGmin =1;
	//判断maxValue,minValue范围
	maxValue = maxValue >255 ? 255:maxValue;
	minValue = minValue >254 ? 254:minValue;

	//find max/min value in image 
	int arrMaxValueInImage[3] ={0};
	int arrMinValueInImage[3] ={0};
	int maxValueInImage,minValueInImage,midValueInImage;
	maxValueInImage =minValueInImage =midValueInImage =0;
	float arrFgmax[3]={0.0};
	float arrFgmin[3]={0.0};
	//find max value in image
	int idxOfvectorOfImage =0;
	if(maxValue >0 && minValue >=0)
	{
		for (vector<Mat>::iterator itr = vectorOfChannelImage.begin();itr != vectorOfChannelImage.end(); itr++,idxOfvectorOfImage++)
		{
			Mat grayImage = *itr;

			if(!CAutoLevel::findMaxValueofImage(grayImage,arrMaxValueInImage[idxOfvectorOfImage],arrMinValueInImage[idxOfvectorOfImage]))
				return false;
			if(arrMaxValueInImage[idxOfvectorOfImage]!=0)
				fGmax = 255.0 / (float)arrMaxValueInImage[idxOfvectorOfImage];
			else
				fGmax =1.5;
			//limit of fGmax scope
			fGmax = fGmax > 1.5? 1.5:fGmax;
			fGmax = fGmax < 0.3? 0.3:fGmax;
			if(arrMinValueInImage[idxOfvectorOfImage]!=0)
				fGmin = (float)minValue / (float)arrMinValueInImage[idxOfvectorOfImage];
			else
				fGmin = 0.0;
			//limit of fGmin scope
			fGmin = fGmin >0.3? 0.3:fGmin;
			fGmin = fGmin <0?   0:fGmin;

			arrFgmax[idxOfvectorOfImage] =fGmax;
			arrFgmin[idxOfvectorOfImage] =fGmin;
		}
		

		if(vectorOfChannelImage.size() ==3)
		{
			fGmax = min(arrFgmax[0],min(arrFgmax[1],arrFgmax[2]));
			fGmin = (arrFgmin[0] +arrFgmin[1] +arrFgmin[2])/3.0;
			maxValueInImage = min(min(arrMaxValueInImage[0],arrMaxValueInImage[1]),arrMaxValueInImage[2]);
			minValueInImage = max(max(arrMinValueInImage[0],arrMinValueInImage[1]),arrMinValueInImage[2]);
			midValueInImage = (maxValueInImage +minValueInImage)/2.0;
		}
		else
		{
			fGmax = arrFgmax[0];
			fGmin = arrFgmin[0];
			maxValueInImage = arrMaxValueInImage[0];
			minValueInImage = arrMinValueInImage[0];
			midValueInImage = (maxValueInImage + minValueInImage)/2.0;
		}
	}
	else
	{
		maxValue =255;minValue=0;
	}
	//lookup table 
	int newImageValueLookupTable[256] ={0};

	for (float idx =0; idx<256;idx++)
	{
		if(idx >= midValueInImage)
			newImageValueLookupTable[(int)idx] = (idx * (1.0+ (fGmax-1.0) *(float)(idx-midValueInImage)/
			(float)(maxValueInImage -midValueInImage)))*((float)maxValue/255.0) +minValue;
		else
		{
			newImageValueLookupTable[(int)idx] = (idx * (1.0 +(fGmin-1.0) *(float)(midValueInImage-idx)/
				(float)(midValueInImage -minValueInImage)));

			newImageValueLookupTable[(int)idx] += minValue; 
		}
	}
	//create lookup table use fGmax&fGmin
	for (float idx =0; idx<256;idx++)
	{
		if(idx >= midValueInImage)
			newImageValueLookupTableResult[(int)idx] = (newImageValueLookupTable[(int)idx] * (1.0+ (fGmaxIn-1.0) *(float)(idx-midValueInImage)/
			(float)(maxValueInImage -midValueInImage)));
		else
			newImageValueLookupTableResult[(int)idx] = (newImageValueLookupTable[(int)idx] * (1.0 +(fGminIn-1.0) *(float)(midValueInImage-idx)/
			(float)(midValueInImage -minValueInImage)));
	}

	for(int idx=0;idx<256;idx++)
	{
		newImageValueLookupTableResult[idx]= newImageValueLookupTableResult[idx] >255 ? 255:newImageValueLookupTableResult[idx];
		newImageValueLookupTableResult[idx]= newImageValueLookupTableResult[idx] <0 ? 0:newImageValueLookupTableResult[idx];
	}


	return true;
}

//背景平滑/去除查找表
bool CImageEnhancement::backGroundLookupTable(bool isBackGroundColorSmooth,int *backGroundLookupTable_r,int *backGroundLookupTable_g,int *backgroundValueTable_b,int backgroundColorThreshold_r,int backgroundColorThreshold_g,int backgroundColorThreshold_b) 
{
	const float CRatio = 300.0/min(min(backgroundColorThreshold_r,backgroundColorThreshold_g),backgroundColorThreshold_b);
	const float CRatioSmooth = 290.0/max(max(backgroundColorThreshold_r,backgroundColorThreshold_g),backgroundColorThreshold_b);
#if 0
	//查询表
	for (int idx =0;idx <256;idx++)
		if(isBackGroundColorSmooth)
			backGroundLookupTable_r[idx] = saturate_cast<uchar>( (backgroundColorThreshold_r*2.0)/ (1.0+pow((float)2.718,(float)(-1.0*((float)idx -backgroundColorThreshold_r)/23)))+0);
		else
			backGroundLookupTable_r[idx] = saturate_cast<uchar>( 255.0/ (1.0+pow((float)2.718,(float)(-1.0*((float)idx -backgroundColorThreshold_r)/15))));

	uchar bookCheckG[256];
	for (int idx =0;idx <256;idx++)
		if(isBackGroundColorSmooth)
			backGroundLookupTable_g[idx] = saturate_cast<uchar>( (backgroundColorThreshold_g*2.0)/ (1.0+pow((float)2.718,(float)(-1.0*((float)idx -backgroundColorThreshold_g)/23)))+0);
		else
			backGroundLookupTable_g[idx] = saturate_cast<uchar>( 255.0/ (1.0+pow((float)2.718,(float)(-1.0*((float)idx -backgroundColorThreshold_g)/15))));

	uchar bookCheckB[256];
	for (int idx =0;idx <256;idx++)
		if(isBackGroundColorSmooth)
			backgroundValueTable_b[idx] = saturate_cast<uchar>( (backgroundColorThreshold_b*2.0)/ (1.0+pow((float)2.718,(float)(-1.0*((float)idx -backgroundColorThreshold_b)/23)))+0);
		else
			backgroundValueTable_b[idx] = saturate_cast<uchar>( 255.0/ (1.0+pow((float)2.718,(float)(-1.0*((float)idx -backgroundColorThreshold_b)/15))));
#endif
	//查询表
	for (int idx =0;idx <256;idx++)
		if(!isBackGroundColorSmooth)
			backGroundLookupTable_r[idx] = saturate_cast<uchar>( (backgroundColorThreshold_r*CRatioSmooth)/ (1.0+pow((float)2.718,(float)(-1.0*((float)idx -backgroundColorThreshold_r)/23)))+10);
		else
			backGroundLookupTable_r[idx] = saturate_cast<uchar>( (backgroundColorThreshold_r*CRatio)/ (1.0+pow((float)2.718,(float)(-1.0*((float)idx -backgroundColorThreshold_r)/15))));

	for (int idx =0;idx <256;idx++)
		if(!isBackGroundColorSmooth)
			backGroundLookupTable_g[idx] = saturate_cast<uchar>( (backgroundColorThreshold_g*CRatioSmooth)/ (1.0+pow((float)2.718,(float)(-1.0*((float)idx -backgroundColorThreshold_g)/23)))+10);
		else
			backGroundLookupTable_g[idx] = saturate_cast<uchar>( (backgroundColorThreshold_g*CRatio)/ (1.0+pow((float)2.718,(float)(-1.0*((float)idx -backgroundColorThreshold_g)/15))));

	for (int idx =0;idx <256;idx++)
		if(!isBackGroundColorSmooth)
			backgroundValueTable_b[idx] = saturate_cast<uchar>( (backgroundColorThreshold_b*CRatioSmooth)/ (1.0+pow((float)2.718,(float)(-1.0*((float)idx -backgroundColorThreshold_b)/23)))+10);
		else
			backgroundValueTable_b[idx] = saturate_cast<uchar>( (backgroundColorThreshold_b*CRatio)/ (1.0+pow((float)2.718,(float)(-1.0*((float)idx -backgroundColorThreshold_b)/15))));


	return true;
}

//TextEnhancement
//[in]src			输入图像
//[in/out]dst		输出图像
bool CImageEnhancement::textEnhancement(Mat src,Mat &dst,uchar *bookCheck)
{
	Mat gray;
	if(3 == src.channels())
		cvtColor(src,gray,CV_RGB2GRAY);
	else
		gray = src;
	//对灰度图进行缩放处理
	if(gray.rows*gray.cols >10000000)
	{
		float fRatio =sqrt(10000000.0/float(gray.rows*gray.cols));
		resize(gray,gray,Size(gray.rows*fRatio,gray.cols*fRatio));
	}

	uchar* dataGray = gray.ptr<uchar>(0);
	vector<int>	vImgValue;
	long lnum =gray.rows*gray.cols;
	for(int idx=0;idx< lnum;idx++,dataGray++)
		vImgValue.push_back(*dataGray);

	sort(vImgValue.begin(),vImgValue.end(),greater<int>());
	float fLimitValue = (float)vImgValue[vImgValue.size()/10*9];  

	gray.release();
	//返回图像类型
	uchar *data = src.ptr<uchar>(0);
	//查找表
	if(bookCheck == NULL)
	{
		uchar bookCheckTmp[256];
		bookCheck = bookCheckTmp;
	}

	for (int idx =0;idx <256;idx++)
		if(idx <=fLimitValue)
			bookCheck[idx] = saturate_cast<uchar>(fLimitValue/(1.0+pow((float)10.0,(float)(-1.0*((float)idx -fLimitValue/1.1)/25.5)))+fLimitValue/8.5);
		else
			bookCheck[idx]=idx;

	//拉伸
	for (int idy=0;idy<dst.rows;idy++ )
	{
		for(int idx=0;idx<dst.cols;idx++)
		{
			for(int idc =0;idc <dst.channels();idc++)
				*data++ =bookCheck[*data];
		}
	}

	dst = src;
	return true;
}

//TextEnhancement
//参数同上
bool CImageEnhancement::textEnhancementTmp(Mat src,Mat &dst,uchar *checkTable /* = nullptr */)
{
	vector<Mat> vMat;
	
	if(3 == src.channels())
		split(src,vMat);
	else
		vMat.push_back(src);

	int idx=0;
	for(vector<Mat>::iterator itr = vMat.begin();itr!=vMat.end();itr++,idx++)
	{
		Mat tmp = *itr;
		//CImageEnhancement::textEnhancementIn(tmp,tmp,checkTable);
		vMat[idx] = tmp;
	}

	merge(vMat,dst);

	return true;
}

//De-screen
//[in/out]			输入/输出图像
//[in]nType			类型
//	0				原图
//	1				杂志
//	2				报纸
//	3				印刷
bool CImageEnhancement::DeScreen(Mat &src,uint nType/* =0 */)
{
	switch(nType)
	{
	case 1:
		blur(src,src,Size(3,3),Point(2,2));
		break;

	case 2:
		blur(src,src,Size(7,7),Point(4,4));
		break;

	default:
		blur(src,src,Size(3,3),Point(2,2));
		break;
	}
	return true;
}

//De-screen
//[in/out]src		输入/输出图像
//[in]nSize			高斯核尺寸
bool CImageEnhancement::DeScreen(Mat &src,int nSzie /* =7 */,int Xsigma/* =0 */,int Ysigma/* =0 */)
{
	if(3 != src.channels())
		return false;

	Mat channel[3]; // RGB
	split(src, channel);
	//对图像高斯处理
	//Mat filterImg[3];
	GaussianBlur(channel[0], channel[0], Size(nSzie, nSzie), Xsigma, Ysigma);
	GaussianBlur(channel[1], channel[1], Size(nSzie, nSzie), Xsigma, Ysigma);
	GaussianBlur(channel[2], channel[2], Size(nSzie, nSzie), Xsigma, Ysigma);

	//channel[0].release();
	//channel[1].release();
	//channel[2].release();

#if 0
	merge(filterImg, 3, src);
	filterImg[0].release();
	filterImg[1].release();
	filterImg[2].release();
#else
	uchar *data = src.ptr<uchar>(0);

	uchar *dataR = channel[0].ptr<uchar>(0);
	uchar *dataG = channel[1].ptr<uchar>(0);
	uchar *dataB = channel[2].ptr<uchar>(0);

	for (int idr=0;idr<src.rows;idr++)
	{
		for (int idc=0;idc <src.cols;idc++,data+=3,dataR++,dataG++,dataB++)
		{
			*data = *dataR;
			*(data+1) = *dataG;
			*(data+2) = *dataB;
		}
	}
#endif
	return true;
}

//Gama
//[in/out]src		输入/输出图像
//[in]fGama			gama值
bool CImageEnhancement::gamaCorrection(Mat &src,float fGama/* =1.0 */)
{
	// build look up table
	unsigned char lut[256];
	for( int i = 0; i < 256; i++ )
	{
		lut[i] = saturate_cast<uchar>(pow((float)(i/255.0),fGama) * 255.0);
	}
	
	uchar *data =src.ptr<uchar>(0);
	const int channels = src.channels();
	switch(channels)
	{
		case 1:
			{
				for(int idx =0;idx<src.rows*src.cols;idx++ )
					*data++ =lut[*data];
				break;
			}
		case 3: 
			{
				for(int idx =0;idx<src.rows*src.cols*3;idx +=3 )
				{
					*data =lut[*data];
					*(data+1) =lut[*(data+1)];
					*(data+2) =lut[*(data+2)];
					data+=3;
				}
				break;
			}
	}
	return true;
}

//图像滤色
//[in/out]src			输入/输出图像
//[in]removeColorType	
//	0					红色
//	1					绿色
//	2					蓝色
bool CImageEnhancement::removeColor(Mat &src,int removeColorType /* =0 */)
{
	if(src.channels() != 3)
		return false;
	Mat dst;
	//图像类型转换
	cvtColor(src,dst,CV_RGB2HSV);
	//图层分解
	vector<Mat> vMatHsv;
	split(dst,vMatHsv);
	Mat matHue,matSaturation,matValue;
	matHue = vMatHsv[0];
	matSaturation =vMatHsv[1];
	matValue =vMatHsv[2];
	vMatHsv.clear();
	//阈值选择
	int nHueMax  = 0;
	int nHueMin = 0;
	if (removeColorType == 0)
	{
		nHueMin = -42.48;
		nHueMax = 42.48;
	}
	else if (removeColorType == 1)
	{
		nHueMin = 0;
		nHueMax = 0;
	}
	else
	{
		nHueMin = 0;
		nHueMax = 0;
	}

	//Hue通道滤色
	uchar *dataSrc =src.ptr<uchar>(0);
	uchar *data = matHue.ptr<uchar>(0);
	uchar *dataValue = matValue.ptr<uchar>(0);
	uchar *dataSaturation = matSaturation.ptr<uchar>(0);
	for (int idr = 0;idr <matHue.rows;idr++)
	{
		for (int idc =0;idc <matHue.cols;idc++,data++,dataValue++,dataSaturation++)
		{
			if ((nHueMin>0&&*data >= nHueMin&& *data < nHueMax)||
				(nHueMin<0&&(255-*data >= nHueMin&&255-*data<=0)||(*data<=nHueMax)))
			{
					*dataValue =0;
			}
		}
	}
	vMatHsv.clear();
	vMatHsv.push_back(matHue);
	vMatHsv.push_back(matSaturation);
	vMatHsv.push_back(matValue);
	//合并
	merge(vMatHsv,src);
	cvtColor(src,src,CV_HSV2BGR);
	return true;
}
