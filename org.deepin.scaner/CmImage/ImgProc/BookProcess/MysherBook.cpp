//#pragma once
#include "MysherBook.h"



//[in]src_left				输入书本左页图像
//[in]src_right             输入书本右页图像
//[out]mbokkInfo_left       输出左页上下边界轮廓
//[out]mbookInfo_right      输出右页上下边界轮廓
MYSHERIMAGE_API void mcvBookStretch_Getlandamarks(MImage *src_left, MImage *src_right, MBookInfo *mbookInfo_left, MBookInfo *mbookInfo_right)
{
	Mat mat_left = CAdapter::mimg2Mat(src_left);
	Mat mat_right = CAdapter::mimg2Mat(src_right);
	//Mat mat_left = imread("D://mysher//CmImage//trunk//Doc//original//2left.jpg");
	//Mat mat_right = imread("D://mysher//CmImage//trunk//Doc//original//2right.jpg");

	float height = 0;
	bool left_bool = false;
	bool right_bool = false;
	MPoint mpoint;
	if (mat_left.empty()==false)
	{
		//left_bool = CBookStretch::GetLandmarks(mat_left,mbookInfo_left,0);
		vector<vector<Point2f> > edges = CBookProcess::DetectEdges(mat_left,0);
		if(edges.size() == 2)
		{
			//关键点提取
			vector<vector<Point2f> > keyPoints = CBookProcess::GetKeyPointsFromEdges(edges,35);
			vector<Point2f> vPT = keyPoints[0];
			vector<Point2f> vPB = keyPoints[1];
			for (int idx=0;idx<vPT.size();idx++)
			{
				mpoint.x = vPT[idx].x;
				mpoint.y = vPT[idx].y;
				
				mbookInfo_left->_arrPT_Top[idx].x = mpoint.x;
				mbookInfo_left->_arrPT_Top[idx].y = mpoint.y;
			}
			mbookInfo_left->_arrPT_Top_Count = vPT.size();

			//转换下边界曲线关键点
			for (int idx=0;idx<vPB.size();idx++)
			{
				mpoint.x = vPB[idx].x;
				mpoint.y = vPB[idx].y;
				
				mbookInfo_left->_arrPT_Bottom[idx].x = mpoint.x;
				mbookInfo_left->_arrPT_Bottom[idx].y = mpoint.y;
			}
			mbookInfo_left->_arrPT_Bottom_Count = vPB.size();
		}
	}
	if (mat_right.empty()==false)
	{
		//right_bool = CBookStretch::GetLandmarks(mat_right,mbookInfo_right,1);
		vector<vector<Point2f> > edges = CBookProcess::DetectEdges(mat_right,1);
		if(edges.size() == 2)
		{
			//关键点提取
			vector<vector<Point2f> > keyPoints = CBookProcess::GetKeyPointsFromEdges(edges,35);
			vector<Point2f> vPT = keyPoints[0];
			vector<Point2f> vPB = keyPoints[1];
			for (int idx=0;idx<vPT.size();idx++)
			{
				mpoint.x = vPT[idx].x;
				mpoint.y = vPT[idx].y;
				
				mbookInfo_right->_arrPT_Top[idx].x = mpoint.x;
				mbookInfo_right->_arrPT_Top[idx].y = mpoint.y;
			}
			mbookInfo_right->_arrPT_Top_Count = vPT.size();

			//转换下边界曲线关键点
			for (int idx=0;idx<vPB.size();idx++)
			{
				mpoint.x = vPB[idx].x;
				mpoint.y = vPB[idx].y;
				
				mbookInfo_right->_arrPT_Bottom[idx].x = mpoint.x;
				mbookInfo_right->_arrPT_Bottom[idx].y = mpoint.y;
			}
			mbookInfo_right->_arrPT_Bottom_Count = vPB.size();
		}
	}
}

////合并提取书书本左右页上下边界轮廓函数
//[in]src				输入书本图像(左右页不分割）
//[out]mbokkInfo_left       输出左页上下边界轮廓
//[out]mbookInfo_right      输出右页上下边界轮廓
MYSHERIMAGE_API void mcvBookStretch_Getlandamarks_double(MImage *src,MBookInfo *mbookInfo_left,MBookInfo *mbookInfo_right)
{
	Mat mat_src = CAdapter::mimg2Mat(src);

	MPoint mpoint;
	if (mat_src.empty()==false)
	{
		vector<vector<Point2f> > left_edges;
		vector<vector<Point2f> > right_edges;
		Mat dst = CBookProcess::DetectEdges_Double( mat_src ,left_edges ,right_edges);
		if(left_edges.size() == 2)
		{
			//关键点提取
			vector<vector<Point2f> > keyPoints = CBookProcess::GetKeyPointsFromEdges(left_edges,35);
			vector<Point2f> vPT = keyPoints[0];
			vector<Point2f> vPB = keyPoints[1];
			for (int idx=0;idx<vPT.size();idx++)
			{
				mpoint.x = vPT[idx].x;
				mpoint.y = vPT[idx].y;
				
				mbookInfo_left->_arrPT_Top[idx].x = mpoint.x;
				mbookInfo_left->_arrPT_Top[idx].y = mpoint.y;
			}
			mbookInfo_left->_arrPT_Top_Count = vPT.size();

			//转换下边界曲线关键点
			for (int idx=0;idx<vPB.size();idx++)
			{
				mpoint.x = vPB[idx].x;
				mpoint.y = vPB[idx].y;
				
				mbookInfo_left->_arrPT_Bottom[idx].x = mpoint.x;
				mbookInfo_left->_arrPT_Bottom[idx].y = mpoint.y;
			}
			mbookInfo_left->_arrPT_Bottom_Count = vPB.size();
		}
		if(right_edges.size() == 2)
		{
			//关键点提取
			vector<vector<Point2f> > keyPoints = CBookProcess::GetKeyPointsFromEdges(right_edges,35);
			vector<Point2f> vPT = keyPoints[0];
			vector<Point2f> vPB = keyPoints[1];
			for (int idx=0;idx<vPT.size();idx++)
			{
				mpoint.x = vPT[idx].x;
				mpoint.y = vPT[idx].y;
				
				mbookInfo_right->_arrPT_Top[idx].x = mpoint.x;
				mbookInfo_right->_arrPT_Top[idx].y = mpoint.y;
			}
			mbookInfo_right->_arrPT_Top_Count = vPT.size();

			//转换下边界曲线关键点
			for (int idx=0;idx<vPB.size();idx++)
			{
				mpoint.x = vPB[idx].x;
				mpoint.y = vPB[idx].y;
				
				mbookInfo_right->_arrPT_Bottom[idx].x = mpoint.x;
				mbookInfo_right->_arrPT_Bottom[idx].y = mpoint.y;
			}
			mbookInfo_right->_arrPT_Bottom_Count = vPB.size();
		}
	}
}

MYSHERIMAGE_API MImage* mcvBookStretch_landamarksStretch_left(MImage *src)
{
	Mat mat_src = CAdapter::mimg2Mat(src);
	Mat result = CTest_Book::Left_BookProcess(mat_src);
	MImage*	dst = CAdapter::Mat2mimg(result);
	return dst;
}

MYSHERIMAGE_API MImage* mcvBookStretch_landamarksStretch_right(MImage *src)
{
	Mat mat_src = CAdapter::mimg2Mat(src);
	Mat result = CTest_Book::Right_BookProcess(mat_src);
	MImage*	dst = CAdapter::Mat2mimg(result);
	return dst;
}


MYSHERIMAGE_API MImage* mcvBookStretch_landamarksStretch_double(MImage *src,MBookInfo *mbookInfo_left,MBookInfo *mbookInfo_right,bool type)
{
	Mat mat_src = CAdapter::mimg2Mat(src);
	Mat result = CTest_Book::Double_BookProcess(mat_src);
	MImage*	dst = CAdapter::Mat2mimg(result);
	return dst;
}


//************************************************************************  
// 函数名称:    mcvBookStretch_CurveShow_double    
// 函数说明:    书本上下轮廓关键点显示  
// 函数参数:    MImage *src    输入原图
// 函数参数:    MBookInfo *mbookIfo    输入上下边界关键点
// 返 回 值:    MImage*   输出显示轮廓线及关键点的图像
//************************************************************************ 
MYSHERIMAGE_API MImage* mcvBookStretch_CurveShow_double(MImage *src,MBookInfo *mbookInfo_left,MBookInfo *mbookInfo_right)
{
	Mat mat_src= CAdapter::mimg2Mat(src);
	const int rNum0 =25;
	if (mbookInfo_left->_arrPT_Top_Count>0)
	{
		//转换曲边
		for (int idx=1;idx <mbookInfo_left->_arrPT_Top_Count;idx++)
		{
			circle(mat_src,Point(mbookInfo_left->_arrPT_Top[idx].x,mbookInfo_left->_arrPT_Top[idx].y),0,Scalar(0,255,255),rNum0);
			line(mat_src,Point(mbookInfo_left->_arrPT_Top[idx-1].x,mbookInfo_left->_arrPT_Top[idx-1].y),Point(mbookInfo_left->_arrPT_Top[idx].x,mbookInfo_left->_arrPT_Top[idx].y),Scalar(0,255,0),10,CV_AA);  
		}
		for (int idx =1;idx <mbookInfo_left->_arrPT_Bottom_Count;idx++)
		{
			circle(mat_src,Point(mbookInfo_left->_arrPT_Bottom[idx].x,mbookInfo_left->_arrPT_Bottom[idx].y),0,Scalar(0,255,255),rNum0);
			line(mat_src,Point(mbookInfo_left->_arrPT_Bottom[idx-1].x,mbookInfo_left->_arrPT_Bottom[idx-1].y),Point(mbookInfo_left->_arrPT_Bottom[idx].x,mbookInfo_left->_arrPT_Bottom[idx].y),Scalar(0,255,0),10,CV_AA);  
		}

		//标注起止位置
		const int rNum =30;
		circle(mat_src,Point(mbookInfo_left->_arrPT_Top[0].x,mbookInfo_left->_arrPT_Top[0].y),0,Scalar(0,0,255),rNum);
		circle(mat_src,Point(mbookInfo_left->_arrPT_Top[mbookInfo_left->_arrPT_Top_Count-1].x,mbookInfo_left->_arrPT_Top[mbookInfo_left->_arrPT_Top_Count-1].y),0,Scalar(0,0,255),rNum);
		circle(mat_src,Point(mbookInfo_left->_arrPT_Bottom[0].x,mbookInfo_left->_arrPT_Bottom[0].y),0,Scalar(0,0,255),rNum);
		circle(mat_src,Point(mbookInfo_left->_arrPT_Bottom[mbookInfo_left->_arrPT_Bottom_Count-1].x,mbookInfo_left->_arrPT_Bottom[mbookInfo_left->_arrPT_Bottom_Count-1].y),0,Scalar(0,0,255),rNum);
	}
	if (mbookInfo_right->_arrPT_Top_Count>0)
	{
		if (mat_src.cols>mat_src.rows)
		{
			//转换曲边
			for (int idx=1;idx <mbookInfo_right->_arrPT_Top_Count;idx++)
			{
				circle(mat_src,Point(2*mat_src.cols/5+mbookInfo_right->_arrPT_Top[idx].x,mbookInfo_right->_arrPT_Top[idx].y),0,Scalar(0,255,255),rNum0);
				line(mat_src,Point(2*mat_src.cols/5+mbookInfo_right->_arrPT_Top[idx-1].x,mbookInfo_right->_arrPT_Top[idx-1].y),Point(2*mat_src.cols/5+mbookInfo_right->_arrPT_Top[idx].x,mbookInfo_right->_arrPT_Top[idx].y),Scalar(0,255,0),10,CV_AA);  
			}
			for (int idx =1;idx <mbookInfo_right->_arrPT_Bottom_Count;idx++)
			{
				circle(mat_src,Point(2*mat_src.cols/5+mbookInfo_right->_arrPT_Bottom[idx].x,mbookInfo_right->_arrPT_Bottom[idx].y),0,Scalar(0,255,255),rNum0);
				line(mat_src,Point(2*mat_src.cols/5+mbookInfo_right->_arrPT_Bottom[idx-1].x,mbookInfo_right->_arrPT_Bottom[idx-1].y),Point(2*mat_src.cols/5+mbookInfo_right->_arrPT_Bottom[idx].x,mbookInfo_right->_arrPT_Bottom[idx].y),Scalar(0,255,0),10,CV_AA);  
			}

			//标注起止位置
			const int rNum =30;
			circle(mat_src,Point(2*mat_src.cols/5+mbookInfo_right->_arrPT_Top[0].x,mbookInfo_right->_arrPT_Top[0].y),0,Scalar(0,0,255),rNum);
			circle(mat_src,Point(2*mat_src.cols/5+mbookInfo_right->_arrPT_Top[mbookInfo_right->_arrPT_Top_Count-1].x,mbookInfo_right->_arrPT_Top[mbookInfo_right->_arrPT_Top_Count-1].y),0,Scalar(0,0,255),rNum);
			circle(mat_src,Point(2*mat_src.cols/5+mbookInfo_right->_arrPT_Bottom[0].x,mbookInfo_right->_arrPT_Bottom[0].y),0,Scalar(0,0,255),rNum);
			circle(mat_src,Point(2*mat_src.cols/5+mbookInfo_right->_arrPT_Bottom[mbookInfo_right->_arrPT_Bottom_Count-1].x,mbookInfo_right->_arrPT_Bottom[mbookInfo_right->_arrPT_Bottom_Count-1].y),0,Scalar(0,0,255),rNum);
		}
		else
		{
			//转换曲边
			for (int idx=1;idx <mbookInfo_right->_arrPT_Top_Count;idx++)
			{
				circle(mat_src,Point(mbookInfo_right->_arrPT_Top[idx].x,mbookInfo_right->_arrPT_Top[idx].y),0,Scalar(0,255,255),rNum0);
				line(mat_src,Point(mbookInfo_right->_arrPT_Top[idx-1].x,mbookInfo_right->_arrPT_Top[idx-1].y),Point(mbookInfo_right->_arrPT_Top[idx].x,mbookInfo_right->_arrPT_Top[idx].y),Scalar(0,255,0),10,CV_AA);  
			}
			for (int idx =1;idx <mbookInfo_right->_arrPT_Bottom_Count;idx++)
			{
				circle(mat_src,Point(mbookInfo_right->_arrPT_Bottom[idx].x,mbookInfo_right->_arrPT_Bottom[idx].y),0,Scalar(0,255,255),rNum0);
				line(mat_src,Point(mbookInfo_right->_arrPT_Bottom[idx-1].x,mbookInfo_right->_arrPT_Bottom[idx-1].y),Point(mbookInfo_right->_arrPT_Bottom[idx].x,mbookInfo_right->_arrPT_Bottom[idx].y),Scalar(0,255,0),10,CV_AA);  
			}

			//标注起止位置
			const int rNum =30;
			circle(mat_src,Point(mbookInfo_right->_arrPT_Top[0].x,mbookInfo_right->_arrPT_Top[0].y),0,Scalar(0,0,255),rNum);
			circle(mat_src,Point(mbookInfo_right->_arrPT_Top[mbookInfo_right->_arrPT_Top_Count-1].x,mbookInfo_right->_arrPT_Top[mbookInfo_right->_arrPT_Top_Count-1].y),0,Scalar(0,0,255),rNum);
			circle(mat_src,Point(mbookInfo_right->_arrPT_Bottom[0].x,mbookInfo_right->_arrPT_Bottom[0].y),0,Scalar(0,0,255),rNum);
			circle(mat_src,Point(mbookInfo_right->_arrPT_Bottom[mbookInfo_right->_arrPT_Bottom_Count-1].x,mbookInfo_right->_arrPT_Bottom[mbookInfo_right->_arrPT_Bottom_Count-1].y),0,Scalar(0,0,255),rNum);
		
		}
	}
	MImage *reDst = CAdapter::Mat2mimg(mat_src);
	return reDst;
}

//书本左右页展平合并函数
//当两张图像上下边界轮廓都正确，输出展平拼接后图像
//当只有一张图像轮廓正确，输出正确的那张图像展平后图像
//当轮廓都不正确时，输出0
//[in]src_left				输入书本左页图像
//[in]src_right             输入书本右页图像
//[in]mbokkInfo_left_old       输入左页上下边界原轮廓
//[in]mbokkInfo_left_new     输入左页上下边界拖动后轮廓
//[in]mbookInfo_right_old      输入右页上下边界原轮廓
//[in]mbookInfo_right_new      输入右页上下边界拖动后轮廓
//[in]type              是否去手指标识（true:为去手指,false：为不去手指）
//[out]                    输出展平后图像

MYSHERIMAGE_API MImage* mcvBookStretch_landamarksStretch(MImage *src_left,MImage *src_right,MBookInfo *mbookInfo_left_old,MBookInfo *mbookInfo_left_new,MBookInfo *mbookInfo_right_old,MBookInfo *mbookInfo_right_new,bool type)
{
	Mat mat_left = CAdapter::mimg2Mat(src_left);
	Mat mat_right = CAdapter::mimg2Mat(src_right);
	
	//Mat mat_left = imread("D://mysher//CmImage//trunk//Doc//original//2left.jpg");
	//Mat mat_right = imread("D://mysher//CmImage//trunk//Doc//original//2right.jpg");
	//type = false;
	float height = 0;
	Mat result_right;
	Mat result_left;
	//mbookInfo_left_old->_arrPT_Top_Count = 0;
	//mbookInfo_right_old->_arrPT_Top_Count = 0;
	if (mbookInfo_left_old->_arrPT_Top_Count>0)
	{
		//result_left = CBookStretch::LandmarksCurveStretch(mat_left, mbookInfo_left, height);
		//关键点数据类型转换
		vector<Point2f> vPT;
		vector<Point2f> vPB;

		vector<Point2f> vPT_old;
		vector<Point2f> vPB_old;
		for (int idx=0;idx <mbookInfo_left_old->_arrPT_Top_Count;idx++)
			vPT_old.push_back(Point2f(mbookInfo_left_old->_arrPT_Top[idx].x,mbookInfo_left_old->_arrPT_Top[idx].y));
	
		for (int idx =0;idx <mbookInfo_left_old->_arrPT_Bottom_Count;idx++)
			vPB_old.push_back(Point2f(mbookInfo_left_old->_arrPT_Bottom[idx].x,mbookInfo_left_old->_arrPT_Bottom[idx].y));


		if (mbookInfo_left_new->_arrPT_Top_Count>0)
		{
			for (int idx=0;idx <mbookInfo_left_new->_arrPT_Top_Count;idx++)
				vPT.push_back(Point2f(mbookInfo_left_new->_arrPT_Top[idx].x,mbookInfo_left_new->_arrPT_Top[idx].y));
	
			for (int idx =0;idx <mbookInfo_left_new->_arrPT_Bottom_Count;idx++)
				vPB.push_back(Point2f(mbookInfo_left_new->_arrPT_Bottom[idx].x,mbookInfo_left_new->_arrPT_Bottom[idx].y));
		}
		else
		{
			vPT = vPT_old;
			vPB = vPB_old;
		}

		if(vPT.size()<2 || vPT_old.size()<2 || vPB.size()<2 || vPB_old.size()<2)
		{
			return src_left;
		}

		bool flag = false;
		if (mbookInfo_left_new->_arrPT_Top_Count>0)
		{
			if ((vPT.size() != vPB.size()) || (vPT.size() != vPT_old.size() || vPB.size() != vPB_old.size()) || (vPT.size() != 2 && vPT.size()!=36))
				flag = true;

			if (vPT.size() == vPB.size())
			{
				for (int ii = 0;ii<vPT.size();ii++)
				{
					float dist_T = norm(vPT[ii]-vPT_old[ii]);
					float dist_B = norm(vPB[ii]-vPB_old[ii]);
					if (dist_T>0.1 || dist_B>0.1)
						flag = true;
				}
			}
		}

		vector<vector<Point2f> > keyPoints;
		keyPoints.push_back(vPT);
		keyPoints.push_back(vPB);
		//判断关键点是否符合条件，并插值恢复
		if (flag)//(vPT.size()!=36 && vPT.size()!=2) || (vPB.size()!=36 && vPB.size()!=2))
		{
			//vPT = CBookStretch::LandmarksInterp(vPT);
			vector<vector<Point2f> > edges = CBookProcess::GetEdgesFromKeyPoints(keyPoints);
			keyPoints = CBookProcess::GetKeyPointsFromEdges(edges,35,false);
		}
		result_left = CBookProcess::Flatting(mat_left,keyPoints);
		if(type)
		{
			//Mat mat_dst = CBookStretch::FingerRemoval_Single(result_left,false);
			vector<vector<Point> > edges;
			Mat mat_dst = CBookProcess::FingerHidding(result_left, 0);
			result_left = mat_dst;
		}
	}
		
	if (mbookInfo_right_old->_arrPT_Top_Count>0)
	{
		//flip(mat_right,mat_right,1);
		//result_right = CBookStretch::LandmarksCurveStretch(mat_right, mbookInfo_right, height);
		//关键点数据类型转换
		vector<Point2f> vPT;
		vector<Point2f> vPB;

		vector<Point2f> vPT_old;
		vector<Point2f> vPB_old;
		for (int idx=0;idx <mbookInfo_right_old->_arrPT_Top_Count;idx++)
			vPT_old.push_back(Point2f(mbookInfo_right_old->_arrPT_Top[idx].x,mbookInfo_right_old->_arrPT_Top[idx].y));
	
		for (int idx =0;idx <mbookInfo_right_old->_arrPT_Bottom_Count;idx++)
			vPB_old.push_back(Point2f(mbookInfo_right_old->_arrPT_Bottom[idx].x,mbookInfo_right_old->_arrPT_Bottom[idx].y));

		if (mbookInfo_right_new->_arrPT_Top_Count>0)
		{
			for (int idx=0;idx <mbookInfo_right_new->_arrPT_Top_Count;idx++)
				vPT.push_back(Point2f(mbookInfo_right_new->_arrPT_Top[idx].x,mbookInfo_right_new->_arrPT_Top[idx].y));
	
			for (int idx =0;idx <mbookInfo_right_new->_arrPT_Bottom_Count;idx++)
				vPB.push_back(Point2f(mbookInfo_right_new->_arrPT_Bottom[idx].x,mbookInfo_right_new->_arrPT_Bottom[idx].y));
		}
		else
		{
			vPT = vPT_old;
			vPB = vPB_old;
		}

		if(vPT.size()<2 || vPT_old.size()<2 || vPB.size()<2 || vPB_old.size()<2)
		{
			return src_right;
		}

		bool flag = false;
		if (mbookInfo_right_new->_arrPT_Top_Count>0)
		{
			if ((vPT.size() != vPB.size()) || (vPT.size() != vPT_old.size() || vPB.size() != vPB_old.size()) || (vPT.size() != 2 && vPT.size()!=36))
				flag = true;

			if (vPT.size() == vPB.size())
			{
				for (int ii = 0;ii<vPT.size();ii++)
				{
					float dist_T = norm(vPT[ii]-vPT_old[ii]);
					float dist_B = norm(vPB[ii]-vPB_old[ii]);
					if (dist_T>0.1 || dist_B>0.1)
						flag = true;
				}
			}
		}

		vector<vector<Point2f> > keyPoints;
		keyPoints.push_back(vPT);
		keyPoints.push_back(vPB);
		//判断关键点是否符合条件，并插值恢复
		if (flag)//(vPT.size()!=36 && vPT.size()!=2) || (vPB.size()!=36 && vPB.size()!=2))
		{
			//vPT = CBookStretch::LandmarksInterp(vPT);
			vector<vector<Point2f> > edges = CBookProcess::GetEdgesFromKeyPoints(keyPoints);
			keyPoints = CBookProcess::GetKeyPointsFromEdges(edges,35,false);
		}
		result_right = CBookProcess::Flatting(mat_right,keyPoints);
		if(type)
		{
			//Mat mat_dst = CBookStretch::FingerRemoval_Single(result_right,false);
			vector<vector<Point> > edges;
			Mat mat_dst = CBookProcess::FingerHidding(result_right, 0);
			result_right = mat_dst;
		}
		flip(result_right,result_right,1);
	}


	if (mbookInfo_left_old->_arrPT_Top_Count>0 && mbookInfo_right_old->_arrPT_Top_Count==0)
	{
		MImage* dst;
		if (result_left.rows<10 || result_left.cols<10 || result_left.empty())
			dst = src_left;
		else
			dst = CAdapter::Mat2mimg(result_left);
		return dst;
	}
	else if (mbookInfo_left_old->_arrPT_Top_Count==0 && mbookInfo_right_old->_arrPT_Top_Count>0)
	{
		MImage* dst;
		if (result_right.cols<10 || result_right.rows<10 || result_right.empty())
			dst = src_right;
		else
			dst = CAdapter::Mat2mimg(result_right);
		return dst;
	}
	else if (mbookInfo_left_old->_arrPT_Top_Count>0 && mbookInfo_right_old->_arrPT_Top_Count>0)
	{
		Mat result;
		int col_1 = result_left.cols;
        int col_2 = result_right.cols;
		double scale= result_left.rows/result_right.rows;
		Size dsize = Size(result_right.cols*scale,result_left.rows);
		resize(result_right, result_right, dsize);
        if (col_1>col_2)
        {
            dsize = Size(result_right.cols*2,result_right.rows);
			Mat result1 = Mat(dsize,CV_8UC3);
            Mat dst_left = result_left(Range::all(),Range(0,col_2));
            //hconcat(dst_left,result_right,result);
			dst_left.copyTo(result1(Rect(0,0,result_right.cols,result_left.rows)));
			result_right.copyTo(result1(Rect(result_right.cols,0,result_right.cols,result_right.rows)));
			result = result1;
        }
        else
        {
			dsize = Size(result_left.cols*2,result_left.rows);
			Mat result1 = Mat(dsize,CV_8UC3);
            Mat dst_right = result_right(Range::all(),Range(0,col_1));
            //hconcat(result_left,dst_right,result);
			result_left.copyTo(result1(Rect(0,0,result_left.cols,result_left.rows)));
			dst_right.copyTo(result1(Rect(result_left.cols,0,dst_right.cols,dst_right.rows)));
			result = result1;
        }
		MImage* dst = CAdapter::Mat2mimg(result);
		return dst;
	}
	else
	{
		MImage* result = 0;
		if (mat_left.empty())
			result = CAdapter::Mat2mimg(mat_right);
		else
			result = CAdapter::Mat2mimg(mat_left);
		return result;
	}
}


//************************************************************************  
// 函数名称:    mcvBookStretch_CurveShow    
// 函数说明:    书本上下轮廓关键点显示  
// 函数参数:    MImage *src    输入原图
// 函数参数:    MBookInfo *mbookIfo    输入上下边界关键点
// 返 回 值:    MImage*   输出显示轮廓线及关键点的图像
//************************************************************************ 
MYSHERIMAGE_API MImage* mcvBookStretch_CurveShow(MImage *src,MBookInfo *mbookInfo)
{
	Mat mat_src= CAdapter::mimg2Mat(src);
	const int rNum0 =25;
	//转换曲边
	for (int idx=1;idx <mbookInfo->_arrPT_Top_Count;idx++)
	{
		circle(mat_src,Point(mbookInfo->_arrPT_Top[idx].x,mbookInfo->_arrPT_Top[idx].y),0,Scalar(0,255,255),rNum0);
		line(mat_src,Point(mbookInfo->_arrPT_Top[idx-1].x,mbookInfo->_arrPT_Top[idx-1].y),Point(mbookInfo->_arrPT_Top[idx].x,mbookInfo->_arrPT_Top[idx].y),Scalar(0,255,0),10,CV_AA);  
	}
	for (int idx =1;idx <mbookInfo->_arrPT_Bottom_Count;idx++)
	{
		circle(mat_src,Point(mbookInfo->_arrPT_Bottom[idx].x,mbookInfo->_arrPT_Bottom[idx].y),0,Scalar(0,255,255),rNum0);
		line(mat_src,Point(mbookInfo->_arrPT_Bottom[idx-1].x,mbookInfo->_arrPT_Bottom[idx-1].y),Point(mbookInfo->_arrPT_Bottom[idx].x,mbookInfo->_arrPT_Bottom[idx].y),Scalar(0,255,0),10,CV_AA);  
	}

	//标注起止位置
	const int rNum =30;
	circle(mat_src,Point(mbookInfo->_arrPT_Top[0].x,mbookInfo->_arrPT_Top[0].y),0,Scalar(0,0,255),rNum);
	circle(mat_src,Point(mbookInfo->_arrPT_Top[mbookInfo->_arrPT_Top_Count-1].x,mbookInfo->_arrPT_Top[mbookInfo->_arrPT_Top_Count-1].y),0,Scalar(0,0,255),rNum);
	circle(mat_src,Point(mbookInfo->_arrPT_Bottom[0].x,mbookInfo->_arrPT_Bottom[0].y),0,Scalar(0,0,255),rNum);
	circle(mat_src,Point(mbookInfo->_arrPT_Bottom[mbookInfo->_arrPT_Bottom_Count-1].x,mbookInfo->_arrPT_Bottom[mbookInfo->_arrPT_Bottom_Count-1].y),0,Scalar(0,0,255),rNum);

	MImage *reDst = CAdapter::Mat2mimg(mat_src);
	return reDst;
}

//*****************************************************
//函数说明：  书本手指检测去除
//MImage *src  输入图像
//int type    书页标记：0为双页，1为左页，2为右页
//返回参数 MImage* 
//*****************************************************
MYSHERIMAGE_API MImage* mcvBookStretch_FingerRemoval(MImage *src_rgb,int type)
{
	Mat src = CAdapter::mimg2Mat(src_rgb);
	Mat mat_dst;
	/*if (type ==0)
	{
		mat_dst = CBookStretch::FingerRemoval(src);
	}
	else*/
	if(true)
	{
		int dtype = 0;
		if (type==2)
			dtype = 1;
		mat_dst = CBookProcess::FingerHidding(src,dtype);
	}
	MImage* dst = CAdapter::Mat2mimg(mat_dst);
	return  dst;
}

//************************************************************************  
// 函数名称:    mcvBookStretch_Enhance  
// 函数说明:    图像灰度对比度增强
// 函数参数:    Mat src    输入图像
// 返 回 值:    Mat  输出增强后灰度图像
//************************************************************************ 
MYSHERIMAGE_API MImage* mcvBookStretch_Enhance(MImage *src_rgb)
{
	Mat src = CAdapter::mimg2Mat(src_rgb);

	double scale;
	Size dsize;
	Mat src_img;
	bool flag = false;
	if (flag)//src.rows>2200)
	{
		scale = 0.8+0.2*(1-sqrt(sqrt(1-(2200.0/src.rows))));
		dsize = Size(src.cols*scale,src.rows*scale);
		resize(src, src_img, dsize);
	}
	else
	{
		src_img = src;
	}
	Mat mat_dst = CBookProcess::Enhance(src_img,5);
	
	//IplImage *iplSrc = &(IplImage)mat_dst;
	//CBrightnessBalance::BrightnessBalance(iplSrc);

	//mat_dst = CBookStretch::BookIllustrations(src,mat_dst);
	if (flag)
	{
		dsize = Size(src.cols,src.rows);
		Mat dst_img;
		resize(mat_dst, dst_img, dsize);
		mat_dst = dst_img;
	}

	MImage* dst;
	dst = CAdapter::Mat2mimg(mat_dst);
	return  dst;
}

//************************************************************************  
// 函数名称:    mcvBookStretch_inpaint
// 函数说明:    曲面展平后图像黑边修复
// 函数参数:    Mat src     输入图像
// 返 回 值:    Mat
//************************************************************************ 
MYSHERIMAGE_API MImage* mcvBookStretch_inpaint(MImage *src_rgb,int type)
{
	Mat src = CAdapter::mimg2Mat(src_rgb);
	Mat mat_dst = CBookProcess::BookStretch_inpaint(src,type);
	MImage* dst = CAdapter::Mat2mimg(mat_dst);
	return  dst;
}

MYSHERIMAGE_API MRectRArray mcvDetectRect1(MImage* src1, MRect rect)
{
	MRectRArray mRectArray;
	const int halfBorder = 20;
	cout << "2-1" << endl;
	Mat matSrc = CAdapter::mimg2Mat(src1);
	if (matSrc.data == 0)
	{
		return mRectArray;
	}
	cout << "2-2" << endl;
	MRectR rrect;
	bool bresult = false;
#if 1
	Point pt[4];
	Rect cvR = Rect(rect.m_nLeft, rect.m_nTop, rect.m_nRight - rect.m_nLeft, rect.m_nBotton - rect.m_nTop);

	bresult = CDetectRectByContours::DetectRect(matSrc, rrect.m_fAngle, pt, cvR);
#else
	IplImage *iplSrc = &(IplImage)matSrc;
	CvPoint pt[4];
	CvRect cvR = cvRect(rect.m_nLeft, rect.m_nTop, rect.m_nRight - rect.m_nLeft, rect.m_nBotton - rect.m_nTop);

	bresult = CDetectRectByContours::DetectRect(iplSrc, rrect.m_fAngle, pt, cvR);
	rrect.m_fAngle = fAngle;
#endif
	cout << "bresult=:" << bresult << endl;
	for (int idx = 0; idx <4; idx++)
	{
		rrect.m_pt[idx].x = pt[idx].x;
		rrect.m_pt[idx].y = pt[idx].y;
	}

	if (bresult = true)
	{
		mRectArray.m_nCount = 1;
		mRectArray.m_rectR[0] = rrect;
	}
	cout << "2-3" << endl;
	return mRectArray;
}