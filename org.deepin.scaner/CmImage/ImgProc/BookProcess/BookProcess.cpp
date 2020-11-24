#include "stdafx.h"
#include "BookProcess.h"

//ȥ����ֵͼ���Ե��ͻ����
//uthreshold��vthreshold�ֱ��ʾͻ�����Ŀ����ֵ�͸߶���ֵ
//type����ͻ��������ɫ��0��ʾ��ɫ��1�����ɫ 
void CBookProcess::delete_jut(Mat& src, Mat& dst, int uthreshold, int vthreshold, int type)
{
	
	int threshold;
	src.copyTo(dst);
	rectangle(dst, Point(0, 0), Point(dst.cols,dst.rows), Scalar(0), 2);
	int height = dst.rows;
	int width = dst.cols;
	int k;  //����ѭ���������ݵ��ⲿ
	for (int i = 0; i < height - 1; i++)
	{
		uchar* p = dst.ptr<uchar>(i);
		for (int j = 0; j < width - 1; j++)
		{
			if (type == 0)
			{
				//������
				if (p[j] == 255 && p[j + 1] == 0)
				{
					if (j + uthreshold >= width)
					{
						for (int k = j + 1; k < width; k++)
							p[k] = 255;
					}
					else
					{
						for (k = j + 2; k <= j + uthreshold; k++)
						{
							if (p[k] == 255) break;
						}
						if (p[k] == 255)
						{
							for (int h = j + 1; h < k; h++)
								p[h] = 255;
						}
					}
				}
				//������
				if (p[j] == 255 && p[j + width] == 0)
				{
					if (i + vthreshold >= height)
					{
						for (k = j + width; k < j + (height - i)*width; k += width)
							p[k] = 255;
					}
					else
					{
						for (k = j + 2 * width; k <= j + vthreshold*width; k += width)
						{
							if (p[k] == 255) break;
						}
						if (p[k] == 255)
						{
							for (int h = j + width; h < k; h += width)
								p[h] = 255;
						}
					}
				}
			}
			else  //type = 1
			{
				//������
				if (p[j] == 0 && p[j + 1] == 255)
				{
					if (j + uthreshold >= width)
					{
						for (int k = j + 1; k < width; k++)
							p[k] = 0;
					}
					else
					{
						for (k = j + 2; k <= j + uthreshold; k++)
						{
							if (p[k] == 0) 
								break;
						}
						if (p[k] == 0)
						{
							for (int h = j + 1; h < k; h++)
								p[h] = 0;
						}
					}
				}
				//������
				if (p[j] == 0 && p[j + width] == 255)
				{
					if (i + vthreshold >= height)
					{
						for (k = j + width; k < j + (height - i)*width; k += width)
							p[k] = 0;
					}
					else
					{
						for (k = j + 2 * width; k <= j + vthreshold*width; k += width)
						{
							if (p[k] == 0) 
								break;
						}
						if (p[k] == 0)
						{
							for (int h = j + width; h < k; h += width)
								p[h] = 0;
						}
					}
				}
			}
		}
	}
}


//**************************************************************
//�ϲ�����ҳ������
//�����⣬����鱾�����±�Ե�����������㷨�������������չƽ
//���룺
//Mat src:�鱾ԭͼ
//�����
//vector<vector<Point2f> >& left_edges:�鱾��ҳ���±�Ե�����а�������������
//��1����һ��Vector<Point>���ϱ�Ե
//��2���ڶ���Vector<Point>���±�Ե
//vector<vector<Point2f> >& rightt_edges:�鱾��ҳ���±�Ե�����а�������������
//��1����һ��Vector<Point>���ϱ�Ե
//��2���ڶ���Vector<Point>���±�Ե
//int :���ز�����-1��ʧ�ܣ�0����ҳ�ɹ���1����ҳ�ɹ���
//***************************************************************
Mat CBookProcess::DetectEdges_Double(Mat src ,vector<vector<Point2f> >& left_edges ,vector<vector<Point2f> >& right_edges)
{
	if (src.empty())
	{
		return src;
	}

	int col = src.cols;
	double scale;
	Mat mat_src = CBookProcess::Resize(src ,scale);
	Mat bw = CBookProcess::Threshold(mat_src);
	line(bw,Point(2*bw.cols/5,bw.rows/2),Point(3*bw.cols/5,bw.rows/2),Scalar(255),10,CV_AA);
	vector<Point> areaMatvPoint = CBookProcess::GetMaxBookContour( bw );

	vector<Point> left_PTT;
	vector<Point> right_PBB;
	Point2f p2t;
	if (areaMatvPoint.size()>200)
	{
		Point left_point4(mat_src.cols/2,mat_src.rows/2-20);
		Point left_point3(mat_src.cols/2,mat_src.rows/2+20);
		Point right_point4(mat_src.cols/2,mat_src.rows/2-20);
		Point right_point3(mat_src.cols/2,mat_src.rows/2+20);
		int left_ind4 = -1;
		int left_ind3 = -1;;
		int right_ind4 = -1;
		int right_ind3 = -1;
		for (int ii = 0;ii<areaMatvPoint.size();ii++)
		{
			if (areaMatvPoint[ii].x==int(2*mat_src.cols/5))
			{
				if (areaMatvPoint[ii].y<right_point4.y)
				{
					right_point4 = areaMatvPoint[ii];
					right_ind4 = ii;
				}
				if (areaMatvPoint[ii].y>right_point3.y)
				{
					right_point3 = areaMatvPoint[ii];
					right_ind3 = ii;
				}
			}
			if (areaMatvPoint[ii].x==int(3*mat_src.cols/5))
			{
				if (areaMatvPoint[ii].y<left_point4.y)
				{
					left_point4 = areaMatvPoint[ii];
					left_ind4 = ii;
				}
				if (areaMatvPoint[ii].y>left_point3.y)
				{
					left_point3 = areaMatvPoint[ii];
					left_ind3 = ii;
				}
			}
		}

		if (left_ind3 != -1 && left_ind4 != -1)
		{
			if (left_ind4<left_ind3)
			{
				for (int ii = left_ind4;ii<=left_ind3;ii++)
				{
					left_PTT.push_back(areaMatvPoint[ii]);
					//circle(mat_src,areaMatvPoint[ii],0,Scalar(0,0,255),10);
				}
			}
			else
			{
				for (int ii = left_ind4;ii<areaMatvPoint.size();ii++)
				{
					left_PTT.push_back(areaMatvPoint[ii]);
					//circle(mat_src,areaMatvPoint[ii],0,Scalar(0,0,255),10);
				}
				for (int ii = 0;ii<=left_ind3;ii++)
				{
					left_PTT.push_back(areaMatvPoint[ii]);
					//circle(mat_src,areaMatvPoint[ii],0,Scalar(0,0,255),10);
				}
			}
		}

		if (right_ind3 != -1 && right_ind4 != -1)
		{
			if (right_ind3<right_ind4)
			{
				for (int ii = right_ind4;ii>=right_ind3;ii--)
				{
					p2t = areaMatvPoint[ii];
					p2t.x = mat_src.cols-p2t.x;
					right_PBB.push_back(p2t);
				}
			}
			else
			{
				for (int ii =right_ind4;ii>=0;ii--)
				{
					p2t = areaMatvPoint[ii];
					p2t.x = mat_src.cols-p2t.x;
					right_PBB.push_back(p2t);
					//circle(mat_src,p2t,0,Scalar(255,0,0),10);
				}
				for (int ii = areaMatvPoint.size()-1;ii>=right_ind3;ii--)
				{
					p2t = areaMatvPoint[ii];
					p2t.x = mat_src.cols-p2t.x;
					right_PBB.push_back(p2t);
					//circle(mat_src,p2t,0,Scalar(0,255,0),10);
				}
			}
		}
		/*int min_y = mat_src.rows;
		int max_y = 0;
		for (int ii = 0;ii<areaMatvPoint.size();ii++)
		{
			if (min_y>areaMatvPoint[ii].y)
				min_y = areaMatvPoint[ii].y;
			if (max_y<areaMatvPoint[ii].y)
				max_y = areaMatvPoint[ii].y;
		}
		Point point4(mat_src.cols/2,min_y+(max_y-min_y)/3);
		Point point3(mat_src.cols/2,max_y-(max_y-min_y)/3);
		float min_dist4 =mat_src.rows;
		float min_dist3 = mat_src.rows;
		float dist;
		int ind4,ind3;
		for (int ii = 0;ii<areaMatvPoint.size();ii++)
		{
			if (areaMatvPoint[ii].x>3*mat_src.cols/5 || areaMatvPoint[ii].x<2*mat_src.cols/5)
			{
				continue;
			}
			dist = norm(areaMatvPoint[ii]-point4);
			if (dist<min_dist4)
			{
				min_dist4 = dist;
				ind4 = ii;
			}
			dist = norm(areaMatvPoint[ii]-point3);
			if (dist<min_dist3)
			{
				min_dist3 = dist;
				ind3 = ii;
			}
		}
		point4 = areaMatvPoint[ind4];
		point3 = areaMatvPoint[ind3];
		//circle(mat_src,point4,0,Scalar(0,255,255),30);
		//circle(mat_src,point3,0,Scalar(0,255,255),30);
		if (ind4<ind3)
		{
			for (int ii = ind4;ii<=ind3;ii++)
			{
				left_PTT.push_back(areaMatvPoint[ii]);
				circle(mat_src,areaMatvPoint[ii],0,Scalar(0,0,255),10);
			}
			for (int ii =ind4;ii>=0;ii--)
			{
				p2t = areaMatvPoint[ii];
				p2t.x = mat_src.cols-p2t.x;
				right_PBB.push_back(p2t);
				//circle(mat_src,p2t,0,Scalar(255,0,0),10);
			}
			for (int ii = areaMatvPoint.size()-1;ii>=ind3;ii--)
			{
				p2t = areaMatvPoint[ii];
				p2t.x = mat_src.cols-p2t.x;
				right_PBB.push_back(p2t);
				//circle(mat_src,p2t,0,Scalar(0,255,0),10);
			}
		}
		else
		{
			for (int ii = ind4;ii>=ind3;ii--)
			{
				p2t = areaMatvPoint[ii];
				p2t.x = mat_src.cols-p2t.x;
				right_PBB.push_back(p2t);
				//circle(mat_src,p2t,0,Scalar(0,255,0),10);
			}
			for (int ii = ind4;ii<areaMatvPoint.size();ii++)
			{
				left_PTT.push_back(areaMatvPoint[ii]);
				//circle(mat_src,areaMatvPoint[ii],0,Scalar(0,0,255),10);
			}
			for (int ii = 0;ii<=ind3;ii++)
			{
				left_PTT.push_back(areaMatvPoint[ii]);
				//circle(mat_src,areaMatvPoint[ii],0,Scalar(0,0,255),10);
			}
		}*/

		Mat temp = mat_src(Rect(0,0,3*mat_src.cols/5,mat_src.rows));
		//vector<vector<Point> > edge = CBookProcess::GetBookEdgesFromContour_double(left_PTT, point3, point4, 3*mat_src.cols/5, mat_src.rows);
		vector<vector<Point> > edge =  CBookProcess::GetBookEdgesFromContour(temp ,left_PTT);
		if (edge.size()==2)
		{			
			edge = CBookProcess::AdjusBookEdges(temp , edge );
			if (edge.size()==2)
			{
				vector<Point> vPT = edge[0];
				vector<Point> vPB = edge[1];
				vector<Point2f> vPTT;
				vector<Point2f> vPBB;
				Point2f p2f;
				for (int idx = 0; idx<vPT.size();idx++)
				{
					p2f.x = float(vPT[idx].x)/scale;
					p2f.y = float(vPT[idx].y)/scale;
					vPTT.push_back(p2f);
					//circle(src,p2f,0,Scalar(0,255,255),10);
				}
				for (int idx = vPB.size()-1; idx>=0;idx--)
				{
					p2f.x = float(vPB[idx].x)/scale;
					p2f.y = float(vPB[idx].y)/scale;
					vPBB.push_back(p2f);
					//circle(src,p2f,0,Scalar(0,0,255),10);
				}
			
				left_edges.clear();
				left_edges.push_back(vPTT);
				left_edges.push_back(vPBB);
			}
		}
		
		edge.clear();
		//edge = CBookProcess::GetBookEdgesFromContour_double(right_PBB, point3, point4, 3*mat_src.cols/5, mat_src.rows);
		edge = CBookProcess::GetBookEdgesFromContour(temp, right_PBB);
		if (edge.size()==2)
		{
			//Mat temp = mat_src(Rect(2*mat_src.cols/5,0,3*mat_src.cols/5,mat_src.rows));
			edge = CBookProcess::AdjusBookEdges(temp , edge );
			if (edge.size()==2)
			{
				vector<Point> vPT = edge[0];
				vector<Point> vPB = edge[1];
				vector<Point2f> vPTT;
				vector<Point2f> vPBB;
				Point2f p2f;

				for (int idx = 0; idx<vPT.size();idx++)
				{
					p2f.x = 3*src.cols/5-float(vPT[idx].x)/scale;
					p2f.y = float(vPT[idx].y)/scale;
					vPTT.push_back(p2f);
					//circle(src,p2f,0,Scalar(0,255,0),10);
				}
				for (int idx = vPB.size()-1; idx>=0;idx--)
				{
					p2f.x = 3*src.cols/5-float(vPB[idx].x)/scale;
					p2f.y = float(vPB[idx].y)/scale;
					vPBB.push_back(p2f);
					//circle(src,p2f,0,Scalar(0,255,255),10);
				}
			
				right_edges.clear();
				right_edges.push_back(vPTT);
				right_edges.push_back(vPBB);
			}
		}
	}
	//left_edges.push_back(vPT);
	//left_edges.push_back(vPB);
	return src;
}


vector<vector<Point> > CBookProcess::GetBookEdgesFromContour_double(vector<Point> vPTT,Point point3,Point point4,float cols, float rows)
{
	///////////////////////////////////////////////////////////////////
	//�鱾���߶�����ȡ
	//
	//��in��P2TT��Ŀ������
	//��out��ind1�����ϽǶ�������
	//��out��ind2�����½Ƕ�������
	//��out��point1�����ϽǶ���
	//��out��point2�����½Ƕ���
	Point2f p2t;
	Point2f point1(5*cols/6-(point3.y-point4.y)/3, 0);//���ϽǶ���
	Point2f point2(5*cols/6-(point3.y-point4.y)/3, rows-1);//���½Ƕ���
	int ind1 = 100;
	int ind2 = vPTT.size()-100;
	double max_dist1 = 0;
	double max_dist2 = 0;

	//���ӵ㣨�����ӵ�Ϊ��㣬ѭ��������������Զ�������������µ�����ֱ��������������Զ�����㣩
	Point2f point5;
	point5.x = 3*cols/4;//5*bw.cols/6-(point4.y+point3.y)/4;
	point5.y = (point4.y+point3.y)/2;
	for(int ii = 0;ii<10;ii++)
	{
		max_dist1 = 0;
		max_dist2 = 0;

		int label = 0;
		for (int i=ind1;i<ind2;i++)
		{
			p2t =vPTT[i];
			if (p2t.x>min(point1.x,point5.x) || p2t.y>point4.y+(point3.y-point4.y)/4)
				continue;
			double dist1 = norm(p2t-point5);
			if (dist1>max_dist1)
			{
				max_dist1 = dist1;
				ind1 = i;
				label = 0;
			}
			else
				label += 1;
			if (label>50)
				break;
			}
		//��ȡ�����ϱ߽綥������
		point1 = vPTT[ind1];
				
		int width = point1.x;
			
		label = 0;
				
		for (int i=ind2;i>ind1;i--)
		{
			p2t = vPTT[i];
			if (p2t.x>min(point2.x,point5.x) || p2t.y<point3.y-(point3.y-point4.y)/4)
				continue;
			double dist2 = norm(p2t-point5);
			if (dist2>max_dist2)
			{
				max_dist2 = dist2;
				ind2 = i;
				label = 0;
			}
			else
			{
				label += 1;
			}
			if(label>50)
				break;
			}
		point2 = vPTT[ind2];

		if (point5.x-(point1.x+point2.x)/2<40 || abs(point1.x-point2.x)<60)
			break;
		//point5.x = max(point1.x,point2.x);
		if (abs(point1.x-point2.x)>100)
		{
			point5.x = max(point1.x,point2.x)-30;
		}
		else
		{
			point5.x = max(point1.x,point2.x)-10;
		}
	}
	////////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////
	//�߽綥���������������ȡ������
	//�����鱾�߽�ǳ��ֺ�ɫ��ʱ���½������㲢�����ϱ߽�������

	//���ϽǶ������
	//��in��P2TT��Ŀ������
	//��in��point1�����ϽǶ���
	//��in/out��ind1�����ϽǶ�������/����������߷��ϱ�׼����������
	//��in��0���ϱ߽����
	//��out��point1_1�����������ϽǶ���
	Point2f point1_1 = point1;
	ind1 = CBookProcess::Vertex_Correction( vPTT, point1,point1_1, ind1, 0, cols,rows);//col,rowΪͼ��Ŀ��
		
	//���½Ƕ������
	//��in��P2TT��Ŀ������
	//��in��point2�����½Ƕ���
	//��in/out��ind2�����½Ƕ�������//����������߷��ϱ�׼����������
	//��in��0���±߽����
	//��out��point2_1�����������½Ƕ���
	Point2f point2_1 = point2;
	ind2 = CBookProcess::Vertex_Correction( vPTT, point2,point2_1, ind2, 1, cols,rows);//col,rowΪͼ��Ŀ��

	
	vector<Point> vvPT;
	vector<Point> vvPB;

	//�ϱ߽�������ȡ
	//��in��P2TT��Ŀ������
	//��in��point1�����ϽǶ���
	//��in��point1_1���������ϽǶ���
	//��in��ind1������������߷��ϱ�׼����������
	//��out��vPT���ϱ߽�����
	//������ȡ
	for(int i= 0;i<=ind1;i++)
	{
		p2t = vPTT[i];
		vvPT.push_back(p2t);
	}

	//��������
	if (point1.x<point1_1.x)
	{
		for(int i= point1_1.x;i>=point1.x;i--)
		{
			p2t.x = i;
			p2t.y = point1_1.y;
			vvPT.push_back(p2t);
		}
	}
	else if (point1.x>point1_1.x)
	{
		for(int i= point1.x;i>=point1_1.x;i--)
		{
			p2t.x = i;
			p2t.y = point1.y;
			vvPT.push_back(p2t);
		}
	}
    

	//�±߽�������ȡ����
	//��in��P2TT��Ŀ������
	//��in��point2�����½Ƕ���
	//��in��point2_1���������½Ƕ���
	//��in��ind2������������߷��ϱ�׼����������
	//��out��vPB���±߽�����
	//������ȡ
	for(int i= ind2;i<vPTT.size();i++)
	{
		p2t = vPTT[i];
		vvPB.push_back(p2t);
	}

	//��������
	if (point2.x<point2_1.x)
	{
		for(int i= point2_1.x;i>=point2.x;i--)
		{
			p2t.x = i;
			p2t.y = point2_1.y;
			vvPB.insert(vvPB.begin()+0,p2t);
		}
	}
	else if (point2.x>point2_1.x)
	{
		for(int i= point2.x;i>=point2_1.x;i--)
		{
			p2t.x = i;
			p2t.y = point2.y;
			vvPB.insert(vvPB.begin()+0,p2t);
		}
	}
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	point4 = vvPT[0];
	point3 = vvPB[vvPB.size()-1];
	vector<vector<Point> > edgePoint;
	edgePoint.push_back(vvPT);
	edgePoint.push_back(vvPB);

	edgePoint = CBookProcess::CheckBookEdges(edgePoint);

	return edgePoint;
}

//**************************************************************
//1.������
//�����⣬����鱾�����±�Ե�����������㷨�������������չƽ
//���룺
//Mat src:�鱾ԭͼ
//int pageType���鱾ҳ���ͣ�0-��ҳ��1-��ҳ
//�����
//vector<Vector<Point> >:�鱾���±�Ե�����а�������������
//��1����һ��Vector<Point>���ϱ�Ե
//��2���ڶ���Vector<Point>���±�Ե
//***************************************************************
vector<vector<Point2f> > CBookProcess::DetectEdges(Mat src,int pageType)
{
	vector<vector<Point2f> > edges;
	if (src.empty())
	{
		return edges;
	}
	
	RotatedRect rect;
	Mat dst;
	Point point_top_left = CBookProcess::object_detection(src,dst,pageType);
	//src = dst;

	int col = dst.cols;
	Mat mat_src;
	double scale = 1;
	mat_src = CBookProcess::Resize(dst ,scale);
	if (pageType==1)
	{
		flip(mat_src,mat_src,1);
	}

	Mat bw = CBookProcess::Threshold(mat_src);

	vector<vector<Point> > edge = CBookProcess::GetBookEdges( bw.clone());

	if (edge.size()==2)
	{
		vector<Point> vvPT = edge[0];
		vector<Point> vvPB = edge[1];
		if (min(vvPB[vvPB.size()-1].x,vvPT[0].x)>11*bw.cols/12-30)
		{
			float mean_t = bw.rows;
			float mean_b = 0;
			int count_t = 0;
			int count_b = 0;
			int index_t_0,index_t_1;
			for (int ii = vvPT.size()-1;ii>=0;ii--)
			{
				if (vvPT[ii].x<=2*bw.cols/3)
				{
					index_t_0 = ii;
				}
				if (vvPT[ii].x<=9*bw.cols/12)
				{
					index_t_1 = ii;
				}
				if(vvPT[ii].x>2*bw.cols/3+40 && vvPT[ii].x<11*bw.cols/12-50)
				{
					if (count_t>0 && vvPT[ii].y>mean_t+1)
					{
						break;
					}
					if (vvPT[ii].y<=mean_t)
					{
						mean_t = vvPT[ii].y;
						count_t =ii;
					}
				}
			}
			

			int index_b_0,index_b_1;
			index_b_1 = vvPB.size()-1;
			int xx = vvPB[vvPB.size()-1].x;
			for (int ii = 0;ii<vvPB.size();ii++)
			{
				if (vvPB[ii].x<=2*bw.cols/3)
				{
					index_b_0 = ii;
				}
				if (vvPB[ii].x<=9*bw.cols/12)
				{
					index_b_1 = ii;
				}
				if(vvPB[ii].x>2*bw.cols/3+40 && vvPB[ii].x<11*bw.cols/12-50)
				{
					if (count_b>0 && vvPB[ii].y<mean_b-1)
					{
						break;
					}
					if (vvPB[ii].y>=mean_b)
					{
						mean_b = vvPB[ii].y;
						count_b =vvPB.size()-ii;
					}
				}
			}

			if (abs(vvPB[index_b_0].y-vvPB[index_b_1].y-vvPT[index_t_0].y+vvPT[index_t_1].y)<4)
			{
				count_t = index_t_1;
			
				count_b = vvPB.size()-index_b_1;
			}
			mean_t = 5*bw.cols/6-vvPT[count_t].x;
			mean_b = 5*bw.cols/6-vvPB[vvPB.size()-count_b].x;

			float mean_t_1 = bw.rows;
			float mean_b_1 = 0;
			int count_t_1 = 0;
			int count_b_1 = 1;
			for (int ii = 0;ii<vvPT.size();ii++)
			{
				if(ii<count_t-100 && vvPT[ii].x>5*bw.cols/6+mean_t)
				{
					if (count_t_1>1 && vvPT[ii].y>mean_t_1+1)
					{
						break;
					}
					if (vvPT[ii].y<=mean_t_1)
					{
						mean_t_1 = vvPT[ii].y;
						count_t_1 =ii;
					}
				}
			}
			for (int ii = vvPB.size()-1;ii>=0;ii--)
			{
				if(ii>vvPB.size()-count_b+100 && vvPB[ii].x>5*bw.cols/6+mean_b)
				{
					if (count_b_1>0 && vvPB[ii].y<mean_b_1-1 )
					{
						int ka = vvPB[ii].x;
						break;
					}
					if (vvPB[ii].y>=mean_b_1)
					{
						mean_b_1 = vvPB[ii].y;
						count_b_1 =vvPB.size()-ii;
					}
				}
			}
			
			/*
			circle(bw, vvPT[count_t], 10, Scalar(0, 0, 255), -1);
			circle(bw, vvPT[count_t_1], 10, Scalar(0, 0, 255), -1);
			circle(bw, vvPB[vvPB.size()-count_b], 10, Scalar(0, 0, 255), -1);
			circle(bw, vvPB[vvPB.size()-count_b_1], 10, Scalar(0, 0, 255), -1);
			line(bw,vvPT[count_t],vvPT[count_t_1],Scalar(255, 0, 255), 5);
			line(bw,vvPB[vvPB.size()-count_b],vvPB[vvPB.size()-count_b_1], Scalar(255, 0, 255), 5);
			line(bw,Point(3*bw.cols/4,0),Point(3*bw.cols/4,bw.rows), Scalar(0, 0, 255), 5);
			line(bw,Point(5*bw.cols/6,0),Point(5*bw.cols/6,bw.rows), Scalar(0, 0, 255), 5);
			line(bw,Point(11*bw.cols/12,0),Point(11*bw.cols/12,bw.rows), Scalar(0, 0, 255), 5);
			namedWindow("1",2);
			imshow("1",bw);
			waitKey(0);*/
			
			float K_t = float(vvPT[count_t_1].y-vvPT[count_t].y)/float(vvPT[count_t_1].x-vvPT[count_t].x);
			float b_t = vvPT[count_t].y-K_t*vvPT[count_t].x;
			float K_b = float(vvPB[vvPB.size()-count_b_1].y-vvPB[vvPB.size()-count_b].y)/float(vvPB[vvPB.size()-count_b_1].x-vvPB[vvPB.size()-count_b].x);
			float b_b = vvPB[vvPB.size()-count_b_1].y-K_b*vvPB[vvPB.size()-count_b_1].x;

			float stdv_t = 0;
			float stdv_b = 0;
			int cc_t = 0;
			int cc_b = 0;
			float min_t = 0;
			float min_b = 0;
			for (int ii = count_t_1;ii<count_t;ii++)
			{
				if(ii<count_t)
				{
					//stdv_t += pow((vvPT[ii].y-mean_t),2);
					float y = K_t*vvPT[ii].x+b_t;
					float ak = (vvPT[ii].y-y);
					if (ak>0)
					{
						if (ak>min_t)
							min_t = ak;
						stdv_t += ak;
						cc_t += 1;
					}
				}
			}
			for (int ii = vvPB.size()-count_b;ii<vvPB.size()-count_b_1;ii++)
			{
				if(ii>vvPB.size()-count_b)
				{
					float y = K_b*vvPB[ii].x+b_b;
					//stdv_b += pow((vvPB[ii].y-mean_b),2);
					float ak = (vvPB[ii].y-y);
					if (ak<0)
					{
						if (ak<min_b)
							min_b = ak;
						stdv_b += ak;
						cc_b += 1;
					}
				}
			}
			
			count_t = count_t-count_t_1;
			count_b = count_b-count_b_1;
			stdv_t = stdv_t/count_t;
			stdv_b = stdv_b/count_b;
			if (((1.0*cc_t/count_t<0.5 && stdv_t<2) || min_t<2.5) && ((1.0*cc_b/count_b<0.5 && stdv_b>-2) || min_b>-2.5))
				edge.clear();
		}
	}

	if (edge.size()==2)
	{
		edge = CBookProcess::AdjusBookEdges(mat_src , edge);
		edge = CBookProcess::CheckBookEdges(edge);
		if (edge.size()==2)
		{
			vector<Point> vPT = edge[0];
			vector<Point> vPB = edge[1];
			vector<Point2f> vPTT;
			vector<Point2f> vPBB;
			Point2f p2f;
			if (pageType==1)
			{
				for (int idx = 0; idx<vPT.size();idx++)
				{
					p2f.x = point_top_left.x+(col-float(vPT[idx].x)/scale);
					p2f.y = point_top_left.y+float(vPT[idx].y)/scale;
					vPTT.push_back(p2f);
				}
				for (int idx = vPB.size()-1; idx>=0;idx--)
				{
					p2f.x = point_top_left.x+(col-float(vPB[idx].x)/scale);
					p2f.y = point_top_left.y+float(vPB[idx].y)/scale;
					vPBB.push_back(p2f);
				}
			}
			else
			{
				for (int idx = 0; idx<vPT.size();idx++)
				{
					p2f.x = point_top_left.x+float(vPT[idx].x)/scale;
					p2f.y = point_top_left.y+float(vPT[idx].y)/scale;
					vPTT.push_back(p2f);
				}
				for (int idx = vPB.size()-1; idx>=0;idx--)
				{
					p2f.x = point_top_left.x+float(vPB[idx].x)/scale;
					p2f.y = point_top_left.y+float(vPB[idx].y)/scale;
					vPBB.push_back(p2f);
				}
			}
			
			edges.clear();
			edges.push_back(vPTT);
			edges.push_back(vPBB);
		}
	}

	return edges;
	
}

//*************************************************************
//������
//�ӻ�ȡ��Ե�ؼ��㼯�ϻ�ȡ���±�Ե��������

//���룺
//vector<vector<Point> > keyPoints�����±�Ե�ؼ��㼯��;

//�����
//vector<Vector<Point> >:���±�Ե��������
//******************************************************************
vector<vector<Point2f> > CBookProcess::GetEdgesFromKeyPoints(vector<vector<Point2f> > keyPoints)
{
	vector<vector<Point2f> > edges;
	for (int ii=0 ; ii<keyPoints.size() ;ii++)
	{
		vector<Point2f> vPoint = keyPoints[ii];
		//��������Ĺؼ����ֵ�����鱾�߽���������
		//��in��vPoint���߽�ؼ���
		//��out��vvPoint���߽�����
		vector<Point2f> vvPoint;
		Point2f p2t;
		float y1,y2,x1,x2;
		float y;
		if (vPoint[0].x>vPoint[1].x)
		{
			//��ҳ��ֵ
			for (int i = 1;i<vPoint.size();i++)
			{
				x1 = vPoint[i-1].x;
				x2 = vPoint[i].x;
				y1 = vPoint[i-1].y;
				y2 = vPoint[i].y;
				for (int j = x1;j>x2;j--)
				{
					p2t.x = j;
					y = ((j-x1)*(y2-y1)/(x2-x1)+y1);
					p2t.y = y;
					vvPoint.push_back(p2t);
				}
			}
		}
		else
		{
			//��ҳ��ֵ
			for (int i = 1;i<vPoint.size();i++)
			{
				x1 = vPoint[i-1].x;
				x2 = vPoint[i].x;
				y1 = vPoint[i-1].y;
				y2 = vPoint[i].y;
				for (int j = x1;j<x2;j++)
				{
					p2t.x = j;
					y = ((j-x1)*(y2-y1)/(x2-x1)+y1);
					p2t.y = y;
					vvPoint.push_back(p2t);
				}
			}
		}
		edges.push_back(vvPoint);
	}
	return edges;
}

//**************************************************************
//2.�����±�Ե�������ϻ�ȡ���±�Ե�ؼ��㼯��
//�����±�Ե�������ϻ�ȡ���±�Ե�ؼ��㼯�ϣ��ؼ������������ƣ�������UI�ϲ���
//���룺
//vector<vector<Point> >�����±�Ե��������
//int���ؼ�������
//�����
//vector<Vector<Point> >:���±�Ե�ؼ��㼯�ϣ��㼯�е���������ڵڶ���������int���ؼ���������
//**************************************************************
vector<vector<Point2f> > CBookProcess::GetKeyPointsFromEdges(vector<vector<Point2f> > edges,int keyPointsCount,bool state)
{
	vector<vector<Point2f> > keyPoints;
	float fRatio = float(keyPointsCount);
	vector<Point2f> vPT = edges[0];
	vector<Point2f> vPB = edges[1];
	Point2f p2t;

	if (vPT.size()==2 && vPB.size()==2)
	{
		keyPoints = edges;
		return keyPoints;
	}

	//////////////////////////////////////////////////////////////////////////
	//���±߽�������ƽ������
	//��in/out��vPT���ϱ߽�����
	//��in/out��vPB���±߽�����

	//�ϱ߽�������ƽ����
	//���������ϱ߽綥��ֱ�߾������5�����أ���yֵ��С�ĵ㿪ʼ������������������
	int ind_max_T = 0;
    int max_dist = 0;
	int min_y = 2000;

	//����ֱ�߲�������
	//��in��vPT���ϱ߽�����
	//��out��K��ֱ��б��
	//��out��C��ֱ�߽ؾ�
	float K = float(vPT[0].y-vPT[vPT.size()-1].y)/float(vPT[0].x-vPT[vPT.size()-1].x);
	float C = float(vPT[0].y)-K*float(vPT[0].x);

	//state = false;
	if (state)
	{
		//����������ֱ�ߵ������룬��������СYֵ������
		//��in��vPT���ϱ߽�����
		//��in��K��ֱ��б��
		//��in��C��ֱ�߽ؾ�
		//��out��max_dist:�ϱ߽�������ֱ�ߵ�������
		//��out��ind_max_T���ϱ߽�������Сyֵ������
		for (int ii = vPT.size()/6.0;ii<3*vPT.size()/4.0;ii++)
		{
			float dist  = abs((K*vPT[ii].x-1*vPT[ii].y+C)/sqrt(pow(K,2)+pow(-1.0,2)));//�㵽ֱ�ߵļ���
			if (dist>max_dist && vPT[ii].y<min_y)
			{
				//ind_max_T = ii;
				max_dist = int(dist);
			}
			if (vPT[ii].y<min_y)// && dist>0.8*float(max_dist))
			{
				ind_max_T = ii;
				min_y = vPT[ii].y;
			}
		}


		if (max_dist>10)
		{
			//��yֵ��С�Ĵ���ʼ�������Ҷ�����������ƽ������
			//��in/out��vPT���ϱ߽�����
			//��in��ind_max_T���ϱ߽�������Сyֵ������
			for (int ii = ind_max_T-1;ii>=0;ii--)
			{
				if (vPT[ii].y<vPT[ii+1].y)
				{
					vPT[ii].y=vPT[ii+1].y;
				}
			}
			for (int ii = ind_max_T;ii<vPT.size()-1;ii++)
			{
				if (vPT[ii+1].y<vPT[ii].y)
				{
					vPT[ii+1].y=vPT[ii].y;
				}
			}
		}
	}
     //�±߽�������ƽ����
	//���������±߽綥��ֱ�߾������5�����أ���yֵ���ĵ㿪ʼ������������������  
	if (state)
	{
		int ind_max_B = 0;
		max_dist = 0;
		int max_y = 0;

		//����ֱ�߲�������
		//��in��vPB���±߽�����
		//��out��K��ֱ��б��
		//��out��C��ֱ�߽ؾ�
		K = float(vPB[0].y-vPB[vPB.size()-1].y)/float(vPB[0].x-vPB[vPB.size()-1].x);
		C = float(vPB[0].y)-K*float(vPB[0].x);

		//����������ֱ�ߵ������룬���������Yֵ������
		//��in��vPB���±߽�����
		//��in��K��ֱ��б��
		//��in��C��ֱ�߽ؾ�
		//��out��max_dist:�±߽�������ֱ�ߵ�������
		//��out��ind_max_B���±߽��������yֵ������
		for (int ii = vPB.size()/6.0;ii<3*vPB.size()/4.0;ii++)
		{
			float dist  = abs((K*vPB[ii].x-1*vPB[ii].y+C)/sqrt(pow(K,2)+pow(-1.0,2)));
			if (dist>max_dist && vPB[ii].y>max_y)
			{
				//ind_max_B = ii;
				max_dist = int(dist);
			}
			if (vPB[ii].y>max_y)// && dist>0.8*float(max_dist))
			{
				ind_max_B = ii;
				max_y = vPB[ii].y;
			}
		}

		if (max_dist>10)
		{
			//��yֵ���Ĵ���ʼ�������Ҷ�����������ƽ������
			//��in/out��vPB���±߽�����
			//��in��ind_max_B���±߽��������yֵ������
			for (int ii = ind_max_B-1;ii>=0;ii--)
			{
				if (vPB[ii].y>vPB[ii+1].y)
				{
					vPB[ii].y=vPB[ii+1].y;
				}
			}

			for (int ii = ind_max_B;ii<vPB.size()-1;ii++)
			{
				if (vPB[ii+1].y>vPB[ii].y)
				{
					vPB[ii+1].y=vPB[ii].y;
				}
			} 
		}
	}
	/////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////
	//��������x�������ص���ɾ��
	//�����������ڵ�������xֵ��ͬ��ɾ������ͼ���ϻ��±߽磨������Ϊ�����ϱ߽磬������Ϊ�����±߽磩�ĵ�
	//��in/out��vPT���ϱ߽�����
	//��in/out��vPB���±߽�����
	for (int ii = 1;ii<vPT.size();ii++)
	{
		if (vPT[ii].x==vPT[ii-1].x)
		{
			if (vPT[ii].y>vPT[ii-1].y)
			{
				vPT.erase(vPT.begin()+ii);
			}
			else
			{
				vPT.erase(vPT.begin()+ii-1);
			}
		}
	}
	
    for (int ii = 1;ii<vPB.size();ii++)
	{
		if (vPB[ii].x==vPB[ii-1].x)
		{
			if (vPB[ii].y>vPB[ii-1].y)
			{
				vPB.erase(vPB.begin()+ii);
			}
			else
			{
				vPB.erase(vPB.begin()+ii-1);
			}
		}
	}
	///////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	//�߽������ؼ�����ȡ
	//��in��vPT���ϱ߽�����
	//��in��vPB���±߽�����
	//��out��vvPT���ϱ߽������ؼ���
	//��out��vvPB���±߽������ؼ���

	//���±߽������������Сֵ��ֵ���㣨�����������ȼ������ڳ����������ļ��㣩
	//��in��vPT���ϱ߽�����
	//��in��vPB���±߽�����
	//��out��deta_y_T���ϱ߽����������Сֵ��ֵ
	//��out��deta_y_B���±߽����������Сֵ�ò�ֵ
	float max_y_T = 0;
	float min_y_T = 10000;
	float max_y_B = 0;
	float min_y_B = 10000;
	int index_min_t = 0;
	int index_max_b = 0;
	for (int i = 0;i<vPB.size();i++)
	{
		if (vPB[i].y>max_y_B) 
		{
			max_y_B = vPB[i].y;
			index_max_b = i;
		}
		if (vPB[i].y<min_y_B) min_y_B = vPB[i].y;
	}
	for (int i = 0;i<vPT.size();i++)
	{
		if (vPT[i].y>max_y_T) max_y_T = vPT[i].y;
		if (vPT[i].y<min_y_T)
		{
			min_y_T = vPT[i].y;
			index_min_t = i;
		}
	}
	float deta_y_T = 7*(max_y_T-min_y_T)+0.001;
	float deta_y_B = 7*(max_y_B-min_y_B)+0.001;

	/*//�±߽��������ȼ���
	//��in��vPB�� �±߽�����
	//��out��dist_B���±߽���������
	float dist_B = 0;
	for (int i = 0;i<vPB.size()-1;i++)
	{
		if (1)//i<vPB.size()-70)
		{
			float ffRatio = abs(vPB[i].y-vPB[i+1].y)/(2*abs(vPB[i].x-vPB[i+1].x+0.001));
			if (vPB[i].x-vPB[i+1].x==0)
				ffRatio = 1;
			dist_B += ((1+ffRatio/2)+float(max_y_B-vPB[i].y)/deta_y_B)*norm(vPB[i]-vPB[i+1]);
		}
		else
		{
			dist_B += abs(vPB[i].x-vPB[i+1].x);
		}
	}*/
	/*
	//�ϱ߽��������ȼ���
	//��in��vPT���ϱ߽�����
	//��out��dist_T���ϱ߽���������
	float dist_T = 0;
	for (int i = 0;i<vPT.size()-1;i++)
	{
		if (1)//i<vPT.size()-70)
		{
			float ffRatio = abs(vPT[i].y-vPT[i+1].y)/(2*abs(vPT[i].x-vPT[i+1].x+0.001));
			if (vPT[i].x-vPT[i+1].x==0)
				ffRatio = 1;
			dist_T += (float(vPT[i].y-min_y_T)/deta_y_T+(1+ffRatio/2))*norm(vPT[i]-vPT[i+1]);
		}
		else
		{
			dist_T += abs(vPT[i].x-vPT[i+1].x);
		}
	}*/
    
	//�ϱ߽��������ȼ���
	//��in��vPT���ϱ߽�����
	//��out��dist_T���ϱ߽���������
	float dist_T = 0;
	for (int i = 0;i<vPT.size()-1;i++)
	{
		if (1)//i<vPT.size()-70)
		{
			float ffRatio = abs(vPT[i].y-vPT[i+1].y)/(abs(vPT[i].x-vPT[i+1].x+0.0000001));
			if (ffRatio>1.5)//abs(vPT[i].x-vPT[i+1].x)<1)
					ffRatio = 2;
			//dist_T += (1+(vPT[i].y-min_y_T)/(deta_y_T))*((norm(Point2f(vPT[i].x-vPT[i+1].x,0)))/(cos(atan(1.8*ffRatio))));
			/*if (i<index_min_t)
			{
				if (ffRatio>0.4)
					dist_T += 1.44*(float(vPT[i].y-min_y_T)/(deta_y_T))*norm(Point2f(vPT[i].x-vPT[i+1].x,0));
				else
					dist_T += (float(vPT[i].y-min_y_T)/(deta_y_T))*norm(Point2f(vPT[i].x-vPT[i+1].x,0));
			}
			else
				dist_T += (float(vPT[i].y-min_y_T)/(deta_y_T))*norm(Point2f(vPT[i].x-vPT[i+1].x,0));*/
			if (i<index_min_t)
				dist_T += (1+(vPT[i].y-min_y_T)/deta_y_T)*(norm(vPT[i]-vPT[i+1])/cos(atan(ffRatio)));
			else
				dist_T += (1+(vPT[i].y-min_y_T)/deta_y_T)*(norm(vPT[i]-vPT[i+1])/cos(atan(ffRatio)));
			/*float zz = 1;
			float ffRatio = abs(vPT[i].y-vPT[i+1].y)/(abs(vPT[i].x-vPT[i+1].x+0.001));
			if (vPT[i].x==vPT[i+1].x)
				ffRatio = 1;
			if (ffRatio>1)
				ffRatio = 1;
			if (i>index_min_t)
				zz = (1+float(vPT[i].y-min_y_T)/deta_y_T)*(1/cos(atan(ffRatio)));
			else
				zz = (1+float(vPT[i].y-min_y_T)/(3*deta_y_T));//*(1/cos(atan(ffRatio)));
			dist_T += (1+ffRatio/2.2)*(zz)*norm(vPT[i]-vPT[i+1]);*/
		}
		else
		{
			dist_T += abs(vPT[i].x-vPT[i+1].x);
		}
	}

	float deta;
	int type;
	if (vPB[0].x>vPB[vPB.size()-1].x)
	{
		deta = 10;
		type = 0;
	}
	else
	{
		deta = -10;
		type = 1;
	}

	//�ֶ���ȡ���±߽��ߵĹؼ���
	int count = 0;
	vector<Point2f> vvPT;
	vector<Point2f> vvPB;
	float dist = 0;
	//�ϱ߽�ֶμ���ؼ���
	//��in��vPT���ϱ߽�����
	//��in��dist_T���ϱ߽���������
	//��in��fRatio���߽�ָ������
	//��out��vvPT���ϱ߽������ؼ���
	vvPT.push_back(vPT[0]);
	for (int j = 1;j<vPT.size();j++)
	{
		if (1)//j<vPT.size()-70)
		{
			float ffRatio = abs(vPT[j].y-vPT[j-1].y)/(abs(vPT[j].x-vPT[j-1].x+0.0000001));
			if (ffRatio>1.5)//abs(vPT[j].x-vPT[j-1].x)<1)
				ffRatio = 2;
			
			if (j<index_min_t)
				dist = dist +(1+(vPT[j].y-min_y_T)/deta_y_T)*(norm(vPT[j]-vPT[j-1])/cos(atan(ffRatio)));
			else
				dist = dist +(1+(vPT[j].y-min_y_T)/deta_y_T)*(norm(vPT[j]-vPT[j-1])/cos(atan(ffRatio)));
		}
		else
		{
			dist += abs(vPT[j].x-vPT[j-1].x);
		}
		if (dist>(dist_T/fRatio))
		{
			p2t = vPT[j];
			vvPT.push_back(vPT[j]);
			count = count+1;
			if (count==fRatio-1) break;
			dist= dist-dist_T/fRatio;
		}
	}
	if (abs(vPT[vPT.size()-1].x-vvPT[vvPT.size()-1].x)>abs(deta))
		vvPT.push_back(vPT[vPT.size()-1]);
	else
	{
		p2t = vPT[vPT.size()-1];
		p2t.x = vvPT[vvPT.size()-1].x-deta;
		vvPT.push_back(p2t);
	}
    
	//�±߽�ֶμ���ؼ���
	//��in��vPB���±߽�����
	//��in��dist_B���±߽���������
	//��in��fRatio���߽�ָ������
	//��out��vvPB���±߽������ؼ���
	dist = 0;
	vvPB.push_back(vPB[0]);
	
	float aa = -1;
	if (vPT[0].x>vPT[vPT.size()-1].x)
		aa = 1;
	float FR = float(vPT[0].x-vPT[vPT.size()-1].x)/float(vPB[0].x-vPB[vPB.size()-1].x);
	float x1 = vvPT[0].x;
	for (int i = 1;i<vvPT.size()-1;i++)
	{
		float x2 = vPB[0].x-aa*abs(vvPT[i].x-x1)/FR;
		for (int j= 0 ;j < vPB.size();j++)
		{
			if (aa*vPB[j].x<aa*x2)
			{
				vvPB.push_back(vPB[j]);
				break;
			}
		}
	}

	if (abs(vPB[vPB.size()-1].x-vvPB[vvPB.size()-1].x)>abs(deta))
		vvPB.push_back(vPB[vPB.size()-1]);
	else
	{
		p2t = vPB[vPB.size()-1];
		p2t.x = vvPB[vvPB.size()-1].x-deta;
		vvPB.push_back(p2t);
	}
	////////////////////////////////////////////////////////////////////////////////////
	if (vvPT.size()==2 || vvPB.size()==2)
	{
		if (vvPT.size()>2)
		{
			vector<Point2f> vPTT;
			vPTT.push_back(vvPT[0]);
			vPTT.push_back(vvPT[vvPT.size()-1]);
			vvPT.clear();
			vvPT = vPTT;
		}
		if (vvPB.size()>2)
		{
			vector<Point2f> vPBB;
			vPBB.push_back(vPB[0]);
			vPBB.push_back(vvPB[vvPB.size()-1]);
			vvPB.clear();
			vvPB = vPBB;
		}
	}

	//************************************************
	Point2f point1 = vvPT[vvPT.size()-1];
	Point2f point4 = vvPT[0];
	Point2f point2 = vvPB[vvPB.size()-1];
	Point2f point3 = vvPB[0];
	//////////////////////////////////////////////////////////////

	//����ؼ��������ж�
	//(�ؼ�����밴˳�����У����ó���x�᷽�������
	//���ؼ���y���겻�ó�����Ӧ���߶������������鱾�߶ȵ�ʮ���֮һ��
	//��in��vvvvPT���ϱ߽�ؼ���
	//��in��vvvvPB���±߽�ؼ���
	//��out�� flag:
	bool flag = true;
	if (type == 0)
	{
		for(int ii = 1;ii<vvPT.size();ii++)
		{
			if (vvPT[ii].x>=vvPT[ii-1].x || \
				(vvPT[ii].y<min(point1.y,point4.y)-(point3.y-point4.y)/15 && \
				vvPT[ii].y>max(point1.y,point4.y)+(point3.y-point4.y)/15))
			{
				flag = false;
			}
		}
		for(int ii = 1;ii<vvPB.size();ii++)
		{
			if (vvPB[ii].x>=vvPB[ii-1].x ||\
				(vvPB[ii].y<min(point2.y,point3.y)-(point3.y-point4.y)/15 &&\
				vvPB[ii].y>max(point2.y,point3.y)+(point3.y-point4.y)/15))
			{
				flag = false;
			}
		}
	}
	else
	{
		for(int ii = 1;ii<vvPT.size();ii++)
		{
			if (vvPT[ii].x<=vvPT[ii-1].x || \
				(vvPT[ii].y<min(point1.y,point4.y)-(point3.y-point4.y)/15 && \
				vvPT[ii].y>max(point1.y,point4.y)+(point3.y-point4.y)/15))
			{
				flag = false;
			}
		}
		for(int ii = 1;ii<vvPB.size();ii++)
		{
			if (vvPB[ii].x<=vvPB[ii-1].x ||\
				(vvPB[ii].y<min(point2.y,point3.y)-(point3.y-point4.y)/15 &&\
				vvPB[ii].y>max(point2.y,point3.y)+(point3.y-point4.y)/15))
			{
				flag = false;
			}
		}
	}
	
	float xx1 = vvPT[0].x;
	float xx2 = vvPT[vvPT.size()-1].x;
	bool errmsg = xx1>xx2;
	for (int ii = 1;ii<vvPT.size();ii++)
	{
		if (errmsg)
		{
			if (vvPT[ii-1].x<=vvPT[ii].x+5 || vvPB[ii-1].x<=vvPB[ii].x+5)
				flag = false;
		}
		else
		{
			if (vvPT[ii].x<=vvPT[ii-1].x+5 || vvPB[ii].x<=vvPB[ii-1].x+5)
				flag = false;
		}
	}
	
	//�жϼ����ҳƫ����εĳ̶�
	//���±߽綥��x��ֵ�����ڡ�0.88����1.14��
	//���±߽綥��y��ֵ�����ڡ�0.82����1.18��
	if(flag && ((point4.x-point1.x)/(point3.x-point2.x)>0.88 && (point4.x-point1.x)/(point3.x-point2.x)<1.14 &&\
		(point3.y-point4.y)/(point2.y-point1.y)>0.82 && (point3.y-point4.y)/(point2.y-point1.y)<1.18))
	{

		keyPoints.push_back(vvPT);
		keyPoints.push_back(vvPB);
	}
	else
	{
		vvPT.clear();
		vvPB.clear();
		vvPT.push_back(Point2f(point3.x,min(point1.y,point4.y)));
		vvPT.push_back(Point2f(point2.x,min(point1.y,point4.y)));
		vvPB.push_back(Point2f(point3.x,max(point2.y,point3.y)));
		vvPB.push_back(Point2f(point2.x,max(point2.y,point3.y)));
		keyPoints.push_back(vvPT);
		keyPoints.push_back(vvPB);
	}
	//************************************************
	return  keyPoints;
}

//*************************************************************
//3.����չƽ
//�������±�Ե�ؼ��㼯������չƽ
//���룺
//Mat src��ԭͼ
//vector<vector<Point> > keyPoints�����±�Ե�������ϣ��ؼ���
//�����
//Mat dst�������ͼ��
//**************************************************************
Mat CBookProcess::Flatting(Mat src, vector<vector<Point2f> > keyPoints)
{
	float height = 0;
	vector<Point2f> vvPT = keyPoints[0];
	vector<Point2f> vvPB = keyPoints[1];
	Mat mat_dst;
	vector<Point2f> Points1;
	vector<Point2f> Points2;
	Point2f P2f;
	Point2f p2t;

	//չƽ��ͼ��߶ȼ���
	if (height==0)
	{
		height = norm(vvPT[0]-vvPB[0]);
	}
	Mat src1 = src.clone();
	if (vvPB.size()!=2)
	{
		//����ָ��fRatio*fRatio_h��С����չƽ
		int fRatio = vvPB.size()-1;//������������ָ��35�ȷ�չƽ
		int fRatio_h = 40;//������ֱ����ָ��20�ȷ�չƽ
		//////////////////////////////////////////////////////////
		//Step 1������������������
		//////////////////////////////
		
		float dist_B = 0;
		float dist_T = 0;
		float dist = 0;
		if(1)
		{
			//�����±߽������߳���
			//��in��vvPB ���������ؼ���
			//��in��fRatio�������߷ָ����
			//��out��dist_B������������
			for (int i = 0;i<vvPB.size()-1;i++)
			{
				/*Point2f p2t1 = vvPB[i];
				Point2f p2t2 = vvPB[i+1];
				dist = norm(vvPB[i]-vvPB[i+1]);*/
				dist_B = dist_B +norm(Point2f(vvPB[i].x-vvPB[i+1].x,3*(vvPB[i].y-vvPB[i+1].y)));
			}

			//�����ϱ߽������߳���
			//��in��vvPT ���������ؼ���
			//��in��fRatio�������߷ָ����
			//��out��dist_T������������
			for (int i = 0;i<vvPT.size()-1;i++)
			{
				dist_T = dist_T +norm(Point2f(vvPT[i].x-vvPT[i+1].x,3*(vvPT[i].y-vvPT[i+1].y)));
			}			
		}
		
		///////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////
		//Step 2����ֵ������ҳ���йؼ���
		//////////////////////////////
		vector<vector<Point2f> > landmarks;
		vector<Point2f> landmark;

		float ind = 15;//fRatio_h/2;
		bool flag = true;
		float max_dist = 0;
		float min_dist = 5000;
		int index = 0;
		for (int ii = 0;ii<=fRatio;ii++)
		{
			float dist = norm(vvPT[ii]-vvPB[ii]);
			if (max_dist<dist)
			{
				max_dist = dist;
				index = ii;
			}
			if (min_dist>dist)
			{
				min_dist = dist;
			}
		}
		/*float K1 = ((vvPB[1].y+vvPT[1].y)/2-(vvPB[vvPB.size()-3].y+vvPT[vvPT.size()-3].y)/2)/((vvPB[1].x+vvPT[1].x)/2-(vvPB[vvPB.size()-3].x+vvPT[vvPT.size()-3].x)/2);
		float B1 = (vvPB[1].y+vvPT[1].y)/2-(K1*(vvPT[1].x+vvPB[1].x)/2);
		float max_dist_T = abs((K1*vvPT[index].x-1*vvPT[index].y+B1)/sqrt(pow(K1,2)+pow(-1.0,2)));
		float max_dist_B = abs((K1*vvPB[index].x-1*vvPB[index].y+B1)/sqrt(pow(K1,2)+pow(-1.0,2)));*/
		float dist1 = norm(vvPT[0]-vvPB[0]);
		float dist2 = norm(vvPT[vvPT.size()-1]-vvPB[vvPB.size()-1]);
		if (((index<6 || index>=30) && (max_dist>min_dist*1.1)) || dist1>1.05*dist2)
		{
			ind = 20;
			flag = false;
		}
		else
		{
			ind = 15;
			flag = true;
		}

		if(flag)
		{
			

			//ʹ�����������ؼ����ֵ����չƽ�����߹ؼ���
			//��in��vvPT ���������ؼ���
			//��in��vvPB ���������ؼ���
			//��in��fRatio�������߷ָ����
			//��in��fRatio_h����ֱ����ָ����
			//��out��landmarks;�����߹ؼ���
			for (int i = 0;i<=fRatio;i++)
			{
				//line(src,vvPT[i],vvPB[i],Scalar(0,250,250),1,CV_AA);  
				landmark.push_back(vvPT[i]);
				for (int j= 0;j<fRatio_h-1;j++)
				{
					p2t.x = vvPT[i].x+((float)(j+1)/fRatio_h)*(vvPB[i].x-vvPT[i].x);
					p2t.y = vvPT[i].y+((float)(j+1)/fRatio_h)*(vvPB[i].y-vvPT[i].y);
					landmark.push_back(p2t);
					//circle(src,p2t,0,Scalar(0,255,255),10);
				}
				landmark.push_back(vvPB[i]);
				landmarks.push_back(landmark);
				landmark.clear();
			}

			if (true)
			{
				Point2f p2f_1= landmarks[1][ind];
				Point2f p2f_2 = landmarks[int(fRatio-5)][ind];

				float K = (p2f_1.y-p2f_2.y)/(p2f_1.x-p2f_2.x);
				float B = p2f_1.y-(K*p2f_1.x);
				float min_sum_sstdv = 0;
				int min_count_sstdv = 0;
				min_sum_sstdv = ((K*landmarks[index][ind].x-(landmarks[index][ind].y)+B)/sqrt(K*K+1));//-(20-ind)*(max_dist_T-max_dist_B)/40;
				/*for (int j = 2; j<10;j++)
				{
 					//min_sum_sstdv += pow(float(int(fRatio/2)-j+1)/int(fRatio/2-1),2)*((K*landmarks[j][ind].x-(landmarks[j][ind].y)+B)/sqrt(K*K+1));
					if ((K*landmarks[j][ind].x-landmarks[j][ind].y+B)/sqrt(K*K+1)>0)
						min_count_sstdv += 1;
				}*/
				if (true)//abs(min_sum_sstdv)>20 || (min_sum_sstdv)<0)
				{
					for (int i = 16;i<26;i++)
					{
						Point2f p2f_21 = landmarks[1][i];

						Point2f p2f_22 = landmarks[int(fRatio-5)][i];
						K = (p2f_21.y-p2f_22.y)/(p2f_21.x-p2f_22.x);
						B = p2f_21.y-(K*p2f_21.x);
						//line(src,p2f_21,Point(p2f_22.x,p2f_22.x*K+B),Scalar(0,0,255),5,CV_AA);  
						float sum_sstdv = 0;
						int count_sstdv = 0;
						sum_sstdv = ((K*landmarks[index][i].x-(landmarks[index][i].y)+B)/sqrt(K*K+1));//-abs(20-i)*(max_dist_T-max_dist_B)/40;
						/*for (int j = 2; j<10;j++)
						{
							//sum_sstdv += pow(float(int(fRatio/2)-j+1)/int(fRatio/2-1),2)*((K*landmarks[j][i].x-(landmarks[j][i].y)+B)/sqrt(K*K+1));
							if ((K*landmarks[j][i].x-landmarks[j][i].y+B)/sqrt(K*K+1)>0)
								count_sstdv += 1;
						}
						if (count_sstdv<=4)
						{
 							if (min_count_sstdv-4<=4-count_sstdv)
							{
								ind = i-1;
								break;
							}
							else
							{
								ind = i;
								break;
							}
						}
						min_count_sstdv = count_sstdv;
						ind = i;*/

						if (abs(min_sum_sstdv)>abs(sum_sstdv))
						{
							min_sum_sstdv = sum_sstdv;
							ind = i;
						}
						
 						/*if (sum_sstdv/min_sum_sstdv<0)
							ind = i-sum_sstdv/(sum_sstdv-min_sum_sstdv);
						min_sum_sstdv = sum_sstdv;*/
					}
				}
			}
			else
			{
				//float ff = max_dist_T/max_dist_B;

				Point2f p2f_1= landmarks[1][ind];
				Point2f p2f_2 = landmarks[fRatio-5][ind];

				float K = (p2f_1.y-p2f_2.y)/(p2f_1.x-p2f_2.x);
				float B = p2f_1.y-K*p2f_1.x;
				float min_sum_sstdv = 0;
				for (int j = 2; j<fRatio/2;j++)
				{
					min_sum_sstdv += abs((K*landmarks[j][ind].x-landmarks[j][ind].y+B)/sqrt(K*K+1));
				}

				if (abs(min_sum_sstdv)>50)
				{
					for (int i = 15;i<26;i++)
					{
						Point2f p2f_21 = landmarks[1][i];
						Point2f p2f_22 = landmarks[fRatio-5][i];

						K = (p2f_21.y-p2f_22.y)/(p2f_21.x-p2f_22.x);
						B = p2f_21.y-K*p2f_21.x;
						//line(src,p2f_21,Point(p2f_22.x,p2f_22.x*K+B),Scalar(0,0,255),5,CV_AA);  
						float sum_sstdv = 0;
						for (int j = 2; j<fRatio/2;j++)
						{
							sum_sstdv += abs((K*landmarks[j][i].x-landmarks[j][i].y+B)/sqrt(K*K+1));
						}
						if (abs(min_sum_sstdv)>abs(sum_sstdv) )
						{
							min_sum_sstdv = sum_sstdv;
							ind = i;
						}
					}
				}
			}
			landmarks.clear();
		}

		/*if (ind==int(ind))
			ind = ind+0.01;*/
		if (1)
		{
			//ʹ�����������ؼ��㼰�鱾���߲�ֵ����չƽ�����߹ؼ���
			//��in��vvPT ���������ؼ���
			//��in��vvPB ���������ؼ���
			//��in��fRatio�������߷ָ����
			//��in��fRatio_h����ֱ����ָ����
			//��out��landmarks;�����߹ؼ���

			//�鱾���߼���
			//��in��vvPT ���������ؼ���
			//��in��vvPB ���������ؼ���
			//��out��p2f_1 ���鱾�鼹���е�
			//��out��p2f_2 ���鱾�߽����е�
			Point2f p2f_1(0,0);
			Point2f p2f_2(0,0);
			for (int ii = 1;ii<6;ii++)
			{
				p2f_1.x = p2f_1.x+((fRatio_h-ind)*vvPT[ii].x+ind*vvPB[ii].x)/(fRatio_h*5);
				p2f_1.y = p2f_1.y+((fRatio_h-ind)*vvPT[ii].y+ind*vvPB[ii].y)/(fRatio_h*5);
				p2f_2.x = p2f_2.x+((fRatio_h-ind)*vvPT[vvPT.size()-(2+ii)].x+ind*vvPB[vvPB.size()-(2+ii)].x)/(fRatio_h*5);
				p2f_2.y = p2f_2.y+((fRatio_h-ind)*vvPT[vvPT.size()-(2+ii)].y+ind*vvPB[vvPB.size()-(2+ii)].y)/(fRatio_h*5);
			}
			//line(src,p2f_1,p2f_2,Scalar(0,255,0),5,CV_AA);
			//return src;
			
			if (abs(p2f_1.y-p2f_2.y)<2)
			{
				//���߽���ˮƽ�������߹ؼ����ֵ
				for (int i = 0;i<=fRatio;i++)
				{
					//��������������Ӧ�ؼ������������ߵĽ���
					Point2f midpoint;
					midpoint.x = (ind*vvPB[i].x+(fRatio_h-ind)*vvPT[i].x)/fRatio_h;
					midpoint.y = p2f_1.y;

					//��ֵ��������ؼ���
					landmark.push_back(vvPT[i]);
					//�ϰ벿�ֹؼ������
					for (int j= 0;j<ind-1;j++)
					{
						p2t.x = midpoint.x-((float)(ind-(j+1))/(ind))*(midpoint.x-vvPT[i].x);
						p2t.y = midpoint.y-((float)(ind-(j+1))/(ind))*(midpoint.y-vvPT[i].y);
						landmark.push_back(p2t);
					}
					landmark.push_back(midpoint);

					//�°벿�ֹؼ������
					for (int j= 0;j<(fRatio_h-ind-1);j++)
					{
						p2t.x = midpoint.x+((float)(j+1)/(fRatio_h-ind))*(vvPB[i].x-midpoint.x);
						p2t.y = midpoint.y+((float)(j+1)/(fRatio_h-ind))*(vvPB[i].y-midpoint.y);
						landmark.push_back(p2t);
					}
					landmark.push_back(vvPB[i]);
					landmarks.push_back(landmark);
					landmark.clear();
				}
			}
			else
			{
				//�����߹ؼ����ֵ
				float K1 = (p2f_1.y-p2f_2.y)/(p2f_1.x-p2f_2.x+0.01);//����б��
				float B1 = p2f_1.y-K1*p2f_1.x;//���߽ؾ�

				for (int i = 0;i<=fRatio;i++)
				{
					//�����Ӧ���������ؼ������������ߵĽ���
					Point2f midpoint;
					if (vvPT[i].x-vvPB[i].x==0)
					{
						midpoint.x = vvPT[i].x;
						midpoint.y = B1+K1*vvPT[i].x;
					}
					else
					{
						float K2 = (vvPT[i].y-vvPB[i].y)/(vvPT[i].x-vvPB[i].x+0.001);//����������Ӧ�ؼ������ߵ�б��
						float B2 = vvPT[i].y-K2*vvPT[i].x;//����������Ӧ�ؼ������ߵĽؾ�
						//�������
						midpoint.x = ((B2-B1)/(K1-K2));
			 			midpoint.y = (B1+K1*midpoint.x);
					}
					landmark.push_back(vvPT[i]);
					//�ϰ벿�ֹؼ������
					for (int j= 0;j<ind-1;j++)
					{
						p2t.x = midpoint.x-((float)(ind-(j+1))/(ind))*(midpoint.x-vvPT[i].x);
						p2t.y = midpoint.y-((float)(ind-(j+1))/(ind))*(midpoint.y-vvPT[i].y);
						landmark.push_back(p2t);
						//circle(src,p2t,0,Scalar(0,255,255),10);
					}
					landmark.push_back(midpoint);
					//�°벿�ֹؼ������
					for (int j= 0;j<(fRatio_h-ind-1);j++)
					{
						p2t.x = midpoint.x+((float)(j+1)/(fRatio_h-ind))*(vvPB[i].x-midpoint.x);
						p2t.y = midpoint.y+((float)(j+1)/(fRatio_h-ind))*(vvPB[i].y-midpoint.y);
						landmark.push_back(p2t);
						//circle(src,p2t,0,Scalar(0,255,255),10);
					}
					landmark.push_back(vvPB[i]);
					landmarks.push_back(landmark);
					landmark.clear();
				}
			}
			/*
			if (abs(p2f_1.y-p2f_2.y)<2)
			{
				//���߽���ˮƽ�������߹ؼ����ֵ
				for (int i = 0;i<=fRatio;i++)
				{
					//��������������Ӧ�ؼ������������ߵĽ���
					Point2f midpoint;
					midpoint.x = (ind*vvPB[i].x+(fRatio_h-ind)*vvPT[i].x)/fRatio_h;
					midpoint.y = p2f_1.y;

					//��ֵ��������ؼ���
					landmark.push_back(vvPT[i]);
					//�ϰ벿�ֹؼ������
					for (int j= 0;j<int(ind);j++)
					{
						p2t.x = midpoint.x-((float)(ind-(j)-1)/(ind))*(midpoint.x-vvPT[i].x);
						p2t.y = midpoint.y-((float)(ind-(j)-1)/(ind))*(midpoint.y-vvPT[i].y);
						landmark.push_back(p2t);
					}
					//landmark.push_back(midpoint);

					//�°벿�ֹؼ������
					for (int j= (fRatio_h-int(ind)-1);j>0;j--)
					{
						p2t.x = midpoint.x+((float)(fRatio_h-ind-j)/(fRatio_h-ind))*(vvPB[i].x-midpoint.x);
						p2t.y = midpoint.y+((float)(fRatio_h-ind-j)/(fRatio_h-ind))*(vvPB[i].y-midpoint.y);
						landmark.push_back(p2t);
					}
					landmark.push_back(vvPB[i]);
					landmarks.push_back(landmark);
					landmark.clear();
				}
			}
			else
			{
				//�����߹ؼ����ֵ
				float K1 = (p2f_1.y-p2f_2.y)/(p2f_1.x-p2f_2.x+0.01);//����б��
				float B1 = p2f_1.y-K1*p2f_1.x;//���߽ؾ�

				for (int i = 0;i<=fRatio;i++)
				{
					//�����Ӧ���������ؼ������������ߵĽ���
					Point2f midpoint;
					if (vvPT[i].x-vvPB[i].x==0)
					{
						midpoint.x = vvPT[i].x;
						midpoint.y = B1+K1*vvPT[i].x;
					}
					else
					{
						float K2 = (vvPT[i].y-vvPB[i].y)/(vvPT[i].x-vvPB[i].x+0.001);//����������Ӧ�ؼ������ߵ�б��
						float B2 = vvPT[i].y-K2*vvPT[i].x;//����������Ӧ�ؼ������ߵĽؾ�
						//�������
						midpoint.x = ((B2-B1)/(K1-K2));
			 			midpoint.y = (B1+K1*midpoint.x);
					}
					landmark.push_back(vvPT[i]);
					//�ϰ벿�ֹؼ������
					for (int j= 0;j<int(ind);j++)
					{
						p2t.x = midpoint.x-((float)(ind-j-1)/(ind))*(midpoint.x-vvPT[i].x);
						p2t.y = midpoint.y-((float)(ind-j-1)/(ind))*(midpoint.y-vvPT[i].y);
						landmark.push_back(p2t);
						//circle(src,p2t,0,Scalar(0,255,255),10);
					}
					//landmark.push_back(midpoint);
					//�°벿�ֹؼ������
					for (int j= fRatio_h-int(ind)-1;j>0;j--)
					{
						p2t.x = midpoint.x+((float)(fRatio_h-ind-j)/(fRatio_h-ind))*(vvPB[i].x-midpoint.x);
						p2t.y = midpoint.y+((float)(fRatio_h-ind-j)/(fRatio_h-ind))*(vvPB[i].y-midpoint.y);
						landmark.push_back(p2t);
						//circle(src,p2t,0,Scalar(0,255,255),10);
					}
					landmark.push_back(vvPB[i]);
					landmarks.push_back(landmark);
					landmark.clear();
				}
			}*/
		}
		//////////////////////////////////////////////////////////////////
		//line(src,landmarks[0][0],landmarks[fRatio][0],Scalar(0,250,250),1,CV_AA);
		//line(src,landmarks[0][fRatio_h],landmarks[fRatio][fRatio_h],Scalar(0,250,250),1,CV_AA); 
		//line(src,landmarks[0][ind],landmarks[fRatio][ind],Scalar(0,250,250),1,CV_AA); 
		//////////////////////////////////////////////////////////
		//Step 3������չƽ
		//////////////////////////////
		//������ȡ�Ĺؼ��㣬ʹ��ͶӰ�任չƽ��ҳ
		//��in��src��ԭʼͼ��
		//��in��landmarks������ؼ���
		//��in��dist_T���ϱ߽糤��
		//��in��dist_B���±߽糤��
		//��in��height��չƽ��ͼ��߶�
		//��out��mat_dst��չƽ��ͼ��
		

		//չƽ��ͼ������Ӧ
		float width = ((dist_T+dist_B)/(2*height))-0.7<0?0.7*height:(dist_T+dist_B)/2;
		width = 0.95*width;
		//width = ((width/(height))-0.7>0.001 && (width/(height))-0.7<0.1)?0.8*height:width;
		//float width1 = norm(landmarks[0][ind]-landmarks[fRatio][ind]);
		//float width = (2*width1+dist_T+dist_B)/4;

		//���������չƽ��ͼ��Ĵ�С
		Size dsize = Size(width/fRatio,2*height/fRatio_h);
		Mat image = Mat(dsize,src.type());
    
		dsize = Size(image.cols*fRatio,image.rows);
		Mat result_1 = Mat(dsize,src.type());
        
        dsize = Size(image.cols*fRatio,image.rows*fRatio_h/2);
		Mat result = Mat(dsize,src.type());

		//���������ͶӰ�任�������
		Points1.push_back(Point2f(float(0), float(0)));
		Points1.push_back(Point2f(float(image.cols-1), float(0)));
		Points1.push_back(Point2f(float(image.cols-1), float(image.rows-1)));
		Points1.push_back(Point2f(float(0), float(image.rows-1)));
    

		//ʹ���������ͼ����з��У��ֿ�ͶӰ�任
		Mat PerspectiveImg;
		Mat PerspectiveMatrix;
		float deta_x = 0.5;
		if (vvPB[0].x<vvPB[fRatio].x)
			deta_x = -0.5;


		//ѭ������ͶӰ�任
		for (int j = 0;j<fRatio_h/2;j++)
		{
			for (int i = 0;i<fRatio;i++)
			{
				//�������ȡ
				Points2.clear();
				P2f.x = float(landmarks[i+1][2*j].x+deta_x);
				P2f.y = float(landmarks[i+1][2*j].y);
				Points2.push_back(P2f);
				P2f.x = float(landmarks[i][2*j].x-deta_x);
				P2f.y = float(landmarks[i][2*j].y);
				Points2.push_back(P2f);
				P2f.x = float(landmarks[i][2*(j+1)].x-deta_x);
				P2f.y = float(landmarks[i][2*(j+1)].y-0.5);
				Points2.push_back(P2f);
				P2f.x = float(landmarks[i+1][2*(j+1)].x+deta_x);
				P2f.y = float(landmarks[i+1][2*(j+1)].y-0.5);
				Points2.push_back(P2f);
				
				/*line(src1,Points2[0],Points2[1],Scalar(0,255,0),5,CV_AA);
				line(src1,Points2[0],Points2[3],Scalar(0,255,0),5,CV_AA);
				line(src1,Points2[2],Points2[1],Scalar(0,255,0),5,CV_AA);
				line(src1,Points2[2],Points2[3],Scalar(0,255,0),5,CV_AA);*/

				PerspectiveMatrix = findHomography(Points2, Points1);//�����ά����任����
				warpPerspective(src, PerspectiveImg ,PerspectiveMatrix, Size(image.cols,image.rows));//͸�ӱ任
            
				//��չƽͼ�����ƴ��
				PerspectiveImg.copyTo(result_1(Rect((result_1.cols-(1+i)*PerspectiveImg.cols),0,PerspectiveImg.cols,result_1.rows)));
		
			}
			//��չƽͼ����ֱƴ��
			result_1.copyTo(result(Rect(0,j*result_1.rows,result.cols,result_1.rows)));
			
		}
		mat_dst = result;
		////////////////////////////////////////////////////////////////////////////////////////
	}
	else
	{
		if (abs(vvPT[0].x-vvPT[1].x)<5 || abs(vvPB[0].x-vvPB[1].x)<5 || vvPB[0].y<=vvPT[0].y+5 || vvPB[1].y<=vvPT[1].y+5)// || max(vvPT[0].x,vvPT[1].x)<50 || max(vvPB[0].x,vvPB[1].x)<50 || max(vvPT[0].y,vvPT[1].y)<50 || max(vvPB[0].y,vvPB[1].y)<50)
		{
			//flip(src,src,1);
			return src;
		}

		//����Ĺؼ���Ϊ2ʱֱ�Ӷ�ͼƬ����ͶӰ�任չƽ
		//���ü��Ķ������꣬ʹ��ͶӰ�任չƽ��ҳ

		//���±߽糤�ȼ���
		//��in��vvPT���ϱ߽綥��
		//��in��vvPB���±߽綥��
		//��out��dist_T���ϱ߽糤��
		//��out��dist_B���±߽糤��
		float dist_T = norm(vvPT[0]-vvPT[1]);
		float dist_B = norm(vvPB[0]-vvPB[1]);

		//չƽ��ͼ������Ӧ
		float width = ((dist_T+dist_B)/(2*height))-0.75<0.1?0.7*height:(dist_T+dist_B)/2;

		//���������չƽ��ͼ��Ĵ�С
		Size dsize = Size(width,height);
		Mat image = Mat(dsize,CV_8UC3);
    
		//����ͼ��ͶӰ�任�������
		vector<Point2f> Points1;
		Points1.push_back(Point2f(float(0), float(0)));
		Points1.push_back(Point2f(float(image.cols-1), float(0)));
		Points1.push_back(Point2f(float(image.cols-1), float(image.rows-1)));
		Points1.push_back(Point2f(float(0), float(image.rows-1)));
		
		//����ͼ��ͶӰ����
		Points2.clear();
		P2f.x = float(vvPT[1].x);
		P2f.y = float(vvPT[1].y);
		Points2.push_back(P2f);
		P2f.x = float(vvPT[0].x);
		P2f.y = float(vvPT[0].y);
		Points2.push_back(P2f);
		P2f.x = float(vvPB[0].x);
		P2f.y = float(vvPB[0].y);
		Points2.push_back(P2f);
		P2f.x = float(vvPB[1].x);
		P2f.y = float(vvPB[1].y);
		Points2.push_back(P2f);
            
		Mat PerspectiveMatrix = findHomography(Points2, Points1);//�����ά����任����
		warpPerspective(src, mat_dst ,PerspectiveMatrix, Size(image.cols,image.rows));//͸�ӱ任


	}
	//Mat mat_dst = CBookProcess::BookStretch_inpaint(result);
	if (mat_dst.cols<20 || mat_dst.rows<20)
	{
		mat_dst = src;
	}
    return mat_dst;
}

//*************************************************************
//4.��ָȥ��
//���룺
//Mat src��չƽ��ԭͼ
//int pageType���鱾ҳ���ͣ�0-��ҳ��1-��ҳ
//�����
//Mat dst�������ͼ��
//*************************************************************
Mat CBookProcess::FingerHidding(Mat src, int pageType)
{
	if (src.empty() || src.cols<20 || src.rows<20)
	{
		return src;
	}

	if (pageType)
	{
		flip(src,src,1);
	}

	

	
	Mat temp = src(Rect(0, 0, src.cols/7 ,src.rows));

	Mat src_hsv;
	cvtColor(temp,src_hsv, CV_BGR2HSV);
	vector<cv::Mat> rgbChannels(3); 
	vector<cv::Mat> rgbratio(3); 
	split(src_hsv, rgbChannels);
	Mat temp1 = rgbChannels[1](Rect(0, temp.rows/4-src.rows/10, src.cols/7 ,src.rows/10));
	Mat mask1 = temp1>0.25*255;
	line(mask1,Point(0,0),Point(0,mask1.rows-1),Scalar(255),3,CV_AA);
	mask1 = CBookProcess::bwlabel_mask(mask1,Point(0,0));
	Point point1(0,0);
	for (int ii = 0;ii<mask1.rows;ii++)
	{
		for (int jj = 0;jj<mask1.cols;jj++)
		{
			if (mask1.ptr<uchar>(ii)[jj]>2)
			{
				if (jj>point1.x)
				{
					point1.x = jj;
					point1.y = ii;
				}
			}
		}
	}
	point1.y = temp.rows/10+point1.y;

	Mat temp2 = rgbChannels[1](Rect(0, 3*temp.rows/4, src.cols/7 ,src.rows/10));
	Mat mask2 = temp2>0.25*255;
	line(mask2,Point(0,0),Point(0,mask2.rows-1),Scalar(255),3,CV_AA);
	mask2 = CBookProcess::bwlabel_mask(mask2,Point(0,0));
	Point point2(0,0);
	for (int ii = 0;ii<mask2.rows;ii++)
	{
		for (int jj = 0;jj<mask2.cols;jj++)
		{
			if (mask2.ptr<uchar>(ii)[jj]>2)
			{
				if (jj>point2.x)
				{
					point2.x = jj;
					point2.y = ii;
				}
			}
		}
	}
	point2.y = 8*temp.rows/10+point2.y;
	
	if ((point1.x>20 && point1.x<temp1.cols-50) && (point2.x>20 && point2.x<temp2.cols-50) )
	{
		if (abs(point2.x-point1.x)>30)
		{
			point2.x = max(point2.x,point1.x);
			point1.x = point2.x;
		}

		vector<Point2f> Points1;
		Points1.push_back(Point2f(float(0), float(0)));
		Points1.push_back(Point2f(float(src.cols-1), float(0)));
		Points1.push_back(Point2f(float(src.cols-1), float(src.rows-1)));
		Points1.push_back(Point2f(float(0), float(src.rows-1)));
	
		vector<Point2f> Points2 = Points1;
		Point2f P2f;
		if (point2.x!=point1.x)
		{
			float K = (point2.y-point1.y)/(point2.x-point1.x);
			//����ͼ��ͶӰ����
			P2f.x = float(point1.x-point1.y/K);
			P2f.y = float(0);
			Points2[0] = (P2f);
			P2f.x = float(point2.x+(src.rows-point2.y)/K);
			P2f.y = float(src.rows-1);
			Points2[3] = (P2f);
		}
		else
		{
			P2f.x = float(point1.x);
			P2f.y = float(0);
			Points2[0] = P2f;
			P2f.x = float(point1.x);
			P2f.y = float(src.rows-1);
			Points2[3] = P2f;
		}
    
		Mat mat_dst;
		Mat PerspectiveMatrix = findHomography(Points2, Points1);//�����ά����任����
		warpPerspective(src, mat_dst ,PerspectiveMatrix, Size(src.cols,src.rows));//͸�ӱ任
		src = mat_dst;
	}


	Mat mask = CBookProcess::FingerDetection(src ,0);

	int xxx = src.cols/7;
	mask = CBookProcess::FingerMaskCorrect(src , mask , 0 , xxx);

	Mat color_mask;
	bool flag = CBookProcess::ColorIdentification(src,mask,color_mask,xxx);
	//bool flag = true;
	if (flag==false)
	{
		src = CBookProcess::FingerFill( src , mask , 0);
	}
	if (pageType)
	{
		flip(src,src,1);
	}
	return src;
}
//**************************************************************
//5.1����������
//��������������±�Ե�ؼ�����ȡ������л��ƣ����ڲ���
//���룺
//Mat src��ԭͼ�������������ԭͼ��
//vector<vector<Point> > edges��
//vector<vector<Point> > keyPoints
//**************************************************************
Mat CBookProcess::DrawEdges(Mat src, vector<vector<Point2f> > edges, vector<vector<Point2f> > keyPoints)
{
	Mat dst = src.clone();
	
	if(edges.size() != 2 || edges[0].size() < 2 || edges[1].size() < 2 ||
		keyPoints.size() != 2 || keyPoints[0].size() < 2 || keyPoints[1].size() < 2 )
	{
		//���±�Ե���Ϸ�
		return dst;
	}

	//ֱ�߿��
	const int thickness = 5;
	//Բֱ��
	const int circleThickness = 20;

	//��Ե��ɫ
	const Scalar colorEdge = Scalar(0,255,0);
	//�ؼ�����ɫ
	const Scalar colorKeyPoint = Scalar(13,112,234);
	//��ֹ����ɫ
	const Scalar colorSEPoint = Scalar(0,0,255);

	//���Ʊ�Ե
	vector<Point2f> vcTops = edges[0];
	vector<Point2f> vcBottoms = edges[1];
	//�������±�Ե
	for (int n = 1;n <vcTops.size();n++)
	{
		Point ptS = vcTops[n-1];
		Point ptE = vcTops[n];
		line(dst, ptS, ptE, colorEdge, thickness, CV_AA);  
	}
	for (int n = 1;n <vcBottoms.size();n++)
	{
		Point ptS = vcBottoms[n-1];
		Point ptE = vcBottoms[n];
		line(dst, ptS, ptE, colorEdge, thickness, CV_AA);  
	}

	//���ƹؼ���
	vector<Point2f> vcTopKeyPoints = keyPoints[0];
	vector<Point2f> vcBottomKeyPoints = keyPoints[1];
	//�������±�Ե�ؼ���
	for (int n = 0;n <vcTopKeyPoints.size();n++)
	{
		Point pt = vcTopKeyPoints[n];
		circle(dst,pt, 0, colorKeyPoint, circleThickness);
	}
	for (int n = 0;n <vcBottomKeyPoints.size();n++)
	{
		Point pt = vcBottomKeyPoints[n];
		circle(dst,pt, 0, colorKeyPoint, circleThickness);
	}

	//��ע��ֹλ��
	Point ptLT = vcTops[0];
	Point ptRT = vcTops[vcTops.size() - 1];
	Point ptLB = vcBottoms[0];
	Point ptRB = vcBottoms[vcBottoms.size()-1];
	circle(dst,Point(ptLT.x, ptLT.y), 0, colorSEPoint, circleThickness);
	circle(dst,Point(ptRT.x, ptRT.y), 0, colorSEPoint, circleThickness);
	circle(dst,Point(ptLB.x, ptLB.y), 0, colorSEPoint, circleThickness);
	circle(dst,Point(ptRB.x, ptRB.y), 0, colorSEPoint, circleThickness);

	line(dst,ptLT,ptLB,colorKeyPoint,thickness,CV_AA);  
	line(dst,ptRT,ptRB,colorKeyPoint,thickness,CV_AA);  

	return dst;
}

//*************************************************************
//������
//��ָ�������

//���룺
//Mat src��չƽ��ԭͼ
//Mat mask����ָĿ����ģͼ��


//�����
//Mat dst��ͳ����ͼ��
//*************************************************************
Mat CBookProcess::FingerFill(Mat src ,Mat mask , int pageType)
{
	Mat dst = src.clone();;
	double average = mean(mask)[0];
	if (average>0.01)
	{
		Mat mask_dst = mask(Rect(0,dst.rows/4,dst.cols/4,dst.rows/2));
        Mat mat_src = src(Rect(0,dst.rows/4,dst.cols/4,dst.rows/2));

		//GaussianBlur(mask_dst, mask_dst, Size(25, 25), 1.0, 1.0);//��˹ģ��
		//Mat element = getStructuringElement( MORPH_RECT,Size(10,10));
		//dilate(mask_dst, mask_dst, element );
		//blur(mask_dst, mask_dst, Size(25, 50));

		bitwise_and(mask_dst>0 ,mask_dst<128,mask);

		Mat mat_mean, mat_stddev;
		vector<cv::Mat> rgbChannels(3);  
		vector<cv::Mat> rgb_maskChannels(3);
		split(mat_src, rgbChannels); 
		mask.convertTo(mask,CV_32F);
		for (int i = 0;i<3;i++)
		{
			src = rgbChannels[i];
			src.convertTo(src,CV_32F);
			src = src.mul(mask/255);
			src.convertTo(src,CV_8UC1);
			rgb_maskChannels[i] = src;
		}
		
		average = mean(mask)[0];
		Mat rgb_mask;
		merge(rgb_maskChannels,rgb_mask);
		meanStdDev(rgb_mask,mat_mean,mat_stddev);
		mat_mean = mat_mean*(255/average);
	
		Mat mask_dst1 = ~mask_dst;
		mask_dst1.convertTo(mask_dst1,CV_32F);
		mask_dst.convertTo(mask_dst,CV_32F);
		for (int i = 0;i<3;i++)
		{
			src = rgbChannels[i];
			src.convertTo(src,CV_32F);
			src = (mat_mean.ptr<double>(0)[i])*(mask_dst/255)+src.mul(mask_dst1/255);
			src.convertTo(src,CV_8UC1);
			rgbChannels[i] = src;
		}
		Mat mat_dst;
		merge(rgbChannels,mat_dst);
		mat_dst.copyTo(dst(Rect(0,dst.rows/4,dst.cols/4,dst.rows/2)));
	}
	return dst;
}


//*************************************************************
//������
//��ָ��ģ����

//���룺
//Mat src��չƽ��ԭͼ
//Mat mask����ָĿ����ģͼ��


//�����
//Mat mask����ָĿ����ģͼ��
//*************************************************************
Mat CBookProcess::FingerMaskCorrect(Mat mat_src ,Mat mask , int pageType,int& out_x)
{
	/*
	Mat src = mat_src.clone();
	double scale = 500.0/src.rows;
    Size dsize = Size(src.cols*scale,src.rows*scale);
    Mat image = Mat(dsize,CV_8UC1);
    resize(src, image, dsize);
	src = image;

	Point point1(0,0);
	Point point2(0,src.rows-1);
	Point point3(src.cols-1,0);
	Point point4(src.cols-1,src.rows-1);
	Point2f point_1(0,src.rows/4);//���ϽǶ���
	Point2f point_2(src.cols*2/7, 5*src.rows/6);//���½Ƕ���

	int x11 = mask.cols;
	int x12 = 0;
	int y11 = mask.rows;
	int y12 = 0;
	int yy;
	int p = 0;
	for (int ii=0;ii<mask.rows;ii++)
	{
		for (int jj=0;jj<mask.cols;jj++)
		{
			if (mask.ptr<uchar>(ii)[jj]>0)
			{
				if (x11>jj)
					x11 = jj;
				if (x12<jj)
				{
					x12 = jj;
					yy = ii;
				}
				if (y11>ii)
					y11 = ii;
				if (y12<ii)
					y12 = ii;
				p += 1;
			}
		}
	}
	int x_max_3 = x12;


	yy = point_1.y+yy;		
	Point2f point_3;
	point_3.x = point_2.x;
	point_3.y = point_1.y+50;//

	point_1.y = point_1.y-(point2.y-point1.y)/8;
	point_2.y = point_2.y+(point2.y-point1.y)/8;
	Mat bw1 = CBookProcess::GradientAdaptiveThreshold_Rect(src,point_1,point_3,1.5);
	point_3.x = point_1.x;
	point_3.y = point_2.y-150;//point_1.y+(point2.y-point1.y)/8+y12;//;yy+30;
	Mat bw2 = CBookProcess::GradientAdaptiveThreshold_Rect(src,point_3,point_2,1.5);
	bitwise_or(bw1,bw2,bw2);
	Mat bw = CBookProcess::GradientAdaptiveThreshold_Rect(src,point_1,point_2,1.5);


	bw2 = CBookProcess::bwlabel(bw2,20);
	Mat element = getStructuringElement( MORPH_RECT,Size(5,3));
	dilate(bw2,bw2, element );
	line(bw2,point1,point2,Scalar(255),5);
	bw2 = CBookProcess::bwlabel(bw2,50);
	bw1 = CBookProcess::bwlabel_mask(bw2,point1);
	bw = CBookProcess::bwlabel_mask(bw2,point2);
	bitwise_or(bw1,bw,bw);
	bitwise_and(bw2,~bw,bw2);
		
	Mat Threah = bw2.clone();
	point_1.x = point_2.x;
	line(Threah,point_1,point_2,Scalar(255),2);
	//return mask;
	Point min_point = CBookProcess::bwlabel_min_x(Threah, point_1);
	int x_max = min_point.x;

	if (x_max>point_2.x-20)
	{
		vector<int> find_index;
		float mean_find = 0;
		for (int ii =bw2.cols/10;ii<=point_2.x/scale;ii++)
		{
			int sum_pix = 0;
			for (int jj =point_1.y;jj<=point_2.y/scale;jj++)
			{
				if (mask.ptr<uchar>(jj)[ii]>0)
				{
					sum_pix += 1;
				}
			}
			mean_find += sum_pix/(point_2.x-20+1);
			find_index.push_back(sum_pix);
		}
		int label  =0;
		for (int ii = 0;ii<find_index.size();ii++)
		{
			if (find_index[ii]==0)
			{
				label += 1;
			}
			else
			{
				label = 0;
			}
			if (label>5)
			{		
				if (ii+bw.cols/10<x_max)
				{
					x_max = (ii+bw.cols/10)*scale;
				}
				break;
			}
		}

	}
		
	bw2 = CBookProcess::bwlabel(bw,20);
	element = getStructuringElement( MORPH_RECT,Size(3,3));
	dilate(bw2,bw2, element );
	line(bw2,point1,point2,Scalar(255),5);
	bw2 = CBookProcess::bwlabel(bw2,50);
	bw1 = CBookProcess::bwlabel_mask(bw2,point1);
	bw = CBookProcess::bwlabel_mask(bw2,point2);
	bitwise_or(bw1,bw,bw);
	bitwise_and(bw2,~bw,bw2);
			
	Threah = bw2.clone();
	point_1.x = point_2.x;
	line(Threah,point_1,point_2,Scalar(255),2);
	//return Threah;
	Point min_point1 = CBookProcess::bwlabel_min_x(Threah, point_1);
	int x_max_2 = min_point1.x;

	int x1,x2,y1,y2;		
	x1 = int(min(point1.x,point2.x));
	if (min(x_max,x_max_2)>x12)
		x_max = min(x_max,x_max_2);
	if (x_max<(point_1.x-15))
	{
		x2 = x_max;
	}
	else
	{
		x2 = x12 + int(min(point1.x,point2.x))+10;
		if (x2>x_max)
			x2 = x_max-5;
	}
	y1 = y11 -20;
	y2 = y12 +20;



	//��ָ�������
	//�ָ�����ֵ��ԭͼ������
	x1 = 0;//x1/scale;
	x2 = x2/scale;
	//return mask_dst;
	int x3 = (x_max_3);
	if (x2<x3)
	{
		for (int ii = x2+1;ii<=x3+10;ii++)
		{
			for (int jj = 0;jj<mask.rows;jj++)
			{
				mask.ptr<uchar>(jj)[ii] = 0;
			}
		}
	}
	*/

	Mat src = mat_src.clone();
	//double scale = 500.0/src.rows;
    Size dsize = Size(mask.cols,mask.rows);
    Mat image = Mat(dsize,CV_8UC1);
    resize(src, image, dsize);

	Point point1(0,0);
	Point point2(0,image.rows-1);
	Point point3(image.cols-1,0);
	Point point4(image.cols-1,image.rows-1);

	int x11 = mask.cols;
	int x12 = 0;
	int y11 = mask.rows;
	int y12 = 0;
	int yy;
	int p = 0;
	for (int ii=mask.rows/4;ii<3*mask.rows/4;ii++)
	{
		for (int jj=0;jj<mask.cols/4;jj++)
		{
			if (mask.ptr<uchar>(ii)[jj]>0)
			{
				if (x11>jj)
					x11 = jj;
				if (x12<jj)
				{
					x12 = jj;
					yy = ii;
				}
				if (y11>ii)
					y11 = ii;
				if (y12<ii)
					y12 = ii;
				p += 1;
			}
		}
	}
	int x_max_3 = x12;
   
	Point2f p2t_1(point1.x,(point2.y-point1.y)/7);
	Point2f p2t_2(point1.x+(point4.x-point1.x)/4,(point2.y-point1.y)/4+50);
	Point2f p2t_3(point2.x,point2.y-(point2.y-point1.y)/4-50);
	Point2f p2t_4(point2.x+(point4.x-point1.x)/4,point2.y-(point2.y-point1.y)/7);

	Mat temp1 = CBookProcess::GradientAdaptiveThreshold_Rect(image,p2t_1,p2t_2,1.6);//����Ӧ�����Ŵ���
	temp1 = CBookProcess::bwlabel(temp1,20);
	Mat element = getStructuringElement( MORPH_RECT,Size(4,3));
	dilate(temp1,temp1, element );
	
	Mat temp2 = CBookProcess::GradientAdaptiveThreshold_Rect(image,p2t_3,p2t_4,1.6);//����Ӧ�����Ŵ���
	temp2 = CBookProcess::bwlabel(temp2,20);
	element = getStructuringElement( MORPH_RECT,Size(4,3));
	dilate(temp2,temp2, element );
	
	p2t_1.x = p2t_2.x;
	p2t_3.x = p2t_4.x;
	line(temp1,p2t_1,p2t_2,Scalar(255),5);
	line(temp2,p2t_3,p2t_4,Scalar(255),5);
	//imwrite("output1.jpg",temp1);
	//imwrite("output2.jpg",temp2);
	Point min_point1 = CBookProcess::bwlabel_min_x(temp1, p2t_1);
	Point min_point2 = CBookProcess::bwlabel_min_x(temp2, p2t_3);
    int x2 = min(min_point1.x,min_point2.x);
	if (x2<mask.cols/15)
		x2 = max(min_point1.x,min_point2.x);

	//��ָ�������
	//�ָ�����ֵ��ԭͼ������
	//return mask_dst;
	int x3 = (x_max_3);
	out_x = x3+2;
	if (x2<x3)
	{
		out_x = x2+2;
		for (int ii = x2+1;ii<=x3+10;ii++)
		{
			for (int jj = mask.rows/4;jj<3*mask.rows/4;jj++)
			{
				mask.ptr<uchar>(jj)[ii] = 0;
			}
		}
	}

    element = getStructuringElement( MORPH_RECT,Size(3,3));
    dilate(mask, mask, element );
    blur(mask, mask, Size(5, 10));
    
    dsize = Size(src.cols,src.rows);
	Mat mask_dst = Mat(dsize,CV_8UC1);
	resize(mask, mask_dst, dsize);
	return mask_dst;
}



//*************************************************************
//������
//��ָ���

//���룺
//Mat src��չƽ��ԭͼ


//�����
//Mat mask����ָĿ����ģͼ��
//*************************************************************
Mat CBookProcess::FingerDetection(Mat src ,int pageType)
{
	//ͼƬ��ֵ����
	double scale = 500.0/src.rows;
    Size dsize = Size(src.cols*scale,src.rows*scale);
    Mat image1 = Mat(dsize,CV_8UC1);
    resize(src, image1, dsize);

	Mat image = image1(Rect(0,image1.rows/4,image1.cols/4,image1.rows/2));
	Mat bw = CBookProcess::detection(image);
	
	Mat temp =  CBookProcess::YCrCb_detect(image);
	cvtColor(temp, temp, CV_BGR2GRAY);
	Mat bw1;
	threshold(temp,bw1,2,255,THRESH_BINARY);
	//return bw1;

	bitwise_or(bw,bw1,bw);

	Mat mask;
	Canny(image,mask,50,150,3);
	//Matmask = CBookProcess::GradientAdaptiveThreshold(image,2.0);
	bitwise_or(bw,mask,mask);
	
	CBookProcess::fillHole(mask, bw);

	bw1 =bw.clone();
	line(bw,Point(0,0),Point(0,image.rows-1),Scalar(255),2);
	
	mask = CBookProcess::bwlabel_mask(bw,Point(0,0));
	bitwise_and(mask,bw1,mask);

	Mat element = getStructuringElement( MORPH_RECT,Size(3,3));
	dilate(mask, mask, element );

	CBookProcess::fillHole(mask,mask);
	//Mat temp1 = mask(Rect(0,mask.rows/4,mask.cols/4,mask.rows/2));
	Mat image01 = Mat::zeros(image1.rows,image1.cols,CV_8UC1);
	mask.copyTo(image01(Rect(0,image1.rows/4,image1.cols/4,image1.rows/2)));
	/*dsize = Size(src.cols,src.rows);
	Mat mask_dst = Mat(dsize,CV_8UC1);
	resize(image01, mask_dst, dsize);*/
	Mat mask_dst = image01;

	/*Mat bw = CBookProcess::HistAdaptiveThreshold(image,false);
	
	Mat bw3 = bw;
	Mat bw5 = Mat::zeros(bw3.size(),CV_8UC1);
	//line(bw,Point(0,0),Point(0,image.rows-1),Scalar(0),2);
	
	Mat bw1,bw2;


	//�ֱ���������������ĸ��������̾��룬����¼��̾���������λ��
	//��ҳ�������
	Point2f point1(0, 0);//���ϽǶ���
	Point2f point2(0, bw.rows-1);//���½Ƕ���

			
	Point2f point_1(0,image.rows/4);//���ϽǶ���
	Point2f point_2(image.cols/3, 3*image.rows/4);//���½Ƕ���
					

	int height = int(point_1.y);
	Mat temp = image(Rect(min(point1.x,point2.x),point_1.y,point_2.x-min(point1.x,point2.x),point_2.y-point_1.y));
	Mat temp1 =  CBookProcess::YCrCb_detect(temp);
	//return temp1;
	cvtColor(temp1, temp1, CV_BGR2GRAY);
	threshold(temp1,bw2,2,255,THRESH_BINARY);
	Mat element = getStructuringElement(MORPH_ELLIPSE,Size(3,3));
	morphologyEx(bw2,bw2, MORPH_OPEN, element);
	element = getStructuringElement( MORPH_RECT,Size(6,3));
	dilate(bw2,bw2, element );
	bw = bw2.clone();


	line(bw2,Point(0,0),Point(0,bw2.rows-1),Scalar(255),5);
	bw2 = CBookProcess::bwlabel(bw2,50);
	bw1 = CBookProcess::bwlabel_mask(bw2,Point(0,0));
	Mat YCrCb_mask;
	bitwise_and(bw1,bw,YCrCb_mask);
		
	for (int ii = 0;ii<bw.rows;ii++)
	{
		for (int jj = 0;jj<bw.cols;jj++)
		{
			bw5.ptr<uchar>(int(point_1.y)+ii)[int(min(point1.x,point2.x))+jj] = bw3.ptr<uchar>(int(point_1.y)+ii)[int(min(point1.x,point2.x))+jj]+YCrCb_mask.ptr<uchar>(ii)[jj];
		}
	}
	bw3 = bw5.clone();
	element = getStructuringElement( MORPH_RECT,Size(15,15));
	dilate(bw3,bw3, element );
	line(bw3,point_1,Point(point_1.x,point_2.y),Scalar(255),2);
	Mat mask1 = CBookProcess::GradientAdaptiveThreshold_Rect(image,point_1,point_2,1.5);
    bitwise_or(mask1,bw3,bw3);
	CBookProcess::fillHole(bw3, bw3);
	Mat bw4 = bw3.clone();
	bw3 = CBookProcess::bwlabel_mask(bw3,point_1);
	bitwise_and(bw4,bw3,bw3);

	dsize = Size(src.cols,src.rows);
	Mat mask_dst = Mat(dsize,CV_8UC1);
	resize(bw3, mask_dst, dsize);
		
	//return mask_dst;

	int x11 = bw3.cols;
	int x12 = 0;
	int y11 = bw3.rows;
	int y12 = 0;
	int yy;
	int p = 0;
	for (int ii=0;ii<bw3.rows;ii++)
	{
		for (int jj=0;jj<bw3.cols;jj++)
		{
			if (bw3.ptr<uchar>(ii)[jj]>0)
			{
				if (x11>jj)
					x11 = jj;
				if (x12<jj)
				{
					x12 = jj;
					yy = ii;
				}
				if (y11>ii)
					y11 = ii;
				if (y12<ii)
					y12 = ii;
				p += 1;
			}
		}
	}
	int x_max_3 = x12;

	//line(bw3,Point(20,point_1.y-5),Point(20,point_2.y+5),Scalar(0),40);
	//return bw3;
	int x21 = bw3.cols;
	int x22 = 0;
	int y21 = bw3.rows;
	int y22 = 0;
	for (int ii=0;ii<bw3.rows;ii++)
	{
		for (int jj=3;jj<12;jj++)
		{
			if (bw3.ptr<uchar>(ii)[jj]>0)
			{
				if (x21>jj)
					x21 = jj;
				if (x22<jj)
				{
					x22 = jj;
				}
				if (y21>ii)
					y21 = ii;
				if (y22<ii)
					y22 = ii;
			}
		}
	}

	int x31 = bw3.cols;
	int x32 = 0;
	int y31 = bw3.rows;
	int y32 = 0;
	for (int ii=0;ii<bw3.rows;ii++)
	{
		for (int jj=40;jj<=point_2.x;jj++)
		{
			if (bw3.ptr<uchar>(ii)[jj]>0)
			{
				if (x31>jj)
					x31 = jj;
				if (x32<jj)
				{
					x32 = jj;
				}
				if (y31>ii)
					y31 = ii;
				if (y32<ii)
					y32 = ii;
			}
		}
	}

	if((y32-y31)>(bw3.rows/2-5) && (y22-y21)<(bw3.rows/2-5))// && src.rows>1400)
	{
		for (int ii = 0;ii<bw.rows;ii++)
		{
			for (int jj = 0;jj<bw.cols;jj++)
			{
				bw5.ptr<uchar>(int(point_1.y)+ii)[int(min(point1.x,point2.x))+jj] = YCrCb_mask.ptr<uchar>(ii)[jj];
			}
		}
		bw3 = bw5.clone();
		element = getStructuringElement( MORPH_RECT,Size(15,15));
		dilate(bw3,bw3, element );
		line(bw3,point_1,Point(point_1.x,point_2.y),Scalar(255),2);
		mask1 = CBookProcess::GradientAdaptiveThreshold_Rect(image,point_1,point_2,1.3);
		bitwise_or(mask1,bw3,bw3);
		CBookProcess::fillHole(bw3, bw3);
		bw4 = bw3.clone();
		bw3 = CBookProcess::bwlabel_mask(bw3,point_1);
		bitwise_and(bw4,bw3,bw3);
		resize(bw3, mask_dst, dsize);

		x11 = bw3.cols;
		x12 = 0;
		y11 = bw3.rows;
		y12 = 0;
		yy;
		p = 0;
		for (int ii=0;ii<bw3.rows;ii++)
		{
			for (int jj=0;jj<bw3.cols;jj++)
			{
				if (bw3.ptr<uchar>(ii)[jj]>0)
				{
					if (x11>jj)
						x11 = jj;
					if (x12<jj)
					{
						x12 = jj;
						yy = ii;
					}
					if (y11>ii)
						y11 = ii;
					if (y12<ii)
						y12 = ii;
					p += 1;
				}
			}
		}
		x_max_3 = x12;
	}*/

	return mask_dst;
}

//**************************************************************
//������
//��ֵ��Ŀ����ͨ����Сͼ�����������㷨�ٶȣ����������С���������㷨����ʱ��Ҫ�������ϵת��

//���룺
//Mat src:�鱾ԭͼ

//�����
//Mat���������ͼ��
//Scale�����ű���������Ϊ1��֤��û�в�ֵ��
//***************************************************************
Mat CBookProcess::Resize(Mat src,double &scale)
{
	if (src.empty())
	{
		scale = 1;
		return src;
	}
	if (src.rows>3200)
	{
		scale= 1.0/2.0;
		Size dsize = Size(src.cols*scale,src.rows*scale);
		Mat image = Mat(dsize,CV_8UC1);
		resize(src, image, dsize);
		src = image;
	}
	else if (src.rows>2000)
	{
		scale= 0.5;
		Size dsize = Size(src.cols*scale,src.rows*scale);
		Mat image = Mat(dsize,CV_8UC1);
		resize(src, image, dsize);
		src = image;
	}
	else if (src.rows<1000)
	{
		scale= 1000.0/src.rows;
		Size dsize = Size(src.cols*scale,src.rows*scale);
		Mat image = Mat(dsize,CV_8UC1);
		resize(src, image, dsize);
		src = image;
	}
	else
	{
		scale = 1;
	}
	return src;
}

//**************************************************************
//������
//��ֵ��

//���룺
//Mat src:�鱾ԭͼ

//�����
//Mat����ֵͼ��
//***************************************************************
Mat CBookProcess::Threshold(Mat src)
{
	if (src.empty())
	{
		return src;
	}
	
	//Mat bw = CBookProcess::HistAdaptiveThreshold(src,true);
	Mat bw = CBookProcess::im2bw(src);
	//bw = CBookProcess::bwlabel(bw,20);

	Mat canny_bw;
	blur( src, src, Size(3,3) );
	Canny(src,canny_bw,50,150,3);
	bitwise_or(canny_bw,bw,bw);	

	bw = CBookProcess::bwlabel(bw,50);

	Mat element = getStructuringElement(MORPH_RECT,Size(5,2));
	morphologyEx(bw,bw, MORPH_CLOSE, element);

	CBookProcess::fillHole(bw,bw);

	//dilate(mask,mask, element );
	CBookProcess::delete_jut(bw, bw, 20, 20, 1);

	return bw;
}

//**************************************************************
//������
//�鱾��Ե��ȡ��ָ��ȡ�鱾���±�Ե����������

//���룺
//Mat bw:��ֵͼ��

//�����
//vector<vector<Point> > edges:�鱾���±�Ե����������Ϊ�գ�������ͼ�񲻺Ϸ�ʱΪ��
//***************************************************************
vector<vector<Point> > CBookProcess::GetBookEdges(Mat bw)
{
	vector<vector<Point> > edges;
	vector<Point> areaMatvPoint = CBookProcess::GetMaxBookContour( bw );
	if (areaMatvPoint.size()>200)
	{
		edges = CBookProcess::GetBookEdgesFromContour(bw ,areaMatvPoint);
	}

	return edges;
}

//**************************************************************
//������
//������ȡ����ȡ����������������

//���룺
//Mat bw:��ֵ��ͼ��

//�����
//vector<Point> �����㼯
//***************************************************************
vector<Point> CBookProcess::GetMaxBookContour(Mat bw)
{
	vector<Point> areaMatvPoint;
	//��������ȡ��vvPointΪ������������
    vector<vector<Point> > vvPoint;
    findContours(bw,vvPoint,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    
    //����ͼ�������������������
    int rectMaxArea =0;
    int rectArea =0;
    RotatedRect rMaxRect;
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
			
        }
    }
	vector<Point> areaMatvPoint_1 = CBookProcess::CheckBookContour(areaMatvPoint);
	return areaMatvPoint_1;
}

//**************************************************************
//������
//�ж��鱾��Χ���Ƿ�Ϸ���������Ϸ��򷵻�

//���룺
//vector<Point> �����㼯

//�����
//vector<Point> �����㼯,����Ϊ��
//***************************************************************
vector<Point> CBookProcess::CheckBookContour(vector<Point> contour)
{
	if (contour.size()<2000)
		contour.clear();
	return contour;
		
}

//**************************************************************
//������
//�鱾���±�Ե������ȡ��

//���룺
//vector<Point> contour,�鱾�����㼯

//�����
//vector<vector<Point> > edges:�鱾���±�Ե����������Ϊ�գ�������ͼ�񲻺Ϸ�ʱΪ��
//***************************************************************
vector<vector<Point> > CBookProcess::GetBookEdgesFromContour(Mat src ,vector<Point> contour)
{
	vector<Point> areaMatvPoint = contour;
	vector<vector<Point> > edgePoint;

	//����Ŀ��������y����������Сֵ
	//��in��areaMatvPoint��Ŀ������
	//��out��min_y��yֵ��Сֵ
	//��out��max_y��yֵ���ֵ
	int max_y = 0;
	int min_y = 10000;
	Point2f p2t;
	for (int i=0;i<areaMatvPoint.size();i++)
	{
		p2t = areaMatvPoint[i];
		if (p2t.y<min_y) min_y = p2t.y;
		if (p2t.y>max_y) max_y = p2t.y;
	}

	///////////////////////////////////////////////////
	//��ҳ�ָ�λ�ö������
	//��in��areaMatvPoint��Ŀ������
	//��out��ind3��Ŀ�����½Ƕ���
	//��out��ind4��Ŀ�����ϽǶ���
	Point2f point3(11*src.cols/12, max_y);//���½Ƕ�������
	Point2f point4(11*src.cols/12, min_y);//���ϽǶ������� 
	double min_dist3 = 10000;
	double min_dist4 = 10000;
	int ind3 = 0;
	int ind4 = 0;
	//�ֱ�������������������������̾��룬����¼��̾���������λ��
	float th = src.cols-20;
	for (int i=0;i<areaMatvPoint.size();i++)
	{
		p2t = areaMatvPoint[i];
		if (p2t.x>th)
			continue;
		double dist3 = norm(p2t-point3);
		if (dist3<min_dist3)
		{
			min_dist3 = dist3;
			ind3 = i;
		}
		double dist4 = norm(p2t-point4);
		if (dist4<min_dist4)
		{
			min_dist4 = dist4;
			ind4 = i;
		}
	}
	///////////////////////////////////////////////////////////////////

	//���ϽǶ�����ʱ�뵽���½Ƕ���������ȡ
	//��in��areaMatvPoint��Ŀ������
	//��in��ind3��Ŀ�����ϽǶ���
	//��in��ind4��Ŀ�����ϽǶ���
	//��out��P2TT��Ŀ������

	vector<Point> P2TT;
	if (ind4<ind3)
	{
		for (int i=ind4;i<ind3;i++)
		{
			p2t = areaMatvPoint[i];
			P2TT.push_back(p2t);
		}
	}
	else
	{
		for (int i=ind4;i<areaMatvPoint.size();i++)
		{
			p2t = areaMatvPoint[i];
			P2TT.push_back(p2t);
       
		}
		for (int i=0;i<ind3;i++)
		{
			p2t = areaMatvPoint[i];
			P2TT.push_back(p2t);
       
		}
	}

	point3 = areaMatvPoint[ind3];
	point4 = areaMatvPoint[ind4];

	///////////////////////////////////////////////////////////////////
	//�鱾���߶�����ȡ
	//
	//��in��P2TT��Ŀ������
	//��out��ind1�����ϽǶ�������
	//��out��ind2�����½Ƕ�������
	//��out��point1�����ϽǶ���
	//��out��point2�����½Ƕ���
	Point2f point1(5*src.cols/6-(point3.y-point4.y)/3, 0);//���ϽǶ���
	Point2f point2(5*src.cols/6-(point3.y-point4.y)/3, src.rows-1);//���½Ƕ���
	int ind1 = 100;
	int ind2 = P2TT.size()-100;
	double max_dist1 = 0;
	double max_dist2 = 0;

	//���ӵ㣨�����ӵ�Ϊ��㣬ѭ��������������Զ�������������µ�����ֱ��������������Զ�����㣩
	Point2f point5;
	point5.x = src.cols;//5*bw.cols/6-(point4.y+point3.y)/4;
	point5.y = (point4.y+point3.y)/2;
	for(int ii = 0;ii<10;ii++)
	{
		max_dist1 = 0;
		max_dist2 = 0;

		int label = 0;
		for (int i=ind1;i<ind2;i++)
		{
			p2t = P2TT[i];
			if (p2t.x>min(point1.x,point5.x) || p2t.y>point4.y+(point3.y-point4.y)/4)
				continue;
			double dist1 = norm(p2t-point5);
			if (dist1>max_dist1)
			{
				max_dist1 = dist1;
				ind1 = i;
				label = 0;
			}
			else
				label += 1;
			if (label>50)
				break;
			}
		//��ȡ�����ϱ߽綥������
		point1 = P2TT[ind1];
				
		int width = point1.x;
			
		label = 0;
				
		for (int i=ind2;i>ind1;i--)
		{
			p2t = P2TT[i];
			if (p2t.x>min(point2.x,point5.x) || p2t.y<point3.y-(point3.y-point4.y)/4)
				continue;
			double dist2 = norm(p2t-point5);
			if (dist2>max_dist2)
			{
				max_dist2 = dist2;
				ind2 = i;
				label = 0;
			}
			else
			{
				label += 1;
			}
			if(label>50)
				break;
			}
		point2 = P2TT[ind2];

		if (point5.x-(point1.x+point2.x)/2<40 || abs(point1.x-point2.x)<60)
			break;
		//point5.x = max(point1.x,point2.x);
		if (abs(point1.x-point2.x)>100)
		{
			point5.x = max(point1.x,point2.x)-30;
		}
		else
		{
			point5.x = max(point1.x,point2.x)-10;
		}
	}
	////////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////
	//�߽綥���������������ȡ������
	//�����鱾�߽�ǳ��ֺ�ɫ��ʱ���½������㲢�����ϱ߽�������

	//���ϽǶ������
	//��in��P2TT��Ŀ������
	//��in��point1�����ϽǶ���
	//��in/out��ind1�����ϽǶ�������/����������߷��ϱ�׼����������
	//��in��0���ϱ߽����
	//��out��point1_1�����������ϽǶ���
	Point2f point1_1 = point1;
	ind1 = CBookProcess::Vertex_Correction( P2TT, point1,point1_1, ind1, 0, src.cols, src.rows);//col,rowΪͼ��Ŀ��
		
	//���½Ƕ������
	//��in��P2TT��Ŀ������
	//��in��point2�����½Ƕ���
	//��in/out��ind2�����½Ƕ�������//����������߷��ϱ�׼����������
	//��in��0���±߽����
	//��out��point2_1�����������½Ƕ���
	Point2f point2_1 = point2;
	ind2 = CBookProcess::Vertex_Correction( P2TT, point2,point2_1, ind2, 1, src.cols,src.rows);//col,rowΪͼ��Ŀ��

	
	vector<Point> vPT;
	vector<Point> vPB;

	//�ϱ߽�������ȡ
	//��in��P2TT��Ŀ������
	//��in��point1�����ϽǶ���
	//��in��point1_1���������ϽǶ���
	//��in��ind1������������߷��ϱ�׼����������
	//��out��vPT���ϱ߽�����
	//������ȡ
	for(int i= 0;i<=ind1;i++)
	{
		p2t = P2TT[i];
		vPT.push_back(p2t);
	}

	//��������
	if (point1.x<point1_1.x)
	{
		for(int i= point1_1.x;i>=point1.x;i--)
		{
			p2t.x = i;
			p2t.y = point1_1.y;
			vPT.push_back(p2t);
		}
	}
	else if (point1.x>point1_1.x)
	{
		for(int i= point1.x;i>=point1_1.x;i--)
		{
			p2t.x = i;
			p2t.y = point1.y;
			vPT.push_back(p2t);
		}
	}
    

	//�±߽�������ȡ����
	//��in��P2TT��Ŀ������
	//��in��point2�����½Ƕ���
	//��in��point2_1���������½Ƕ���
	//��in��ind2������������߷��ϱ�׼����������
	//��out��vPB���±߽�����
	//������ȡ
	for(int i= ind2;i<P2TT.size();i++)
	{
		p2t = P2TT[i];
		vPB.push_back(p2t);
	}

	//��������
	if (point2.x<point2_1.x)
	{
		for(int i= point2_1.x;i>=point2.x;i--)
		{
			p2t.x = i;
			p2t.y = point2_1.y;
			vPB.insert(vPB.begin()+0,p2t);
		}
	}
	else if (point2.x>point2_1.x)
	{
		for(int i= point2.x;i>=point2_1.x;i--)
		{
			p2t.x = i;
			p2t.y = point2.y;
			vPB.insert(vPB.begin()+0,p2t);
		}
	}
	////////////////////////////////////////////////////////////////////////
	edgePoint.push_back(vPT);
	edgePoint.push_back(vPB);

	edgePoint = CBookProcess::CheckBookEdges(edgePoint);

	return edgePoint;
}

//**************************************************************
//������
//�ж��鱾���±�Ե�����Ƿ�Ϸ���

//���룺
//vector<vector<Point> > edges:�鱾���±�Ե����������Ϊ�գ�������ͼ�񲻺Ϸ�ʱΪ��

//�����
//vector<vector<Point> > edges:�鱾���±�Ե����������Ϊ�գ�������ͼ�񲻺Ϸ�ʱΪ��
//***************************************************************
vector<vector<Point> > CBookProcess::CheckBookEdges(vector<vector<Point> > edges)
{
	vector<Point> vPT = edges[0];
	vector<Point> vPB = edges[1];

	Point point1 = vPT[vPT.size()-1];
	Point point4 = vPT[0];

	Point point3 = vPB[vPB.size()-1];
	Point point13 = vPB[vPB.size()-2];
	Point point2 = vPB[0];

	if((point3.y-point4.y)/(((point4.x-point1.x)+(point3.x-point2.x))/2)>2)
	{
		edges.clear();
	}
	if(abs((point4.x-point1.x)-(point3.x-point2.x))>0.15*(point3.y-point4.y))
	{
		edges.clear();
	}
	if(abs((point2.y-point1.y)-(point3.y-point4.y))>0.2*(point3.y-point4.y))
	{
		edges.clear();
	}
	if((point2.y-point1.y)<300)
	{
		edges.clear();
	}
	return edges;
}

//**************************************************************
//������
//У�����±�Ե����������
//��1����֤���µ����
//��2��ȥ�����ڲ��֣����ں�ȵ���

//���룺
//vector<vector<Point> > edges:�鱾���±�Ե����������Ϊ�գ�������ͼ�񲻺Ϸ�ʱΪ��

//�����
//vector<vector<Point> > edges:�鱾���±�Ե����������Ϊ�գ�������ͼ�񲻺Ϸ�ʱΪ��
//***************************************************************
vector<vector<Point> > CBookProcess::AdjusBookEdges(Mat src ,vector<vector<Point> > edges,bool type)
{
	vector<Point> vPT;
	vector<Point> vPB;
	vPT = edges[0];
	vPB = edges[1];

	//�ϱ߽綥����ȡ
	//��in��vPT���ϱ߽�����
	//��out��point1������
	//��out��point4������
	Point2f point1 = vPT[vPT.size()-1];
	Point2f point4 = vPT[0];

	//�±߽綥����ȡ
	//��in��vPB���±߽�����
	//��out��point2������
	//��out��point3������
	Point2f point2 = vPB[0];
	Point2f point3 = vPB[vPB.size()-1];


	for (int idx = 0;idx<vPT.size();idx++)
	{
		if (vPT[idx].x<(src.cols*(3/4.0)))
			break;

	}
	for (int idx = vPB.size()-1;idx>0;idx--)
	{
		if (vPB[idx].x<(src.cols*(3/4.0)))
			break;

	}
	////////////////////////////////////////////////////////////////
	//Step 5:������������ƽ������
	//*******
	//�������±߽������쳣��ȥ����ݣ���ݣ��쳣������
	//��Tim��һ��ʾ��ͼ
	//��in��rc��ԭͼ
	//��in/out��vPT���ϱ߼�����
	//��in/out��vPB���±߽�����
	CBookProcess::Curve_Smooth( src, vPB, vPT);

	if(1)
	{
		//��Ϊƽ�������ֱ�С�����ñ��볬�����������Խ��ϱ�Եflip
		//��in/out��vPT���ϱ߼�����
		for (int i = 0;i<vPT.size();i++)
		{
			vPT[i].y = src.rows-vPT[i].y;	
		}

		//���±߽���������ƽ��
		//���ƽ��
		//��in/out��vPT���ϱ߼�����
		//��in/out��vPB���±߽�����
		for (int j = 0;j<1;j++)
		{
			for (int i = 2;i<vPB.size()-2;i++)
			{
				//vPB[i].y = (vPB[i-2].y+vPB[i+2].y)/9.0+2.0*(vPB[i-1].y+vPB[i+1].y)/9.0+(vPB[i].y)/3.0;
				vPB[i].y = (vPB[i-2].y+vPB[i+2].y+vPB[i-1].y+vPB[i+1].y+vPB[i].y)/5.0;
			}
			for (int i = 2;i<vPT.size()-2;i++)
			{
				//vPT[i].y = (vPT[i-2].y+vPT[i+2].y)/9.0+2.0*(vPT[i-1].y+vPT[i+1].y)/9.0+(vPT[i].y)/3.0;
				vPT[i].y = (vPT[i-2].y+vPT[i+2].y+vPT[i-1].y+vPT[i+1].y+vPT[i].y)/5.0;
			}
		}

		//��Ϊƽ�������ֱ�С�����ñ��볬�����������Խ��ϱ�Եflip
		//��in/out��vPT���ϱ߼�����
		for (int i = 0;i<vPT.size();i++)
		{
			vPT[i].y = src.rows-vPT[i].y;	
		}
	}
	///////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////
	//Step 6:�鼹�߼�鼰�鱾��ҳ���±߽�������ȡ
	//*******
	//�鱾�鼹�߶����⼰�鱾��ҳ����������������ȡ
	vector<Point> vvPT;
	vector<Point> vvPB;
	if (type)
	{	
		vector<int> index;
		edges.clear();
		edges.push_back(vPT);
		edges.push_back(vPB);

		//�鱾�鼹�߶�����
		//��in��edgePoint��Ŀ��������������
		//��out��index: �鼹�߶�������
		bool flag = CBookProcess::Midline_detection(edges,index, src);

		//�鼹�߶�����ȡ
		//��in��vPT���ϱ߽�����
		//��in��vPB���±߽�����
		//��out��point3���鼹���¶���
		//��out��point4���鼹���¶���
		int ind_T = index[0];
		int ind_B = index[1];
		point4 = vPT[ind_T];
		point3 = vPB[ind_B];

		// ��ȡ�鱾��ҳ���±߽���
		//��in��vPT���ϱ߽�����
		//��in��ind_T���鼹���϶�������
		//��in��vPB���±߽�����
		//��in��ind_B���鼹���¶�������
		//��out��vvPT���鱾��ҳ�ϱ߽�����
		//��out��vvPB���鱾��ҳ�±߽�����
		
		Point2f p2t;
		for(int i= ind_T;i<vPT.size();i++)
		{
			p2t = vPT[i];
			vvPT.push_back(p2t);
		}
		for(int i= 0;i<=ind_B;i++)
		{
			p2t = vPB[i];
			vvPB.push_back(p2t);
		}
	}
	else
	{
		vvPT = vPT;
		vvPB = vPB;
	}

	CBookProcess::Curve_Smooth_1( src, vvPB, vvPT);
	///////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////
	//Step 7:�鱾�߽���������±߽���ȡ
	//*******
	//�߽綥�����
	//��in��edgePoint����ҳ������������
	//��in] x_max���鱾�ı����ݱ߽磨Լ���߽��߽����ķ�Χ)
	//��out��index: �߽��߽����󶥵�����
	edges.clear();
	//index.clear();
	edges.push_back(vvPT);
	edges.push_back(vvPB);
	if (vvPT.size()<100 || vvPB.size()<100)
		return edges;
	edges = CBookProcess::KeepParallel(edges, src);	
	// ���±߽�����ȡ
	vector<Point> vvvPT;
	vector<Point> vvvPB;
	vvvPT = edges[0];
	vvvPB = edges[1];
	
	////////////////////////////////////////////////////////////////////
	int min_ind_T,max_ind_T;
	int min_ind_B,max_ind_B;
	int min_x = 2000;
	int max_x = 0;
	for (int ii =0;ii<vvvPT.size();ii++)
	{
		if (vvvPT[ii].x<min_x)
		{
			min_x = vPT[ii].x;
			min_ind_T = ii;
		}
		if (vvvPT[ii].x>max_x)
		{
			max_x = vvvPT[ii].x;
			max_ind_T = ii;
		}
	}

	if (min_ind_T!=vvvPT.size()-1)
	{
		for(int ii=vvvPT.size()-1;ii >= min_ind_T;ii--)
		{
			vvvPT.erase(vvvPT.begin()+ii);
		}
	}
	if (max_ind_T!=0)
	{
		for(int ii = max_ind_T;ii>=0;ii--)
		{
			vvvPT.erase(vvvPT.begin()+ii);
		}
	}

	min_x = 2000;
	max_x = 0;
	for (int ii =0;ii<vvvPB.size();ii++)
	{
		if (vvvPB[ii].x<min_x)
		{
			min_x = vvvPB[ii].x;
			min_ind_B = ii;
		}
		if (vvvPB[ii].x>max_x)
		{
			max_x = vvvPB[ii].x;
			max_ind_B = ii;
		}
	}

	if (max_ind_B!=vvvPB.size()-1)
	{
		for(int ii=vvvPB.size()-1;ii>= max_ind_B;ii--)
		{
			vvvPB.erase(vvvPB.begin()+ii);
		}
	}
	if (min_ind_B!=0)
	{
		for(int ii = min_ind_B;ii>=0;ii--)
		{
			vvvPB.erase(vvvPB.begin()+ii);
		}
	}
	////////////////////////////////////////////////////////////////
	//Step 8:��������ؼ�����ȡ
	//*******
	//�ϱ߽綥����ȡ
	//��in��vPT���ϱ߽�����
	//��out��point1������
	//��out��point4������
	point1 = vvvPT[vvvPT.size()-1];
	point4 = vvvPT[0];

	//�±߽綥����ȡ
	//��in��vPB���±߽�����
	//��out��point2������
	//��out��point3������
	point3 = vvvPB[vvvPB.size()-1];
	point2 = vvvPB[0];

	//�±߽綥����ȡ
	//��in�� vvvPT���ϱ߽�����
	//��in�� vvvPB���±߽�����
	//��out��length_PT���ϱ߽����������
	//��out��length_PB���±߽����������
	//��out��fR�����±߽�������������İٷֱ�
	int length_PT = vvvPT.size();
	int length_PB = vvvPB.size();
	float FM = float(length_PB+length_PT);
	float FF;
	if (length_PB>=length_PT)
		FF = (length_PB-length_PT);
	else
		FF = (length_PT-length_PB);
	float fR = 2*FF/(FM+0.001);

	//�±߽綥����ȡ
	//��in�� point1������
	//��in�� point2������
	//��in�� point3������
	//��in�� point4������
	//��out��H_W����ҳ��/��H/W)��
	float H_W = (point2.y-point1.y+point3.y-point4.y)/(point4.x-point1.x+point3.x-point2.x);

	//�ж�����
	//���±߽�������������İٷֱ�С��0.16��
	//�ϱ߽��������������ͼ���ȵ����֮һ
	//�±߽��������������ͼ���ȵ����֮һ
	//��ҳ��/��H/W)��С��2.5
	//�鼹�߶���x�������С���鼹�߸߶ȵĶ�ʮ��֮һ
	if (fR<0.16 && length_PT>0.2*src.cols && length_PB>0.2*src.cols && H_W<2.5\
		&& (abs(point4.x-point3.x)<0.1*(point3.y-point4.y)))
	{
		edges.clear();
		edges.push_back(vvvPT);
		edges.push_back(vvvPB);
	}
	else
	{
		vPT.clear();
		vPB.clear();
		vPT.push_back(Point(max(point3.x,point4.x),min(point1.y,point4.y)));
		vPT.push_back(Point(min(point2.x,point1.x),min(point1.y,point4.y)));
		vPB.push_back(Point(min(point2.x,point1.x),max(point3.y,point2.y)));
		vPB.push_back(Point(max(point3.x,point4.x),max(point3.y,point2.y)));

		edges.clear();
		edges.push_back(vPT);
		edges.push_back(vPB);
	}
	return edges;
}

//*************************************************************
//������
//��ҳ��ɫ�ж�
//���룺
//Mat src��չƽ��ԭͼ
//Mat Fibger_mask����ָĿ����ģͼ��
//�����
//Mat mask����ҳ��ɫ�����ֵͼ��
//*************************************************************
bool CBookProcess::ColorIdentification(Mat src,Mat Finger_mask,Mat &mask,int out_x)
{
	bool flag = false;
	if (src.empty())
		return flag;
	float S2  = mean(Finger_mask)[0]*Finger_mask.cols*Finger_mask.rows/255;
	if (S2<20)// || S2/(Finger_mask.rows/2*Finger_mask.cols/4)>0.5)
	{
		flag = true;
		return flag;
	}
	Mat temp = src(Rect(0, src.rows/4, out_x ,src.rows/2));

	Mat bw = Mat::zeros(temp.size(),CV_8UC1);
	mask = bw.clone();

	Mat src_hsv;
	cvtColor(temp,src_hsv, CV_BGR2HSV);

	Mat mat_mean, mat_stddev;
    meanStdDev(src_hsv, mat_mean, mat_stddev);
	double m1 = mat_mean.at<double>(0, 0);
    double s1 = mat_stddev.at<double>(0, 0);
	double m2 = mat_mean.at<double>(0, 1);
    double s2 = mat_stddev.at<double>(0, 1);
	double m3 = mat_mean.at<double>(0, 2);
    double s3 = mat_stddev.at<double>(0, 2);

	vector<cv::Mat> rgbChannels(3); 
	vector<cv::Mat> rgbratio(3); 
	split(src_hsv, rgbChannels);

	/*Mat im1 = rgbChannels[0];
	im1.convertTo(im1, CV_32FC1);
	Mat im2 = rgbChannels[1];
	im2.convertTo(im2, CV_32FC1);
	Mat im3 = rgbChannels[2];
	im3.convertTo(im3, CV_32FC1);

	rgbratio[0] = im1/(im2+0.001);
	rgbratio[1] = im1/(im3+0.001);
	rgbratio[2] = im2/(im3+0.001);

	for (int ii = 0;ii<3;ii++)
	{
		bitwise_or(rgbratio[0]>1.33 ,rgbratio[0]<0.75,bw);
		bitwise_or(bw,mask,mask);
	}

	Mat gray;
	cvtColor(src,gray, CV_BGR2GRAY);*/

	double avg1 = mean(rgbChannels[1])[0];
	bitwise_or(rgbChannels[1]>0.25*255,mask,mask);
	//bitwise_or(rgbChannels[1]>0.3*rgbChannels[2],mask,mask);


	bw = rgbChannels[2]>70;
	//Mat bw1 = rgbChannels[2]<200;
	//bitwise_and(bw,bw1,bw);
	bitwise_and(bw,mask,mask);
	
	mask = CBookProcess::bwlabel(mask,50);

	bw = Finger_mask(Rect(0, src.rows/4, out_x ,src.rows/2));
	bitwise_and(~bw,mask,mask);

	double avg = mean(mask)[0];
	if (avg/255>0.05)
		flag = true;

	if (flag==true)
	{
		if(true)
		{
			Mat scharrx,scharry;
			Scharr(rgbChannels[0], scharrx, CV_16S, 1, 0, 1, 0, BORDER_DEFAULT);
			convertScaleAbs(scharrx, scharrx);
			Scharr(rgbChannels[0], scharry, CV_16S, 0, 1, 1, 0, BORDER_DEFAULT);
			convertScaleAbs(scharry, scharry);
			Mat result;
			addWeighted(scharrx, 0.5, scharry, 0.5, 0, result);


			double threshld = mean(result)[0];
			bw = (result>(1.5*threshld));
			bw = CBookProcess::bwlabel(bw,10);
			bitwise_and(~mask,bw,bw);
			//imshow("1",bw);
			//waitKey(0);
			double avg = mean(bw)[0];
			if ((avg*bw.cols*bw.rows/255)>20)
				flag = true;
		}
		else
			{
			blur(bw, bw,Size(20, 20));
			bitwise_and(bw>0 ,bw<128,mask);
			Mat temp = rgbChannels[0].mul(mask/255);
			double avg = mean(mask)[0];
			double avg1 = 255*mean(temp)[0]/avg;
			float SS = avg*bw.cols*bw.rows/255;
			float S1 = 0;
			for (int ii = bw.rows/4-50;ii<3*bw.rows/4+50;ii++)
			{
				for (int jj = 0;jj<bw.cols/4+50;jj++)
				{
					if (mask.ptr<uchar>(ii)[jj]>0)
					{
						if (temp.ptr<uchar>(ii)[jj]<avg1-10 || temp.ptr<uchar>(ii)[jj]>avg1+10)
						{
							S1 += 1;
						}
					}
				}
			}
			if (S1/SS<0.1)
			{
				flag = false;
			}
			mask = temp;
		}
	}
	
	return flag; 
}

//*************************************************************
//������
//��ҳȫҳ��ɫ�ж�
//���룺
//Mat src��չƽ��ԭͼ
//�����
//bool
//*************************************************************
bool CBookProcess::ColorIdentification_all(Mat src)
{
	bool flag = false;
	if (src.empty() || src.channels()==1)
		return flag;

	double scale = 500.0/src.rows;
    Size dsize = Size(src.cols*scale,src.rows*scale);
    Mat temp = Mat(dsize,CV_8UC3);
	resize(src,temp,dsize);
	Mat bw = Mat::zeros(temp.size(),CV_8UC1);
	Mat mask = bw.clone();

	Mat src_hsv;
	cvtColor(temp,src_hsv, CV_BGR2HSV);
	vector<cv::Mat> rgbChannels(3); 
	vector<cv::Mat> rgbratio(3); 
	split(src_hsv, rgbChannels);

	/*Mat im1 = rgbChannels[0];
	im1.convertTo(im1, CV_32FC1);
	Mat im2 = rgbChannels[1];
	im2.convertTo(im2, CV_32FC1);
	Mat im3 = rgbChannels[2];
	im3.convertTo(im3, CV_32FC1);

	rgbratio[0] = im1/(im2+0.001);
	rgbratio[1] = im1/(im3+0.001);
	rgbratio[2] = im2/(im3+0.001);

	for (int ii = 0;ii<3;ii++)
	{
		bitwise_or(rgbratio[0]>1.33 ,rgbratio[0]<0.75,bw);
		bitwise_or(bw,mask,mask);
	}

	Mat gray;
	cvtColor(src,gray, CV_BGR2GRAY);*/

	double avg1 = mean(rgbChannels[1])[0];
	bitwise_or(rgbChannels[1]>0.25*255,mask,mask);
	bw = rgbChannels[2]>30;
	bitwise_and(bw,mask,mask);

	mask = CBookProcess::bwlabel(mask.clone(),50);

	double avg = mean(mask)[0];
	if (avg/255>0.05)
		flag = true;
	
	return flag; 
}

//*************************************************************
//������
//��ҳ��ֵ������ֵ��ֵ��+��ɫĿ���ֵ����
//���룺
//Mat src��ԭͼ
//�����
//Mat ����ֵͼ��
//*************************************************************
Mat CBookProcess::im2bw(Mat src)
{
	Mat mask;
	if (src.channels()==3)
	{
		Mat src_hsv;
		//cvtColor(src,src_hsv, CV_BGR2HSV);
		vector<cv::Mat> rgbChannels(3); 
		vector<cv::Mat> rgbratio(3); 
		//split(src_hsv, rgbChannels);
		split(src, rgbChannels);

		double avg1 = mean(rgbChannels[1])[0];
		mask = rgbChannels[1]>75;//0.45*255;

	
		Mat bw = rgbChannels[2]>75;
		bitwise_and(bw,mask,mask);
		bw = rgbChannels[0]>75;
		bitwise_and(bw,mask,mask);

		cvtColor(src,src, CV_BGR2GRAY);
		bw = src>80;
		bitwise_or(bw,mask,mask);
		/*Mat gray;
		cvtColor(src,src, CV_BGR2GRAY);
		Mat bw = src>85;

		blur(src,gray,Size(5,50));
		mask = src>gray;
		blur(src,gray,Size(50,5));
		bitwise_or(src>gray,mask,mask);

		bitwise_and(bw,mask,bw);
		//bitwise_or(bw,mask,mask);
		mask = CBookProcess::bwlabel(bw,20);
		*/
	}
	else
	{
		mask = src>80;
	}
	return mask;
}



//************************************************************************  
// ��������:    Midline_detection  
// ����˵��:	�鱾�鼹������
// ��������:    vector<vector<Point> >& edgePoint  ���±߽���������
// ��������:    vector<int>& index �����������¶������������������е��±�
// ��������:    int row  ͼ��߶�
// ��������:    int col  ͼ����
// �� �� ֵ:    bool
//************************************************************************ 
bool CBookProcess::Midline_detection(vector<vector<Point> > edgePoint,vector<int>& index , Mat src)
{
	int row = src.rows;
	int col = src.cols;
	vector<Point> vPT = edgePoint[0];
	vector<Point> vPB = edgePoint[1];
	Point2f p2t;

	////////////////////////////////////////////////////////////////////
	//�鱾�鼹��������
	//��
	//��in��vPT���ϱ߽�����
	//��in��vPB���±߽�����
	//��out��ind_T���鼹���϶�������
	//��out��ind_B���鼹���¶�������
	int i = 0;
	int ind_T = 0;
	int ind_B = vPB.size()-1;

	int ind;
	float min_dist;
	float dist;
	float min_min_dist = 2*row;
	Point2f point;
	int count = 0;
#if 0
	//ѭ���������±߽��ڡ�2/3W--W����Χ�ڵ���̾���㣬��Ϊ�鱾�鼹�ߵĳ�ʼ����
	while (i<vPT.size())
	{
		min_dist = row;
		point = vPT[i];
		if (point.x<2*col/3)// || point.x>11*col/12)
		{
			i = i+1;
			continue;
		}
		for (int j=0;j<vPB.size();j++)
		{
			p2t = vPB[j];
			if (p2t.x<2*col/3)// || p2t.x>11*col/12)
			{
				continue;
			}
			dist = norm(p2t-point);
			if (int(dist)<=min_dist)
			{
				min_dist = int(dist);
				ind = j;
				count = 0;
			}
			else
			{
				count += 1;
			}
			if (count>50)
				break;
		}
        
		if (min_min_dist>=min_dist)
		{
			min_min_dist = min_dist;
			ind_T = i;
			ind_B = ind;
		}
		
		/*if (0.1+min_min_dist>=min_dist)
		{
			ind_T = i;
			ind_B = ind;
		}*/
		i= i+1;
	}
#endif
#if 1

	float mean_t = src.rows;
	float mean_b = 0;
	int count_t = 0;
	int count_b = 0;

	for(int ii = vPT.size()-1;ii>=0;ii--)
	{
		if(vPT[ii].x>3*src.cols/4 && vPT[ii].x<5*src.cols/6)
		{
			if (count_t>0 && vPT[ii].y>mean_t+1)
			{
				break;
			}
			if (vPT[ii].y<=mean_t)
			{
				mean_t = vPT[ii].y;
				count_t =ii;
			}
		}
	}
	for (int ii = 0;ii<vPB.size();ii++)
	{
		if(vPB[ii].x>3*src.cols/4 && vPB[ii].x<5*src.cols/6)
		{
			if (count_b>0 && vPB[ii].y<mean_b-1)
			{
				break;
			}
			if (vPB[ii].y>=mean_b)
			{
				mean_b = vPB[ii].y;
				count_b = ii;
			}
		}
	}

	float mean_t_1 = src.rows;
	float mean_b_1 = 0;
	int count_t_1 = 0;
	int count_b_1 = vPT.size()-1;
	if (min(vPT[0].x,vPB[vPB.size()-1].x)>11*col/12-30)
	{
		for(int ii = 0;ii<vPT.size();ii++)
		{
			if (vPT[ii].x>5*src.cols/6)
			{
				if (count_t_1>0 && vPT[ii].y>mean_t_1+1)
				{
					break;
				}
				if(vPT[ii].y<=mean_t_1)
				{
					mean_t_1 = vPT[ii].y;
					count_t_1 = ii;
				}
			}
		}
		for (int ii = vPB.size()-1;ii>=0;ii--)
		{
			if (vPB[ii].x>5*src.cols/6)
			{
				if (count_b_1<vPT.size()-1 && vPB[ii].y<mean_b_1-1)
				{
					break;
				}
				if(vPB[ii].y>=mean_b_1)
				{
					mean_b_1 = vPB[ii].y;
					count_b_1 = ii;
				}
			}
		}
	}


	//ѭ���������±߽��ڡ�2/3W--W����Χ�ڵ���̾���㣬��Ϊ�鱾�鼹�ߵĳ�ʼ����
	while (i<vPB.size())
	{
		min_dist = 2*row;
		point = vPB[i];
		if (i<count_b || i>count_b_1)// || point.x>11*col/12)
		{
			i = i+1;
			continue;
		}
		for (int j=vPT.size()-1;j>0;j--)
		{
			p2t = vPT[j];
			if (j>count_t || i<count_t_1)//p2t.x<3*col/4)// || p2t.x>11*col/12)
			{
				continue;
			}
			dist = norm(p2t-point);
			if (dist<=min_dist)
			{
				min_dist = dist;
				ind = j;
				count = 1;
			}
			else
			{
				count += 1;
			}
			if (count>20)
				break;
		}
        
		if(min_min_dist>=min_dist && count>0)
		{
			min_min_dist = min_dist;
			ind_T = ind;
			ind_B = i;
			if (count>20)
				break;
		}
		
		/*if (0.1+min_min_dist>=min_dist)
		{
			ind_T = i;
			ind_B = ind;
		}*/
		i= i+1;
	}
	//////////////////////////////////////////////////////////////////////////////////////

	point = vPT[ind_T];
	min_dist = 2*row;
	min_min_dist = 2*row;
	count = 0;
	for (int j=0;j<vPB.size();j++)
	{
		p2t = vPB[j];
		if (j<count_b || j>count_b_1)//p2t.x<3*col/4)// || p2t.x>11*col/12)
		{
			continue;
		}
		dist = norm(p2t-point);
		if (dist<=min_dist)
		{
			min_dist = dist;
			ind = j;
			count = 1;
		}
		else
		{
			count += 1;
		}
		if(count>20)
			break;
	}
        
	if (min_min_dist>=min_dist && count>0)
	{
		min_min_dist = min_dist;
		ind_B = ind;
	}
#endif
	
#if 1
	////////////////////////////////////////////////////////////////////
	//�鱾�鼹��������
	//��
	//��in��vPT���ϱ߽�����
	//��in��vPB���±߽�����
	//��out��ind_T���鼹���϶�������
	//��out��ind_B���鼹���¶�������
	i = 0;
	int ind_T_1 = 0;
	int ind_B_1 = vPB.size()-1;
	min_min_dist = 2*row;
	count = 0;

	
	//ѭ���������±߽��ڡ�2/3W--W����Χ�ڵ���̾���㣬��Ϊ�鱾�鼹�ߵĳ�ʼ����
	min_dist = 2*row;
	point = vPB[ind_B];
	for (int j=0;j<vPT.size();j++)
	{
		p2t = vPT[j];
		if (j>count_t || j<count_t_1)
		{
			continue;
		}
		dist = norm(p2t-point);
		if (dist<=min_dist)
		{
			min_dist = dist;
			ind = j;
			count = 1;
		}
		else
		{
			count += 1;
		}
		if (count>20)
			break;
	}
        
	if (min_min_dist>=min_dist && count>0)
	{
		min_min_dist = min_dist;
		ind_T_1 = ind;
		ind_B_1 = ind_B;
	}
		
	//////////////////////////////////////////////////////////////////////////////////////
	min_dist = 2*row;
	count = 0;
	point = vPT[ind_T_1];
	
	for (int j=vPB.size()-1;j>0;j--)
	{
		p2t = vPB[j];
		if (j<count_b || j>count_b_1)
		{
			continue;
		}
		dist = norm(p2t-point);
		if (dist<=min_dist)
		{
			min_dist = dist;
			ind = j;
			count = 1;
		}
		else
		{
			count += 1;
		}
		if(count>20)
			break;
	}
        
	if (min_min_dist>=min_dist && count>0)
	{
		min_min_dist = min_dist;
		ind_B_1 = ind;
		//ind_T_1 = i;
	}

#endif
	/////////////////////////////////////////////////////////////////////////////////////
	//�鼹�߶������
	//��in��vPT���ϱ߽�����
	//��in��vPB���±߽�����
	//��in/out��ind_T���鼹���϶�������
	//��in/out��ind_B���鼹���¶�������

	//�鼹���϶������
	int ind_TT = ind_T;
	int ind_BB = ind_B;
	if(abs(ind_T-ind_T_1)>10 && (abs(vPT[ind_T].x-vPB[ind_B].x)>50 || vPT[ind_T].x<5*col/6-40))
	{
		int aa1 = vPT[ind_T].y;
		int aa2 = vPT[ind_T_1].y;
		if(abs(vPT[ind_T].x-col*5/6) < abs(vPT[ind_T_1].x-5*col/6) || vPT[ind_T].y>vPT[ind_T_1].y-10*row/1000)
		{
			ind_TT = ind_T_1;
			ind_T = ind_T_1;
			
		}
	}
	if(abs(ind_B-ind_B_1)>10 && (abs(vPT[ind_T].x-vPB[ind_B].x)>50 || vPB[ind_B].x<5*col/6-40))
	{
		int aa1 = vPB[ind_B].y;
		int aa2 = vPB[ind_B_1].y;
		if(abs(vPB[ind_B].x-col*5/6) < abs(vPB[ind_B_1].x-5*col/6) || vPB[ind_B].y<vPB[ind_B_1].y+10*row/1000)
		{
			ind_BB = ind_B_1;
			ind_B = ind_B_1;
		}
	}


	//********************************************************
	Point p2f1(col,0);
	Point p2f2 (col,row);
	min_dist = col;
	for (int ii = ind_T;ii<vPT.size();ii++)
	{
		if(vPT[ii].x<3*col/4)
			break;
		float dist = norm(vPT[ii]-p2f1);
		if (dist<min_dist)
		{
			ind_T = ii;
			min_dist = dist;
		}
	}
	min_dist = col;
	for (int ii = ind_B;ii>=0;ii--)
	{
		if(vPB[ii].x<3*col/4)
			break;
		float dist = norm(vPB[ii]-p2f2);
		if (dist<min_dist)
		{
			ind_B = ii;
			min_dist = dist;
		}
	}
	//********************************************************



	int xx = vPT[ind_T].x;
	int yy = vPT[ind_T].y;
	int xx1 = xx;
	int yy1 = yy;
	float deta_height;
	bool errMsg = false;
	//�鼹���϶���X�������3/4Wʱ���ڡ�xx--2/3W����������ͻ��Ϊ�µ��鼹�϶���
	//��in��vPT���ϱ߽�����
	//��in/out��ind_T���鼹���϶�������
	//��out�� errMsg�������ɹ�״̬����
	if (xx>(3*col/4))
	{
		for (int i = ind_T+1;i<vPT.size();i++)
		{
			if (vPT[i].x<3*col/4)
			{
				break;
			}
			if (vPT[i].x==xx1-1)
			{
				deta_height = abs(yy1-vPT[i].y);
				xx1 = vPT[i].x;
				yy1 = vPT[i].y;
				if (yy1<yy-10)
					errMsg = true;

				if (deta_height>=6)
				{
					if(yy>vPT[i].y)
					{
						ind_T = i;
						errMsg = true;
					}
					else
					{
						break;
					}
					//if (type == 0)
					//	ind_T = ind_TT;
				}
				ind_TT = i;
			}
			else
			{

				continue;
			}
		}
	}

	//�鼹���϶���X����С��3/4W�� ����δ������ͻ���ʱ���ڡ�xx--W����������ͻ��Ϊ�µ��鼹�϶���
	//��in��vPT���ϱ߽�����
	//��in/out��ind_T���鼹���϶�������
	//��in�� errMsg�����������ɹ����״̬����
	if (xx<=(3*col/4) || errMsg==false)
	{
		ind_TT = ind_T;
		xx1 = xx;
		yy1 = yy;
		for (int i = ind_T-1;i>0;i--)
		{
			if (vPT[i].x==xx1+1)
			{
				deta_height = abs(yy1-vPT[i].y);
				xx1 = vPT[i].x;
				yy1 = vPT[i].y;
				if (deta_height>=6)
				{
					if(yy>vPT[i].y)
					{
						ind_T = ind_TT;
					}
					break;
				}
				ind_TT = i;
			}
			else
			{
				continue;
			}
		}
	}

	//�鼹���¶������
	xx = vPB[ind_B].x;
	yy = vPB[ind_B].y;
	xx1 = xx;
	yy1 = yy;
	errMsg = false;
	//�鼹���¶���X�������3/4Wʱ���ڡ�xx--2/3W����������ͻ��Ϊ�µ��鼹�¶���
	//��in��vPB���±߽�����
	//��in/out��ind_B���鼹���¶�������
	//��out�� errMsg�������ɹ�״̬����
	if (xx>(3*col/4))
	{
		for (int i = ind_B-1;i>0;i--)
		{
			if (vPB[i].x<3*col/4)
			{
				continue;
			}
			if (vPB[i].x==xx1-1)
			{
				deta_height = abs(yy1-vPB[i].y);
				xx1 = vPB[i].x;
				yy1 = vPB[i].y;
				if (yy1>yy+10)
					errMsg = true;
				if (deta_height>=6)
				{
					if (yy<vPB[i].y)
					{
						ind_B = i;
						errMsg = true;
					}
					else
					{
						break;
					}
				}
				ind_BB = i;
			}
			else
			{
				continue;
			}
		}
	}
		
	//�鼹���¶���X����С��3/4W�� ����δ������ͻ���ʱ���ڡ�xx--W����������ͻ��Ϊ�µ��鼹�¶���
	//��in��vPB�� �±߽�����
	//��in/out��ind_B���鼹���¶�������
	//��in�� errMsg�����������ɹ����״̬����
	if (xx<=(3*col/4) || errMsg==false)
	{
		ind_BB = ind_B;
		xx1 = xx;
		yy1 = yy;
		for (int i = ind_B+1;i<vPB.size();i++)
		{
			if (vPB[i].x==xx1+1)
			{
				deta_height = abs(yy1-vPB[i].y);
				xx1 = vPB[i].x;
				yy1 = vPB[i].y;
						
				if (deta_height>=6)
				{
					if(yy<vPB[i].y)
					{
						ind_B = ind_BB;
					}
					break;
				}
				ind_BB = i;
			}
			else
			{
				continue;
			}
		}
	}
	
	//���¶���x���귶Χ����ΧԼ������
	//���鱾�鼹�߶�������С��3/4W �� ���¶���Xֵ������1/20W)(W:ͼ�����ؿ�ȣ�
	//��in��vPT���ϱ߽�����
	//��in��vPB���±߽�����
	//��in/out��ind_T���鼹���϶�������
	//��in/out��ind_B���鼹���¶�������
	/*if (vPT[ind_T].x<3*col/4 || vPT[ind_T].x>11*col/12 || vPB[ind_B].x<3*col/4 || vPB[ind_B].x>11*col/12 || abs(vPT[ind_T].x-vPB[ind_B].x)>col/20)
	{
		for (int i = 0;i<vPT.size();i++)
		{
			if (vPT[i].x == int(5*col/6))
			{
				ind_T = i;
			}
		}
		for (int i = vPB.size()-1;i>0;i--)
		{
			if (vPB[i].x == int(5*col/6))//-vPT[vPT.size()-1].x+vPB[0].x)
			{
				ind_B = i;
			}
		}
	}*/

	////////////////////////////////////////////////////////////////////////////////////////////////////
	index.push_back(ind_T);
	index.push_back(ind_B);
	return true;
}


//************************************************************************  
// ��������:	 Curve_Smooth 
// ����˵��:    �鱾������������ƽ��
// ��������:	mat src				  ԭͼ��
//				vector<Point>& vPB    ������
//				vector<Point>& vPT	  ������
// �� �� ֵ:    void
//************************************************************************ 
void CBookProcess::Curve_Smooth(Mat src,vector<Point>& vPB,vector<Point>& vPT, int type)
{
	Point p2t;
	int th = 3*src.cols/4;
	if (type == 1)
	{
		th = src.cols;
	}
	//�������±߽������쳣
	 for(int ii = 0;ii<10;ii++)
	{
		float d_B = 0;
		float dist_BB = 0;
		float deta_x = 0;
		float deta_x_1 = 0;
		float fRi = 0;
		float fRi_max = 0;
		int ind_x,ind_y;
		ind_x = 0;
		for (int i = 0;i<vPB.size();i++)
		{
			d_B = 0;
			if (vPB[i].x>th)
			{
				break;
			}
			for (int j = i;j<vPB.size()-1;j++)
			{
				if (vPB[j].x>th)
				{
					break;
				}
				d_B = d_B +norm(vPB[j]-vPB[j+1]);
				deta_x = abs(vPB[j].x-vPB[i].x)+1;
				if (deta_x>10)
				{
					fRi = d_B/deta_x;
					if ((fRi>fRi_max && deta_x>deta_x_1) || ( fRi>1.5 &&  deta_x>deta_x_1 && (d_B-dist_BB)/(deta_x-deta_x_1)>1.5))
					{
						dist_BB = d_B;
						fRi_max = fRi;
						deta_x_1 = deta_x;
						ind_x = i;
						ind_y = j;
					}
				}
			}
		}
		if (fRi_max>1.5)
		{
			int x1 = vPB[ind_x].x;
			int y1 = vPB[ind_x].y;
			int x2 = vPB[ind_y].x;
			int y2 = vPB[ind_y].y;
			for (int i = ind_x;i<ind_y;i++)
			{ 
				vPB.erase(vPB.begin()+ind_x);
			}
			float K = float(y1-y2)/float(x1-x2+0.001);
			for (int i = x2-1;i>=x1;i--)
			{
				p2t.x = i;
				p2t.y = int(y1-K*float(x1-i));
				vPB.insert(vPB.begin()+ind_x,p2t);
			}
		}
		else
			break;
	}
    for(int ii = 0;ii<20;ii++)
	{
		float d_T = 0;
		float dist_TT = 0;
		float deta_x = 0;
		float fRi = 0;
		float fRi_max = 0;
		int ind_x,ind_y;
		float deta_x_1 = 0;
		ind_x = vPT.size();
		for (int i=vPT.size()-1;i>=0;i--)
		{
			if (vPT[i].x>th)
			{
				break;
			}
			d_T = 0;
			for (int j = i;j>0;j--)
			{
				if (vPT[j].x>th)
				{
					break;
				}
				d_T = d_T +norm(vPT[j]-vPT[j-1]);
				deta_x = abs(vPT[j].x-vPT[i].x)+1;
				if (deta_x>10)
				{
					fRi = d_T/deta_x;
					if ((fRi>fRi_max && deta_x>deta_x_1) || (fRi>1.5 &&  deta_x>deta_x_1 && (d_T-dist_TT)/(deta_x-deta_x_1)>1.5))
					{
						dist_TT = d_T;
						fRi_max = fRi;
						deta_x_1 = deta_x;
						ind_x = i;
						ind_y = j;
					}
				}
			}
		}
		if (fRi_max>1.5)
		{
			int x1 = vPT[ind_y].x;
			int y1 = vPT[ind_y].y;
			int x2 = vPT[ind_x].x;
			int y2 = vPT[ind_x].y;
			for (int i = ind_y;i<ind_x;i++)
			{ 
				vPT.erase(vPT.begin()+ind_y);
			}
			float K = float(y1-y2)/float(x1-x2+0.001);
			for (int i = x2;i<x1;i++)
			{
				p2t.x = i;
				p2t.y = int(y1-K*float(x1-i));
				vPT.insert(vPT.begin()+ind_y,p2t);
			}
		}
		else
			break;
	}

	/*for (int i = 0;i<vPT.size();i++)
	{
		vPT[i].y = src.rows-vPT[i].y;	
	}

    for (int j = 0;j<2;j++)
	{
		for (int i = 2;i<vPB.size()-2;i++)
		{
			vPB[i].y = (vPB[i-2].y+vPB[i+2].y)/9.0+2.0*(vPB[i-1].y+vPB[i+1].y)/9.0+(vPB[i].y)/3.0;
		}
			for (int i = 2;i<vPT.size()-2;i++)
		{
			vPT[i].y = (vPT[i-2].y+vPT[i+2].y)/9.0+2.0*(vPT[i-1].y+vPT[i+1].y)/9.0+(vPT[i].y)/3.0;
			
			}
	}

	for (int i = 0;i<vPT.size();i++)
	{
		vPT[i].y = src.rows-vPT[i].y;	
	}*/
	}


//************************************************************************  
// ��������:	 Curve_Smooth_1 
// ����˵��:    �鱾������������ƽ��
// ��������:	mat src				  ԭͼ��
//				vector<Point>& vPB    ������
//				vector<Point>& vPT	  ������
// �� �� ֵ:    void
//************************************************************************ 
void CBookProcess::Curve_Smooth_1(Mat src,vector<Point>& vPB,vector<Point>& vPT, int type)
{
	Point p2t;
	int th = src.cols;

	//�������±߽������쳣
	 for(int ii = 0;ii<10;ii++)
	{
		float d_B = 0;
		float dist_BB = 0;
		float deta_x = 0;
		float deta_x_1 = 0;
		float fRi = 0;
		float fRi_max = 1;
		int ind_x,ind_y;
		ind_x = 0;
		for (int i = vPB.size()-1;i>0;i--)
		{
			d_B = 0;
			if (vPB[i].x<th/2)
			{
				break;
			}
			for (int j = i;j>0;j--)
			{
				if (vPB[j].x<th/2)
				{
					break;
				}
				d_B = d_B +norm(vPB[j]-vPB[j-1]);
				deta_x = abs(vPB[j].x-vPB[i].x)+1;
				if (deta_x>10)
				{
					fRi = d_B/deta_x;
					if ((fRi>fRi_max && deta_x>deta_x_1 && vPB[j].y<vPB[i].y) || ( fRi>1.5 &&  deta_x>deta_x_1 && (d_B-dist_BB)/(deta_x-deta_x_1)>1.5  && vPB[j].y<vPB[i].y))
					{
						dist_BB = d_B;
						fRi_max = fRi;
						deta_x_1 = deta_x;
						ind_x = i;
						ind_y = j;
					}
				}
			}
		}
		if (fRi_max>1.5)
		{
			Point pt1 = Point(vPT[vPT.size()-1].x,max(vPB[0].y,vPB[vPB.size()-1].y));
			Point pt2 = Point(vPT[0].x,pt1.y);
			vPB.clear();
			vPB.push_back(pt1);
			vPB.push_back(pt2);
			break;
		}
		else
			break;
	}
    for(int ii = 0;ii<10;ii++)
	{
		float d_T = 0;
		float dist_TT = 0;
		float deta_x = 0;
		float fRi = 0;
		float fRi_max = 1;
		int ind_x,ind_y;
		float deta_x_1 = 0;
		ind_x = vPT.size();
		for (int i=0;i<vPT.size()-1;i++)
		{
			if (vPT[i].x<th/2)
			{
				break;
			}
			d_T = 0;
			for (int j = i;j<vPT.size()-1;j++)
			{
				if (vPT[j].x<th/2)
				{
					break;
				}
				d_T = d_T +norm(vPT[j]-vPT[j+1]);
				deta_x = abs(vPT[j].x-vPT[i].x)+1;
				if (deta_x>10)
				{
					fRi = d_T/deta_x;
					if ((fRi>fRi_max && deta_x>deta_x_1  && vPT[j].y>vPT[i].y) || (fRi>1.5 &&  deta_x>deta_x_1 && (d_T-dist_TT)/(deta_x-deta_x_1)>1.5 && vPT[j].y>vPT[i].y))
					{
						dist_TT = d_T;
						fRi_max = fRi;
						deta_x_1 = deta_x;
						ind_x = i;
						ind_y = j;
					}

				}
			}
		}
		if (fRi_max>1.5)
		{
			Point pt1 = Point(vPB[vPB.size()-1].x,min(vPT[0].y,vPT[vPT.size()-1].y));
			Point pt2 = Point(vPB[0].x,pt1.y);
			if (abs(pt1.y-pt2.y)>40)
				pt1.y = pt2.y;
			vPT.clear();
			vPT.push_back(pt1);
			vPT.push_back(pt2);
			break;
		}
		else
			break;
	}

}

//************************************************************************  
// ��������:	 Vertex_Correction
// ����˵��:    �鱾���߶������
// ��������:	mat src				  ԭͼ��
//				vector<Point> areaMatvPoint  �鱾������
//				Point2f point         ��ʼ����
//				Point2f &point_news   �¶���
//				int ind1			  �¶���������ʼλ��
//				int ind2			  �¶���������ֹλ��
//				int type			  �����Ķ���0Ϊ�϶��㣬1Ϊ�¶���
// �� �� ֵ:   int
//************************************************************************ 
int CBookProcess::Vertex_Correction(vector<Point> areaMatvPoint,Point2f point,Point2f &point_new, int ind1,int type,int col ,int row)
{
	//�鱾�߽��϶������
	Point2f p2t;
	Point2f point1_1(point.x, 0+type*row);

	int ind_T = 0;
	int xx = areaMatvPoint[ind1].x;
	int yy = areaMatvPoint[ind1].y;
	int xx1 = xx;
	int yy1 = yy;
	int deta_height;
	if (type ==1)
	{
		for (int i = ind1;i<areaMatvPoint.size();i++)
		{
			if(areaMatvPoint[i].x>xx+col/10)
			{
				/*if((yy1-yy)>30 && ind_T==0)
				{
					ind_T = i;
				}*/
				break;
			}
			if (areaMatvPoint[i].x>xx1)
			{
				deta_height = -(yy1-areaMatvPoint[i].y);
				if (deta_height>-10)
				{
					xx1 = areaMatvPoint[i].x;
					yy1 = areaMatvPoint[i].y;
				}
				if (deta_height>10 && (areaMatvPoint[i].y-yy)>30)
				{
					ind_T = i;
				}
			}
		}
	}
	else
	{
		for (int i = ind1;i>0;i--)
		{
			if (areaMatvPoint[i].x>xx+col/10)
			{
				/*if(yy-yy1>30 && ind_T==0)
				{
					ind_T = i;
				}*/
				break;
			}
			
			if (areaMatvPoint[i].x>xx1)
			{
				deta_height = -(areaMatvPoint[i].y-yy1);
				if (deta_height>-10)
				{
					xx1 = areaMatvPoint[i].x;
					yy1 = areaMatvPoint[i].y;
				}
				if (deta_height>10 && (yy-areaMatvPoint[i].y)>30)
				{
					ind_T = i;
				}
			}
		}
	}
	
	xx1 = xx;
	yy1 = yy;
	int ind_B = 0;
	int deta_width;
	if (type ==1)
	{
		for (int i = ind1;i>0;i--)
		{
			if(areaMatvPoint[i].y<yy-row/10 || areaMatvPoint[i].x<(xx-col/10))
			{
				/*if(xx-xx1>30 && ind_B==0)
				{
					ind_B = i;
				}*/
				break;
			}
			if (areaMatvPoint[i].y<yy1)
			{
				deta_width = (xx1-areaMatvPoint[i].x);
				
				if (deta_width>-10)
				{
					xx1 = areaMatvPoint[i].x;
					yy1 = areaMatvPoint[i].y;
				}
				if (deta_width>10 && (xx-areaMatvPoint[i].x)>30)
				{	
					ind_B = i;
				}
			}
		}
	}
	else
	{
		for (int i = ind1;i<areaMatvPoint.size();i++)
		{
			if(areaMatvPoint[i].y>yy+row/10)
			{
				/*if(xx-xx1>30 && ind_B==0)
				{
					ind_B = i;
				}*/
				break;
			}
			if (areaMatvPoint[i].y>yy1)
			{
				deta_width = (xx1-areaMatvPoint[i].x);
				if (deta_width>-10)
				{
					xx1 = areaMatvPoint[i].x;
					yy1 = areaMatvPoint[i].y;
				}
				if (deta_width>10 && (xx-areaMatvPoint[i].x)>30)
				{
					
					ind_B = i;
				}
			}
		}
	}
	if (ind_B==0 && ind_T==0)
		point_new = point;
	else if (ind_T !=0)
	{
		point_new = areaMatvPoint[ind_T];
		ind1 = ind_T;
	}
	else
	{
		point_new = areaMatvPoint[ind_B];
	}
	return ind1;
}



//************************************************************************  
// ��������:    BookStretch_inpaint
// ����˵��:    ����չƽ��ͼ��ڱ��޸�
// ��������:    Mat src     ����ͼ��
// �� �� ֵ:    Mat
//************************************************************************ 
Mat CBookProcess::BookStretch_inpaint(Mat src,int type)
{
	
	if (src.empty() || src.cols<20 || src.rows<20)
		return src;
	
	double scale = 600.0/src.rows;
	Size dsize = Size(src.cols*scale,src.rows*scale);
	Mat mat_src;
	resize(src, mat_src, dsize);
	bool flag = false;
	if (true || type==0)
	{
		/*Mat finger_mask = Mat::zeros(mat_src.size(),CV_8UC1);
		line(finger_mask,Point(0,0),Point(0,finger_mask.rows-1),Scalar(255),20,CV_AA);  
		Mat color_mask;*/
		flag = CBookProcess::ColorIdentification_all(mat_src);
	}
	
	if (flag && type ==0)
	{
		if(mat_src.cols<=200)
			return src;
		Mat hsv_src;
		cvtColor(mat_src, hsv_src, CV_BGR2HSV);
		vector<cv::Mat> rgbChannels(3); 
		split(hsv_src, rgbChannels);

		Mat bw1;
		bitwise_and(rgbChannels[1]<int(0.35*255),rgbChannels[2]<50,bw1);
		bitwise_or(bw1,rgbChannels[2]<30,bw1);
		Mat bw = Mat::zeros(Size(mat_src.cols-2*int(scale*40),mat_src.rows-2*int(scale*40)),CV_8UC1);
		bw.copyTo(bw1(Rect(scale*40,scale*40,bw.cols,bw.rows)));
		
		bw = bw1.clone();
		rectangle(bw1,Rect(0,0,bw1.cols,bw1.rows),Scalar(255),1,1,0);
		Point vpoint(0,0);
		bw1 = CBookProcess::bwlabel_mask(bw1.clone(),vpoint);
		//return bw1;
		bw1 = CBookProcess::bwlabel(bw1,15);
		Mat element = getStructuringElement(MORPH_ELLIPSE,Size(3,3));
		//morphologyEx(bw,bw, MORPH_OPEN, element);
		dilate(bw1,bw1, element );		
		bitwise_and(bw1,bw,bw);
		
		dsize = Size(src.cols,src.rows);
		Mat mask = Mat(dsize,CV_8UC1);
		resize(bw, mask, dsize);
		element = getStructuringElement(MORPH_ELLIPSE,Size(4,4));
		dilate(mask>0, mask, element );
		rectangle(mask,Rect(0,0,mask.cols,mask.rows),Scalar(255),3,1,0);
		//return mask;
		Mat mask_dst;
		inpaint(src, mask, mask_dst, 3, INPAINT_NS);
		for (int j = 0;j<mask_dst.cols;j++)
		{
			mask_dst.ptr<uchar>(0,j)[0] = mask_dst.ptr<uchar>(1,j)[0];
			mask_dst.ptr<uchar>(0,j)[1] = mask_dst.ptr<uchar>(1,j)[1];
			mask_dst.ptr<uchar>(0,j)[2] = mask_dst.ptr<uchar>(1,j)[2];
		}
		for (int j = 0;j<mask_dst.rows;j++)
		{
			mask_dst.ptr<uchar>(j,0)[0] = mask_dst.ptr<uchar>(j,1)[0];
			mask_dst.ptr<uchar>(j,0)[1] = mask_dst.ptr<uchar>(j,1)[1];
			mask_dst.ptr<uchar>(j,0)[2] = mask_dst.ptr<uchar>(j,1)[2];
		}
		return mask_dst;
	}
	else if (type ==1 && mat_src.channels()==3)
	{	
		vector<cv::Mat> rgbChannels(3);
		split(mat_src, rgbChannels);
		Mat bw1(mat_src.size(),CV_8UC1,Scalar(255));
		for(int idx = 0;idx<3;idx++)
		{
			bitwise_and(bw1,rgbChannels[idx]<int(0.6*mean(rgbChannels[idx])[0]),bw1);
		}
	
		Mat mask = bw1.clone();
		Mat mask_clone = mask.clone();
		rectangle(mask,Point(0,0),Point(mask.cols-1,mask.rows-1),Scalar(255),5,1,0);
		mask = CBookProcess::bwlabel_mask(mask,Point(0,0));

		bitwise_and(mask,mask_clone,mask);

		Mat element = getStructuringElement(MORPH_ELLIPSE,Size(5,5));
		dilate(mask, mask, element );

	

		if (type==1 && mean(~mask)[0]/255.0<0.85)
		{
			return src;
		}

		Mat mask_dst;
		inpaint(mat_src, mask, mask_dst, 3, INPAINT_NS);
		for (int j = 0;j<mask_dst.cols;j++)
		{
			mask_dst.ptr<uchar>(0,j)[0] = mask_dst.ptr<uchar>(1,j)[0];
			mask_dst.ptr<uchar>(0,j)[1] = mask_dst.ptr<uchar>(1,j)[1];
			mask_dst.ptr<uchar>(0,j)[2] = mask_dst.ptr<uchar>(1,j)[2];
		}
		for (int j = 0;j<mask_dst.rows;j++)
		{
			mask_dst.ptr<uchar>(j,0)[0] = mask_dst.ptr<uchar>(j,1)[0];
			mask_dst.ptr<uchar>(j,0)[1] = mask_dst.ptr<uchar>(j,1)[1];
			mask_dst.ptr<uchar>(j,0)[2] = mask_dst.ptr<uchar>(j,1)[2];
		}

		//dilate(mask_dst, mask_dst, element );
		//blur(mask_dst, mask_dst, Size(3, 3));

		dsize = Size(src.cols,src.rows);
		Mat dst;
		resize(mask_dst,dst, dsize);
		/*
		namedWindow("0",WINDOW_NORMAL);
		imshow("0",mask);
		waitKey(0);*/

		if (type ==5)
		{
			Rect rectangle_Rect = Rect(60,60,src.cols-120,src.rows-120);
			Mat temp = src(rectangle_Rect);
			temp.copyTo(dst(rectangle_Rect));
		}
		else
		{
			Mat mask_1 = Mat(dsize,CV_8UC3);;
			resize(mask,mask_1, dsize);
			mask_1 = mask_1>0;

			uchar *data = dst.ptr<uchar>(0);
			uchar *data1 = src.ptr<uchar>(0);
			uchar *data2 = mask_1.ptr<uchar>(0);
			for (int idr = 0; idr<src.rows; idr++)
			{
				for (int idc = 0; idc<src.cols; idc++,data2++)
				{
					if (*data2==0)
					{
						*data = *data1;
						*(data+1) = *(data1+1);
						*(data+2) = *(data1+2);
					}
					data += 3;
					data1 += 3;
				}
			}
		}
		return dst;
	}
	else
	{
		Mat dst;
		/*double scale = 600.0/src.rows;
		Size dsize = Size(scale*src.cols,scale*src.rows);
		Mat image = Mat(dsize,CV_8UC3);
		resize(src, image, dsize);*/
	
		//�̶���ģ����
		int num = 40*scale;
		//ͼ���޸���Ĥ����
		dsize = Size(mat_src.cols,mat_src.rows);
		Mat mask = Mat::zeros(dsize,CV_8UC1);
		
		if (mat_src.channels()==3)
		{
			/*vector<cv::Mat> rgbChannels(3); ; 
			split(mat_src, rgbChannels);
			Mat temp1 = rgbChannels[0];
			Mat temp2 = rgbChannels[1];
			Mat temp3 = rgbChannels[2];
			Mat bw1;
			bitwise_and(temp2<int(0.6*mean(temp2)[0]),temp3<int(0.6*mean(temp3)[0]),bw1);
			bitwise_and(bw1,temp1<int(0.6*mean(temp1)[0]),bw1);*/
			Mat src1;
			Mat gray;
			cvtColor(mat_src,gray, CV_BGR2GRAY);
			blur(gray,src1,Size(100,100));
			Mat bw1 = gray<src1*0.95;
			bitwise_or(bw1,gray<mean(gray)[0]*0.6,bw1);
			
			if (false)
			{
				vector<cv::Mat> rgbChannels(3); ; 
				split(mat_src, rgbChannels);
				Mat mask(bw1.size(),bw1.type(),Scalar(255));
				for(int idx = 0;idx<3;idx++)
				{
					bitwise_and(mask,rgbChannels[idx]<int(0.8*mean(rgbChannels[idx])[0]),mask);
				}
				bitwise_and(bw1,mask,bw1);
			}

			if (type == 0)
			{
				Mat bw = Mat::zeros(Size(mat_src.cols-2*int(scale*60),mat_src.rows-2*int(scale*60)),CV_8UC1);
				bw.copyTo(bw1(Rect(scale*60,scale*60,bw.cols,bw.rows)));
			}
			/*else
			{
				CBookProcess::delete_jut(bw1,bw1,20,20,1);
			}*/

			mask = bw1.clone();
			Mat mask_clone = mask.clone();
			rectangle(mask,Point(0,0),Point(mask.cols-1,mask.rows-1),Scalar(255),5,1,0);
			mask = CBookProcess::bwlabel_mask(mask,Point(0,0));

			bitwise_and(mask,mask_clone,mask);

			Mat element = getStructuringElement(MORPH_ELLIPSE,Size(5,5));
			dilate(mask, mask, element );

			//dsize = Size(mask.cols-60,mask.rows-60);
			//Mat inpaint_mask = Mat::zeros(dsize,CV_8UC3);
			//inpaint_mask.copyTo(image(Rect(30,30,inpaint_mask.cols,inpaint_mask.rows)));
		

			if (type==1 && mean(~mask)[0]/255.0<0.85)
			{
				return src;
			}

			Mat mask_dst;
			inpaint(mat_src, mask, mask_dst, 3, INPAINT_NS);
			for (int j = 0;j<mask_dst.cols;j++)
			{
				mask_dst.ptr<uchar>(0,j)[0] = mask_dst.ptr<uchar>(1,j)[0];
				mask_dst.ptr<uchar>(0,j)[1] = mask_dst.ptr<uchar>(1,j)[1];
				mask_dst.ptr<uchar>(0,j)[2] = mask_dst.ptr<uchar>(1,j)[2];
			}
			for (int j = 0;j<mask_dst.rows;j++)
			{
				mask_dst.ptr<uchar>(j,0)[0] = mask_dst.ptr<uchar>(j,1)[0];
				mask_dst.ptr<uchar>(j,0)[1] = mask_dst.ptr<uchar>(j,1)[1];
				mask_dst.ptr<uchar>(j,0)[2] = mask_dst.ptr<uchar>(j,1)[2];
			}

			//dilate(mask_dst, mask_dst, element );
			//blur(mask_dst, mask_dst, Size(3, 3));

			dsize = Size(src.cols,src.rows);
			
			resize(mask_dst,dst, dsize);
			/*
			namedWindow("0",WINDOW_NORMAL);
			imshow("0",mask);
			waitKey(0);*/

			if (type ==5)
			{
				Rect rectangle_Rect = Rect(60,60,src.cols-120,src.rows-120);
				Mat temp = src(rectangle_Rect);
				temp.copyTo(dst(rectangle_Rect));
			}
			else
			{
				Mat mask_1 = Mat(dsize,CV_8UC3);;
				resize(mask,mask_1, dsize);
				mask_1 = mask_1>0;

				uchar *data = dst.ptr<uchar>(0);
				uchar *data1 = src.ptr<uchar>(0);
				uchar *data2 = mask_1.ptr<uchar>(0);
				for (int idr = 0; idr<src.rows; idr++)
				{
					for (int idc = 0; idc<src.cols; idc++,data2++)
					{
						if (*data2==0)
						{
							*data = *data1;
							*(data+1) = *(data1+1);
							*(data+2) = *(data1+2);
						}
						data += 3;
						data1 += 3;
					}
				}
				/*
				vector<Mat> rgbChannels_temp(3);
				split(dst, rgbChannels_temp);
				vector<Mat> rgbChannels_src(3);
				split(src, rgbChannels_src);
		
				//mask.convertTo(mask, CV_32FC1);
				for (int ii = 0; ii < 3; ii++)
				{
					Mat img = rgbChannels_src[ii];
					Mat img1 = rgbChannels_temp[ii];
					img = img.mul(~mask_1 / 255);
					img1 = img1.mul(mask_1 / 255);
					img.convertTo(img, CV_32FC1);
					img1.convertTo(img1, CV_32FC1);
					img = img + img1;
					img.convertTo(img, CV_8UC1);
					rgbChannels_src[ii] = img;
				}
				merge(rgbChannels_src, dst);
				*/
			}
		}
		else
		{
			mask = mat_src<mean(mat_src)[0]*0.6;
			Mat mask_clone = mask.clone();
			rectangle(mask,Point(0,0),Point(mask.cols-1,mask.rows-1),Scalar(255),2,1,0);
			mask = CBookProcess::bwlabel_mask(mask,Point(0,0));

			bitwise_and(mask,mask_clone,mask);

			Mat element = getStructuringElement(MORPH_ELLIPSE,Size(5,5));
			dilate(mask, mask, element );

			if (type==1 && mean(~mask)[0]/255.0<0.85)
			{
				return src;
			}
			
			dsize = Size(src.cols,src.rows);
			Mat dst = src.clone();//Mat(dsize,CV_8UC1);
			Mat mask_1 = Mat(dsize,CV_8UC1);;
			resize(mask,mask_1, dsize);
			mask_1 = mask_1>0;
			rectangle(mask_1,Rect(0,0,mask_1.cols,mask_1.rows),Scalar(255),10,1,0);
			dst.setTo(int(mean(src)[0]),mask_1);
			return dst;
			
		}

		return dst;
	}
}


//************************************************************************  
// ��������:    bwlabel_min_x   
// ����˵��:    ��ֵͼ��ָ����ͨ����߽�x����ֵ��ʹ�����������㷨���㣩
// ��������:    Mat bw    �����ֵͼ��
// ��������:    Point vpoint    ����ָ�������
// �� �� ֵ:    Point
//************************************************************************ 
Point  CBookProcess::bwlabel_min_x(Mat bw,Point vpoint)
{
	Point min_point;
	int x_min = bw.cols;
	int width = bw.cols;
	int height = bw.rows;
	int count = 0;
	vector<Point> vPoint;
	Point p2t;
	int j = vpoint.x;
	int i = vpoint.y;
	if(bw.ptr<uchar>(i)[j]!=0)
	{
		p2t.x = j;
		p2t.y = i;
		vPoint.push_back(p2t);
		count = count+1;
		bw.ptr<uchar>(i)[j]=0;
		int num=0;
		while(num<count)
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
						if (x_min>x)
						{
							x_min = x;
							min_point = p2t;
						}
						bw.ptr<uchar>(y)[x] =0;
						count =  count+1;
					}
				}
			}
			num = num+1;
		}
	}
	return min_point;
}

//************************************************************************  
// ��������:    GradientAdaptiveThreshold_Rect  
// ����˵��:    ��������Ӧ�ݶȶ�ֵ����Ե���
// ��������:    Mat src    ����ͼ��
// ��������:    Point point_1   ָ���������Ͻ�����
// ��������:    Point point_2   ָ���������½�����
// ��������:    float threshold   ����Ӧ�ݶȷŴ���
// �� �� ֵ:    Mat 
//************************************************************************ 
Mat CBookProcess::GradientAdaptiveThreshold_Rect(Mat src,Point point_1,Point point_2,float threshold)
{
	Mat gray;

	if(src.channels() == 3)
	{
		cvtColor(src, gray, CV_BGR2GRAY);
	}
	else
	{
		gray = src.clone();
	}
    Size dsize = Size(gray.cols,gray.rows);
    if (point_1.x<1)
		point_1.x = 1;
	if (point_1.y<1)
		point_1.y = 1;
	if (point_2.x>src.cols-2)
		point_2.x = src.cols-2;
	if (point_2.y>src.rows-2)
		point_2.y = src.rows-2;
	Mat temp = gray(Rect(point_1.x, point_1.y, point_2.x-point_1.x+1, point_2.y-point_1.y+1));
	Mat dst = CBookProcess::GradientAdaptiveThreshold(temp,threshold);
	Mat mat_dst = Mat::zeros(dsize,CV_8UC1);
    for (int i = point_1.y; i < point_2.y; i++)
    {
        for (int j = point_1.x; j < point_2.x; j++)
        {
            mat_dst.ptr<uchar>(i)[j] = dst.ptr<uchar>(i-point_1.y)[j-point_1.x];
        }
    }
	/*Mat element = getStructuringElement(MORPH_ELLIPSE,Size(3,3));
    morphologyEx(mat_dst, mat_dst, MORPH_OPEN, element);
	element = getStructuringElement( MORPH_RECT,Size(6,3));
	dilate(mat_dst, mat_dst, element );*/
	return mat_dst;
}

//����Ӧ�ݶȶ�ֵ���㷨
//[in]src				����ͼ��
//[out]dst				���ͼ��
Mat CBookProcess::GradientAdaptiveThreshold(Mat src,float th)
{
	Mat gray;

	if(src.channels() == 3)
	{
		cvtColor(src, gray, CV_BGR2GRAY);
	}
	else
	{
		gray = src.clone();
	}
    int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	Mat grad;
	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;
	/// �� X�����ݶ�
	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	Sobel( gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
	convertScaleAbs( grad_x, abs_grad_x );

	/// ��Y�����ݶ�
	//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
	Sobel( gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
	convertScaleAbs( grad_y, abs_grad_y );

	/// �ϲ��ݶ�(����)
	addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );

	

	float avg = mean(grad)[0];
	float thresh = th*avg<40?40:th*avg;

	Mat mat_dst = grad>thresh;
	return mat_dst;
}

//************************************************************************  
// ��������:    bwlabel   
// ����˵��:    ��ֵͼ��С��ͨ��ɾ��
// ��������:    Mat bw    �����ֵͼ��
// ��������:    int threshold ɾ����ͨ����ֵ
// �� �� ֵ:    Mat  ���ͼ��
//************************************************************************ 
Mat  CBookProcess::bwlabel(Mat bw,int threshold)
{
#if 0
	int width = bw.cols;
	int height = bw.rows;
	int p = 0;
	vector<Point> vPoint;
	Point p2t;
	Mat parent = Mat::zeros(height,width,CV_8UC1); 
	for(int i =1;i<height-1;i++)
	{
		for (int j =1;j<width-1;j++)
		{
			p = 0;
			vPoint.clear();
			if(bw.at<uchar>(i,j)!=0)
			{
				p2t.x = j;
				p2t.y = i;
				vPoint.push_back(p2t);
				p = p+1;
				parent.at<uchar>(i,j) = 255;
				bw.at<uchar>(i,j)=0;
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
								parent.at<uchar>(y,x) = 255;
								bw.at<uchar>(y,x) =0;
								p = p+1;
							}
						}
				   }
					num = num+1;
				}
			}
			if (p<threshold)
			{
			 for (int x = 0;x<p;x++)
			 {
				parent.at<uchar>(vPoint[x].y,vPoint[x].x) = 0;
			 }
			}
		}
	}
	return parent;
#else
	Mat dst = Mat::zeros(bw.size(), CV_8UC1 );
	vector<Point> areaMatvPoint;
	//��������ȡ��vvPointΪ������������
    vector<vector<Point> > vvPoint;
    findContours(bw,vvPoint,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    if (vvPoint.size()>0)
	{
		//����ͼ�������������������
		int rectMaxArea =0;
		int rectArea =0;
		RotatedRect rMaxRect;
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
			
			}
		}
		vvPoint.clear();
		vvPoint.push_back(areaMatvPoint);
		drawContours( dst, vvPoint ,-1, Scalar(255),-1); 
		
	}
	return dst;
#endif
}


//ֱ��ͼ����Ӧ��ֵ���㷨
//[in]src				����ͼ��
//[out]dst				���ͼ��
Mat CBookProcess::HistAdaptiveThreshold(Mat src,bool type)
{
    Mat dst;
	Mat mat_dst;
	Mat image;
	//ͼ����ɫ�ռ�ת��
	//��in��src��ԭʼͼ��
	//��out��image����ͨ��ͼ��
	if(src.channels()==3)
	{
		Mat src_hsv;
		cvtColor(src, src_hsv, CV_BGR2HSV);
		vector<cv::Mat> rgbChannels(3);  
		split(src_hsv, rgbChannels); 
		image = rgbChannels[2];
	}
	else
	{
		image = src.clone();
	}
	/*
	//���߾��Ȼ�����
	//��in/out��image�����뵥ͨ��ͼ/���߾��Ⱥ�ͼ��
	if (type)
	{
		CBookProcess::IM_illumination_processing (image, mat_dst);
		//blur(image, mat_dst, Size(100,100));
		//dst = mat_dst.mul(image>60);
		image =mat_dst.mul((image>60)/255);
		Mat element = getStructuringElement(MORPH_RECT,Size(5,5));
		morphologyEx(image, image, MORPH_OPEN, element);
	}
	*/

	//dst = image>mat_dst;
	Mat gray = image.clone();
	Mat gray_clone = gray.clone();
	
	/// ����ͼ��ֱ��ͼ:
	//��in��gray_clone����ͨ��ͼ��
	//��out��hist��ֱ��ͼ
	int histSize = 255;
	float range[] = {0,256};
	const float* histRange = {range};
	bool uniform = true;
	bool accumulate = false;
	Mat hist;
	calcHist( &gray_clone , 1, 0, Mat(), hist, 1, &histSize, &histRange, uniform, accumulate );
	

	//ƽ��ֱ��ͼ
	//���ƽ��
	//��in/out��hist��ֱ��ͼ
	hist.convertTo( hist,CV_64FC1);
	int col = hist.cols;
	int row = hist.rows;
	for (int zz = 0;zz<5;zz++)
	{
		for(int i = 3;i<hist.rows-3;i++)
		{
			float aa = 0;
			for(int j=0;j<3;j++)
			{
				aa += hist.ptr<double>(0)[i-j]*(3.0-j)/9+hist.ptr<double>(0)[i+j]*(3.0-j)/9;
			}
			aa = aa-hist.ptr<double>(0)[i]*(3.0)/9;
			hist.ptr<double>(0)[i] = aa;
		}
	}

	//����ֱ��ͼ������Ѷ�ֵ����ֵ
	//��in��hist��ֱ��ͼ
	//��out��thresh��ͼ���ֵ����ֵ
	int x = 0;
	float aa = hist.ptr<double>(0)[0];
	float bb;
	for (int i = 1;i<120;i++)
	{	
		bb = hist.ptr<double>(0)[i];
		if (aa<bb)
		{
			aa = bb;
			x = i;
		}
	}

	int y = x;
	int thresh;
	
	for (int i = x; i<235;i+20)
	{
		int min_hist = hist.ptr<double>(0)[i];
		for (int j = i;j<i+20;j++)
		{
			if (hist.ptr<double>(0)[j]<min_hist)
			{
				min_hist = hist.ptr<double>(0)[j];
				y = j;
			}
		}
		if (aa>min_hist)
		{
			aa = min_hist;
		}
		else
		{
			thresh = y;
			break;
		}
	}


	Mat bw;
	if(type)
	{
		//��ֵУ��
		thresh = (thresh<50)?thresh:50;
		thresh = (thresh>70)?thresh:70;
	}
	else
	{
		//��ֵУ��
		thresh = (thresh<80)?thresh:80;
		thresh = (thresh>100)?thresh:100;

		//������ԭʼͼ��ʹ�ò�ɫ��ֵ��ֵ��
		//��������ָ��⣩
		//��in��src��ԭʼͼ��
		//��in��bw����ֵͼ��
		vector<cv::Mat> rgbChannels(3);  
		split(src, rgbChannels); 
		image = rgbChannels[2];
		Mat bw1,bw2;
		bitwise_or(rgbChannels[0]/rgbChannels[2]<0.7,rgbChannels[0]/rgbChannels[2]>1.3,bw1);
		bitwise_or(rgbChannels[1]/rgbChannels[2]<0.7,rgbChannels[1]/rgbChannels[2]>1.3,bw2);
		bitwise_or(bw1,bw2,bw);
	}

	threshold(gray,dst,thresh,255,THRESH_BINARY);//ͼ���ֵ��
	if (type==false)
	{
		bitwise_or(bw,dst,dst);//�ϲ���ֵͼ��
	}
	//dst = CBookProcess::bwlabel(dst,100);//ɾ��С��100���ص���ͨ��

	//��ֵͼ������
	Mat element = getStructuringElement(MORPH_ELLIPSE,Size(5,1));
	//morphologyEx(dst, dst, MORPH_OPEN, element);
	//dilate(dst, dst, element);
	return dst;
}

//************************************************************************  
// ��������:    bwlabel_mask   
// ����˵��:    ��ֵͼ��ָ����ͨ����������
// ��������:    Mat bw    �����ֵͼ��
// ��������:    Point vpoint    ����ָ�������
// �� �� ֵ:    Mat  ���ͼ��
//************************************************************************ 
Mat  CBookProcess::bwlabel_mask(Mat bw,Point vpoint)
{
	int width = bw.cols;
	int height = bw.rows;
	int p = 0;
	vector<Point> vPoint;
	Point p2t;
	Mat parent = Mat::zeros(height,width,CV_8UC1); 
	vPoint.clear();
	int j = vpoint.x;
	int i = vpoint.y;
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
	return parent;
}


/*YCrCb��ɫ�ռ�Cr,Cb��Χɸѡ��*/
Mat CBookProcess::YCrCb_detect(Mat &src)
{
    Mat img = src.clone();
    Mat skinCrCbHist = Mat::zeros(Size(256, 256), CV_8UC1);
	if (src.channels()<3 || src.empty())
	{
		return  Mat::zeros(img.size(), CV_8UC1);
	}
    //����opencv�Դ�����Բ���ɺ���������һ����ɫ��Բģ��
    ellipse(skinCrCbHist, Point(113, 155.6), Size(23.4, 15.2), 43.0, 0.0, 360.0, Scalar(255, 255, 255), -1);
	//ellipse(skinCrCbHist, Point(113, 155.6), Size(15.2, 10.2), 43.0, 0.0, 360.0, Scalar(255, 255, 255), -1);
    Mat ycrcb_image;
    Mat output_mask = Mat::zeros(img.size(), CV_8UC1);
    cvtColor(img, ycrcb_image, CV_BGR2YCrCb); //����ת���ɵ�YCrCb�ռ�
    for (int i = 0; i < img.cols; i++)   //������ԲƤ��ģ�ͽ���Ƥ�����
        for (int j = 0; j < img.rows; j++) 
        {
            Vec3b ycrcb = ycrcb_image.at<Vec3b>(j, i);
            if (skinCrCbHist.at<uchar>(ycrcb[1], ycrcb[2]) > 0)   //���������Ƥ��ģ����Բ�����ڣ��õ����Ƥ�����ص�
                output_mask.at<uchar>(j, i) = 255;
        }

    Mat detect;
    img.copyTo(detect,output_mask);  //���ط�ɫͼ
    return detect;
}

void CBookProcess::fillHole(const Mat srcBw, Mat &dstBw)
{
	Size m_Size = srcBw.size();
	Mat Temp=Mat::zeros(m_Size.height+2,m_Size.width+2,srcBw.type());//��չͼ��
	srcBw.copyTo(Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));
 
	cv::floodFill(Temp, Point(0, 0), Scalar(255));
 
	Mat cutImg;//�ü���չ��ͼ��
	Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);
 
	dstBw = srcBw | (~cutImg);
}

//************************************************************************  
// ��������:    IM_illumination_processing          
// ����˵��:    �Ҷ�ͼ����ղ����ȴ��� 
// ��������:    Mat & src_img   ����ͼ��  
// ��������:    Mat & dst_img   ���ͼ��  
//************************************************************************  
void CBookProcess::IM_illumination_processing (const Mat src, Mat &dst_img)
{
	Mat image;
	vector<cv::Mat> rgbChannels(3); 
	if (src.channels() == 3)
	{
		Mat src_hsv;
		cvtColor(src, src_hsv, CV_BGR2HSV);
		split(src_hsv, rgbChannels); 
		image = rgbChannels[2];
	}
	else
	{
		image = src;
	}

	Mat mat_dst;
	Mat im = image.clone();
	blur(image, mat_dst, Size(100,100));
	//image = image-mat_dst;
	image.convertTo(image, CV_32FC1);
	mat_dst.convertTo(mat_dst, CV_32FC1);
	mat_dst = mat_dst-10;
	image = image-mat_dst;
	//��һ����0~255  
	//normalize(image, image, 0, 255, CV_MINMAX);
	//ת����8bitͼ����ʾ  
	//convertScaleAbs(image, image);
	image = image+128;
	//��һ����0~255  
	//normalize(image, image, 0, 255, CV_MINMAX);
	image.convertTo(image, CV_8UC1);
	//image = image>mat_dst;
	//image = image.mul((im>40)/255);
	if (src.channels()==3)
	{
		rgbChannels[2] = image;
		Mat dst_hsv;
		merge(rgbChannels,dst_hsv);
		cvtColor(dst_hsv,dst_img, CV_HSV2BGR);
	}
	else
	{
		dst_img = image;
	}
}


//************************************************************************  
// ��������:    mcvBookStretch_Enhance  
// ����˵��:    ͼ��ҶȶԱȶ���ǿ
// ��������:    Mat src    ����ͼ��
// �� �� ֵ:    Mat  �����ǿ��Ҷ�ͼ��
//************************************************************************ 
Mat  CBookProcess::Enhance(Mat src,int type)
{
	//int kk = 4;
	Mat dst;
	if(type == 1)
	{
		Mat mat_dst;
		Mat gray;
		if(src.channels()==3)
		{
			cvtColor(src, gray, CV_BGR2GRAY);
		}
		else
		{
			gray = src.clone();
		}
		CBookProcess::IM_illumination_processing (gray, dst);
		//bool errMsg = CBookStretch::colorEnhance(mat_dst,gray,128);
		mat_dst = CBookProcess::Gamma_Enhance(dst);
		//CBookStretch::IM_illumination_processing (dst, dst);
		//threshold(mat_dst,mat_dst,35,255,THRESH_BINARY);
		cvtColor(mat_dst, dst, CV_GRAY2BGR);
	}

	else if(type==2)
	{
		Mat gray;
		Mat src_hsv;
		if (src.channels()==3)
		{
			cvtColor(src, gray, CV_BGR2GRAY);
			/*vector<cv::Mat> rgbChannels(3);  
			split(src_hsv, rgbChannels); 
			gray = rgbChannels[2];*/
		}
		else
		{
			gray = src;
		}

		Mat gray_gauss;
		GaussianBlur(gray,gray_gauss,Size(3,3),0);

		Mat mat_dst;
		int winSize = 2;

		CBookProcess::IM_illumination_processing (gray_gauss,mat_dst);
		bool aa =  CBookProcess::ACE_Enhance(gray_gauss,mat_dst,winSize,2.5);//����Ӧͼ����ǿ�㷨��ǿͼ��Աȶ�
	
		//medianBlur(dst, mat_dst, 3);
	
		cvtColor(mat_dst, dst, CV_GRAY2BGR);
	}
	else if(type==3)
	{
		Mat mat_dst;
		Mat gray;
		if(src.channels()==3)
		{
			cvtColor(src, gray, CV_BGR2GRAY);
		}
		else
		{
			gray = src.clone();
		}
		CBookProcess::IM_illumination_processing(gray, mat_dst);
		int aa = mat_dst.channels();
		int rows = mat_dst.rows;
		int cols = mat_dst.cols;
		cvtColor(mat_dst, dst, CV_GRAY2BGR);

		//dsize = Size(src.cols,src.rows);
		//resize(mat_dst, dst, dsize);
	}
	else if(type==4)
	{
		Mat mat_dst;
		Mat gray;
		if(src.channels()==3)
		{
			cvtColor(src, gray, CV_BGR2GRAY);
		}
		else
		{
			gray = src.clone();
		}
		CBookProcess::IM_illumination_processing (gray, dst);
		mat_dst = CBookProcess::multiScaleSharpen(dst,5);
	
		cvtColor(mat_dst, dst, CV_GRAY2BGR);
	}
	else if (type ==5)
	{
		Mat gray;
		Mat mat_dst;
		if(src.channels()==3)
		{
			cvtColor(src, gray, CV_BGR2GRAY);
		}
		else
		{
			gray = src.clone();
		}
		CBookProcess::IM_illumination_processing(gray, mat_dst);
		dst = CBookProcess::BrightnessBalance(mat_dst);
	}
	return dst;	
}


//************************************************************************  
// ��������:    mcvBookStretch_colorEnhance 
// ����˵��:    ��ɫͼ����ǿ(����)
// ��������:    Mat src     ����ͼ��
// ��������:    Mat &src    ���ͼ��
// �� �� ֵ:    bool
//************************************************************************ 
bool  CBookProcess::colorEnhance(Mat src,Mat &dst,float mean_pix,float minp,float maxp)
{
	Mat originalMat =src.clone();

	vector<Mat> vMat;

	Mat imgR = src.clone();
	float meanp = 255*(mean_pix-minp)/(maxp-minp);
	float aa = 255/(maxp-minp);
	//��ͼ�������ǿ
	//sigmoid����256����Ӧ����ֵ
	float sigmoid256Fidelity[256]={0};//����
	for(float idx =minp;idx<=maxp;idx++)
	{
		if (false)
		{
			sigmoid256Fidelity[(int)idx] = 255.0/(1.0+pow((float)2.718,(float)(-1.0*(aa*(255-minp) -meanp)/15.0)));
		}
		else
		{
			sigmoid256Fidelity[(int)idx] = 255.0/(1.0+pow((float)2.718,(float)(-1.0*(aa*(idx-minp) -meanp)/15.0)));
		}
	}
	//����ͼ���а�ɫ����ռ�ı���
	int whiteNum =0;
	int zerosNum =0;
	//��ɫͼ��
	uchar *dataR = imgR.ptr<uchar>(0);

	for(int idr =0;idr <src.rows;idr++)
		for(int idc =0;idc<src.cols;idc++)
		{
			*dataR++ = sigmoid256Fidelity[*dataR];
		}
		dst = imgR;

	return true;
}

//************************************************************************  
// ��������:    BrightnessBalance
// ����˵��:    ����ƽ���㷨
// ��������:    Mat src     ����ͼ��
// �� �� ֵ:    Mat
//************************************************************************ 
Mat CBookProcess::BrightnessBalance(Mat src)
{
	Mat dst;
	Mat image;
	if(src.channels()==3)
	{
		cvtColor(src, image, CV_BGR2GRAY);
	}
	else
	{
		image = src.clone();
	}

	GaussianBlur(image, image, Size(3, 3), 1.0, 1.0);//��˹ģ��
	/*Mat gray;
	CBookProcess::IM_illumination_processing (image, gray);
	image = gray;*/

	Mat mat_dst;
	blur(image, mat_dst, Size(60,60));

	image.convertTo(image, CV_32FC1);
	mat_dst.convertTo(mat_dst, CV_32FC1);

	mat_dst = mat_dst-8;
	image = mat_dst-image;
	
	mat_dst = 255-image;
	mat_dst.convertTo(mat_dst, CV_8UC1);
	
	//return mat_dst;
	double men = mean(mat_dst)[0];

	double minv = 0.0, maxv = 0.0;
	double* minp = &minv;
	double* maxp = &maxv;
 
	minMaxIdx(mat_dst,minp,maxp);
	//equalizeHist(mat_dst,image);
	CBookProcess::colorEnhance(mat_dst,image,6*(maxv+minv)/10,0,255);

	cvtColor(image, dst, CV_GRAY2BGR);
	

	return dst;
}

//************************************************************************  
// ��������:    multiScaleSharpen
// ����˵��:    ��߶ȶԱȶ������㷨
// ��������:    Mat src     ����ͼ��
// ��������:    int Radius     �߶Ȳ���
// �� �� ֵ:    Mat
//************************************************************************ 
Mat CBookProcess::multiScaleSharpen(Mat Src, int Radius)
{
	//GaussianBlur(Src, Src, Size(3, 3), 1.5, 1.5);//��˹ģ��
	int rows = Src.rows;
	int cols = Src.cols;
	int cha = Src.channels();
	Mat B1, B2, B3;
	GaussianBlur(Src, B1, Size(Radius, Radius), 1.0, 1.0);//��˹ģ��
	GaussianBlur(Src, B2, Size(Radius*2-1, Radius*2-1), 2.0, 2.0);
	GaussianBlur(Src, B3, Size(Radius*4-1, Radius*4-1), 4.0, 4.0);
 
	double w1 = 0.5;
	double w2 = 0.5;
	double w3 = 0.25;
 
	Mat dst = Mat::zeros(Src.size(), Src.type());
	for (size_t i = 0; i < rows; i++)
	{
		uchar* src_ptr = Src.ptr<uchar>(i);
		uchar* dst_ptr = dst.ptr<uchar>(i);
		uchar* B1_ptr = B1.ptr<uchar>(i);
		uchar* B2_ptr = B2.ptr<uchar>(i);
		uchar* B3_ptr = B3.ptr<uchar>(i);
		for (size_t j = 0; j < cols; j++)
		{
			for (size_t c = 0; c < cha; c++)
			{
				int  D1 = src_ptr[j+c] - B1_ptr[j + c];
				int  D2 = B1_ptr[j + c] - B2_ptr[j + c];
				int  D3 = B2_ptr[j + c] - B3_ptr[j + c];
				int  sign = (D1 > 0) ? 1 : -1;
				dst_ptr[j + c] = saturate_cast<uchar>((1 - w1*sign)*D1 - w2*D2 + w3*D3 + src_ptr[j + c]);
			}
		}
	}
	GaussianBlur(dst, dst, Size(3, 3), 1.5, 1.5);//��˹ģ��
	return dst;
}

//************************************************************************  
// ��������:    Gamma_Enhance
// ����˵��:    ٤��任ͼ����ǿ
// ��������:    Mat src     ����ͼ��
// �� �� ֵ:    Mat
//************************************************************************ 
Mat CBookProcess::Gamma_Enhance(Mat src)
{
	Mat image;
	if(src.channels()==3)
	{
		cvtColor(src, image, CV_BGR2GRAY);
	}
	else
	{
		image = src.clone();
	}
	GaussianBlur(image, image, Size(3, 3), 1.5, 1.5);//��˹ģ��
	Mat bw = CBookProcess::GradientAdaptiveThreshold(src,1.8);

	bw =  CBookProcess::bwlabel(bw,20);

	Mat element = getStructuringElement(MORPH_ELLIPSE,Size(5,2));
	dilate(bw, bw, element );

	double pix = pow(140,2.0);
	//Mat imageGamma1 = Mat::ones(image.size(), CV_32FC1)*255*255;
	//Mat imageGamma2 = Mat::ones(image.size(), CV_32FC1)*255*255;
	Mat imageGamma2(image.size(), CV_32FC1);
	Mat imageGamma1(image.size(), CV_32FC1);

	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			for (int k = 0;k<image.channels();k++)
			{
				if (image.ptr<uchar>(i, j)[k]<=100 || bw.ptr<uchar>(i)[j]>0)
				{
					imageGamma1.ptr<float>(i, j)[k] = pow((double)image.ptr<uchar>(i, j)[k],2);//(image.at<Vec3b>(i, j)[0])*(image.at<Vec3b>(i, j)[0])*(image.at<Vec3b>(i, j)[0]);
				}
				else
				{
					imageGamma1.ptr<float>(i, j)[k] = pix;//pow((double)image.ptr<uchar>(i, j)[k],0.33);
				}
			}
		}
	}

	//��һ����0~255  
	normalize(imageGamma1, imageGamma1, 0, 255, CV_MINMAX);
	//ת����8bitͼ����ʾ  
	convertScaleAbs(imageGamma1, imageGamma1);
	return  imageGamma1;
	/*//��һ����0~255  
	normalize(DstImg, DstImg, 0, 255, CV_MINMAX);
	//ת����8bitͼ����ʾ  
	convertScaleAbs(DstImg, DstImg);*/
	//Mat imageGamma  =imageGamma2+imageGamma1;*/
	//return DstImg;
}

//************************************************************************  
// ��������:    ACE_Enhance          
// ����˵��:    ��ͨ����ǿ  
// ��������:    Mat & src_img   ����ͼ��  
// ��������:    Mat & dst_img   ���ͼ��  
// ��������:    unsigned int half_winSize   ��ǿ���ڵİ봰��С  
// ��������:    double Max_Q            ���Qֵ  
// �� �� ֵ:    bool  
//************************************************************************  
bool  CBookProcess::ACE_Enhance(Mat& src_img, Mat& dst_img, unsigned int half_winSize, double Max_Q)  
{  
    if (!src_img.data)  
    {  
        cout << "û������ͼ��" << endl;  
        return false;  
    }  

	int rows(src_img.rows);  
    int cols(src_img.cols);  
    unsigned char* data = nullptr;  
    unsigned char* data1 = nullptr;  

	//���㡢����ƽ��ֵΪ��ɫ�ķ���ͼƬ
	Mat mat_mean, mat_stddev;
   // meanStdDev(src_img,mat_mean,mat_stddev);
	double average = mean(src_img)[0];
	int thresh1 = int(0.8*average);//int(0.8*mat_mean.at<double>(0,0));
	int thresh = int(average);//(mat_mean.at<double>(0,0));
	int thresh2 = thresh;
	Mat DstImg = Mat::ones(rows, cols,CV_8UC1)*average;//(int(mat_mean.at<double>(0,0))); 
	
	//��Ե��⡢����
	Mat bw = CBookProcess::GradientAdaptiveThreshold(src_img,1.8);

	bw =  CBookProcess::bwlabel(bw,20);

	Mat bw_mask;
	CBookProcess::fillHole(bw, bw_mask);
	bw = bw_mask;

	Mat element = getStructuringElement(MORPH_ELLIPSE,Size(5,2));
	dilate(bw, bw, element );

	//dst_img = bw;
	//return true;

	/////ʹ��clahe����Ӧ����
	/*Ptr<CLAHE> clahe = createCLAHE();
	clahe->setClipLimit(4);
	clahe->setTilesGridSize(Size(10,10));
	clahe->apply(src_img, src_img);*/

	vector<vector<Point> > vvPoint;
    findContours(bw,vvPoint,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    
    //����ͼ�������
	Point2f p2f[4];
	RotatedRect rrect;
	float x11,x12,x13,x14;
	float y11,y12,y13,y14;
	float y1,y2,x1,x2;
	vector<Point2f> Points1;
	vector<Point2f> Points2;

	double Area;
	int rectArea =0;
	vector<Point> areaMatvPoint;
    for (vector<vector<Point> >::iterator itr=vvPoint.begin();itr!=vvPoint.end();itr++)
    {
        rrect =minAreaRect(*itr);
		rectArea =rrect.size.area();
		areaMatvPoint.clear();
        areaMatvPoint = *itr;
		Area = contourArea(areaMatvPoint);
		rrect.points(p2f);
		x11 = p2f[0].x;
		x12 = p2f[1].x;
		x13 = p2f[2].x;
		x14 = p2f[3].x;
		y11 = p2f[0].y;
		y12 = p2f[1].y;
		y13 = p2f[2].y;
		y14 = p2f[3].y;
		x1 = min(min(x11,x12),min(x13,x14));
		x2 = max(max(x11,x12),max(x13,x14));
		y1 = min(min(y11,y12),min(y13,y14));
		y2 = max(max(y11,y12),max(y13,y14));

		/*int height = y2-y1;
		if(height>80)
			half_winSize = 5;
		else if(height>50)
			half_winSize = 4;
		else if(height>35)
			half_winSize = 3;
		else
			half_winSize = 2;*/

		//�߽�Լ��
		if (x1<half_winSize)
			x1 = half_winSize;
		if (y1<half_winSize)
			y1 = half_winSize;
		
		if (x2>bw.cols-half_winSize-1)
			x2 = bw.cols-half_winSize-2;
		if (y2>bw.rows-half_winSize-1)
			y2 = bw.rows-half_winSize-2;
	 
		if (x2<=x1)
			continue;
		if (y2<=y1)
			continue;

		if ((y2-y1)/(x2-x1)>3 && (x2-x1<100))
		{
			continue;
		}
		for (int i = y1; i < y2; i++)  
		{  
			data = DstImg.ptr<unsigned char>(i);  
			data1 = src_img.ptr<unsigned char>(i);  
			for (int j = x1; j < x2; j++)  
			{  
				//�ж����ص��Ƿ�ΪĿ���
				if ((bw.ptr<uchar>(i)[j]==0))
				{
					continue;
				}
				cv::Mat temp = src_img(cv::Rect(j - half_winSize, i - half_winSize, half_winSize * 2 + 1, half_winSize * 2 + 1));   //��ȡ����ͼ��  
	

				meanStdDev(temp,mat_mean,mat_stddev);
				double MeanVlaue =  mat_mean.at<double>(0,0); 
				double varian =  mat_stddev.at<double>(0,0);
				//double MeanVlaue =  mean(temp)[0];//GetMeanValue(temp);  
				//double varian =  GetVarianceValue(temp, MeanVlaue); 

				if (varian> 0.1)
				{  
					double cg = 100.0 /varian; 
					cg = cg > Max_Q ? Max_Q : cg;  
					double pixelvalue = cg*((double)data1[j] - MeanVlaue);

					int temp = MeanVlaue + pixelvalue;   

					temp = temp > 220 ? 255 : temp;  
					temp = temp < 0 ? 0 : temp; 
					data[j] = temp;  
				} 
				else
				{
					data[j] = data1[j];
				}
			}
		} 
	}
    dst_img = DstImg; 
	
    return true;  
} 


Mat CBookProcess::detection(Mat src)
{
	Mat gray;
	if(src.channels() == 3)
	{
		cvtColor(src, gray, CV_BGR2GRAY);
	}
	else
	{
		gray = src.clone();
	}

	//���㡢����ƽ��ֵΪ��ɫ�ķ���ͼƬ
	Mat mat_mean, mat_stddev;
	meanStdDev(gray,mat_mean,mat_stddev); 
	double average = mean(gray)[0];
	Mat mask = gray<0.8*average;//(0.8*average>128?128:0.8*average);

	/*Mat src_hsv;
	cvtColor(src, src_hsv, CV_BGR2HSV);

	vector<cv::Mat> rgbChannels(3);  
	split(src_hsv, rgbChannels); 
	Mat bw1 = rgbChannels[1]<0.35*255;
	Mat bw2 = gray>55;
	Mat bw;
	bitwise_and(bw1,bw2,bw);

	bitwise_and(bw1,mask,mask);*/
	return mask;
}


//************************************************************************  
// ��������:    KeepParallel_1
// ����˵��:	�鱾���߶������
// ��������:    vector<vector<Point> >& edgePoint  ���±߽���������
// ��������:    int row  ͼ��߶�
// ��������:    int x_max ���ߵ������ܷ�Χ��Լ��������
// �� �� ֵ:    vector<vector<Point> >
//************************************************************************ 
vector<vector<Point> > CBookProcess::KeepParallel(vector<vector<Point> > edgePoint, Mat src)
{


	vector<Point> vvPT = edgePoint[0];
	vector<Point> vvPB = edgePoint[1];

	Point point1_4 = vvPT[0];
	Point point1_3 = vvPB[vvPB.size()-1];

	//��ȡ��ҳ�ĸ�����
	//��in��vvPT���ϱ߽�����
	//��in��vvPB���±߽�����
	//��out��point1�����ϽǶ���
	//��out��point2�����½Ƕ���
	//��out��point3�����½Ƕ���
	//��out��point4�����ϽǶ���
	Point2f point1 = vvPT[vvPT.size()-1];
	Point2f point2 = vvPB[0];
	Point2f point3 = vvPB[vvPB.size()-1];
	Point2f point4 = vvPT[0];
	Point2f p2t;

	///////////////////////////////////////////////////////////
	int max_y_T = 0;
	int min_y_B = src.rows;
	for (int ii = 0;ii<vvPT.size();ii++)
	{
		if (max_y_T<vvPT[ii].y)
			max_y_T = vvPT[ii].y;
	}
	for (int ii = 0;ii<vvPB.size();ii++)
	{
		int zz = vvPB[ii].y;
		if (min_y_B>vvPB[ii].y)
			min_y_B = vvPB[ii].y;
	}

	float scale= 500.0/src.rows;
	Size dsize = Size(src.cols*scale,src.rows*scale);
	Mat image = Mat(dsize,CV_8UC1);
	resize(src, image, dsize);

	Point2f p2t_1(scale*point1.x,scale*(max_y_T+(point2.y-point1.y)/10));
	Point2f p2t_2(scale*(point2.x+(point4.x-point1.x)/3),scale*(max_y_T+(point2.y-point1.y)/4+100));
	Point2f p2t_3(scale*point2.x,scale*(min_y_B-(point2.y-point1.y)/4-100));
	Point2f p2t_4(scale*(point2.x+(point4.x-point1.x)/3),scale*(min_y_B-(point2.y-point1.y)/10));

	Mat temp1 = CBookProcess::GradientAdaptiveThreshold_Rect(image,p2t_1,p2t_2,1.6);//����Ӧ�����Ŵ���
	temp1 = CBookProcess::bwlabel(temp1,20);
	Mat element = getStructuringElement( MORPH_RECT,Size(4,3));
	dilate(temp1,temp1, element );
	
	Mat temp2 = CBookProcess::GradientAdaptiveThreshold_Rect(image,p2t_3,p2t_4,1.6);//����Ӧ�����Ŵ���
	temp2 = CBookProcess::bwlabel(temp2,20);
	element = getStructuringElement( MORPH_RECT,Size(4,3));
	dilate(temp2,temp2, element );
	
	p2t_1.x = p2t_2.x;
	p2t_3.x = p2t_4.x;
	line(temp1,p2t_1,p2t_2,Scalar(255),(point4.x-point1.x)/10);
	line(temp2,p2t_3,p2t_4,Scalar(255),(point4.x-point1.x)/10);
	//imwrite("output1.jpg",temp1);
	//imwrite("output2.jpg",temp2);
	Point min_point1 = CBookProcess::bwlabel_min_x(temp1, p2t_1);
	Point min_point2 = CBookProcess::bwlabel_min_x(temp2, p2t_3);

	min_point1.x = min_point1.x/scale;
	min_point1.y = min_point1.y/scale;
	min_point2.x = min_point2.x/scale;
	min_point2.y = min_point2.y/scale;

	float K1 = (min_point1.y-min_point2.y)/(min_point1.x-min_point2.x+0.001);
	float K2 = (point4.y-point3.y)/(point4.x-point3.x+0.001);

	float angle1,angle2,angle3;
	if (abs(K1)>50)
		angle1 = 90;
	else
	{
		angle1= (180.0/3.1415)*atan(K1);
		if (angle1<0)
		{
			angle1 = int(360+angle1+0.5)%180;
		}
		else
		{
			angle1 = int(angle1+0.5)%180;
		}
	}

	if (abs(K2)>50)
		angle2 = 90;
	else
	{
		angle2= (180.0/3.1415)*atan(K2);
		if (angle2<0)
		{
			angle2 = int(360+angle2+0.5)%180;
		}
		else
		{
			angle2 = int(angle2+0.5)%180;
		}
	}

	bool flag = true;
	if ((point1.x+(point4.x-point1.x)/3-min_point1.x)<16)
		flag = false;
	if ((point2.x+(point4.x-point1.x)/3-min_point2.x)<16)
		flag = false;
	if (flag ==false)
		angle1 = angle2+5;

	if (abs(angle1-angle2)<2.5)
	{
		point1_4 = min_point1;
		point1_3 = min_point2;
	}

	float x_max = min(min_point1.x,min_point2.x);

	
	///////////////////////////////////////////////////////////////////////////
	
	///////////////////////////////////////////////////////////////////
	

	//���߶������
	//��in��vvPT���ϱ߽�����
	//��in��vvPB���±߽�����
	//��in��point1�����ϽǶ���
	//��in��point2�����½Ƕ���
	//��in��point3�����½Ƕ���
	//��in��point4�����ϽǶ���
	//��out��ind_T�������϶�������
	//��out��ind_B�������¶�������
		
	int ind_T = vvPT.size()-1;
	int ind_B = 0;

	//���߶������
	float x11 = point1.x;
	float y11 = point1.y;
	float x12 = point4.x;
	float y12 = point4.y;
	float x21 = point2.x;
	float y21 = point2.y;
	float x22 = point3.x;
	float y22 = point3.y;
	float x1 = x11;
	float x2 = x21;
	float height = 0;
	float height1,height1_1,height2,height2_1;
	
	//////////////////////////////////////////////////////////////////////////
	//�����϶������
	//��in��vvPT���ϱ߽�����
	//��in��x11�����ϽǶ���x����
	//��in��x_max�����ߵ������ܷ�Χ�������ı����ݱ߽磩
	//��out��ind_T�������϶�������
	//��out��point1�����ϽǶ���
	//��out��errMsg_T�������϶������״̬����
	int label = 0;
	x1 = x11;
 	height1 = point1.y;
	bool errMsg_T = false;
	int ind_T_1;
	while (x1<x_max-(point4.x-point1.x)/40)
	{
		x1 = x1+10;
		int kk = vvPT.size()-1;
		while (kk>0)
		{
			p2t = vvPT[kk];
			if (p2t.x>x1)
			{
				break;
			}
			kk = kk-1;
		}

		height1_1 = p2t.y;
		if (height1-height1_1>=2.2)
		{
			label += 1;
			if(height1-height1_1>=6)
			{
				point1 = p2t;
				ind_T_1 = kk;
				errMsg_T = true;
			}
					
			if (label>2)
			{
				point1 = p2t;
				ind_T_1 = kk;
				errMsg_T = true;
			}
		}
		else if(height1-height1_1<-4)
		{
			errMsg_T = false;
			label = 0;
		}
		else 
		{
			label = 0;
		}
		height1 = height1_1;
	}
	if (errMsg_T)
	{
		ind_T = ind_T_1;
	}
	///////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//�����¶������
	//��in��vvPB�� �±߽�����
	//��in��x21�����½Ƕ���x����
	//��in��point1�����ϽǶ���
	//��in��x_max�����ߵ������ܷ�Χ�������ı����ݱ߽磩
	//��out��ind_B�������¶�������
	//��out��point2�����½Ƕ���
	//��out��errMsg_B�������¶������״̬����
	bool errMsg_B = false;
	int ind_B_1;
	x2 = x21;
	label = 0;
	float x23 = x21+(point1.x-x11);
			
	x2 = x23-(point1.x-x11)/2;
	if (errMsg_T)
	{
		x_max = (x23+(x22-x23)/30);
	}
	else
	{
		x_max = x_max-(point3.x-point2.x)/40;
	}
	height2 = src.rows-point2.y;
	while (x2<x_max)
	{
		x2 = x2+10;
		int zz = 0;
		while (zz<vvPB.size())
		{
			p2t = vvPB[zz];
			if (p2t.x>x2)
			{
				break;
			}
			zz = zz+1;
		}

		height2_1 = src.rows-p2t.y;
		if (height2-height2_1>=2.2)
		{
			label += 1;
			if (height2-height2_1>=6)
			{
				point2 = p2t;
				ind_B_1 = zz;
				errMsg_B = true;
			}
			if (label>2)
			{
				point2 = p2t;
				ind_B_1 = zz;
				errMsg_B = true;
			}
		}
		else if (height2-height2_1<-4)
		{
			errMsg_B = false;
			label = 0;
		}
		else
		{
			label = 0;
		}
		height2 = height2_1;
	}
	if(errMsg_B)
	{
		ind_B = ind_B_1;
	}
	////////////////////////////////////////////////////////////////////////////////

	
	if (errMsg_B && errMsg_T)
	{
		if (abs(angle1-angle2)<2.5 && abs((point4.x-point1.x)-(point3.x-point2.x))>0.04*(point4.x-point1.x))// && abs((point4.x-point1.x)-(point3.x-point2.x))>0.03*(point4.x-point1.x))
		{
			if(point1.x>point2.x-(point2.y-point1.y)/K1)
			{
				x2 = max(point2.x,point1.x-(point1.y-point2.y)/K1);
				int zz = 0;
				while (zz<vvPB.size())
				{
					p2t = vvPB[zz];
					if (p2t.x>x2)
					{
						break;
					}
					zz = zz+1;
				}
				point2 = p2t;
				ind_B = zz;
			}
			else
			{
				x2 = max(point1.x,point2.x-(point2.y-point1.y)/K1);
				int kk = vvPT.size()-1;
				while (kk<vvPT.size())
				{
					p2t = vvPT[kk];
					if (p2t.x>x2)
					{
						break;
					}
					kk = kk-1;
				}
				point1 = p2t;
				ind_T = kk;
			}
		}
		else if(abs((point4.x-point1.x)-(point3.x-point2.x))>0.05*(point4.x-point1.x))
		{
			if (point2.x>point1_3.x-(point1_4.x-point1.x))
			{
				x2 = max(point1.x,point1_4.x-(point1_3.x-point2.x));
				int kk = vvPT.size()-1;
				while (kk<vvPT.size())
				{
					p2t = vvPT[kk];
					if (p2t.x>x2)
					{
						break;
					}
					kk = kk-1;
				}
				point1 = p2t;
				ind_T = kk;
			}
			else
			{
				x2 = max(point2.x,point1_3.x-(point1_4.x-point1.x));
				int zz = 0;
				while (zz<vvPB.size())
				{
					p2t = vvPB[zz];
					if (p2t.x>x2)
					{
						break;
					}
					zz = zz+1;
				}
				point2 = p2t;
				ind_B = zz;
			}
		}
		/*
		//x2 = point2_1.x+(point1.x-point1_1.x);
		//x2 = max(point2.x,point1_3.x-(point1_4.x-point1.x));
		//x2 = point3.x-((point3.x-vvPB[0].x)*(point4.x-point1.x))/(point4.x-vvPT[vvPT.size()-1].x);
		int zz = 0;
		while (zz<vvPB.size())
		{
			p2t = vvPB[zz];
			if (p2t.x>x2)
			{
				break;
			}
			zz = zz+1;
		}
		point2 = p2t;
		ind_B = zz;
		*/
	}
	else if (errMsg_B==false && errMsg_T)
	{
		if (abs(angle1-angle2)<2.5)
			x2 = max(point2.x,point1.x-(point1.y-point2.y)/K1);
		else
			x2 = max(point2.x,point3.x-(point4.x-point1.x));
		//x2 = point2_1.x+(point1.x-point1_1.x);
		//x2 = max(point2.x,point1_3.x-(point1_4.x-point1.x));
		//x2 = point3.x-((point3.x-vvPB[0].x)*(point4.x-point1.x))/(point4.x-vvPT[vvPT.size()-1].x);
		int zz = 0;
		while (zz<vvPB.size())
		{
			p2t = vvPB[zz];
			if (p2t.x>x2)
			{
				break;
			}
			zz = zz+1;
		}
		point2 = p2t;
		ind_B = zz;
		errMsg_B = true;
	}


	else if(errMsg_T==false && errMsg_B)
	{
		if (abs(angle1-angle2)<2.5)
			x2 = max(point1.x,point2.x-(point2.y-point1.y)/K1);
		else
			x2 = max(point1.x,point4.x-(point3.x-point2.x));
		//x2 = point1_1.x+(point2.x-point2_1.x);
		//x2 = max(point1.x,point1_4.x-(point1_3.x-point2.x));
		//x2 = point4.x-((point3.x-vvPB[0].x)*(point4.x-point1.x))/(point3.x-vvPB[vvPB.size()-1].x);
		int kk = vvPT.size()-1;
		while (kk<vvPT.size())
		{
			p2t = vvPT[kk];
			if (p2t.x>x2)
			{
				break;
			}
			kk = kk-1;
		}
		point1 = p2t;
		ind_T = kk;
		errMsg_T = true;
	}
	else
	{
		float tt = 0.03*float(point4.x-point1.x);
		
		if (abs(angle1-angle2)<2.5 && abs(float(point4.x-point1.x)-float(point3.x-point2.x))>0.85*tt)
		{
			if(point1.x>point2.x-(point2.y-point1.y)/K1)
			{
				x2 = max(point2.x,point1.x-(point1.y-point2.y)/K1);
				int zz = 0;
				while (zz<vvPB.size())
				{
					p2t = vvPB[zz];
					if (p2t.x>x2)
					{
						break;
					}
					zz = zz+1;
				}
				point2 = p2t;
				ind_B = zz;
			}
			else
			{
				x2 = max(point1.x,point2.x-(point2.y-point1.y)/K1);
				int kk = vvPT.size()-1;
				while (kk<vvPT.size())
				{
					p2t = vvPT[kk];
					if (p2t.x>x2)
					{
						break;
					}
					kk = kk-1;
				}
				point1 = p2t;
				ind_T = kk;
			}
		}
		if (abs(float(point4.x-point1.x)-float(point3.x-point2.x))>tt)
		{
			if (point2.x>point1_3.x-(point1_4.x-point1.x))
			{
				x2 = max(point1.x,point1_4.x-(point1_3.x-point2.x));
				int kk = vvPT.size()-1;
				while (kk<vvPT.size())
				{
					p2t = vvPT[kk];
					if (p2t.x>x2)
					{
						break;
					}
					kk = kk-1;
				}
				point1 = p2t;
				ind_T = kk;
			}
			else
			{
				x2 = max(point2.x,point1_3.x-(point1_4.x-point1.x));
				int zz = 0;
				while (zz<vvPB.size())
				{
					p2t = vvPB[zz];
					if (p2t.x>x2)
					{
						break;
					}
					zz = zz+1;
				}
				point2 = p2t;
				ind_B = zz;
			}
			errMsg_T = true;
			errMsg_B = true;
		}
	}
	
	/////////////////////////////////////////////////////////////////////////////

	point1 = vvPT[ind_T];
	point2 = vvPB[ind_B];
	
	if (abs(float(point4.x-point1.x)-float(point3.x-point2.x))>0.03*float(point4.x-point1.x) && (abs(float(point1.x-vvPT[vvPT.size()-1].x)-float(point2.x-vvPB[0].x))>0.5*float(point4.x-point1.x)))
	{
		if ((point4.x-point1.x)<(point3.x-point2.x))
		{
			x2 = max(float(vvPT[vvPT.size()-1].x) ,point4.x-(point3.x-point2.x));
			int kk = vvPT.size()-1;
			while (kk<vvPT.size())
			{
				p2t = vvPT[kk];
				if (p2t.x>x2)
				{
					break;
				}
				kk = kk-1;
			}
			point1 = p2t;
			ind_T = kk;
		}
		else
		{
			x2 = max(float(vvPB[0].x),point3.x-(point4.x-point1.x));
			int zz = 0;
			while (zz<vvPB.size())
			{
				p2t = vvPB[zz];
				if (p2t.x>x2)
				{
					break;
				}
				zz = zz+1;
			}
			point2 = p2t;
			ind_B = zz;
		}
		errMsg_T = true;
		errMsg_B = true;
	}


	// ���±߽�����ȡ
	vector<Point> vvvPT;
	vector<Point> vvvPB;

	for(int i= 0;i<=ind_T;i++)
	{
		p2t = vvPT[i];
		vvvPT.push_back(p2t);
	}
	for(int i= ind_B;i<vvPB.size();i++)
	{
		p2t = vvPB[i];
		vvvPB.push_back(p2t);
	}

	
	edgePoint.clear();
	edgePoint.push_back(vvvPT);
	edgePoint.push_back(vvvPB);
	return edgePoint;
}

Point CBookProcess::object_detection(Mat src,Mat& dst,int type)
{
	Point point(0,0);
	if (src.empty())
		return point;

	double scale= 500.0/src.rows;
	Size dsize = Size(src.cols*scale,src.rows*scale);
	Mat image = Mat(dsize,CV_8UC1);
	resize(src, image, dsize);
	
	Mat bw = CBookProcess::im2bw(image);
	if (type==0)
	{
		line(bw,Point(2*bw.cols/3,bw.rows/2),Point(bw.cols-1,bw.rows/2),Scalar(255),5,CV_AA); 
	}
	else
	{
		line(bw,Point(0,bw.rows/2),Point(bw.cols/3,bw.rows/2),Scalar(255),5,CV_AA); 
	}
	vector<vector<Point> > vvPoint;
    findContours(bw,vvPoint,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    
    //����ͼ�������������������
    int rectMaxArea =0;
    int rectArea =0;
	vector<Point> areaMatvPoint;
    RotatedRect rMaxRect;
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
        }
    }
	
	if (areaMatvPoint.size()>200)
	{
		Point2f p2f[4];
		rMaxRect.points(p2f);
		Point point1 = p2f[0];
		Point point2 = p2f[0];
		for (int ii = 0;ii<4;ii++)
		{
			if (point1.x>p2f[ii].x)
				point1.x = p2f[ii].x;
			if (point1.y>p2f[ii].y)
				point1.y = p2f[ii].y;
			if (point2.x<p2f[ii].x)
				point2.x = p2f[ii].x;
			if (point2.y<p2f[ii].y)
				point2.y = p2f[ii].y;

		}
		point1.x = point1.x/scale-10;
		point1.y = point1.y/scale-10;
		point2.x = point2.x/scale+20;
		point2.y = point2.y/scale+20;

		if (point1.x<0)
			point1.x = 0;
		if (point1.y<0)
			point1.y = 0;
		

		if (point2.x>src.cols-1)
			point2.x = src.cols-1;
		if (point2.y>src.rows-1)
			point2.y = src.rows-1;

		if (type ==1)
			point1.x = (src.cols-point2.x-1)/6;
		else
			point2.x = src.cols-1-point1.x/6;

		if (point1.x<0)
			point1.x = 0;
		if (point2.x>src.cols-1)
			point2.x = src.cols-1;

		Mat mat_dst = src(Rect(point1.x,point1.y,(point2.x-point1.x),(point2.y-point1.y)));
		dst = mat_dst.clone();

		point = point1;
	}
	else
	{
		dst = src.clone();
	}
	return point;
}