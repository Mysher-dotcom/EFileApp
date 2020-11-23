#include "stdafx.h"
#include "newColorEnhance.h"
#include "BrightnessBalance.h"
#define TPYE 1

//************************************************************************  
// 函数名称:    textEnhance
// 函数说明:    黑白文档增强算法
// 函数参数:    Mat src    输入RGB原图像
// 返 回 值:	Mat	   处理后图像
//************************************************************************ 
Mat  CnewColorEnhance::textEnhance(cv::Mat &src)
{
	if (src.empty())
	{
		return src;
	}

	/*if(src.channels() == 3)
	{
		cvtColor(src, src, CV_BGR2GRAY);
	}*/
	Mat temp,temp1;
	if (src.channels()==3)
	{
		if(1)
		{
			vector<Mat> channels(3);        //分离通道；
			split(src, channels);

			Mat temp;
			if (max(src.rows,src.cols)>3000)
				resize(src,temp,Size(0,0),0.3,0.3,1);
			else if(max(src.rows,src.cols)>1500)
				resize(src,temp,Size(0,0),0.5,0.5,1);
			else 
				temp = src.clone();
			vector<float> dst = findThreshold(temp,200);
			//vector<float> dst = findThreshold(src,200);
			if (dst.size()==3)
			{
				//float mean = min(dst[0],min(dst[1],dst[2]));
				float mean = (dst[0]+dst[1]+dst[2])/3;
				channels[0] = channels[0]*(mean/dst[0]);
				channels[1] = channels[1]*(mean/dst[1]);
				channels[2] = channels[2]*(mean/dst[2]);
				
				
				merge(channels, src);
				//split(src, channels);
			}
		}
		//return src;
		if (1)
		{
			Mat tmp = src.clone();
			bilateralFilter(tmp, src, 10, 30, 30);
		}
		//guidedFilter(src,src,5,0.1);
		
		cvtColor(src, src, CV_BGR2HSV);
		vector<cv::Mat> rgbChannels(3); 
		split(src, rgbChannels);
		
		Mat kernel = Mat::zeros(25, 25, CV_32F);
		int count = 0;
		for (int y = 0;y<kernel.rows;y++)
		{
			for (int x = 0;x<kernel.cols;x++)
			{
				if (x == 13 || y==13 || x==y || 24-y==x)
				{
					kernel.ptr<float>(y)[x] = 1.0/95;
					count += 1;
				}
			}
		}

		//rgbChannels[1] /= 1.2;
		//rgbChannels[2] /= 1.1;
		rgbChannels[1] *= 1.1;
		for(int ii = 2;ii<3;ii++)
		{
			Mat gray = rgbChannels[ii];
#if TPYE
			filter2D(gray, temp, -1, kernel);
			//blur(temp,temp,Size(5,5));
			Mat mask = (temp-5>gray);

			uchar* data = gray.ptr<uchar>(0);
			uchar* data1 = temp.ptr<uchar>(0);
			uchar* data2 = mask.ptr<uchar>(0);
			//uchar* data3 = gray6.ptr<uchar>(0);
			int pix_max = 0;
			int pix_min = 255;
			for (int y = 0;y<gray.rows;y++)
			{
				for (int x = 0;x<gray.cols;x++,data++,data1++,data2++)
				{
					

					if (*data2>0)
					{
						float tmp = 1.5*(float(*data)-float(*data1));
						/*if (tmp >10)
							tmp = 10;
						if (tmp<-15)
						{	
							tmp  = -15;
							if (*data>100)
								tmp  = -10;
						}*/
						*data = saturate_cast<uchar>(float(*data) + tmp);
					}
					else
					{
						if (*data>100 && *data<235)
							*data = saturate_cast<uchar>(*data+10);//*data-float(*data-*data1);
						else if(*data>=235 && *data<=245)
							*data = 245;//saturate_cast<uchar>(int(*data)*2-245);//*data-float(*data-*data1);
						else if(*data>=90 && *data<=100)
							*data = saturate_cast<uchar>(int(*data)*2-90);//*data-float(*data-*data1);
					}
				}
			}
#else
			//GaussianBlur(gray, temp, Size(0,0), 10);
			blur(gray,temp,Size(7,7));
			//filter2D(gray, temp, -1, kernel);
			addWeighted(gray, 1.6, temp, -0.6, 0, gray);
#endif
			rgbChannels[ii] = gray.clone();
		}
		//temp1 = temp.clone();
		merge(rgbChannels, temp1);
		cvtColor(temp1, src, CV_HSV2BGR);
	}
	else
	{
		Mat kernel = Mat::zeros(51, 51, CV_32F);
		int count = 0;
		for (int y = 0;y<kernel.rows;y++)
		{
			for (int x = 0;x<kernel.cols;x++)
			{
				if (x == 25 || y==25 || x==y || 50-y==x)
				{
					kernel.ptr<float>(y)[x] = 1.0/201;
					count += 1;
				}
			}
		}

		Mat gray = src.clone();
		filter2D(gray, temp, -1, kernel);
		//blur(src,temp,Size(60,1));
		//blur(src,temp1,Size(1,60));
		/// 合并梯度(近似)
		//addWeighted(temp, 0.5, temp1, 0.5, 0, temp);
		//return temp;
		temp = ~(temp-8>gray);
		blur(temp,temp,Size(3,3));
		src = temp.clone();
	}

	
	return src;
	
}

//************************************************************************  
// 函数名称:    black_whiteEnhance
// 函数说明:    同态滤波彩色增强算法
// 函数参数:    Mat src    输入RGB原图像
// 返 回 值:	Mat	   处理后图像
//************************************************************************ 
 Mat CnewColorEnhance::black_whiteEnhance(cv::Mat src, bool type)
 {
	Mat dst;
	if (src.channels()==3)
	{
		cvtColor(src,src,CV_BGR2GRAY);
		if(!type)
		{
			Mat temp;
			blur(src,temp,Size(10,10));
			addWeighted(src, 2.0, temp, -1.0, 0, src);
		}
	}
	if (type)
	{
		int thresh = CnewColorEnhance::unevenLightCompensate(src,100);//图像亮度均化
		//return src;
#if 1

		float avg = 128;

		int th_max =255;
		int th_min = 0;
		float Bot = min(240.0,avg+50.0);
		float tt = Bot-avg;
		float Top = avg-tt;
		if (Top<50)
		{
			tt = 50-Top;
			Bot = Bot-tt;
			Top = 50;
		}

		//float Top = max(50.0,0.9*avg-60)>avg?avg:max(50.0,0.9*avg-60);
		float idthr = (Bot + Top)/2;
		float checkBook[256];
		for (int idx=0;idx<256;idx++)
		{
			if(idx<=Top)
				checkBook[idx] = th_min;//(float(idx) / (Top)) * ( th_min );
			else if (idx>=Bot)
			{
				if(true || Bot>245)
					checkBook[idx] = th_max;
				else
					checkBook[idx] = (float(idx - Bot) / (255 - Bot)) * ( 245 - Bot ) + Bot + 10;
			}
			else
			{
				if(true || Bot>245)
					checkBook[idx] = (float(idx - Top) / (Bot - Top)) * (th_max-th_min) + th_min;
				else
					checkBook[idx] = (float(idx - Top) / (Bot - Top)) * (Bot - th_min +10) + th_min;
			}
		
		}

		uchar *data = src.ptr<uchar>(0);
		for(int idr =0;idr<src.rows;idr++)
		{
			for(int idc=0;idc<src.cols;idc++)
			{
				*data = checkBook[*data];
				*data = checkBook[*data];
				data += 1;
			}
		}
		blur(src,src,Size(3,3));
#endif
	}
	//return src;

	if (true)
	{
		Mat mean_1;
		Mat mask = im2bw(src,mean_1);

		Mat mean_Mat = src.clone();

		float fRiao = mean(mask)[0];
		float th = mean(src)[0];
		if (fRiao>20)
			th = min(220.0,0.9*th*255/(255-fRiao));
		mean_Mat.setTo(th>128?th:128,mask);
		
		mask = mean_1<src;
		absdiff(mean_Mat,src, src);
		//imsubtract(mean_Mat,src);
		src.setTo(0,mask);
		src = ~src;
	}

	long prob[256]={0};
	for(int y=0;y<src.rows;y++)
	{
		for(int x=0;x<src.cols;x++)
		{
			prob[src.at<uchar>(y,x)]++;
		}
	}

	//map参数计算
	float fRadioBottom = 0.016;//0.016
	float fRadioTop = 0.30;
	int nThBottom = src.rows * src.cols *src.channels() * fRadioBottom;
	int nThTop  = src.rows * src.cols * src.channels() * fRadioTop;
	long sumacc=0;
	int T_bottom = -1;
	int T_top = -1;
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


	T_top = 245;
	T_bottom=80;
	//生成map
	uchar map[256];

	for(int i=0;i<256;i++)
	{
		if(i<=T_bottom)
		{
			map[i]=0;
		}
		else if(i<T_top)
		{
			//map[i]=(uchar)(( (i-T_bottom)*1.0/(T_top-T_bottom))*255);
			map[i] =saturate_cast<uchar>(255.0/(1.0+pow(float(2.714),float(0.06*(T_top-80-i)))));
			//map[i]=(uchar)(sqrt( (i-100)*1.0/(255-100))*255);
			//map[i] =i;
		}
		else
		{
			map[i]=255;
		}
	}
	//映射
	for(int y=0;y<src.rows;y++)
	{
		for(int x=0;x<src.cols;x++)
		{
			src.at<uchar>(y,x) =map[src.at<uchar>(y,x)];
		}
	}
	return src;
 }


//计算图像sobel梯度
//[in]src			输入图像
Mat CnewColorEnhance::im2bw(Mat src,Mat &mean_1)
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
	if (min(gray.cols,gray.rows)>1500)
	{
		int height1 = gray.rows*0.5;
		resize(gray,gray,Size(int(gray.cols*height1/gray.rows),height1));
	}


#if 0
	int aa = 100;//mean(src)[0]*0.36;
	Mat mask = gray<(aa>100?aa:100);
#else
	double avg = mean(gray)[0];
	Mat avg_1;
	int len = 4;
	resize(gray,avg_1,Size(len,len));
	for (int ii = 0;ii<len;ii++)
	{
		for (int jj = 0;jj<len;jj++)
		{
			Rect rrect = Rect(ii*gray.cols/len,jj*gray.rows/len,(ii+1)*gray.cols/len,(jj+1)*gray.rows/len);
			if (rrect.x+rrect.width>gray.cols)
			{
				rrect.width = gray.cols-rrect.x;
			}
			if (rrect.y+rrect.height>gray.rows)
			{
				rrect.height = gray.rows-rrect.y;
			}
			Mat temp = gray(rrect);
			avg_1.ptr<uchar>(ii)[jj] = mean(temp)[0];
		}
	}

	int wd = 20;
	int he = 20;
	Mat mean_mask = Mat::zeros(Size(wd,he),CV_8UC1);
	for (int ii = 0;ii<wd;ii++)
	{
		for (int jj = 0;jj<he;jj++)
		{
			Rect rrect = Rect(ii*gray.cols/wd,jj*gray.rows/he,gray.cols/wd,gray.rows/he);
			
			if (rrect.x+rrect.width>gray.cols)
			{
				rrect.width = gray.cols-rrect.x;
			}
			if (rrect.y+rrect.height>gray.rows)
			{
				rrect.height = gray.rows-rrect.y;
			}
			Mat temp = gray(rrect);
			Mat mat_mean, mat_stddev;
			meanStdDev(temp,mat_mean,mat_stddev);
			double th = mat_mean.at<double>(0,0);
			double aa2 = mat_stddev.at<double>(0,0);
			float th2 = 1;

			if (aa2<30 || th>150)
			{
				th2 = 4;
				if (th<80)
				{
					th = 80;//min(100.0,3*th);
					
				}
				else
				{
					int tt = avg_1.ptr<uchar>(int(ii/5))[int(jj/5)];
					if (th<tt)
						th = tt*0.8;
					else
						th = th*0.9;
				}
			}
			else if(th<80)
			{
				th2 = 120/aa2>1?120/aa2:1;
				th = 80;//min(100.0,3*th);
			}
			else
			{
				th2 = 120/aa2>1?120/aa2:1;
				th = 0.9*th;
			}

			mean_mask.at<uchar>(jj,ii) = th;

		}
	}
	resize(mean_mask,mean_mask,gray.size());
	Mat mask = gray<(mean_mask*0.95);
#endif

	Mat temp;
	if (0)
	{
		Mat kernel = Mat::zeros(51, 51, CV_32F);
		int count = 0;
		for (int y = 0;y<kernel.rows;y++)
		{
			for (int x = 0;x<kernel.cols;x++)
			{
				if (x == 25 || y==25 || x==y || 50-y==x)
				{
					kernel.ptr<float>(y)[x] = 1.0/201;
					count += 1;
				}
			}
		}

		filter2D(gray, temp, -1, kernel);
	}
	else
	{
		blur(gray,temp,Size(60,60));
		blur(temp,temp,Size(60,60));
	}
	temp = gray<temp.mul(0.95);
	
	bitwise_or(temp,mask,mask);

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
	temp = grad>(mean(grad)[0]*1.5>50?mean(grad)[0]*1.5:50);
	
	bitwise_or(temp,mask,mask);

	int ith = max(60,int(min(temp.rows,temp.cols)/15));
	rectangle(temp, Rect(0,0,temp.cols-1,temp.rows-1), Scalar(0),ith);
	//line(temp,Point(temp.cols,0),Point(temp.cols,temp.rows),Scalar(0),int(temp.cols/7));
	//line(temp,Point(0,0),Point(0,temp.rows),Scalar(0),int(temp.cols/7));


#if 1
	// 提取连通区域，并剔除小面积联通区域
    /*std::vector<std::vector<cv::Point>> contours;
    cv::findContours(temp, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
    contours.erase(std::remove_if(contours.begin(), contours.end(), 
        [](const std::vector<cv::Point>& c){return cv::contourArea(c) < 5; }), contours.end());

    // 显示图像并保存
    Mat temp1 = Mat::zeros(temp.size(),CV_8UC1); 
    cv::drawContours(temp1, contours, -1, cv::Scalar(255), cv::FILLED);
	temp = temp1.clone();*/


	int width = temp.cols;
	int height = temp.rows;
	Mat parent = Mat::zeros(height,width,CV_8UC1); 
	if (true)
	{
		int count = 0;	
		int p = 0;
		vector<Point> vPoint;
		Point p2t;
		
		for(int i =1;i<height-1;i++)
		{
			for (int j =1;j<width-1;j++)
			{
				count = 0;
				p = 0;
				vPoint.clear();
				if(temp.ptr<uchar>(i)[j]!=0)
				{
					p2t.x = j;
					p2t.y = i;
					vPoint.push_back(p2t);
					p = p+1;
					parent.ptr<uchar>(i)[j] = 255;
					temp.ptr<uchar>(i)[j]=0;
					mask.ptr<uchar>(i)[j]=0;
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
								if(mask.ptr<uchar>(y)[x]!=0)
								{
									p2t.x = x;
									p2t.y = y;
									vPoint.push_back(p2t);
									parent.ptr<uchar>(y)[x] = 255;
									if (temp.ptr<uchar>(y)[x]>0)
									{
										temp.ptr<uchar>(y)[x] =0;
										count += 1;
									}
									mask.ptr<uchar>(y)[x] =0;
									p = p+1;
								}
							}
					   }
						num = num+1;
					}
				}

				if (p<10)
				{
				 for (int x = 0;x<p;x++)
				 {
					parent.ptr<uchar>(vPoint[x].y)[vPoint[x].x] = 0;
				 }
				}
				else if ((float(p)/count)>20 && count<50)
				{
				 for (int x = 0;x<p;x++)
				 {
					parent.ptr<uchar>(vPoint[x].y)[vPoint[x].x] = 0;
				 }
				}
			}
		}
	}
#endif
	//fillHole(parent, parent);
	resize(parent,parent,Size(src.cols,src.rows));
	resize(mean_mask,mean_1,Size(src.cols,src.rows));
	/*namedWindow("2",2);
	imshow("2",parent);
	waitKey(0);*/
	return parent;
}

int CnewColorEnhance::unevenLightCompensate(Mat &image, int blockSize)//图像亮度均化
{
	if (image.channels() == 3)
		cvtColor(image, image, CV_BGR2GRAY);

	double average = mean(image)[0];

#if 1
	Mat blockImage;
	if(0)
	{
		Mat kernel = Mat::zeros(51, 51, CV_32F);
		int count = 0;
		for (int y = 0;y<kernel.rows;y++)
		{
			for (int x = 0;x<kernel.cols;x++)
			{
				if (x == 25 || y==25 || x==y || 50-y==x)
				{
					kernel.ptr<float>(y)[x] = 1.0/201;
					count += 1;
				}
			}
		}

		filter2D(image, blockImage, -1, kernel);
	}
	else
	{
		blur(image, blockImage, Size(blockSize, blockSize));
	}
	Mat mask = blockImage<min(100.0,0.7*average);
	blockImage.setTo(150,mask);
	//blockImage.convertTo(blockImage, CV_32FC1);
#else
	int rows_new = ceil(double(image.rows) / double(blockSize));
	int cols_new = ceil(double(image.cols) / double(blockSize));
	Mat blockImage;
	blockImage = Mat::zeros(rows_new, cols_new, CV_32FC1);
	for (int i = 0; i < rows_new; i++)
	{
		for (int j = 0; j < cols_new; j++)
		{
			int rowmin = i * blockSize;
			int rowmax = (i + 1)*blockSize;
			if (rowmax > image.rows) rowmax = image.rows;
			int colmin = j * blockSize;
			int colmax = (j + 1)*blockSize;
			if (colmax > image.cols) colmax = image.cols;
			Mat imageROI = image(Range(rowmin, rowmax), Range(colmin, colmax));
			double temaver = mean(imageROI)[0];
			if (temaver>0.8*average)
				blockImage.at<float>(i, j) = temaver;
			else
				blockImage.at<float>(i, j) = 150;
		}
	}
#endif
	average = 150;
	//blockImage = blockImage - average;
	//Mat image2;
	//image.convertTo(image2, CV_32FC1);
#if 0
	Mat dst = image2 - blockImage;
	dst.convertTo(image, CV_8UC1);
#else
	uchar *data = image.ptr<uchar>(0);
	uchar *data1 = blockImage.ptr<uchar>(0);
	for (int idr = 0; idr<blockImage.rows; idr++)
	{
		for (int idc = 0; idc<blockImage.cols; idc++,data++,data1++)
		{
			*data = saturate_cast<uchar>(float(*data)-float(*data1)+average);
		}
	}
	//image2.convertTo(image, CV_8UC1);
#endif
	image.setTo(average,image>average);
	return int(average+0.5);
}

int CnewColorEnhance::unevenLightCompensate1(Mat &image, int blockSize)//带插图文档优化
{
	if (image.channels() == 3)
	{
		vector<Mat> channels(3);        //分离通道；
		split(image, channels);
		Mat temp;
		if (max(image.rows,image.cols)>3000)
			resize(image,temp,Size(0,0),0.3,0.3,1);
		else if(max(image.rows,image.cols)>1500)
			resize(image,temp,Size(0,0),0.5,0.5,1);
		else 
			temp = image.clone();
		vector<float> dst = findThreshold(temp,200);


		if (dst.size()==3)
		{
			/*float mean = (dst[0]+dst[1]+dst[2])/3;
			channels[0] = channels[0]*(mean/dst[0]);
			channels[1] = channels[1]*(mean/dst[1]);
			channels[2] = channels[2]*(mean/dst[2]);*/
			float aa = max(dst[0],max(dst[1],dst[2]));
			channels[0] = channels[0]+(aa-dst[0]);
			channels[1] = channels[1]+(aa-dst[1]);
			channels[2] = channels[2]+(aa-dst[2]);
			merge(channels, image);
			//split(src, channels);
		}

		Mat src = image.clone();
	
		if(1)
		{
			uchar* data = src.ptr<uchar>(0);
			//uchar* data1 = image.ptr<uchar>(0);
			for (int ii = 0;ii<image.rows;ii++)
			{
				for(int jj = 0;jj<image.cols;jj++,data+=3)
				{
					int aa = min(*(data+2),min(*data,*(data+1)));
					int aa1 = max(*(data+2),max(*data,*(data+1)));
					if (1.0*aa1/aa>1.15 || aa<80)
					{
						*data = 0;
						*(data+1) = 0;
						*(data+2)= 0;
						/*if (aa<100 && 1.0*aa1/aa<1.1)
						{
							*data1 *= 0.6;
							*(data1+1) *= 0.6;
							*(data1+2) *= 0.6;
						}*/
					}

					
				}
			}
			
		}

		vector<Mat> maskchannels(3);        //分离通道；
		split(src, maskchannels);
		Mat mask = maskchannels[0]>0;
		double avg1 = mean(mask)[0];
		double average = 128;

		if (avg1>0)
			average = max(128.0,255*min(mean(maskchannels[0])[0],min(mean(maskchannels[1])[0],mean(maskchannels[2])[0]))/avg1);

		for(int ii = 0;ii<3;ii++)
		{
			Mat im = channels[ii];

			Mat img = maskchannels[ii];

			int rows_new = ceil(double(img.rows) / double(blockSize));
			int cols_new = ceil(double(img.cols) / double(blockSize));
			Mat blockImage;
			blockImage = Mat::zeros(rows_new, cols_new, CV_8UC1);
			for (int i = 0; i < rows_new; i++)
			{
				for (int j = 0; j < cols_new; j++)
				{
					int rowmin = i * blockSize;
					int rowmax = (i + 1)*blockSize;
					if (rowmax > img.rows) rowmax = img.rows;
					int colmin = j * blockSize;
					int colmax = (j + 1)*blockSize;
					if (colmax > img.cols) colmax = img.cols;
					Mat imageROI = img(Range(rowmin, rowmax), Range(colmin, colmax));
					Mat mask1 = imageROI>0;
					double temaver = mean(imageROI)[0];
					double temaver1 = mean(mask1)[0];
					if (temaver1>10)
						temaver = temaver*255/temaver1;
					else
						temaver = average;
					blockImage.at<uchar>(i, j) = temaver;
				}
			}
			resize(blockImage,blockImage,img.size());

			uchar *data = im.ptr<uchar>(0);
			uchar *data1 = blockImage.ptr<uchar>(0);
			for (int idr = 0; idr<blockImage.rows; idr++)
			{
				for (int idc = 0; idc<blockImage.cols; idc++,data++,data1++)
				{
					*data = saturate_cast<uchar>(float(*data)-float(*data1)+average);
				}
			}
			channels[ii] = im;
			
		}
		merge(channels, image);

		//return 1;
		//cvtColor(image, image, CV_BGR2HSV);
		//split(image, channels);
		Mat gray = channels[2];
		
		float Top,Bot;
		Point pt_R = CBrightnessBalance::findMaxThreshold1(channels[2],0.1,0.1);
		Bot = pt_R.x;
		if (((Bot-average)<15 && avg1>20) || average>Bot || (avg1>180))
			Bot = average-5;//pt_R.x;

		Top = pt_R.y;

		float Ith = Bot;
		Bot = Bot>245?245:Bot;
	
		float th = 255;//Bot+20<240?240:255;
		Bot = Bot<150?150:Bot;
		float th_min = 0;//Bot-200;
	
		//Top = Top<30?min(min(pt_B.y,pt_G.y),pt_R.y):Top;
		Top = Top>30?30:Top;
	

		float idthr = (Bot + Top)/2;
		float checkBook[256];
		for (int idx=0;idx<256;idx++)
		{
			if(idx<=Top)
				checkBook[idx] = idx;//th_min;//(float(idx) / (Top)) * ( th_min );
			else if (idx>=Bot)
			{
				if(true || Bot>245)
					checkBook[idx] = th;
				else
					checkBook[idx] = (float(idx - Bot) / (255 - Bot)) * ( 245 - Bot ) + Bot + 10;
			}
			else
			{
				if(idx>idthr)
					checkBook[idx] = (float(idx - idthr) / (Bot - idthr)) * (th-idthr) + idthr;
				else
					checkBook[idx] = idx;//(float(idx - Top) / (Bot - Top)) * (Bot - th_min +10) + th_min;
			}
		
		}

		uchar *data = image.ptr<uchar>(0);
		for(int idr =0;idr<src.rows;idr++)
		{
			for(int idc=0;idc<src.cols;idc++)
			{
				*data = checkBook[*data];
				*(data+1) = checkBook[*(data+1)];
				*(data+2) = checkBook[*(data+2)];
				/**data = checkBook[*data];
				*(data+1) = checkBook[*(data+1)];
				*(data+2) = checkBook[*(data+2)];*/
				data += 3;
			}
		}
	}
	else
	{
		double average = mean(image)[0];

#if 1
		Mat blockImage;
		if(0)
		{
			Mat kernel = Mat::zeros(51, 51, CV_32F);
			int count = 0;
			for (int y = 0;y<kernel.rows;y++)
			{
				for (int x = 0;x<kernel.cols;x++)
				{
					if (x == 25 || y==25 || x==y || 50-y==x)
					{
						kernel.ptr<float>(y)[x] = 1.0/201;
						count += 1;
					}
				}
			}

			filter2D(image, blockImage, -1, kernel);
		}
		else
		{
			blur(image, blockImage, Size(blockSize, blockSize));
		}
		Mat mask = blockImage<min(100.0,0.7*average);
		blockImage.setTo(average,mask);
		//blockImage.convertTo(blockImage, CV_32FC1);
#else
		int rows_new = ceil(double(image.rows) / double(blockSize));
		int cols_new = ceil(double(image.cols) / double(blockSize));
		Mat blockImage;
		blockImage = Mat::zeros(rows_new, cols_new, CV_32FC1);
		for (int i = 0; i < rows_new; i++)
		{
			for (int j = 0; j < cols_new; j++)
			{
				int rowmin = i * blockSize;
				int rowmax = (i + 1)*blockSize;
				if (rowmax > image.rows) rowmax = image.rows;
				int colmin = j * blockSize;
				int colmax = (j + 1)*blockSize;
				if (colmax > image.cols) colmax = image.cols;
				Mat imageROI = image(Range(rowmin, rowmax), Range(colmin, colmax));
				double temaver = mean(imageROI)[0];
				if (temaver>0.8*average)
					blockImage.at<float>(i, j) = temaver;
				else
					blockImage.at<float>(i, j) = average;
			}
		}
#endif
		//average = 150;
		//blockImage = blockImage - average;
		//Mat image2;
		//image.convertTo(image2, CV_32FC1);
#if 0
		Mat dst = image2 - blockImage;
		dst.convertTo(image, CV_8UC1);
#else
		uchar *data = image.ptr<uchar>(0);
		uchar *data1 = blockImage.ptr<uchar>(0);
		for (int idr = 0; idr<blockImage.rows; idr++)
		{
			for (int idc = 0; idc<blockImage.cols; idc++,data++,data1++)
			{
				*data = saturate_cast<uchar>(float(*data)-float(*data1)+average);
			}
		}
		//image2.convertTo(image, CV_8UC1);
#endif
		image.setTo(average,image>average);
	}
	return 1;
}

int CnewColorEnhance::unevenLightCompensate2(Mat &image, int blockSize)//红印文档优化
{
	if (image.channels() == 3)
	{
		vector<Mat> channels(3);        //分离通道；
		split(image, channels);
		Mat temp;
		if (max(image.rows,image.cols)>3000)
			resize(image,temp,Size(0,0),0.3,0.3,1);
		else if(max(image.rows,image.cols)>1500)
			resize(image,temp,Size(0,0),0.5,0.5,1);
		else 
			temp = image.clone();
		vector<float> dst = findThreshold(temp,200);


		if (dst.size()==3)
		{
			/*float mean = (dst[0]+dst[1]+dst[2])/3;
			channels[0] = channels[0]*(mean/dst[0]);
			channels[1] = channels[1]*(mean/dst[1]);
			channels[2] = channels[2]*(mean/dst[2]);*/
			float aa = max(dst[0],max(dst[1],dst[2]));
			channels[0] = channels[0]+(aa-dst[0]);
			channels[1] = channels[1]+(aa-dst[1]);
			channels[2] = channels[2]+(aa-dst[2]);
			merge(channels, image);
			//split(src, channels);
		}

		Mat src = image.clone();
	
		if(1)
		{
			uchar* data = src.ptr<uchar>(0);
			//uchar* data1 = image.ptr<uchar>(0);
			for (int ii = 0;ii<image.rows;ii++)
			{
				for(int jj = 0;jj<image.cols;jj++,data+=3)
				{
					int aa = min(*(data+2),min(*data,*(data+1)));
					int aa1 = max(*(data+2),max(*data,*(data+1)));
					if (1.0*aa1/aa>1.15 || aa<80)
					{
						*data = 0;
						*(data+1) = 0;
						*(data+2)= 0;
						/*if (aa<100 && 1.0*aa1/aa<1.1)
						{
							*data1 *= 0.6;
							*(data1+1) *= 0.6;
							*(data1+2) *= 0.6;
						}*/
					}

					
				}
			}
			
		}

		vector<Mat> maskchannels(3);        //分离通道；
		split(src, maskchannels);
		Mat mask = maskchannels[0]>0;
		double avg1 = mean(mask)[0];
		double average = 128;

		if (avg1>0)
			average = max(128.0,255*min(mean(maskchannels[0])[0],min(mean(maskchannels[1])[0],mean(maskchannels[2])[0]))/avg1);

		for(int ii = 0;ii<3;ii++)
		{
			Mat im = channels[ii];

			Mat img = maskchannels[ii];

			int rows_new = ceil(double(img.rows) / double(blockSize));
			int cols_new = ceil(double(img.cols) / double(blockSize));
			Mat blockImage;
			blockImage = Mat::zeros(rows_new, cols_new, CV_8UC1);
			for (int i = 0; i < rows_new; i++)
			{
				for (int j = 0; j < cols_new; j++)
				{
					int rowmin = i * blockSize;
					int rowmax = (i + 1)*blockSize;
					if (rowmax > img.rows) rowmax = img.rows;
					int colmin = j * blockSize;
					int colmax = (j + 1)*blockSize;
					if (colmax > img.cols) colmax = img.cols;
					Mat imageROI = img(Range(rowmin, rowmax), Range(colmin, colmax));
					Mat mask1 = imageROI>0;
					double temaver = mean(imageROI)[0];
					double temaver1 = mean(mask1)[0];
					if (temaver1>10)
						temaver = temaver*255/temaver1;
					else
						temaver = average;
					blockImage.at<uchar>(i, j) = temaver;
				}
			}
			resize(blockImage,blockImage,img.size());

			uchar *data = im.ptr<uchar>(0);
			uchar *data1 = blockImage.ptr<uchar>(0);
			for (int idr = 0; idr<blockImage.rows; idr++)
			{
				for (int idc = 0; idc<blockImage.cols; idc++,data++,data1++)
				{
					*data = saturate_cast<uchar>(float(*data)-float(*data1)+average);
				}
			}
			channels[ii] = im;
			
		}
		merge(channels, image);

		//return 1;
		//cvtColor(image, image, CV_BGR2HSV);
		//split(image, channels);
		Mat gray = channels[2];
		
		float Top,Bot;
		Point pt_R = CBrightnessBalance::findMaxThreshold1(channels[2],0.1,0.05);
		Bot = pt_R.x;
		if (((Bot-average)<15 && avg1>20) || average>Bot || (avg1>180))
			Bot = average-5;//pt_R.x;

		Top = pt_R.y/2;

		float Ith = Bot;
		Bot = Bot>245?245:Bot;
	
		float th = 255;//Bot+20<240?240:255;
		Bot = Bot<150?150:Bot;
		float th_min = 0;//Bot-200;
	
		//Top = Top<30?min(min(pt_B.y,pt_G.y),pt_R.y):Top;
		Top = Top>80?80:Top;
		Top = Top<30?30:Top;
	

		float idthr = (Bot + Top)/2;
		float checkBook[256];
		for (int idx=0;idx<256;idx++)
		{
			if(idx<=Top)
				checkBook[idx] = 0;//idx;//th_min;//(float(idx) / (Top)) * ( th_min );
			else if (idx>=Bot)
			{
				if(true || Bot>245)
					checkBook[idx] = th;
				else
					checkBook[idx] = (float(idx - Bot) / (255 - Bot)) * ( 245 - Bot ) + Bot + 10;
			}
			else
			{
				if(false && idx>idthr)
					checkBook[idx] = (float(idx - idthr) / (Bot - idthr)) * (th-idthr) + idthr;
				else
					checkBook[idx] = (float(idx - Top) / (Bot - Top)) * th;
			}
		
		}

		uchar *data = image.ptr<uchar>(0);
		for(int idr =0;idr<src.rows;idr++)
		{
			for(int idc=0;idc<src.cols;idc++)
			{
				*data = checkBook[*data];
				*(data+1) = checkBook[*(data+1)];
				*(data+2) = checkBook[*(data+2)];
				/**data = checkBook[*data];
				*(data+1) = checkBook[*(data+1)];
				*(data+2) = checkBook[*(data+2)];*/
				data += 3;
			}
		}
	}
	return 1;
}

bool CnewColorEnhance::imsubtract(Mat src1, Mat &src2)
{
	if (src1.empty() || src2.empty())
		return false;

	uchar *data = src2.ptr<uchar>(0);
	uchar *data1 = src1.ptr<uchar>(0);
	for (int idr = 0; idr<src1.rows; idr++)
	{
		for (int idc = 0; idc<src1.cols; idc++,data++,data1++)
		{
			*data = saturate_cast<uchar>(float(*data1)-float(*data));
		}
	}
	return true;
}

void  CnewColorEnhance::fillHole(Mat srcBw, Mat &dstBw)
{
	Size m_Size = srcBw.size();
	Mat Temp=Mat::zeros(m_Size.height+2,m_Size.width+2,srcBw.type());//延展图像
	srcBw.copyTo(Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));
 
	cv::floodFill(Temp, Point(0, 0), Scalar(255));
 
	Mat cutImg;//裁剪延展的图像
	Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);
 
	dstBw = srcBw | (~cutImg);
}

//************************************************************************  
//Adaptive Contrast Enhancement（自适应对比度增强，ACE）
//函数功能：获取图像的局部均值与局部标准差的图
//函数名称：adaptContrastEnhancement
//函数参数：Mat &scr：输入图像，为三通道RGB图像；
//函数参数：Mat &dst：增强后的输出图像，为三通道RGB图像；
//函数参数：float fratioTop：比例；
//函数参数：float fratioBot：比例；
//返回类型：bool
//************************************************************************  

bool CnewColorEnhance::photoshopEnhancement(Mat &src, float fratioTop /* = 0.05 */,float fratioBot /*=0.05*/)
{
	if (!src.data || src.channels()!= 3)  //判断图像是否被正确读取；
	{
		cerr << "自适应对比度增强函数读入图片有误";
		return false;
	}
	CnewColorEnhance::unevenLightCompensate1(src,100);
	return true;
	Point pt_B,pt_G,pt_R;
	if (1)
	{
		vector<Mat> channels(3);        //分离通道；
		split(src, channels);

	
		/*vector<float> dst = findThreshold(src,200);
		if (dst.size()==3)
		{
			float mean = (dst[0]+dst[1]+dst[2])/3;
			channels[0] = channels[0]*(mean/dst[0]);
			channels[1] = channels[1]*(mean/dst[1]);
			channels[2] = channels[2]*(mean/dst[2]);

			merge(channels, src);
			split(src, channels);
		}*/

		pt_B = CBrightnessBalance::findMaxThreshold1(channels[0],fratioTop,fratioBot);
		pt_G = CBrightnessBalance::findMaxThreshold1(channels[1],fratioTop,fratioBot);
		pt_R = CBrightnessBalance::findMaxThreshold1(channels[2],fratioTop,fratioBot);
	}

	float Top,Bot;
	Bot = max(max(pt_R.x,pt_G.x),pt_B.x);
	Top = min(min(pt_R.y,pt_G.y),pt_B.y);

	/*
	pt_B = CBrightnessBalance::findMaxThreshold1(channels[0],0.1,0.05);
	pt_G = CBrightnessBalance::findMaxThreshold1(channels[1],0.1,0.05);
	pt_R = CBrightnessBalance::findMaxThreshold1(channels[2],0.1,0.1);
	*/
	float Ith = Bot;
	Bot = Bot>245?245:Bot;
	
	float th = 255;//Bot+20<240?240:255;
	Bot = Bot<150?150:Bot;
	float th_min = 0;//Bot-200;
	
	//Top = Top<30?min(min(pt_B.y,pt_G.y),pt_R.y):Top;
	Top = Top>30?30:Top;
	

	float idthr = (Bot + Top)/2;
	float checkBook[256];
	for (int idx=0;idx<256;idx++)
	{
		if(idx<=Top)
			checkBook[idx] = th_min;//(float(idx) / (Top)) * ( th_min );
		else if (idx>=Bot)
		{
			if(true || Bot>245)
				checkBook[idx] = th;
			else
				checkBook[idx] = (float(idx - Bot) / (255 - Bot)) * ( 245 - Bot ) + Bot + 10;
		}
		else
		{
			if(true || Bot>245)
				checkBook[idx] = (float(idx - Top) / (Bot - Top)) * (th-th_min) + th_min;
			else
				checkBook[idx] = (float(idx - Top) / (Bot - Top)) * (Bot - th_min +10) + th_min;
		}
		
	}

	uchar *data = src.ptr<uchar>(0);
	for(int idr =0;idr<src.rows;idr++)
	{
		for(int idc=0;idc<src.cols;idc++)
		{
			*data = checkBook[*data];
			*(data+1) = checkBook[*(data+1)];
			*(data+2) = checkBook[*(data+2)];
			*data = checkBook[*data];
			*(data+1) = checkBook[*(data+1)];
			*(data+2) = checkBook[*(data+2)];
			data += 3;
		}
	}
	if (1)
	{	
		Mat ycc;    //转换空间到YCrCb；
		cvtColor(src, ycc, COLOR_BGR2YCrCb);
		vector<Mat> channels(3);        //分离通道；
		split(ycc, channels);
		Point pt = CBrightnessBalance::findMaxThreshold1(channels[0],0.2,0.7);
		//Mat temp = Mat::ones(channels[0].size(),CV_32FC1)*1.2;
	
		channels[0] = channels[0]*(min(1.2,255.0/pt.x));
		merge(channels, ycc);
		cvtColor(ycc, src, COLOR_YCrCb2BGR);
	}
	
	return true;

}


//************************************************************************  
//color_restoration（颜色恢复）
//函数功能：颜色恢复
//函数名称：color_restoration
//函数参数：Mat &scr：输入图像，为三通道RGB图像；
//返回类型：Mat
//************************************************************************  
Mat CnewColorEnhance::color_restoration(Mat &src)
{
	if (!src.data || src.channels()!= 3)  //判断图像是否被正确读取；
	{
		cerr << "传入图片有误";
		return src;
	}
	
	/*
	Msrcr msrcr;
	vector<double> sigema;
	vector<double> weight;
	for(int i = 0; i < 3; i++)
		weight.push_back(1./3);
	// 由于MSRCR.h中定义了宏USE_EXTRA_SIGMA，所以此处的vector<double> sigema并没有什么意义
	sigema.push_back(30);
	sigema.push_back(150);
	sigema.push_back(300);
	//vector<int> aa = msrcr.CreateFastKernel(30);
	Mat dst;
	msrcr.MultiScaleRetinexCR(src.clone(), dst, weight, sigema, 128, 128);
	return dst;*/

	if (1)
	{
		if(0)
		{
			Mat image = src.clone();
	
			if(1)
			{
				uchar* data = src.ptr<uchar>(0);
				//uchar* data1 = image.ptr<uchar>(0);
				for (int ii = 0;ii<image.rows;ii++)
				{
					for(int jj = 0;jj<image.cols;jj++,data+=3)
					{
						int aa = min(*(data+2),min(*data,*(data+1)));
						int aa1 = max(*(data+2),max(*data,*(data+1)));
						if (1.0*aa1/aa>1.15 || aa<80)
						{
							*data = 0;
							*(data+1) = 0;
							*(data+2)= 0;
							/*if (aa<100 && 1.0*aa1/aa<1.1)
							{
								*data1 *= 0.6;
								*(data1+1) *= 0.6;
								*(data1+2) *= 0.6;
							}*/
						}

					
					}
				}
			
			}
			int blockSize = 100;
			vector<Mat> channels(3);        //分离通道；
			split(image, channels);

			vector<Mat> maskchannels(3);        //分离通道；
			split(src, maskchannels);
			Mat mask = maskchannels[0]>0;
			double avg1 = mean(mask)[0];
			double average = 128;
			if (avg1>0)
				average = max(128.0,255*min(mean(maskchannels[0])[0],min(mean(maskchannels[1])[0],mean(maskchannels[2])[0]))/avg1);

			for(int ii = 0;ii<3;ii++)
			{
				Mat im = channels[ii];

				Mat img = maskchannels[ii];

				int rows_new = ceil(double(img.rows) / double(blockSize));
				int cols_new = ceil(double(img.cols) / double(blockSize));
				Mat blockImage;
				blockImage = Mat::zeros(rows_new, cols_new, CV_8UC1);
				for (int i = 0; i < rows_new; i++)
				{
					for (int j = 0; j < cols_new; j++)
					{
						int rowmin = i * blockSize;
						int rowmax = (i + 1)*blockSize;
						if (rowmax > img.rows) rowmax = img.rows;
						int colmin = j * blockSize;
						int colmax = (j + 1)*blockSize;
						if (colmax > img.cols) colmax = img.cols;
						Mat imageROI = img(Range(rowmin, rowmax), Range(colmin, colmax));
						Mat mask1 = imageROI>0;
						double temaver = mean(imageROI)[0];
						double temaver1 = mean(mask1)[0];
						if (temaver1>10)
							temaver = temaver*255/temaver1;
						else
							temaver = average;
						blockImage.at<uchar>(i, j) = temaver;
					}
				}
				resize(blockImage,blockImage,img.size());
			

				uchar *data = im.ptr<uchar>(0);
				uchar *data1 = blockImage.ptr<uchar>(0);
				for (int idr = 0; idr<blockImage.rows; idr++)
				{
					for (int idc = 0; idc<blockImage.cols; idc++,data++,data1++)
					{
						*data = saturate_cast<uchar>(float(*data)-float(*data1)+average);
					}
				}
				channels[ii] = im;
			}
			merge(channels, src);
		}

		Mat temp;
		if (max(src.rows,src.cols)>3000)
			resize(src,temp,Size(0,0),0.3,0.3,1);
		else if(max(src.rows,src.cols)>1500)
			resize(src,temp,Size(0,0),0.5,0.5,1);
		else 
			temp = src.clone();
		vector<float> dst = findThreshold(temp,200);
		vector<Mat> channels(3);        //分离通道；
		split(src, channels);
		if (dst.size()==3)
		{
			/*float mean = (dst[0]+dst[1]+dst[2])/3;
			channels[0] = channels[0]*(mean/dst[0]);
			channels[1] = channels[1]*(mean/dst[1]);
			channels[2] = channels[2]*(mean/dst[2]);*/
			float aa = max(dst[0],max(dst[1],dst[2]));
			channels[0] = channels[0]+(aa-dst[0]);
			channels[1] = channels[1]+(aa-dst[1]);
			channels[2] = channels[2]+(aa-dst[2]);
		}
		merge(channels, src);
		return src;
		cvtColor(src, src, COLOR_BGR2HSV);
		vector<Mat> channels1(3);        //分离通道；
		split(src, channels1);
		channels1[1] /= 1.35;
		//channels1[1] = channels1[2]*180.0/255;
		equalizeHist(channels[2], channels[2]);


		merge(channels1, src);
		cvtColor(src, src, COLOR_HSV2BGR);

		//vector<Mat> channels(3);        //分离通道；
		split(src, channels);

	
		dst = findThreshold(src, 220, 0);
		vector<float> dst1 = findThreshold(~src, 200, 0);
		if (dst.size()==3)
		{
			/*float mean = (dst[0]+dst[1]+dst[2])/3;
			channels[0] = channels[0]*(mean/dst[0]);
			channels[1] = channels[1]*(mean/dst[1]);
			channels[2] = channels[2]*(mean/dst[2]);*/

			for(int ii = 0;ii<3;ii++)
			{
				float Top,Bot;
				Bot = dst[ii];
				if (dst1.size()==3)
					Top = 255-dst1[ii]>20?20:255-dst1[ii];
				else
				{
					Point pt = CBrightnessBalance::findMaxThreshold1(channels[ii],0.1,0.1);
					Top = pt.y>20?20:pt.y;
				}
				float idthr = (Bot + Top)/2;
				float th = dst[ii]>245?255:dst[ii]+5;
				float th_min = 0;
				float checkBook[256];
				for (int idx=0;idx<256;idx++)
				{
					if(idx<=Top)
						checkBook[idx] = th_min;//(float(idx) / (Top)) * ( th_min );
					else if (idx>=Bot)
					{
						if(true || Bot>245)
							checkBook[idx] = th;
						else
							checkBook[idx] = (float(idx - Bot) / (255 - Bot)) * ( 245 - Bot ) + Bot + 10;
					}
					else
					{
						if(true || Bot>245)
							checkBook[idx] = (float(idx - Top) / (Bot - Top)) * (th-th_min) + th_min;
						else
							checkBook[idx] = (float(idx - Top) / (Bot - Top)) * (Bot - th_min +10) + th_min;
					}
		
				}

				Mat tmp = channels[ii];
				uchar *data = tmp.ptr<uchar>(0);
				for(int idr =0;idr<src.rows;idr++)
				{
					for(int idc=0;idc<src.cols;idc++)
					{
						*data = checkBook[*data];
						*data = checkBook[*data];
						data += 1;
					}
				}
				channels[ii] = tmp.clone();
			}
			
			merge(channels, src);
			

			
			cvtColor(src, src, COLOR_BGR2HSV);
			vector<Mat> channels1(3);        //分离通道；
			split(src, channels1);
			channels1[1] *= 1.15;

			Mat temp;
			blur(channels1[2],temp,Size(7,7));
			//filter2D(gray, temp, -1, kernel);
			addWeighted(channels1[2], 1.5, temp, -0.5, 0, channels1[2]);

			merge(channels1, src);
			cvtColor(src, src, COLOR_HSV2BGR);
		}
		else
		{
			Mat tmp = CnewColorEnhance::textEnhance(src);
		}
	}
	else
	{
		vector<float> dst = findThreshold(src, 220, 0);

		cvtColor(src, src, COLOR_BGR2HSV);
		vector<Mat> channels(3);        //分离通道；
		split(src, channels);
		channels[1] /= 1.35;

		if (false && dst.size()==3)
		{
			float Top,Bot;

			Point pt = CBrightnessBalance::findMaxThreshold1(channels[2],0.1,0.1);
			Bot = pt.x;
			Top = pt.y>30?30:pt.y;
		
			float idthr = (Bot + Top)/2;
			float th = 255;
			float th_min = 0;
			float checkBook[256];
			for (int idx=0;idx<256;idx++)
			{
				if(idx<=Top)
					checkBook[idx] = th_min;//(float(idx) / (Top)) * ( th_min );
				else if (idx>=Bot)
				{
					if(true || Bot>245)
						checkBook[idx] = th;
					else
						checkBook[idx] = (float(idx - Bot) / (255 - Bot)) * ( 245 - Bot ) + Bot + 10;
				}
				else
				{
					if(true || Bot>245)
						checkBook[idx] = (float(idx - Top) / (Bot - Top)) * (th-th_min) + th_min;
					else
						checkBook[idx] = (float(idx - Top) / (Bot - Top)) * (Bot - th_min +10) + th_min;
				}
		
			}

			Mat tmp = channels[2];
			uchar *data = tmp.ptr<uchar>(0);
			for(int idr =0;idr<src.rows;idr++)
			{
				for(int idc=0;idc<src.cols;idc++)
				{
					*data = checkBook[*data];
					*data = checkBook[*data];
					data += 1;
				}
			}
			channels[2] = tmp.clone();
		}

		merge(channels, src);
		cvtColor(src, src, COLOR_HSV2BGR);
		


		
		/*int width = src.cols;
		int height = src.rows;
		int channel = src.channels();
		cout << "width=: " << width << " ;height=: " << height << " ;channels=: "<<channel<< endl;
		uchar* buffer = (uchar*)malloc(sizeof(uchar)*height*width*channel);
		memcpy(buffer, src.data, height*width*channel);
		Msrcr::MSRCR( buffer, src.cols, src.rows, src.channels() );   
		if (channel == 3)
		{
			cv::Mat temp(height, width, CV_8UC3, buffer);
			src = temp.clone();
		}
		else if (channel == 1)
		{
			cv::Mat temp(height, width, CV_8UC1, buffer);
			src = temp.clone();
		}*/
	}
	return src;

}

vector<float> CnewColorEnhance::findThreshold(Mat src,int thresh, int type)
{
	vector<float> dst;
	if (src.channels()!=3)
		return dst;

	int valueArr_B[256]={0};
	int valueArr_G[256]={0};
	int valueArr_R[256]={0};

	const uchar *data = src.ptr<uchar>(0);

	for (int idr=0;idr < src.rows;idr++)
	{
		for (int idc=0;idc <src.cols;idc++,data = data+3)
		{
			int aa = min(*data,min(*(data+1),*(data+2)));
			if (aa>=thresh && aa<255 && max(abs(*data/float(*(data+1)+0.00001)-1),abs(*data/float(*(data+2)+0.00001)-1))<=0.15)
			{
				valueArr_B[*data]++;
				valueArr_G[*(data+1)]++;
				valueArr_R[*(data+2)]++;
			}
		}
	}

	float lSum_B =0;
	float lSum_G =0;
	float lSum_R =0;
	float mean_B =0;
	float mean_G =0;
	float mean_R =0;
	int nThres = src.rows*src.cols;
	for (int idx=255;idx >= thresh;idx--)
	{
		lSum_B += valueArr_B[idx];
		lSum_G += valueArr_G[idx];
		lSum_R += valueArr_R[idx];
		mean_B += valueArr_B[idx]*idx;
		mean_G += valueArr_G[idx]*idx;
		mean_R += valueArr_R[idx]*idx;
		
	}
	if (type)
	{
		float fRiao = 1.0/(256-thresh);
		for (int idx = thresh;idx<256;idx++)
		{
			if (valueArr_B[idx]/float(lSum_B)>fRiao || valueArr_B[idx]>1000)
				mean_B = idx;
			if (valueArr_G[idx]/float(lSum_G)>fRiao || valueArr_G[idx]>1000)
				mean_G = idx;
			if (valueArr_R[idx]/float(lSum_R)>fRiao || valueArr_R[idx]>1000)
				mean_R = idx;
		}
		if (mean_B>=thresh && mean_G>=thresh && mean_R>=thresh)
		{
 			dst.push_back(mean_B);
			dst.push_back(mean_G);
			dst.push_back(mean_R);
		}
		return dst;
	}
	if (min(min(lSum_B,lSum_G),lSum_R)>min(2000.0,(float(nThres)*0.05)))
	{
		mean_B = mean_B/lSum_B;
		mean_G = mean_G/lSum_G;
		mean_R = mean_R/lSum_R;
		dst.push_back(mean_B);
		dst.push_back(mean_G);
		dst.push_back(mean_R);
		return dst;
	}
	else
	{
		return dst;
		int valueArr_B[256]={0};
		int valueArr_G[256]={0};
		int valueArr_R[256]={0};

		const uchar *data = src.ptr<uchar>(0);

		for (int idr=0;idr < src.rows;idr++)
		{
			for (int idc=0;idc <src.cols;idc++,data = data+3)
			{
				if (max(abs(*data/float(*(data+1)+0.00001)-1),abs(*data/float(*(data+2)+0.00001)-1))<=0.1)
				{
					valueArr_B[*data]++;
					valueArr_G[*(data+1)]++;
					valueArr_R[*(data+2)]++;
				}
			}
		}
		float lSum_B =0;
		float lSum_G =0;
		float lSum_R =0;
		float mean_B =0;
		float mean_G =0;
		float mean_R =0;
		int nThres = src.rows*src.cols;
		for (int idx=255;idx >= 0;idx--)
		{
			lSum_B += valueArr_B[idx];
			lSum_G += valueArr_G[idx];
			lSum_R += valueArr_R[idx];
			mean_B += valueArr_B[idx]*idx;
			mean_G += valueArr_G[idx]*idx;
			mean_R += valueArr_R[idx]*idx;
		}
		if (min(min(lSum_B,lSum_G),lSum_R)/float(nThres)>0.05)
		{
			mean_B = mean_B/lSum_B;
			mean_G = mean_G/lSum_G;
			mean_R = mean_R/lSum_R;
			dst.push_back(mean_B);
			dst.push_back(mean_G);
			dst.push_back(mean_R);
			return dst;
		}
		else
		{
			return dst;
		}
	}
}
//************************************************************************  
//Adaptive Contrast Enhancement（自适应对比度增强，ACE）
//函数功能：获取图像的局部均值与局部标准差的图
//函数名称：adaptContrastEnhancement
//函数参数：Mat &scr：输入图像，为三通道RGB图像；
//函数参数：Mat &dst：增强后的输出图像，为三通道RGB图像；
//函数参数：int winSize：局部均值的窗口大小，应为单数；
//函数参数：int maxCg：增强幅度的上限；
//返回类型：bool
//************************************************************************  
bool CnewColorEnhance::adaptContrastEnhancement(Mat &scr, Mat &dst, int winSize,int maxCg)
{
	if (!scr.data)  //判断图像是否被正确读取；
	{
		cerr << "自适应对比度增强函数读入图片有误";
		return false;
	}

	Mat ycc;                        //转换空间到YCrCb；
	//cvtColor(scr, ycc, COLOR_RGB2YCrCb);
	cvtColor(scr, ycc, COLOR_BGR2Lab);

	vector<Mat> channels(3);        //分离通道；
	split(ycc, channels);
#if 0
	/*
	/////使用clahe自适应均衡
	Ptr<CLAHE> clahe = createCLAHE();
	clahe->setClipLimit(4);
	clahe->setTilesGridSize(Size(200,200));
	clahe->apply(channels[0], channels[0]);*/
	
	equalizeHist(channels[0], channels[0]);
#else
	Mat localMeansMatrix(scr.rows , scr.cols , CV_32FC1);
	Mat localVarianceMatrix(scr.rows , scr.cols , CV_32FC1);
	
	if (!getVarianceMean(channels[0], localMeansMatrix, localVarianceMatrix, winSize))   //对Y通道进行增强；
	{
		cerr << "计算图像均值与标准差过程中发生错误";
		return false;
	}

	Mat temp = channels[0].clone();

	Scalar  mean;
	Scalar  dev;
	meanStdDev(temp, mean, dev);

	float meansGlobal = mean.val[0];
	Mat enhanceMatrix(scr.rows, scr.cols, CV_8UC1);

	for (int i = 0; i < scr.rows; i++)            //遍历，对每个点进行自适应调节
	{
		for (int j = 0; j < scr.cols; j++)
		{
			if (localVarianceMatrix.at<float>(i, j) >= 0.01)
			{
				float cg = 0.2*meansGlobal / localVarianceMatrix.at<float>(i, j);
				float cgs = cg > maxCg ? maxCg : cg;
				cgs = cgs < 1 ? 1 : cgs;
				
				int e = localMeansMatrix.at<float>(i, j) + cgs* (temp.at<uchar>(i, j) - localMeansMatrix.at<float>(i, j));
				if (e > 255){ e = 255; }
				else if (e < 0){ e = 0; }
				enhanceMatrix.at<uchar>(i, j) = e;
			}
			else
			{
				enhanceMatrix.at<uchar>(i, j) = temp.at<uchar>(i, j);
			}
		}
			
	}
	
	channels[0] = enhanceMatrix;    //合并通道，转换颜色空间回到RGB
#endif
	merge(channels, ycc);

	//cvtColor(ycc, dst, COLOR_YCrCb2RGB);
	cvtColor(ycc, dst, COLOR_Lab2BGR);
	return true;
}

//************************************************************************  
// 函数名称:    Color_HomoFilter 
// 函数说明:    同态滤波彩色增强算法
// 函数参数:    Mat src    输入RGB原图像
// 返 回 值:	Mat	   处理后图像
//************************************************************************ 
 Mat CnewColorEnhance::Color_HomoFilter(cv::Mat src)
 {
	 Mat dst;
	vector <Mat> yuv;
	split(src, yuv);

	for (int ii = 0;ii<yuv.size();ii++)
	{
		yuv[ii] = HomoFilter(yuv[ii]);
	}

    merge(yuv, dst);
	return dst;
 }

Mat CnewColorEnhance::HomoFilter(cv::Mat src)
{
	src.convertTo(src, CV_64FC1);
    int rows = src.rows;
    int cols = src.cols;
    int m = rows % 2 == 1 ? rows + 1 : rows;
    int n = cols % 2 == 1 ? cols + 1 : cols;
    copyMakeBorder(src, src, 0, m - rows, 0, n - cols, BORDER_CONSTANT, Scalar::all(0));
    rows = src.rows;
    cols = src.cols;
    Mat dst(rows, cols, CV_64FC1);
    //1. ln
    for(int i = 0; i < rows; i++){
        double *srcdata = src.ptr<double>(i);
        double *logdata = src.ptr<double>(i);
        for(int j = 0; j < cols; j++){
            logdata[j] = log(srcdata[j] + 0.0001);
        }
    }
    //2. dct
    Mat mat_dct = Mat::zeros(rows, cols, CV_64FC1);
    dct(src, mat_dct);
    //3. 高斯同态滤波器
    Mat H_u_v;
    double gammaH = 1.5;
    double gammaL = 0.5;
    double C = 1;
    double  d0 = (src.rows / 2) * (src.rows / 2) + (src.cols / 2) * (src.cols / 2);
    double  d2 = 0;
    H_u_v = Mat::zeros(rows, cols, CV_64FC1);
    for(int i = 0; i < rows; i++){
        double * dataH_u_v = H_u_v.ptr<double>(i);
        for(int j = 0; j < cols; j++){
            d2 = pow(i, 2.0) + pow(j, 2.0);
            dataH_u_v[j] = (gammaH - gammaL) * (1 - exp(-C * d2 / d0)) + gammaL;
        }
    }
    H_u_v.ptr<double>(0)[0] = 1.1;
    mat_dct = mat_dct.mul(H_u_v);
    //4. idct
    idct(mat_dct, dst);
    //exp
    for(int i = 0; i < rows; i++){
        double  *srcdata = dst.ptr<double>(i);
        double *dstdata = dst.ptr<double>(i);
        for(int j = 0; j < cols; j++){
            dstdata[j] = exp(srcdata[j]);
        }
    }
    dst.convertTo(dst, CV_8UC1);
    return dst;
}


void CnewColorEnhance::ALTMRetinex(const Mat src, Mat &dst, bool LocalAdaptation, bool ContrastCorrect)
{
	int filterFactor = 1;
	Mat my_img = src.clone();
    Mat orig_img = src.clone();
    //imshow("original",my_img);

    Mat simg;

    cvtColor(my_img, simg, CV_BGR2GRAY);

    long int N = simg.rows*simg.cols;

    int histo_b[256];
    int histo_g[256];
    int histo_r[256];

    for(int i=0; i<256; i++){
        histo_b[i] = 0;
        histo_g[i] = 0;
        histo_r[i] = 0;
    }
    Vec3b intensity;

    for(int i=0; i<simg.rows; i++){
        for(int j=0; j<simg.cols; j++){
            intensity = my_img.at<Vec3b>(i,j);
           
            histo_b[intensity.val[0]] = histo_b[intensity.val[0]] + 1;
            histo_g[intensity.val[1]] = histo_g[intensity.val[1]] + 1;
            histo_r[intensity.val[2]] = histo_r[intensity.val[2]] + 1;
        }
    }

    for(int i = 1; i<256; i++){
        histo_b[i] = histo_b[i] + filterFactor * histo_b[i-1];
        histo_g[i] = histo_g[i] + filterFactor * histo_g[i-1];
        histo_r[i] = histo_r[i] + filterFactor * histo_r[i-1];
    }

    int vmin_b=0;
    int vmin_g=0;
    int vmin_r=0;
    int s1 = 3;
    int s2 = 3;

    while(histo_b[vmin_b+1] <= N*s1/100){
        vmin_b = vmin_b +1;
    }
    while(histo_g[vmin_g+1] <= N*s1/100){
        vmin_g = vmin_g +1;
    }
    while(histo_r[vmin_r+1] <= N*s1/100){
        vmin_r = vmin_r +1;
    }

    int vmax_b = 255-1;
    int vmax_g = 255-1;
    int vmax_r = 255-1;

    while(histo_b[vmax_b-1]>(N-((N/100)*s2)))
    {   
        vmax_b = vmax_b-1;
    }
    if(vmax_b < 255-1){
        vmax_b = vmax_b+1;
    }
    while(histo_g[vmax_g-1]>(N-((N/100)*s2)))
    {   
        vmax_g = vmax_g-1;
    }
    if(vmax_g < 255-1){
        vmax_g = vmax_g+1;
    }
    while(histo_r[vmax_r-1]>(N-((N/100)*s2)))
    {   
        vmax_r = vmax_r-1;
    }
    if(vmax_r < 255-1){
        vmax_r = vmax_r+1;
    }

    for(int i=0; i<simg.rows; i++)
    {
        for(int j=0; j<simg.cols; j++)
        {
           
            intensity = my_img.at<Vec3b>(i,j);

            if(intensity.val[0]<vmin_b){
                intensity.val[0] = vmin_b;
            }
            if(intensity.val[0]>vmax_b){
                intensity.val[0]=vmax_b;
            }


            if(intensity.val[1]<vmin_g){
                intensity.val[1] = vmin_g;
            }
            if(intensity.val[1]>vmax_g){
                intensity.val[1]=vmax_g;
            }


            if(intensity.val[2]<vmin_r){
                intensity.val[2] = vmin_r;
            }
            if(intensity.val[2]>vmax_r){
                intensity.val[2]=vmax_r;
            }

            my_img.at<Vec3b>(i,j) = intensity;
        }
    }

    for(int i=0; i<simg.rows; i++){
        for(int j=0; j<simg.cols; j++){
           
            intensity = my_img.at<Vec3b>(i,j);
            intensity.val[0] = (intensity.val[0] - vmin_b)*255/(vmax_b-vmin_b);
            intensity.val[1] = (intensity.val[1] - vmin_g)*255/(vmax_g-vmin_g);
            intensity.val[2] = (intensity.val[2] - vmin_r)*255/(vmax_r-vmin_r);
            my_img.at<Vec3b>(i,j) = intensity;
        }
    }   
    
    
    // sharpen image using "unsharp mask" algorithm
    Mat blurred; double sigma = 1, threshold = 5, amount = 1;
    GaussianBlur(my_img, blurred, Size(), sigma, sigma);
    Mat lowContrastMask = abs(my_img - blurred) < threshold;
    Mat sharpened = my_img*(1+amount) + blurred*(-amount);
    my_img.copyTo(sharpened, lowContrastMask);    

    //imshow("New Image",sharpened);
   // waitKey(0);
    
    hconcat(orig_img, sharpened, dst);
}

 //导向滤波器
 Mat CnewColorEnhance::guidedFilter(cv::Mat& I, cv::Mat& p, int r, float eps)
 {
     /*
     × GUIDEDFILTER   O(N) time implementation of guided filter.
     ×
     ×   - guidance image: I (should be a gray-scale/single channel image)
     ×   - filtering input image: p (should be a gray-scale/single channel image)
     ×   - local window radius: r
     ×   - regularization parameter: eps
     */

     cv::Mat _I;
     I.convertTo(_I, CV_32FC1);
     I = _I;

     cv::Mat _p;
     p.convertTo(_p, CV_32FC1);
     p = _p;

     //因为opencv自带的boxFilter（）中的Size,比如9x9,我们说半径为4
     r = 2 * r + 1;

     //mean_I = boxfilter(I, r) ./ N;
     cv::Mat mean_I;
     cv::boxFilter(I, mean_I, CV_32FC1, cv::Size(r, r));

     //mean_p = boxfilter(p, r) ./ N;
     cv::Mat mean_p;
     cv::boxFilter(p, mean_p, CV_32FC1, cv::Size(r, r));

     //mean_Ip = boxfilter(I.*p, r) ./ N;
     cv::Mat mean_Ip;
     cv::boxFilter(I.mul(p), mean_Ip, CV_32FC1, cv::Size(r, r));

     //cov_Ip = mean_Ip - mean_I .* mean_p; % this is the covariance of (I, p) in each local patch.
     cv::Mat cov_Ip = mean_Ip - mean_I.mul(mean_p);

     //mean_II = boxfilter(I.*I, r) ./ N;
     cv::Mat mean_II;
     cv::boxFilter(I.mul(I), mean_II, CV_32FC1, cv::Size(r, r));

     //var_I = mean_II - mean_I .* mean_I;
     cv::Mat var_I = mean_II - mean_I.mul(mean_I);

     //a = cov_Ip ./ (var_I + eps); % Eqn. (5) in the paper;
     cv::Mat a = cov_Ip / (var_I + eps);

     //b = mean_p - a .* mean_I; % Eqn. (6) in the paper;
     cv::Mat b = mean_p - a.mul(mean_I);

     //mean_a = boxfilter(a, r) ./ N;
     cv::Mat mean_a;
     cv::boxFilter(a, mean_a, CV_32FC1, cv::Size(r, r));

     //mean_b = boxfilter(b, r) ./ N;
     cv::Mat mean_b;
     cv::boxFilter(b, mean_b, CV_32FC1, cv::Size(r, r));

     //q = mean_a .* I + mean_b; % Eqn. (8) in the paper;
     cv::Mat q = mean_a.mul(I) + mean_b;

     return q;
 }

bool CnewColorEnhance::getVarianceMean(Mat src, Mat &localMeansMatrix, Mat &localVarianceMatrix, int winSize)
{
	if (src.empty())
		return false;

	int aa = localMeansMatrix.cols;
	int bb = localMeansMatrix.rows;
	Mat mean;
	Mat dev;
	Mat temp;
	for (int ii = 0;ii<src.rows;ii++)
	{
		for (int jj = 0;jj<src.cols;jj++)
		{
			int x1 = jj-winSize>0?jj-winSize:0;
			int y1 = ii-winSize>0?ii-winSize:0;
			if (x1>src.cols-2*winSize)
			{
				x1 = src.cols-2*winSize;
			}
			if(y1>src.rows-2*winSize)
			{
				y1 = src.rows-2*winSize;
			}
			
			temp = src(Rect(x1,y1,2*winSize,2*winSize));
			meanStdDev(temp, mean, dev);
			localMeansMatrix.at<float>(ii,jj) = mean.at<double>(0, 0);
			localVarianceMatrix.at<float>(ii,jj) = dev.at<double>(0, 0);

		}
	}
	return true;
}

void CnewColorEnhance::mybilateralFilter(cv::Mat& src, cv::Mat& dst, cv::Size wsize, double spaceSigma, double colorSigma)
{
	
}

int CnewColorEnhance::ImageStretchByHistogram(IplImage *src1,IplImage *dst1)
/*************************************************
Function:      通过直方图变换进行图像增强，将图像灰度的域值拉伸到0-255
src1:               单通道灰度图像                  
dst1:              同样大小的单通道灰度图像 
*************************************************/
{
	assert(src1->width==dst1->width);
	double p[256],p1[256],num[256];
 
	memset(p,0,sizeof(p));
	memset(p1,0,sizeof(p1));
	memset(num,0,sizeof(num));
	int height=src1->height;
	int width=src1->width;
	long wMulh = height * width;
 
	//statistics
	for(int x=0;x<src1->width;x++)
	{
		for(int y=0;y<src1-> height;y++){
			uchar v=((uchar*)(src1->imageData + src1->widthStep*y))[x];
			num[v]++;
		}
	}
	//calculate probability
	for(int i=0;i<256;i++)
	{
		p[i]=num[i]/wMulh;
	}
 
	//p1[i]=sum(p[j]);	j<=i;
	for(int i=0;i<256;i++)
	{
		for(int k=0;k<=i;k++)
			p1[i]+=p[k];
	}
 
	// histogram transformation
	for(int x=0;x<src1->width;x++)
	{
		for(int y=0;y<src1-> height;y++){
			uchar v=((uchar*)(src1->imageData + src1->widthStep*y))[x];
			((uchar*)(dst1->imageData + dst1->widthStep*y))[x]= p1[v]*255+0.5;            
		}
	}
	return 0;
}
 
void CnewColorEnhance::OnYcbcrY(Mat &src)
{
	IplImage temp = IplImage(src);
	IplImage* workImg = &temp;

	IplImage* Y = cvCreateImage(cvGetSize(workImg),IPL_DEPTH_8U,1);
	IplImage* Cb= cvCreateImage(cvGetSize(workImg),IPL_DEPTH_8U,1);
	IplImage* Cr = cvCreateImage(cvGetSize(workImg),IPL_DEPTH_8U,1);
	IplImage* Compile_YCbCr= cvCreateImage(cvGetSize(workImg),IPL_DEPTH_8U,3);
	IplImage* dst1=cvCreateImage(cvGetSize(workImg),IPL_DEPTH_8U,3);
 
	int i;
	cvCvtColor(workImg,dst1,CV_BGR2YCrCb);
	cvSplit(dst1,Y,Cb,Cr,0);
 
 	ImageStretchByHistogram(Y,dst1);
 
 	for(int x=0;x<workImg->height;x++)
 	{
 		for(int y=0;y<workImg->width;y++)
 		{
 			CvMat* cur=cvCreateMat(3,1,CV_32F);
 			cvmSet(cur,0,0,((uchar*)(dst1->imageData+x*dst1->widthStep))[y]);
 			cvmSet(cur,1,0,((uchar*)(Cb->imageData+x*Cb->widthStep))[y]);
 			cvmSet(cur,2,0,((uchar*)(Cr->imageData+x*Cr->widthStep))[y]);
 
 			for(i=0;i<3;i++)
 			{
 				double xx=cvmGet(cur,i,0);
 				((uchar*)Compile_YCbCr->imageData+x*Compile_YCbCr->widthStep)[y*3+i]=xx;
 			}
 		}
 	}
	cvCvtColor(Compile_YCbCr,workImg,CV_YCrCb2BGR);

	src = cvarrToMat(workImg);
}

