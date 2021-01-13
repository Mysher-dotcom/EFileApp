#include "stdafx.h"
#include "Compare.h"
#include "BrightnessBalance.h"
#include "AutoLevel.h"
#include "../Feature/DetectRectByContours_new.h"
#include "../ColorSpace/AdaptiveThreshold.h"
//自动调整色阶
bool AdjustLevelAuto1(Mat src,Mat &dst)
{
	int nThreshold_Low = 50;
	if(!src.data)
	{
		return false;
	}



	IplImage temp = IplImage(dst);
	IplImage *ipl = &temp;
	int nThreshold = CAdaptiveThreshold::OptimalThreshold(ipl);
	CAutoLevel::StrechHistogram(dst.data, dst.cols, dst.rows, dst.channels()*8, dst.step, nThreshold_Low, nThreshold,0,255);	

	return true;
}
//[in] src_rect1 :外接矩形信息
//[in]gray：输入图像
//[out] Mat：返回裁切图
//jessie 2016-12-22 15:10:00
Mat crop(RotatedRect src_rect1,Mat gray)
{
	int dstw,dsth;
	Point2f vertices[4];
	Point2f verdst[4];

	if(src_rect1.size.width<src_rect1.size.height)
	{
		if(src_rect1.angle<(-45)&&src_rect1.angle>(-90))
		{
			src_rect1.angle=src_rect1.angle+90;
		}
		//rMaxRect.size.width += 100;  //宽度增加100，以防定位偏差
		src_rect1.points(vertices);
		dstw = src_rect1.size.width;
		dsth = src_rect1.size.height;

		verdst[0] = Point2f(0,dsth);
		verdst[1] = Point2f(0,0);
		verdst[2] = Point2f(dstw,0);
		verdst[3] = Point2f(dstw,dsth); 
	}
	else 
	{
		//rMaxRect.size.height += 100;
		src_rect1.points(vertices);
		dstw = src_rect1.size.height;
		dsth = src_rect1.size.width;
		verdst[0] = Point2f(dstw,dsth);
		verdst[1] = Point2f(0,dsth);
		verdst[2] = Point2f(0,0);
		verdst[3] = Point2f(dstw,0);

	}

	Mat Idst = Mat(dsth,dstw,CV_8UC1);
	Mat warpMatrix = getPerspectiveTransform(vertices, verdst);
	warpPerspective(gray, Idst, warpMatrix, Idst.size(), INTER_LINEAR, BORDER_CONSTANT);

	return Idst;
}
//[in] src ：输入图像
//[out] RotatedRect：返回外接矩形信息
//jessie 2016-12-22 17:10:00
RotatedRect DetectBorder(Mat src )
{
	Mat dst;
	RotatedRect rMaxRect;
	if(!src.data)
	{
		return rMaxRect;
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
	IplImage temp =(IplImage)gray;
	IplImage* asrc=&temp;


	int threshold1 = CAdaptiveThreshold::OptimalThreshold(asrc);


	threshold(gray,bw1,threshold1,255,CV_THRESH_BINARY);
	
	vector<vector<Point> > contours;
	vector<vector<Point> > resultcontours;
	findContours(bw1,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);

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
	drawContours(result,resultcontours,-1, CV_RGB(255,255,255),5);
	drawContours(result1, resultcontours, 0, CV_RGB(0,0,0), CV_FILLED);
	result= result1+result;
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
	drawContours(dst,resultcontours,-1, CV_RGB(255,0,0),5);
	/*drawContours(src,resultcontours[0],-1, CV_RGB(255,0,0),5);
	cv::namedWindow("src",0);
	cv::imshow("src",src);
	cv::waitKey ();*/

	rMaxRect = cv::minAreaRect(resultcontours[0]);
	return rMaxRect;
}

Mat Cgrad(Mat gray)
{
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
	Mat temp = grad>(mean(grad)[0]*1.5>50?mean(grad)[0]*1.5:50);

	return temp;

}
//2020-6-22 15:58:18
//新版两张图像比对
bool CCompare::Compare1(Mat Src1,Mat Src2,int nThreshold,vector<CvRect> rect)
{
	clock_t start, end;
	start = clock();

	Mat Src1gray,Src2gray;
	if(Src1.empty()||Src2.empty()) 
	{
		return false;
	}
	else
	{
		if(Src1.rows!=Src2.rows || Src1.cols!=Src2.cols || Src1.channels ()!=Src2.channels ())
		{
			return false;
		}

		if (0)
		{
			int width = 600;
			int height = int(1.0*Src1.rows*width/Src1.cols);
			Size dsize = Size(width,height);
			resize(Src1,Src1,dsize);
			resize(Src2,Src2,dsize);
		}
		else
		{
			float fRiao = 600 / float(min(Src1.cols, Src1.rows));
			if (fRiao<0.25)
			{
				resize(Src1, Src1, Size(0,0),0.5,0.5,1);
				resize(Src1, Src1, Size(0,0),0.5,0.5,1);
				resize(Src1, Src1, Size(0,0),fRiao/0.25,fRiao/0.25,1);

				resize(Src2, Src2, Size(0,0),0.5,0.5,1);
				resize(Src2, Src2, Size(0,0),0.5,0.5,1);
				resize(Src2, Src2, Size(0,0),fRiao/0.25,fRiao/0.25,1);
			}
			else if(fRiao<0.5)
			{
				resize(Src1, Src1, Size(0,0),0.5,0.5,1);
				resize(Src1, Src1, Size(0,0),fRiao/0.5,fRiao/0.5,1);

				resize(Src2, Src2, Size(0,0),0.5,0.5,1);
				resize(Src2, Src2, Size(0,0),fRiao/0.5,fRiao/0.5,1);
			}
			else
			{
				resize(Src1, Src1, Size(0,0),fRiao,fRiao,1);

				resize(Src2, Src2, Size(0,0),fRiao,fRiao,1);
			}
		}


		if (Src1.channels() == 1)
			Src1gray = Src1.clone();
		
		else
			cvtColor(Src1,Src1gray, CV_BGR2GRAY);
		

		if (Src2.channels() == 1)
			Src2gray = Src2.clone();

		else
			cvtColor(Src2,Src2gray, CV_BGR2GRAY);

		

		GaussianBlur( Src1gray, Src1gray, Size( 5, 5), 0, 0 );
		GaussianBlur( Src2gray, Src2gray, Size( 5, 5), 0, 0 );

		//cv::equalizeHist (Src1gray,Src1gray);
		//cv::equalizeHist (Src2gray,Src2gray);
		
		Mat bw1=abs(Src1gray-Src2gray);
		if (1)
		{
			Mat mask1 = Src1gray>80;
			Mat mask2 = Src2gray>80;
			Mat mask;
			bitwise_or(mask1,mask2,mask);
			bitwise_and(mask1,mask2,mask2);
			bitwise_and(mask,~mask2,mask);
			mask1 = Src2gray>Src1gray;
			Mat temp1 = Src2gray.mul(mask1/255.0);
			Mat temp2 = Src1gray.mul(~mask1/255.0);
			temp1 = temp1+temp2;
			mask1 = 0.33*temp1;
			mask1.setTo(30,mask1<30);
			bw1 = bw1>mask1;
			bitwise_and(bw1,mask,bw1);
		}

		Src1gray = Cgrad(Src1gray);
		Src2gray = Cgrad(Src2gray);

		Mat bw=abs(Src1gray-Src2gray);
		bw = bw>20;
		
		bitwise_or(bw1,bw,bw);
		//imshow("3",Src2gray);
		//waitKey(0);
		
		if(rect.size()>0)
		{
			bool aa = false;
			for(int i=0;i<rect.size();i++)
			{
				Mat bw1 = bw(rect[i]);
				vector<vector<Point> > vvPoint;
				findContours(bw1.clone(),vvPoint,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
				if (vvPoint.size()>0)
				{
					//遍历图像的轮廓，求出最大轮廓
					int rectMaxArea =0;
					int rectArea =0;
					vector<Point> areaMatvPoint;
					RotatedRect rMaxRect;
					for (vector<vector<Point>>::iterator itr=vvPoint.begin();itr!=vvPoint.end();itr++)
					{
						RotatedRect rrect =minAreaRect(*itr);
						rectArea =rrect.size.area();
						if( rectArea >rectMaxArea )
						{
							//Rect rMaxRect(rMaxRect.center.x-rMaxRect.size.width/2,rMaxRect.center.y-rMaxRect.size.height/2,rMaxRect.size.width,rMaxRect.size.height);
							rMaxRect =rrect;
							rectMaxArea = rectArea;
							areaMatvPoint.clear();
							areaMatvPoint = *itr;
						}
					}
					if(rMaxRect.size.width>20 && rMaxRect.size.height>20)
					{
						continue;
					}
				}
		
				vvPoint.clear();
				//Mat Src1gray1 = Src1gray(rect[i]);
				bw1 = Src2gray(rect[i]);
				//bitwise_and(Src1gray2,Src2gray1,bw1);
				findContours(bw1.clone(),vvPoint,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
				if (vvPoint.size()>0)
				{
					//遍历图像的轮廓，求出最大轮廓
					int rectMaxArea =0;
					int rectArea =0;
					vector<Point> areaMatvPoint;
					RotatedRect rMaxRect;
					for (vector<vector<Point>>::iterator itr=vvPoint.begin();itr!=vvPoint.end();itr++)
					{
						RotatedRect rrect =minAreaRect(*itr);
						rectArea =rrect.size.area();
						if( rectArea >rectMaxArea )
						{
							//Rect rMaxRect(rMaxRect.center.x-rMaxRect.size.width/2,rMaxRect.center.y-rMaxRect.size.height/2,rMaxRect.size.width,rMaxRect.size.height);
							rMaxRect =rrect;
							rectMaxArea = rectArea;
							areaMatvPoint.clear();
							areaMatvPoint = *itr;
						}
					}
					if(rMaxRect.size.width>50 && rMaxRect.size.height>50)
					{
						aa = true;
					}
				}
			}
			return aa;
		}
		else
		{
			vector<Point2f> PT4;
			vector<vector<Point> > vvPoint;
			//bool type1 = false;
			if (1)
			{
				Point pts[4];
				float angle;
				bool type = CDetectRectByContours_new::DetectRect(Src2, angle ,pts);
				
				if (type)
				{
					for (int ii = 0;ii<4;ii++)
					{
						PT4.push_back(pts[ii]);
					}
				}
				else
				{
					//type1 = CDetectRectByContours_new::DetectRect(Src1, angle ,pts);
					//if (!type1)
					return false;
				}
			}
			else
			{
				findContours(Src2gray.clone(),vvPoint,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
				if (vvPoint.size()>0)
				{
					vector<vector<Point>> vvPoint1;
					vector<RotatedRect> cRect;
					//遍历图像的轮廓，求出最大轮廓
					int rectMaxArea =1000;
					int rectArea =0;
					vector<Point> areaMatvPoint;
					RotatedRect rMaxRect;
					for (vector<vector<Point>>::iterator itr=vvPoint.begin();itr!=vvPoint.end();itr++)
					{
						RotatedRect rrect =minAreaRect(*itr);
						rectArea =rrect.size.area();
					
						if (rrect.size.height>70 && rrect.size.width>70)
						{
							if (rectArea>rectMaxArea)
							{
								cRect.insert(cRect.begin(),rrect);
								rMaxRect =rrect;
								rectMaxArea = rectArea;
								areaMatvPoint= *itr;
								vvPoint1.insert(vvPoint1.begin(),*itr);
							}
							else
							{
								cRect.push_back(rrect);
								vvPoint1.push_back(*itr);
							}
						}
					}
				
					if (1)
					{
						int idex = 0;
						while (idex<cRect.size())
						{
							if (cRect[idex].size.width<70 || cRect[idex].size.height<70)
							{
								idex += 1;
								continue;
							}
							PT4.clear();
							rMaxRect = cRect[idex];
							areaMatvPoint = vvPoint1[idex];
							idex += 1;

							Point2f pt[4];
							Point2f pf(0,0);

							if(1)
							{
								rMaxRect.points(pt);
								if(1)
								{
									Mat mask = Mat::zeros(bw.size(),bw.type());
									vector<Point> pts;
									pts.push_back(pt[0]);
									pts.push_back(pt[1]);
									pts.push_back(pt[2]);
									pts.push_back(pt[3]);
									vector<vector<Point>> contours;
									contours.push_back(pts);
									drawContours(mask,contours,-1,Scalar(255),-1);
								
									double avg = mask.cols*mask.rows*mean(mask)[0]/255;
									float fRiao = avg/rMaxRect.size.area();
									if (fRiao<0.8)
										continue;
								}
								float min_dist = 20000;
								int idx = 0;
								for (int ii = 0;ii<4;ii++)
								{
									float dist = norm(pf-pt[ii]);
									if (dist<min_dist)
									{
										min_dist = dist;
										idx = ii;
									}
								}

								vector<Point> pts;
								for (int ii = 0;ii<4;ii++)
								{
									pts.push_back(pt[(ii+idx)%4]);
								}
							}
					
					
							rMaxRect.points(pt);
							float min_dist = 20000;
							int idx = 0;
							for (int ii = 0;ii<4;ii++)
							{
								float dist = norm(pf-pt[ii]);
								if (dist<min_dist)
								{
									min_dist = dist;
									idx = ii;
								}
							}

							//drawContours(bw,vvPoint1,-1,Scalar(0),-1);
							for (int ii = 0;ii<4;ii++)
							{
								PT4.push_back(pt[(ii+idx)%4]);
								//line(bw,pt[(ii+idx)%4],pt[(ii+idx+1)%4],Scalar(255),3);
							}

							//imshow("5",bw);
							//waitKey(0);
						

							vector<float> length;
							length.push_back(2000);
							length.push_back(2000);
							length.push_back(2000);
							length.push_back(2000);
							int indx[4];
							for (int ii = 0;ii<areaMatvPoint.size();ii++)
							{
								for(int jj = 0;jj<4;jj++)
								{
									Point2f pf = areaMatvPoint[ii];
									float dist = norm(PT4[jj]-pf);
									if (dist<length[jj])
									{
										length[jj] = dist;
										indx[jj] = ii;
									}
								}
							}
						
							vector<int> count1;
							int cc = 0;
							if (1)
							{
							
								for(int jj = 0;jj<4;jj++)
								{
									float len = norm(PT4[jj]-PT4[(jj+1)%4]);
									float num = 0;
									if (indx[(jj+1)%4]>indx[jj])
									{
										for(int kk = 0;kk<indx[jj];kk++)
											num += norm(areaMatvPoint[kk]-areaMatvPoint[kk+1]);
										for(int kk = indx[(jj+1)%4];kk<areaMatvPoint.size()-1;kk++)
											num += norm(areaMatvPoint[kk]-areaMatvPoint[kk+1]);
									}
									else
									{
										for(int kk = indx[(jj+1)%4];kk<indx[jj]-1;kk++)
											num += norm(areaMatvPoint[kk]-areaMatvPoint[kk+1]);
									}

									if (abs(len-num)/len<0.1)
									{
										cc += 1;
										count1.push_back(1);
									}
									else
										count1.push_back(0);

								}
							}


							float count = 0;
							bool index = true;
							for(int jj = 0;jj<4;jj++)
							{
								if (length[jj]<min(rMaxRect.size.width,rMaxRect.size.height)*0.07)
								{
									if (PT4[jj].x<3 || PT4[jj].x>bw.cols-4 || PT4[jj].y<3 || PT4[jj].y>bw.rows-4)
									{
										count += 0.5;
										continue;
									}
									count+=1;
									if ((count==2 || count== 2.5) && (count1[jj-1]==1 && length[jj-1]<min(rMaxRect.size.width,rMaxRect.size.height)*0.07))
										index = false;
								}
				
							}
							if (cc<3  && cc>=1)
							{
								if (count<2)
								{
									PT4.clear();
									//return false;
								}
								else if (count<2.6 && index)
								{
									PT4.clear();
									//return false;
								}
								else
									break;
							}
							else if(cc>2)
								break;
							else
								PT4.clear();
						}

					}

				}
			}

			
			vvPoint.clear();
			findContours(bw.clone(),vvPoint,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
			int rectMaxArea =100;
			if (vvPoint.size()>0 && PT4.size()>0)
			{
				//遍历图像的轮廓，求出最大轮廓
				int rectArea =0;
				vector<Point> areaMatvPoint;
				RotatedRect rMaxRect;
				for (vector<vector<Point>>::iterator itr=vvPoint.begin();itr!=vvPoint.end();itr++)
				{
					RotatedRect rrect =minAreaRect(*itr);
					rectArea =rrect.size.area();
					if( rectArea >1000 && rrect.size.width>40 && rrect.size.height>40)
					{
						//Rect rMaxRect(rMaxRect.center.x-rMaxRect.size.width/2,rMaxRect.center.y-rMaxRect.size.height/2,rMaxRect.size.width,rMaxRect.size.height);
						rMaxRect =rrect;
						//rectMaxArea = rectArea;	
						areaMatvPoint.clear();
						areaMatvPoint = *itr;
						double ak = pointPolygonTest(PT4, Point2f(rMaxRect.center.x,rMaxRect.center.y), true);
						if (ak>=0)
							return false;
						else if (rectArea >rectMaxArea)
							rectMaxArea = rectArea;			
					}
				}
				if (rectMaxArea>1000)
					return true;
				/*if(rMaxRect.size.width>40 && rMaxRect.size.height>40)
				{
					end = clock();
					float aa = ((double)(end - start) / (CLOCKS_PER_SEC)) * 1000;
					if(PT4.size()==4)
					{
						double ak = pointPolygonTest(PT4, Point2f(rMaxRect.center.x,rMaxRect.center.y), true);
						if (ak<0)
							return true;
					}
					return false;
				}
				*/
			}
			/*if (rectMaxArea<500)
			{
				if (type1 && PT4.size()!=4)
					return true;
			}*/
			if (PT4.size()==4)
				return true;
			
			end = clock();
			float aa = ((double)(end - start) / (CLOCKS_PER_SEC)) * 1000;
			
			return false;
		}
	}
}

bool CCompare::Compare(Mat Src1,Mat Src2,int nThreshold,vector<CvRect> rect)
{
	Mat Src1gray,Src2gray;
	
	Mat nearminer;
	vector<Mat> imgageROIvector;
	if(Src1.empty()||Src2.empty()) 
	{
		return false;
	}
	else
	{
		
		if(Src1.rows!=Src2.rows&&Src1.cols!=Src2.cols&&Src1.channels ()!=Src2.channels ())
		{
			return false;
		}
		resize(Src1,Src1,Size(0,0),0.5,0.5);
		resize(Src2,Src2,Size(0,0),0.5,0.5);
		
		if (Src1.channels() == 1)
			Src1gray = Src1.clone();
		
		else
			cvtColor(Src1,Src1gray, CV_BGR2GRAY);
		

		if (Src2.channels() == 1)
			Src2gray = Src2.clone();

		else
			cvtColor(Src2,Src2gray, CV_BGR2GRAY);



		if(rect.size()>0)
		{
			bool isright1 = false;
			bool isright2 = true;

			GaussianBlur( Src1gray, Src1gray, Size( 5, 5), 0, 0 );
			GaussianBlur( Src2gray, Src2gray, Size( 5, 5), 0, 0 );

			cv::equalizeHist (Src1gray,Src1gray);
			cv::equalizeHist (Src2gray,Src2gray);
			
			//imshow("Src2gray",Src2gray);
			//cv::waitKey();
			double sumOfnum=0;
			nearminer=abs(Src1gray-Src2gray);
			for(int i=0;i<rect.size();i++)
			{
				double sumOfnum=0;
				Mat imageROI=nearminer(rect[i]);
				//Mat imageROI=image(Range::all(),Range::all());
				imgageROIvector.push_back (imageROI);
				for(int i=0;i<imageROI.rows;i++)
				{
					for(int j=0;j<imageROI.cols;j++)
					{
						if(imageROI.at<uchar>(i,j)>nThreshold)//150
						{
							sumOfnum++;
						}
					}
				}

				//sumvector.push_back(sumOfnum);

				//imshow("imageROI",imageROI);
				//cv::waitKey();
				if(sumOfnum<(rect[i].height+rect[i].width)*0.05)
				{
					isright1=true;
				}
				else
				{
					return false;
					//isright2= false;
				}
			}

			/*if(isright1&&!isright2)
			{
			return false;
			}
			else
			{
			return true;
			}*/
		}
		else
		{
			//********************************************************
			//jessie 2016-12-21 19:46:18
			//V0.0.4 添加边框检测，弥补光照或阴影带来的噪音问题

			Mat gray1,gray2;
			if(Src1.channels () == 1)
			{
				cvtColor(Src1,gray1,CV_GRAY2BGR);
				cvtColor(Src2,gray2,CV_GRAY2BGR);
			}
			else
			{
				Src1.copyTo(gray1);
				Src2.copyTo(gray2);
			}
			RotatedRect src_rect1=DetectBorder(gray1);
			RotatedRect src_rect2=DetectBorder(gray2);
			//********************************************************

			/*namedWindow("nearminer",CV_WINDOW_NORMAL);
			imshow("nearminer",Src1);
			cv::waitKey();
			namedWindow("nearminer",CV_WINDOW_NORMAL);
			imshow("nearminer",Src2);
			cv::waitKey();*/
			//**************************************
			//jessie 2016-10-20 16:19:54

			//V0.0.3 添加AutoLeavel,均衡光照或阴影带来的噪音问题

			/*CAutoLevel::*/AdjustLevelAuto1(Src1gray, Src1gray);
			/*CAutoLevel::*/AdjustLevelAuto1(Src2gray, Src2gray);



			/*namedWindow("true",CV_WINDOW_NORMAL);
			imshow("true",Src1);
			cv::waitKey();
			namedWindow("Src2",CV_WINDOW_NORMAL);
			imshow("Src2",Src2);
			cv::waitKey();*/
			//**************************************
			GaussianBlur( Src1gray, Src1gray, Size( 3, 3), 0, 0 );
		    GaussianBlur( Src2gray, Src2gray, Size( 3, 3), 0, 0 );



			//cv::equalizeHist (Src2gray,Src2gray);

			double sumOfnum=0;
			nearminer=abs(Src1gray-Src2gray);
			//namedWindow("nearminer",CV_WINDOW_NORMAL);
			//imshow("nearminer",Src1gray);
			//cv::waitKey();
			//namedWindow("nearminer",CV_WINDOW_NORMAL);
			//imshow("nearminer",Src2gray);
			//cv::waitKey();
			//namedWindow("nearminer",CV_WINDOW_NORMAL);
			//imshow("nearminer",nearminer);
			//cv::waitKey();

			//nearminer.copyTo(another);
			//Mat image=nearminer;
			//**************************************
			//jessie 2016-10-20 16:19:54
			//V0.0.3 
			//进一步去掉二值化后的噪声，添加形态学相减结果更好滤波。
			Mat element =getStructuringElement(MORPH_RECT,Size(2,2));
			erode(nearminer,nearminer,element,Point(-1,-1),0);
			//********************************************
			// meanStdDev(nearminer,nearfmeana,nearfstddeva,nearfmaska);
			// cout<<"nearfmeana"<<nearfmeana<<"              nearfstddeva"<<nearfstddeva<<endl;
			// cout<<nearminer<<endl;
			/*threshold(nearminer,nearminer,150,255,THRESH_BINARY );*/
			for(int i=0;i<nearminer.rows;i++)
			{
				for(int j=0;j<nearminer.cols;j++)
				{
					if(nearminer.at<uchar>(i,j)>nThreshold)//150
					{
						//  another.at<uchar>(i,j)=255;
						sumOfnum++;
						//image.at<uchar>(i,j)=255;
					}
					/*else
					{
					another.at<uchar>(i,j)=0;
					}*/

				}
			}
			/*namedWindow("true",CV_WINDOW_NORMAL);
			imshow("true",nearminer);
			cv::waitKey();*/
			// imshow("another",another);
			// threshold(nearminer, nearminer,nearfmeana.at<double>(0,0), 255, THRESH_BINARY);
			/*namedWindow("image",CV_WINDOW_NORMAL);
			imshow("image",image);*/
			//cv::waitKey();
			Mat grayimageROI1,grayimageROI2;
			if(sumOfnum<(nearminer.rows+nearminer.cols)*14.5)
				//有原来的2改为80 jessie 2016-12-21 16:47:01
			{
				//添加轮廓进一步对比，以消除阴影的影响

				if(sumOfnum<(nearminer.rows+nearminer.cols)*2)
				{
					return true;
				}
				else if(abs(src_rect1.center.x-src_rect2.center.x)<10
					&&abs(src_rect1.center.y-src_rect2.center.y)<10
					&&abs(src_rect1.size.width-src_rect2.size.width)<20
					&&abs(src_rect1.size.height-src_rect2.size.height)<20)
				{
					Mat Idst1 =crop(src_rect1,gray1);
					Mat Idst2=crop(src_rect1,gray2);
					Mat cannyimage1,cannyimage2;
					Canny( Idst1, cannyimage1, 50, 150,3 );
					Canny( Idst2, cannyimage2, 50, 150,3 );
					int num1=cv::countNonZero(cannyimage1);
					Mat minerimage=abs(cannyimage1-cannyimage2);
					int num=cv::countNonZero(minerimage);
					if(abs(num-num1)<900)
					{
						return true;
					}
					else
					{
						return false;
					}

				}
				
				else
				{
					return false;
				}
			}
			else if(sumOfnum<(nearminer.rows+nearminer.cols)*80)
				//有原来的2改为80 jessie 2016-12-21 16:47:01
			{
				//添加轮廓进一步对比，以消除阴影的影响
				
				if(abs(src_rect1.center.x-src_rect2.center.x)<20
				&&abs(src_rect1.center.y-src_rect2.center.y)<20
				&&abs(src_rect1.size.width-src_rect2.size.width)<40
				&&abs(src_rect1.size.height-src_rect2.size.height)<40)
				{
					//Rect rect1;
					//rect1.x=src_rect1.center.x-src_rect1.size.width*0.5;
					//rect1.y=src_rect1.center.y-src_rect1.size.height*0.5;
					//rect1.width=src_rect1.size.width;
					//rect1.height=src_rect1.size.height;
					//Mat imageROI1 = gray1(rect1);
					//imageROI1.copyTo(grayimageROI1);
					///*Rect rect2;
					//rect2.x=src_rect2.center.x-src_rect2.size.width*0.5;
					//rect2.y=src_rect2.center.y-src_rect2.size.height*0.5;
					//rect2.width=src_rect2.size.width;
					//rect2.height=src_rect2.size.height;*/
					//Mat imageROI2 = gray2(rect1);
					//imageROI2.copyTo(grayimageROI2);
					//Mat cannyimage1,cannyimage2;
					//Canny( imageROI1, cannyimage1, 50, 150,3 );
					//Canny( imageROI2, cannyimage2, 50, 150,3 );

					Mat Idst1 =crop(src_rect1,gray1);
					Mat Idst2=crop(src_rect1,gray2);
					Mat cannyimage1,cannyimage2;
					Canny( Idst1, cannyimage1, 50, 150,3 );
					Canny( Idst2, cannyimage2, 50, 150,3 );
					int num1=cv::countNonZero(cannyimage1);
					Mat minerimage=abs(cannyimage1-cannyimage2);
					/*cv::namedWindow ("minerimage",0);
					imshow("minerimage",minerimage);
					cv::waitKey();*/
					int numminer=cv::countNonZero(minerimage);
					if(abs(numminer-num1)<=(minerimage.rows+minerimage.cols))
					{
						return true;
					}
					else
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
			else
			{
				/*if(abs(src_rect1.center.x-src_rect2.center.x)<10
				&&abs(src_rect1.center.y-src_rect2.center.y)<10
				&&abs(src_rect1.size.width-src_rect2.size.width)<20
				&&abs(src_rect1.size.height-src_rect2.size.height)<20)
				{
				return true;
				}
				else
				{
				return false;
				}*/
				return false;
			}
		}
	}
	return false;

}
