#include "stdafx.h"
#include "BrightnessBalance.h"
#include "Histogram.h"
//V1.0.4 封装整理 10.25

CBrightnessBalance::CBrightnessBalance(void)
{
}


CBrightnessBalance::~CBrightnessBalance(void)
{
}

/*************************************全局资源*************************************/
//线程资源
//HANDLE Thread_r=NULL;
//HANDLE Thread_g=NULL;
//HANDLE Thread_b=NULL;		
//线程的独立资源
IplConvKernel* element_r=NULL;
IplConvKernel* element_g=NULL;
IplConvKernel* element_b=NULL;
//高斯核
CvMat kernel_r;
CvMat kernel_g;
CvMat kernel_b;
CvMat kernel_image;
//中间变量
IplImage * temp_r=NULL;
IplImage * temp_g=NULL;
IplImage * temp_b=NULL;		
//分通道图像
IplImage * pImgr=NULL;
IplImage * pImgg=NULL;
IplImage * pImgb=NULL;		
/*************************************全局资源*************************************/
/*************************************全局变量*************************************/
//线程挂起信号量
bool CalcOver_r=false;
bool CalcOver_g=false;
bool CalcOver_b=false;
bool isDoc=false;				//决定文档模式还是非文档模式，缺省=false
int T_bottom=-1;int T_top=-1;	//直方图参数
/*************************************全局变量*************************************/
/*************************************Run*************************************/
#if 0
void RunR()
{
	while(true)
	{
		if(NULL==Thread_r)
		{
			return;
		}
		if(isDoc){	//文档模式使用高斯截面
			cvFilter2D(pImgr,temp_r,&kernel_r,cvPoint(-1,-1));///对图像做卷积
			
			cvSub(temp_r,pImgr,pImgr);//把两个数组的对应元素相减
			/*cvSaveImage("D:\\Filter2D_r.jpg",temp_r);
			cvSaveImage("D:\\Sub_r.jpg",pImgr);*/
		}
		else{		//非文档模式使用顶帽变换
			//cvMorphologyEx(pImgr,pImgr,temp_r,element_r, MORPH_TOPHAT);
		}
		CalcOver_r=true;
		SuspendThread(Thread_r);
	}
}
void RunG()
{

	while(true)
	{
		/*if(NULL==Thread_g)
		{
			return;
		}*/
		if(isDoc){
			cvFilter2D(pImgg,temp_g,&kernel_g,cvPoint(-1,-1));
			cvSub(temp_g,pImgg,pImgg);
			/*cvSaveImage("D:\\Filter2D_g.jpg",temp_r);
			cvSaveImage("D:\\Sub_g.jpg",pImgr);*/
		}
		else{
			//cvMorphologyEx(pImgg,pImgg,temp_g,element_g, MORPH_TOPHAT);
		}
		CalcOver_g=true;
		SuspendThread(Thread_g);
	}
}
void RunB()
{

	while(true)
	{
		/*if(NULL==Thread_b)
		{
			return;
		}
		if(isDoc){
			cvFilter2D(pImgb,temp_b,&kernel_b,cvPoint(-1,-1));
			cvSub(temp_b,pImgb,pImgb);
			/*cvSaveImage("D:\\Filter2D_b.jpg",temp_r);
			cvSaveImage("D:\\Sub_b.jpg",pImgr);*/
		}
		else{
			//cvMorphologyEx(pImgb,pImgb,temp_b,element_b, MORPH_TOPHAT);
		}
		CalcOver_b=true;
		SuspendThread(Thread_b);
	}
}
#endif
/*************************************Run*************************************/
void GrayStretch(IplImage * image)
{
	T_top = 245;
	//生成map
	uchar map[256];
	for(int i=0;i<256;i++)
	{
		if(i<=T_bottom)
		{
			map[i]=0;
		}
		else if(i<T_top){
			map[i]=(uchar)(( (i-T_bottom)*1.0/(T_top-T_bottom))*255);
			//map[i] =saturate_cast<uchar>(255.0/(1.0+pow(float(2.714),float(0.06*(T_top-70-i)))));
			//map[i]=(uchar)(sqrt( (i-100)*1.0/(255-100))*255);
			//map[i] =i;
		}
		else
		{
			map[i]=255;
		}
	}
	//映射
	for(int y=0;y<image->height;y++)
	{
		uchar * pDataSrc=(uchar*)(image->imageData+y*image->widthStep);
		for(int x=0;x<image->width;x++)
		{
			pDataSrc[x]=map[pDataSrc[x]];
		}
	}
}
void histtest(IplImage * r ,IplImage * g ,IplImage * b)
{
	long prob[256]={0};

	if (g == NULL || b == NULL)
	{
		for(int y=0;y<r->height;y++)
		{
			uchar * pDataSrc_r=(uchar*)(r->imageData+y*r->widthStep);
			for(int x=0;x<r->width;x++)
			{
				prob[pDataSrc_r[x]]++;
			}
		}
	}
	else
	{
		for(int y=0;y<r->height;y++)
		{
			uchar * pDataSrc_r=(uchar*)(r->imageData+y*r->widthStep);
			uchar * pDataSrc_g=(uchar*)(g->imageData+y*g->widthStep);
			uchar * pDataSrc_b=(uchar*)(b->imageData+y*b->widthStep);
			for(int x=0;x<r->width;x++)
			{
				prob[pDataSrc_r[x]]++;
				prob[pDataSrc_g[x]]++;
				prob[pDataSrc_b[x]]++;
			}
		}
	}
	//计算概率分布
	
	//map参数计算
	float fRadioBottom = 0.016;//0.016
	float fRadioTop = 0.30;
	int nThBottom = r->height * r->width *3 * fRadioBottom;
	int nThTop  = r->height * r->width * 3 * fRadioTop;
	long sumacc=0;
	for(int m=0;m<256;m++)
	{
		sumacc+=prob[m];
		if(sumacc > nThBottom && -1==T_bottom)
		{
			T_bottom=m;
		}
		if(sumacc > nThTop && -1==T_top )
		{
			T_top=m;
		}
	}
	int tx=0;
}

bool Global_Init(CvSize imgSize, int depth)
{
	//形态学结构元素，全局仅初始化一次
	//int size_wh   =      isDoc?20:80;
	//element_r     = cvCreateStructuringElementEx(size_wh,size_wh,(size_wh+1)/2,(size_wh+1)/2,CV_SHAPE_RECT);
	//element_g    = cvCreateStructuringElementEx(size_wh,size_wh,(size_wh+1)/2,(size_wh+1)/2,CV_SHAPE_RECT);
	//element_b    = cvCreateStructuringElementEx(size_wh,size_wh,(size_wh+1)/2,(size_wh+1)/2,CV_SHAPE_RECT);
	//高斯均值核
	const int kernel_wh = 61;
	//const int kernel_wh = 61;
	//const int kernel_wh = 91;
	static float kernel_data[kernel_wh*kernel_wh];
	for (int index = 0; index<kernel_wh*kernel_wh; index++)
	{
		kernel_data[index] = 1.0 / kernel_wh / kernel_wh;
	}
#if 1
	kernel_image = cvMat(kernel_wh, kernel_wh, CV_32FC1, kernel_data);
#else
	kernel_r	=	cvMat(kernel_wh, kernel_wh, CV_32FC1, kernel_data);
	kernel_g    =   cvMat(kernel_wh,kernel_wh,CV_32FC1,kernel_data);
	kernel_b    =   cvMat(kernel_wh,kernel_wh,CV_32FC1,kernel_data);
#endif

	
	////中间变量,图像大小不变情况下仅需初始化一次
	//temp_r = cvCreateImage(imgSize, depth, 1);
	//temp_g = cvCreateImage(imgSize, depth, 1);
	//temp_b = cvCreateImage(imgSize, depth, 1);
	////分通道缓存,图像大小不变情况下仅需初始化一次
	//pImgr = cvCreateImage(imgSize, depth, 1);
	//pImgg = cvCreateImage(imgSize, depth, 1);
	//pImgb = cvCreateImage(imgSize, depth, 1);
	

	
#if 1
	
#else
	//创建线程资源并挂起，全局仅一次
	Thread_r = CreateThread(NULL, 10000000, (LPTHREAD_START_ROUTINE)RunR, NULL, CREATE_SUSPENDED, NULL);
	Thread_g = CreateThread(NULL, 10000000, (LPTHREAD_START_ROUTINE)RunG, NULL, CREATE_SUSPENDED, NULL);
	Thread_b = CreateThread(NULL, 10000000, (LPTHREAD_START_ROUTINE)RunB, NULL, CREATE_SUSPENDED, NULL);
#endif
	return true;
}

#if 0
bool Global_Dispose()
{
	CloseHandle(Thread_r);
	CloseHandle(Thread_g);
	CloseHandle(Thread_b);

	//element_r==NULL?__nop():cvReleaseStructuringElement(&element_r);
	//element_g==NULL?__nop():cvReleaseStructuringElement(&element_g);
	//element_b==NULL?__nop():cvReleaseStructuringElement(&element_b);

	//cvReleaseImage(&temp_r);
	//cvReleaseImage(&temp_g);
	//cvReleaseImage(&temp_b);

	//cvReleaseImage(&pImgr);
	//cvReleaseImage(&pImgg);
	//cvReleaseImage(&pImgb);
	return true;
}
#endif
void ImageBackFilter(IplImage * image)
{
	//非文档使用顶帽变换，需要反色
	//WriteLog(L"ImageBackFilter");
	if (!isDoc)
	{
		cvNot(image, image);
	}
	//三线程并行处理三通道
	if (image->nChannels == 3)
	{
		pImgr = cvCreateImage(cvGetSize(image), image->depth, 1);
		pImgg = cvCreateImage(cvGetSize(image), image->depth, 1);
		pImgb = cvCreateImage(cvGetSize(image), image->depth, 1);
		cvSplit(image, pImgb, pImgg, pImgr, 0);
	}
	else
	{
		pImgr = image;
	}
	CalcOver_r = false;
	CalcOver_g = false;
	CalcOver_b = false;

	bool loukong = false;
#if 1
	if (isDoc && pImgr != NULL) {  //文档模式使用高斯截面
		temp_r = cvCreateImage(cvGetSize(pImgr), pImgr->depth, 1);
		cvFilter2D(pImgr, temp_r, &kernel_image, cvPoint(-1, -1));
		
		if (loukong)
		{
			IplImage * temp= NULL;
			temp = cvCreateImage(cvGetSize(temp_r), image->depth, 1);
			cvNot(temp_r, temp);
			cvThreshold(temp, temp, 155, 255,CV_THRESH_TRUNC);
			cvNot(temp, temp_r);
			cvReleaseImage(&temp);
		}
		
		cvSub(temp_r, pImgr, pImgr);
		cvReleaseImage(&temp_r);
		cvNot(pImgr, pImgr);
	}
	else {         //非文档模式使用顶帽变换
				   //cvMorphologyEx(pImgr,pImgr,temp_r,element_r, MORPH_TOPHAT);
	}
	if (isDoc && pImgg != NULL) {
		temp_g = cvCreateImage(cvGetSize(pImgg), pImgg->depth, 1);
		cvFilter2D(pImgg, temp_g, &kernel_image, cvPoint(-1, -1));
		
		if (loukong)
		{
			IplImage * temp= NULL;
			temp = cvCreateImage(cvGetSize(temp_g), image->depth, 1);
			cvNot(temp_g, temp);
			cvThreshold(temp, temp, 155, 255,CV_THRESH_TRUNC);
			cvNot(temp, temp_g);
			cvReleaseImage(&temp);
		}
		
		cvSub(temp_g, pImgg, pImgg);
		cvReleaseImage(&temp_g);
		cvNot(pImgg, pImgg);
	}
	else {
		//cvMorphologyEx(pImgg,pImgg,temp_g,element_g, MORPH_TOPHAT);
	}
	if (isDoc && pImgb != NULL) {
		temp_b = cvCreateImage(cvGetSize(pImgb), pImgb->depth, 1);
		cvFilter2D(pImgb, temp_b, &kernel_image, cvPoint(-1, -1));
		
		if (loukong)
		{
			IplImage * temp= NULL;
			temp = cvCreateImage(cvGetSize(temp_b), image->depth, 1);
			cvNot(temp_b, temp);
			cvThreshold(temp, temp, 155, 255,CV_THRESH_TRUNC);
			cvNot(temp, temp_b);
			cvReleaseImage(&temp);
		}
		
		cvSub(temp_b, pImgb, pImgb);
		cvReleaseImage(&temp_b);
		cvNot(pImgb, pImgb);
	}
	else {
		//cvMorphologyEx(pImgb,pImgb,temp_b,element_b, MORPH_TOPHAT);
	}
#else
	//WriteLog(L"ResumeThread");
	ResumeThread(Thread_r);
	ResumeThread(Thread_g);
	ResumeThread(Thread_b);
	//主线程等待部分
	while(!CalcOver_r){
	     Sleep(10);
	     ;}
	while(!CalcOver_g){
	     Sleep(10);
	     ;}
	while(!CalcOver_b){
	     Sleep(10);
	     ;}
#endif

	//WriteLog(L"cvNot");
	//合并三通道
	
	
	

	/*cvSaveImage("D:\\Not_r.jpg",pImgr);
	cvSaveImage("D:\\Not_g.jpg",pImgg);
	cvSaveImage("D:\\Not_b.jpg",pImgb);*/
	//cvMerge(pImgb,pImgg,pImgr,0,image); 
	//通道调整拉伸
	//cvSplit(image,pImgb,pImgg,pImgr,0); 
	//WriteLog(L"histtest");
#if 1
	histtest(pImgr, pImgg, pImgb);
	//cout << "计算参数：" << T_bottom << " \t " << T_top << endl;	
	if (pImgr)
		GrayStretch(pImgr);
	if (pImgg)
		GrayStretch(pImgg);
	if (pImgb)
		GrayStretch(pImgb);
#endif
	//WriteLog(L"cvMerge");
	/*cvSaveImage("D:\\GrayStretch_r.jpg",pImgr);
	cvSaveImage("D:\\GrayStretch_g.jpg",pImgg);
	cvSaveImage("D:\\GrayStretch_b.jpg",pImgb);*/
	if (pImgb && pImgg && pImgr)
	{
		cvMerge(pImgb, pImgg, pImgr, 0, image);
		cvReleaseImage(&pImgr);
		pImgr = NULL;
		cvReleaseImage(&pImgg);
		pImgg = NULL;
		cvReleaseImage(&pImgb);
		pImgb = NULL;
	}
	else if (pImgr)
	{
		image = pImgr;
	}
}

bool CBrightnessBalance::BrightnessBalance(IplImage * srcImage)
{
	isDoc = true;

	//初始化资源
	Global_Init(cvGetSize(srcImage),srcImage->depth);
	ImageBackFilter(srcImage);				//核心算法部分
	
	//Global_Dispose();
	return true;
}


//黑白文档图像背景移除(分块法进行处理):nSize >=3 && src.rows >3 && src.cols >3
//nSize:图像分块的大小
//[in] preproccessType
// 0 不做预处理
// 1 log
// 2 root
void CBrightnessBalance::AdaptiveMakeBeautiful(Mat &src0,int nBoxSize,int preproccessType)
{
	Mat src;
	switch(preproccessType)
	{
	case 1:
		src = CHistogram::HistogramLog(src0);
		break;

	case 2:
		src = CHistogram::HistogramRoot(src0);
		break;

	default:
		src = src0.clone();
		break;
	}

	Mat dst;
	int nColorImage =src.channels();
	//原图设置为灰色图
	if(src.channels() ==3)
	{
		cvtColor(src,dst,COLOR_RGB2GRAY);
	}
	else
		dst = src.clone();

	//卷积
	//全局阈值
	medianBlur(dst ,dst , 3);
	Point pt = CBrightnessBalance::findMaxThreshold1(dst,0.2,0.2);
	int nglobalThreshold = pt.x;
	Mat mask = CBrightnessBalance::im2bw(dst);
	

	/*namedWindow("1",WINDOW_NORMAL);
	imshow("1",mask);
	waitKey(0);*/

	//计算分块多少
	int mw = (src.cols + nBoxSize - 1)/nBoxSize;
	int mh = (src.rows + nBoxSize - 1)/nBoxSize;

	//前一步的计算值
	int nThresLeft;
	vector<int> vThresArr;

	//Mask图
	Mat matThr;
	matThr.create(mh,mw,CV_8UC1);
	uchar *dataThr = matThr.ptr<uchar>(0);

	//遍历每个区块
	for (int idr=0;idr <mh;idr++)
	{
		for (int idc =0;idc <mw;idc++/*,dataThr++,dataIn++*/,dataThr++)
		{
			//分块区域
			Rect rect;
			rect.x = idc*nBoxSize;
			rect.y = idr*nBoxSize;
			rect.width = nBoxSize; 
			rect.height = nBoxSize;

			//边缘分块
			if(idr == mh -1)
				rect.height = src.rows -1 - rect.y;

			if(idc == mw -1)
				rect.width = src.cols -1 - rect.x;

			//分块区域
			Mat roiImg = dst(rect);
			Mat matRoi = dst(rect).clone();
			Mat matgrad = mask(rect).clone();

#if 0
			//卷积
			float fdif = 1.0/10.0;
			Mat kern33 = (Mat_<float>(3,3) << fdif, fdif ,fdif,
				fdif, fdif, fdif, 
				fdif, fdif, fdif);

			//增强
			Mat matRoiCon;
			filter2D(matRoi,matRoiCon,-1,kern33);
			addWeighted(matRoi,0.5,matRoiCon,0.5,0,matRoi);
			matRoiCon.release();
#endif

#if 1
			//局部阈值
			Point pt = CBrightnessBalance::findMaxThreshold1(matRoi,0.2,0.2);
			Point ptOri = pt;
			//对局部阈值进行适当调整
			pt.x += (nglobalThreshold - pt.x)/3;
			pt.x = pt.x > nglobalThreshold ? nglobalThreshold : pt.x;

			int nThreshold = pt.x;

			//初始值
			if(idr ==0 )
				vThresArr.push_back(nThreshold);
			if(0 == idc)
				nThresLeft = nThreshold;
			int nThresholdTmp = nThreshold;

			//阈值同左侧，上侧加权求和
			if(idc >0 && idr >0)
				nThreshold = nThreshold + (nThresLeft-nThreshold)/2 +(vThresArr[idc-1] -nThreshold)/2;

			//前一步的值
			nThresLeft = nThresholdTmp;
			vThresArr[idc] = nThresholdTmp;

			//查询表
			uchar bookCheckR[256];

			//调整系数
			float fratio =0.2;
			fratio += 0.6*((float)nglobalThreshold -(float)abs(pt.y -pt.x))/(float)nglobalThreshold * 
				((float)pt.x/(float)nglobalThreshold) *
				((float)pt.x/(float)nglobalThreshold)
				;

			//float fratio0 = (float)max(ptOri.x - ptOri.y,pt.x/2)/(float)nglobalThreshold;

			float fratio1 =1.0;
			if( 0 == idc || mw-1 == idc)
				fratio1 =0.8;
			else if( 1 == idc || mw -2 == idc)
				fratio1 =0.9;
			
			//fratio = 1.0 -fratio ;

			//*dataThr = nThreshold*(float)(1.0 -fratio0)*fratio1;
			double tt = mean(matgrad)[0];
			double th = mean(matRoi)[0];
			if (tt>2.5)
				*dataThr = 0.9*nThreshold*fratio*fratio1;
			else
			{
				if (th < 100)
					*dataThr = nThreshold*fratio*fratio1*1.2;
				else
					*dataThr = nThreshold*fratio*fratio1*0.8;
				
			}
#else
			
			double tt = mean(matgrad)[0];
			double th = mean(matRoi)[0];
			if (tt>40)
				*dataThr = int(th);
			else
			{
				if (th>mean_v)
					*dataThr = 0.8*th;
				else
					*dataThr = 0.8*th;
			}
			
			/*if (pt.y>max(160,int(0.8*nglobalThreshold_d)))
			{
				*dataThr = int(0.8*avg);//>128?int(0.8*aa):128;
			}
			else
			{
				*dataThr = int(avg);
			}*/
#endif
		}
	}

	resize(matThr,matThr,dst.size());

	//blur(dst ,matThr, Size(64,64));

	
	//temp 
	//imwrite("C:\\Users\\Xue.Bicycle\\Desktop\\test0.bmp",matThr);

	vector<vector<uchar> > vvBookCheck;
	vector<uchar> vBookCheck;

	for (int idthr =0; idthr <256;idthr ++)
	{
		vBookCheck.clear();
		for (int idx =0;idx <256;idx++)
		{
			if(idx > idthr )
				vBookCheck.push_back(saturate_cast<uchar>( (255.0)/ (1.0+pow((float)2.518,(float)(-1.0*((float)idx -idthr)/10)))));	
			else
			{
				int nTmp = saturate_cast<uchar>( (255.0)/ (1.0+pow((float)1.02,(float)(-1.0*((float)idx -idthr)*2.0))));
				vBookCheck.push_back(idx - (idx - nTmp)*2/3);	
			}
		}
		vvBookCheck.push_back(vBookCheck);
	}
#if 0
	uchar* data = dst.ptr<uchar>(0);
	dataThr = matThr.ptr<uchar>(0);
	for (int idr =0; idr < dst.rows; idr++)
	{
		for (int idc =0; idc <dst.cols ;idc++,data++,dataThr++)
		{
			*data = vvBookCheck[*dataThr][*data];
		}
	}
#else
	uchar* dataIn = src.ptr<uchar>(0);
	uchar* data = src0.ptr<uchar>(0);
	dataThr = matThr.ptr<uchar>(0);
	for (int idr =0; idr < dst.rows; idr++)
	{
		for (int idc =0; idc <dst.cols ;idc++,data+=nColorImage,dataIn+=nColorImage,dataThr++)
		{
			uchar uValue =vvBookCheck[*dataThr][*dataIn];
			for(int idx=0;idx < nColorImage;idx++)
				*(data+idx) = uValue;
		}
	}
	
	
#endif
}


//计算图像sobel梯度
//[in]src			输入图像

Mat CBrightnessBalance::im2bw(const Mat src)
{
	Mat gray;
	if (src.channels() == 3)
	{
		cvtColor(src, gray, CV_BGR2GRAY);
	}
	else
	{
		gray = src.clone();
	}

	resize(gray,gray,Size(int(gray.cols*1000/gray.rows),1000));

	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	Mat grad;
	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;
	/// 求 X方向梯度
	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	Sobel(gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);

	/// 求Y方向梯度
	//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
	Sobel(gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(grad_y, abs_grad_y);

	/// 合并梯度(近似)
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);
	Mat bw = grad>(mean(grad)[0]*1.8>60?mean(grad)[0]*1.8:60);

	int width = bw.cols;
	int height = bw.rows;
	Mat parent = Mat::zeros(height,width,CV_8UC1); 
	if (true)
	{
		
		int p = 0;
		vector<Point> vPoint;
		Point p2t;
		
		for(int i =1;i<height-1;i++)
		{
			for (int j =1;j<width-1;j++)
			{
				p = 0;
				vPoint.clear();
				if(bw.ptr<uchar>(i)[j]!=0)
				{
					p2t.x = j;
					p2t.y = i;
					vPoint.push_back(p2t);
					p = p+1;
					parent.ptr<uchar>(i)[j] = 255;
					bw.ptr<uchar>(i)[j]=0;
					int num=0;
					while(num<p)
					{
						for(int y = vPoint[num].y-1;y<=vPoint[num].y+1;y++)
						{
							if (y<0 || y>height-1)
								continue;
							for(int x = vPoint[num].x-1;x<=vPoint[num].x+1;x++)
							{
								if (x<0 || x>width-1)
									continue;
								if(bw.ptr<uchar>(y)[x]!=0)
								{
									p2t.x = x;
									p2t.y = y;
									vPoint.push_back(p2t);
									parent.ptr<uchar>(y)[x] = 255;
									bw.ptr<uchar>(y)[x] =0;
									p = p+1;
								}
							}
					   }
						num = num+1;
					}
				}
				if (p<30)
				{
				 for (int x = 0;x<p;x++)
				 {
					parent.ptr<uchar>(vPoint[x].y)[vPoint[x].x] = 0;
				 }
				}
			}
		}
	}
	resize(parent,parent,Size(src.cols,src.rows));
	return parent;
}


//计算图像波峰个数及最佳阈值
//[in]src			输入图像

int CBrightnessBalance::OptimalThreshold(const Mat src)
{
	int valueArr[256]={0};

	const uchar *data1 = src.ptr<uchar>(0);

	for (int idr=0;idr < src.rows;idr++)
	{
		for (int idc=0;idc <src.cols;idc++,data1++)
		{
			valueArr[*data1]++;
		}
	}
	for (int idx=254;idx > 1;idx--)
	{
		valueArr[idx] = (valueArr[idx]+valueArr[idx-1]+valueArr[idx+1])/3;
	}
	
	int index = 0;
	int th = 0;
	for(int idx = 0;idx<256;idx++)
	{
		if (valueArr[idx]>th)
		{
			th = valueArr[idx];
			index = idx;
		}
	}

	if (index>mean(src)[0])
	{
		int tt = valueArr[index] ;
		for (int idx=index-1;idx > 0;idx--)
		{
			if (valueArr[idx] <tt)
			{
				tt = valueArr[idx] ;
				index = idx;
			}
			else
			{
				break;
			}
		}
		int index1 = index;
		int th = valueArr[index];
		for(int idx = index-1;idx>0;idx--)
		{
			if (valueArr[idx]>th && valueArr[idx]>3*valueArr[index])
			{
				th = valueArr[idx];
				index1 = idx;
			}
		}
	}
	else
	{
		int tt = valueArr[index] ;
		for (int idx=index+1;idx < 256;idx++)
		{
			if (valueArr[idx] <tt)
			{
				tt = valueArr[idx] ;
				index = idx;
			}
			else
			{
				break;
			}

		}
		int index1 = index;
		int th = valueArr[index];
		for(int idx = index+1;idx<256;idx++)
		{
			if (valueArr[idx]>th && valueArr[idx]>3*valueArr[index])
			{
				th = valueArr[idx];
				index1 = idx;
			}
		}
	}
	return index;
}

//计算图像中的亮点位置(private)
//[in]src			输入图像
//[in]fratioTop		比例
Point CBrightnessBalance::findMaxThreshold1(const Mat &src,float fratioTop /* = 0.3 */,float fratioBot /* = 0.3*/)
{
	Point pt;
	pt.x = 128;
	pt.y = 128;
	int valueArr[256]={0};

	const uchar *data = src.ptr<uchar>(0);

	for (int idr=0;idr < src.rows;idr++)
	{
		for (int idc=0;idc <src.cols;idc++,data++)
		{
			valueArr[*data]++;
		}
	}

	long lSum =0;
	int nThres = src.rows*src.cols*fratioTop;
	for (int idx=255;idx > 0;idx--)
	{
		lSum += valueArr[idx];
		if(lSum >nThres)
		{
			pt.x = idx;
			break;
		}
	}

	lSum =0;
	nThres = src.rows*src.cols*fratioBot;
	for (int idx=0;idx < 255;idx++)
	{
		lSum += valueArr[idx];
		if(lSum >nThres)
		{
			pt.y = idx;
			break;
		}
	}

	return pt;
}
pair<int,int> CBrightnessBalance::findMaxThreshold(const Mat &src,float fratioTop /* = 0.3 */,float fratioBot /* = 0.3*/)
{
	pair<int,int> pt;
	pt.first = 128;
	pt.second = 128;
	int valueArr[256]={0};

	const uchar *data = src.ptr<uchar>(0);

	for (int idr=0;idr < src.rows;idr++)
	{
		for (int idc=0;idc <src.cols;idc++,data++)
		{
			valueArr[*data]++;
		}
	}

	long lSum =0;
	int nThres = src.rows*src.cols*fratioTop;
	for (int idx=255;idx > 0;idx--)
	{
		lSum += valueArr[idx];
		if(lSum >nThres)
		{
			pt.first = idx;
			break;
		}
	}

	lSum =0;
	for (int idx=0;idx < 255;idx++)
	{
		lSum += valueArr[idx];
		if(lSum >nThres)
		{
			pt.second = idx;
			break;
		}
	}

	return pt;
}
//////////////////////////////////////////////////////////////////////////

//图像文档化/背景移除/整体提亮算法
//[in/out]src			输入输出图像
//[in]nSize				分块尺寸的大小
//[in]preProccessType	预处理的选择
//	0	不做处理
//	1	log
//	2	root
void CBrightnessBalance::movingLevel(Mat &src,int nSize,int preProccessType)
{
	//预处理
	Mat gray = CBrightnessBalance::preProcess(src,preProccessType);

	//计算模板
	Mat mask = CBrightnessBalance::createMask(gray,nSize);
	
	//调整模板获查找表
	Mat matThr;
	vector<vector<uchar> > colorTable = CBrightnessBalance::getColorTable(mask,src,matThr);

	//调整替换图像
	CBrightnessBalance::adjustImage(src,matThr,colorTable);
}


//预处理
//[in/out]src			输入/输出图像(log/root操作会改变原图)
//[in]preProccessType	预处理的选择
//	0	不做处理
//	1	log
//	2	root
//返回值：Mat --> 相应的灰度图像
Mat CBrightnessBalance::preProcess(Mat &src,int preProccessType)
{
	//选择预处理模式
	switch(preProccessType)
	{
	case 1:
		src = CHistogram::HistogramLog(src);
		break;

	case 2:
		src = CHistogram::HistogramRoot(src);
		break;

	default:
		break;
	}

	Mat gray;
	//原图设置为灰色图
	if(src.channels() ==3)
	{
		cvtColor(src,gray,COLOR_RGB2GRAY);
	}
	else
		gray = src.clone();

	return gray;
}

//获取阈值模板
//[in]src			输入图像
//[in]nSize			分块尺寸的大小
//返回值:阈值模板图像
Mat CBrightnessBalance::createMask(Mat gray,int nSize)
{
	//全局阈值
	extremeValue twoThrValue = CBrightnessBalance::findMaxThreshold(gray);
	int nglobalThreshold = twoThrValue.first;

	//计算分块多少
	int mw = (gray.cols + nSize - 1)/nSize;
	int mh = (gray.rows + nSize - 1)/nSize;

	//前一步的计算值
	int nThresLeft;
	vector<int> vThresArr;

	//Mask图
	Mat matThr;
	matThr.create(mh,mw,CV_8UC1);
	uchar *dataThr = matThr.ptr<uchar>(0);

	//遍历每个区块
	for (int idr=0;idr <mh;idr++)
	{
		for (int idc =0;idc <mw;idc++,dataThr++)
		{
			//分块区域
			Rect rect;
			rect.x = idc*nSize;
			rect.y = idr*nSize;
			rect.width = nSize; 
			rect.height = nSize;

			//边缘分块
			if(idr == mh -1)
				rect.height = gray.rows -1 - rect.y;

			if(idc == mw -1)
				rect.width = gray.cols -1 - rect.x;

			//分块区域
			Mat roiImg = gray(rect);
			Mat matRoi = gray(rect).clone();

			//局部阈值
			twoThrValue = CBrightnessBalance::findMaxThreshold(matRoi,0.2,0.1);
			//对局部阈值进行适当调整
			twoThrValue.first += (nglobalThreshold - twoThrValue.first)/3;
			twoThrValue.first = twoThrValue.first > nglobalThreshold ? nglobalThreshold:twoThrValue.first;
			
			int nThreshold = twoThrValue.first;

			//初始值
			if(idr ==0 )
				vThresArr.push_back(nThreshold);
			if(0 == idc)
				nThresLeft = nThreshold;
			int nThresholdTmp = nThreshold;

			//阈值同左侧，上侧加权求和
			if(idc >0 && idr >0)
				nThreshold = nThreshold + (nThresLeft-nThreshold)/2 +(vThresArr[idc-1] -nThreshold)/2;

			//前一步的值
			nThresLeft = nThresholdTmp;
			vThresArr[idc] = nThresholdTmp;

			//调整系数
			float fratio =0.2;
			fratio += 0.6*((float)nglobalThreshold -(float)abs(twoThrValue.second -twoThrValue.first))/(float)nglobalThreshold * 
				((float)twoThrValue.first/(float)nglobalThreshold) *
				((float)twoThrValue.first/(float)nglobalThreshold)
				;

			float fratio1 =1.0;
			if( 0 == idc || mw-1 == idc)
				fratio1 =0.8;
			else if( 1 == idc || mw -2 == idc)
				fratio1 =0.9;
			
			*dataThr = nThreshold*fratio*fratio1;
		}
	}

	resize(matThr,matThr,gray.size());

	return matThr;
}

//获取查找表，和调整后的模板
//[in]src			输入模板
//[out]dst			返回调整后的模板
//返回值：查找表
vector<vector<uchar> > CBrightnessBalance::getColorTable(const Mat &mask,const Mat &src,Mat &dst)
{
	//还原到原始尺寸
	resize(mask,dst,src.size());

	//创建查找表
	vector<vector<uchar> > vvBookCheck;
	vector<uchar> vBookCheck;


	for (int idthr =0; idthr <256;idthr ++)
	{
		vBookCheck.clear();
		for (int idx =0;idx <256;idx++)
		{
			if(idx > idthr )
				//这里主要是sigmod函数的应用，将大于阈值的地方尽快拉伸到255(白色)
				vBookCheck.push_back(saturate_cast<uchar>( (255.0)/ (1.0+pow((float)2.518,(float)(-1.0*((float)idx -idthr)/10)))));	
			else
			{
				//低于阈值的部分，采用不同的sigmod函数。
				int nTmp = saturate_cast<uchar>( (255.0)/ (1.0+pow((float)1.02,(float)(-1.0*((float)idx -idthr)*2.0))));
				//较小低于阈值部分向下拉伸的程度，保证了文字边缘部分不会变化的特别突兀。
				vBookCheck.push_back(idx - (idx - nTmp)*2/3);	
			}
		}
		vvBookCheck.push_back(vBookCheck);
	}

	return vvBookCheck;
}

//调整替换图像
//[in/out]src		输入/输出图像
//[in]matThr		控制模板
void CBrightnessBalance::adjustImage(Mat &src,const Mat &matThr,vector<vector<uchar> > colorTable)
{
	//替换原图
	uchar* data = src.ptr<uchar>(0);
	const uchar* dataThr = matThr.ptr<uchar>(0);
	
	//根据colorTable对原图进行替换，首先遍历过程中根据模板mask对应的值，即为当前部分的区分
	//阈值，根据区分阈值。调用其对应的调整表colorTable[*dataThr]。根据原图中的值再调用其
	//调整后应该替换的值colorTable[*dataThr][*data]。
	for (int idr =0; idr < src.rows; idr++)
	{
		for (int idc =0; idc <src.cols ;idc++,data += src.channels(),dataThr++)
		{
			uchar uValue =colorTable[*dataThr][*data];
			for(int idx=0;idx < src.channels();idx++)
				*(data+idx) = uValue;
		}
	}
}

//获取阈值
//[in]pt				图像上下阈值
//[in]leftThreshold		左侧阈值
//[in]topThreshold		上侧阈值
#if 0
int CBrightnessBalance::getThresholds(Point pt,Point loc,Size maskSize,int nglobalThreshold,int nThresLeft,int topThreshold)
{
	//对局部阈值进行适当调整
	pt.x += (nglobalThreshold - pt.x)/3;
	int nThreshold = pt.x;
	int nThresholdTmp = nThreshold;

	//阈值同左侧，上侧加权求和
	if(idc >0 && idr >0)
		nThreshold = nThreshold + (nThresLeft-nThreshold)/2 +(topThreshold-nThreshold)/2;

	//前一步的值
	nThresLeft = nThresholdTmp;
	vThresArr[idc] = nThresholdTmp;

	//调整系数
	float fratio =0.2;
	fratio += 0.6*((float)nglobalThreshold -(float)abs(pt.y -pt.x))/(float)nglobalThreshold * 
		((float)pt.x/(float)nglobalThreshold) *
		((float)pt.x/(float)nglobalThreshold)
		;

	float fratio1 =1.0;
	if( 0 == idc || mw-1 == idc)
		fratio1 =0.8;
	else if( 1 == idc || mw -2 == idc)
		fratio1 =0.9;

	int nThreshold = nThreshold*fratio*fratio1;

	return nThreshold;
}
#endif