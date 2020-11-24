#include "stdafx.h"
#include "Test_Book.h"
#include "../Enhancement/newColorEnhance.h"

namespace CTest_Book
{
	Mat Left_BookProcess(Mat src)
	{
		vector<vector<Point2f> > edges = CBookProcess::DetectEdges(src, 0);
		if (edges.size() != 2 || edges[0].size() < 2 || edges[1].size() < 2)
		{
			//上下边缘不合法
			return src;
		}
		//关键点提取
		vector<vector<Point2f> > keyPoints = CBookProcess::GetKeyPointsFromEdges(edges, 35);
		//绘制边缘
		//Mat dst = CBookProcess::DrawEdges(src, edges, keyPoints);

		//展平
		Mat flat = CBookProcess::Flatting(src, keyPoints);
		//去手指
		Mat figureHidding = CBookProcess::FingerHidding(flat, 0);
		figureHidding = CBookProcess::BookStretch_inpaint(figureHidding);
		//返回
		Mat dst = figureHidding;
		return dst;
	}

	Mat Right_BookProcess(Mat src)
	{
		vector<vector<Point2f> > edges = CBookProcess::DetectEdges(src, 1);
		if (edges.size() != 2 || edges[0].size() < 2 || edges[1].size() < 2)
		{
			//上下边缘不合法
			return src;
		}
		//关键点提取
		vector<vector<Point2f> > keyPoints = CBookProcess::GetKeyPointsFromEdges(edges, 35);
		//绘制边缘
		//Mat dst = CBookProcess::DrawEdges(src, edges, keyPoints);

		//展平
		Mat flat = CBookProcess::Flatting(src, keyPoints);
		flip(flat, flat, 1);
		//去手指
		Mat figureHidding = CBookProcess::FingerHidding(flat, 1);
		figureHidding = CBookProcess::BookStretch_inpaint(figureHidding);
		//返回
		Mat dst = figureHidding;
		return dst;
	}

	Mat Double_BookProcess(Mat src)
	{
		vector<vector<Point2f> > left_edges;
		vector<vector<Point2f> > right_edges;
		Mat temp = CBookProcess::DetectEdges_Double(src, left_edges, right_edges);

		Mat result_left, result_right;

		if (left_edges.size() == 2)
		{
			Mat src_left = src(Rect(0, 0, 3 * src.cols / 5, src.rows));
			//关键点提取
			vector<vector<Point2f> > keyPoints = CBookProcess::GetKeyPointsFromEdges(left_edges, 35);
			//展平
			Mat flat = CBookProcess::Flatting(src_left, keyPoints);
			//去手指
			Mat figureHidding = CBookProcess::FingerHidding(flat, 0);
			result_left = CBookProcess::BookStretch_inpaint(figureHidding);
			result_left = CnewColorEnhance::black_whiteEnhance(result_left,true);
		}
		if (right_edges.size() == 2)
		{
			Mat src_right = src(Rect(2 * src.cols / 5, 0, 3 * src.cols / 5, src.rows));
			//关键点提取
			vector<vector<Point2f> > keyPoints = CBookProcess::GetKeyPointsFromEdges(right_edges, 35);
			//展平
			Mat flat = CBookProcess::Flatting(src_right, keyPoints);
			flip(flat, flat, 1);
			//去手指
			Mat figureHidding = CBookProcess::FingerHidding(flat, 1);
			result_right = CBookProcess::BookStretch_inpaint(figureHidding);
			result_right = CnewColorEnhance::black_whiteEnhance(result_right,true);
		}

		Mat result;
		if (left_edges.size() == 2 && right_edges.size() == 2)
		{
			double scale = float(result_left.rows) / float(result_right.rows);
			Size dsize = Size(result_right.cols*scale, result_left.rows);
			resize(result_right, result_right, dsize);
			int col_1 = result_left.cols;
			int col_2 = result_right.cols;
			if (col_1 > col_2)
			{
				dsize = Size(result_right.cols * 2, result_right.rows);
				Mat result1 = Mat(dsize, result_right.type());
				Mat dst_left = result_left(Range::all(), Range(0, col_2));
				//hconcat(dst_left,result_right,result);
				dst_left.copyTo(result1(Rect(0, 0, result_right.cols, result_left.rows)));
				result_right.copyTo(result1(Rect(result_right.cols, 0, result_right.cols, result_right.rows)));
				result = result1;
			}
			else
			{
				dsize = Size(result_left.cols * 2, result_left.rows);
				Mat result1 = Mat(dsize, result_right.type());
				Mat dst_right = result_right(Range::all(), Range(0, col_1));
				//hconcat(result_left,dst_right,result);
				result_left.copyTo(result1(Rect(0, 0, result_left.cols, result_left.rows)));
				dst_right.copyTo(result1(Rect(result_left.cols, 0, dst_right.cols, dst_right.rows)));
				result = result1;
			}
		}
		else if (left_edges.size() == 2 && right_edges.size() != 2)
		{
			result = result_left;
		}
		else if (left_edges.size() != 2 && right_edges.size() == 2)
		{
			result = result_right;
		}
		else
		{
			result = src;
		}
		return result;
	}
}
