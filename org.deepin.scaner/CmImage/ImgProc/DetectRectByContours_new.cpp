#include "stdafx.h"
#include "DetectRectByContours_new.h"
#include "./BrightnessBalance.h"
#include "newColorEnhance.h"

//???งเ??
//???????:
//[in]src??????
//[out]angle????งา???
//[out]pt?? ????CvPoint pt[4]
//[in]rect?????????
//????????:
//?????????????????
bool CDetectRectByContours_new::DetectRect_scanner(Mat src, float & angle, Point* pt, Rect rect)
{
        if (src.empty())
                return false;

        //if (rect.x<0 || rect.y<0 ||rect.width>src.cols || rect.height>src.rows || src.cols<20 ||src.rows<20)
        //	return false;
        //medianBlur(src,src, 5);
        clock_t start, end;
        start = clock();

        int width = src.cols;
        int height = src.rows;

        bool isOriginalRect  = false;
        if(rect.x < 0 || rect.y < 0 || rect.width < 1 || rect.height < 1 ||
                rect.x > src.cols || rect.y > src.rows || rect.width > src.cols || rect.height > src.rows )
        {
                rect.x = 0;
                rect.y = 0;

                isOriginalRect =true;
        }
        else
        {
                src = src(rect).clone();
                isOriginalRect  = true;
        }

        float fRiao = 1;
        Mat gray = Resize(src,fRiao);

        Mat bw,bw1;
        if (gray.channels()==3)
        {
                vector<cv::Mat> rgbChannels(3);
                split(gray, rgbChannels);
                bw = rgbChannels[0]>65;
                bitwise_or(bw,rgbChannels[1]>65,bw);
                bitwise_or(bw,rgbChannels[2]>65,bw);

                Mat gray1;
                cvtColor(gray,gray1, CV_BGR2GRAY);
                //bitwise_or(bw,gray1<1,bw);
                bw1 = gray1<1;
                //blur(bw1,bw1,Size(3,3));
                //bw1 = bw1>0;
        }
        else
        {
                bw = gray>40;
                bw1 = gray<1;
        }

        vector<Point2f> pts;
        bool type = findMaxConyours_scanner(bw, bw1, pts);

        if (type)
        {
                float width1 = 2*int(norm(pts[0]-pts[1])/2);
                float height1 = 2*int(norm(pts[2]-pts[1])/2);
                if (width1>50 && height1>40)
                {
                        for (int ii = 0;ii<pts.size();ii++)
                        {
                                /*if (ii<=1)
                                {
                                        if(pts[ii].y>2)
                                                pts[ii].y += 3;
                                }
                                else
                                {
                                        if(pts[ii].y<bw.rows-3)
                                                pts[ii].y -= 2;
                                }
                                if (ii==0 ||ii==3)
                                {
                                        if(pts[ii].x>2)
                                                pts[ii].x += 3;
                                }
                                else
                                {
                                        if(pts[ii].x<bw.cols-3)
                                                pts[ii].x -= 2;
                                }
                                */
                                pt[ii].x = pts[ii].x/fRiao + rect.x;
                                pt[ii].y = pts[ii].y/fRiao + rect.y;

                                //pt[ii].x = pt[ii].x < 0? 0:(pt[ii].x > width-1 ? width-1:pt[ii].x);
                                //pt[ii].y = pt[ii].y < 0? 0:(pt[ii].y > height-1 ? height-1:pt[ii].y);
                        }
                        //return true;
                }
                else
                {
                        /*pt[0].x = 0;
                        pt[0].y = 0;
                        pt[1].x = src.cols-1;
                        pt[1].y = 0;
                        pt[2].x = src.cols-1;
                        pt[2].y = src.rows-1;
                        pt[3].x = 0;
                        pt[3].y = src.rows-1;
                        angle = 0;*/
                        return false;
                }
        }
        else
        {
                /*pt[0].x = 0;
                pt[0].y = 0;
                pt[1].x = src.cols-1;
                pt[1].y = 0;
                pt[2].x = src.cols-1;
                pt[2].y = src.rows-1;
                pt[3].x = 0;
                pt[3].y = src.rows-1;
                angle = 0;*/
                return false;
        }

        end = clock();
        float aa = ((double)(end - start) / (CLOCKS_PER_SEC)) * 1000;
        return true;
}

//???งเ??
//???????:
//[in]src??????
//[out]angle????งา???
//[out]pt?? ????CvPoint pt[4]
//[in]rect?????????
//????????:
//?????????????????
bool CDetectRectByContours_new::DetectRect(Mat src, float & angle, Point* pt, Rect rect)
{
        if (src.empty())
                return false;

        //if (rect.x<0 || rect.y<0 ||rect.width>src.cols || rect.height>src.rows || src.cols<20 ||src.rows<20)
        //	return false;
        //medianBlur(src,src, 5);
        clock_t start, end;
        start = clock();

        int width = src.cols;
        int height = src.rows;

        bool isOriginalRect  = false;
        if(rect.x < 0 || rect.y < 0 || rect.width < 1 || rect.height < 1 ||
                rect.x > src.cols || rect.y > src.rows || rect.width > src.cols || rect.height > src.rows )
        {
                rect.x = 0;
                rect.y = 0;

                isOriginalRect =true;
        }
        else
        {
                src = src(rect).clone();
                isOriginalRect  = true;
        }

        float fRiao = 1;
        Mat gray = Resize(src,fRiao);

        Mat bw = im2bw(gray);

        vector<Point2f> pts;
        bool type = findMaxConyours(bw, pts);

        if (type)
        {
                float width1 = 2*int(norm(pts[0]-pts[1])/2);
                float height1 = 2*int(norm(pts[2]-pts[1])/2);
                if (width1>50 && height1>40)
                {
                        for (int ii = 0;ii<pts.size();ii++)
                        {
                                /*if (ii<=1)
                                {
                                        if(pts[ii].y>2)
                                                pts[ii].y += 3;
                                }
                                else
                                {
                                        if(pts[ii].y<bw.rows-3)
                                                pts[ii].y -= 2;
                                }
                                if (ii==0 ||ii==3)
                                {
                                        if(pts[ii].x>2)
                                                pts[ii].x += 3;
                                }
                                else
                                {
                                        if(pts[ii].x<bw.cols-3)
                                                pts[ii].x -= 2;
                                }
                                */
                                pt[ii].x = pts[ii].x/fRiao + rect.x;
                                pt[ii].y = pts[ii].y/fRiao + rect.y;

                                pt[ii].x = pt[ii].x < 0? 0:(pt[ii].x > width-1 ? width-1:pt[ii].x);
                                pt[ii].y = pt[ii].y < 0? 0:(pt[ii].y > height-1 ? height-1:pt[ii].y);
                        }
                        //return true;
                }
                else
                {
                        /*pt[0].x = 0;
                        pt[0].y = 0;
                        pt[1].x = src.cols-1;
                        pt[1].y = 0;
                        pt[2].x = src.cols-1;
                        pt[2].y = src.rows-1;
                        pt[3].x = 0;
                        pt[3].y = src.rows-1;
                        angle = 0;*/
                        return false;
                }
        }
        else
        {
                /*pt[0].x = 0;
                pt[0].y = 0;
                pt[1].x = src.cols-1;
                pt[1].y = 0;
                pt[2].x = src.cols-1;
                pt[2].y = src.rows-1;
                pt[3].x = 0;
                pt[3].y = src.rows-1;
                angle = 0;*/
                return false;
        }

        end = clock();
        float aa = ((double)(end - start) / (CLOCKS_PER_SEC)) * 1000;
        return true;
}

//?๗???????ฆย???
//???????:
//[in]src??????
//[out]angle????งา???
//[out]pt?? ????CvPoint pt[4]
//[in]rect?????????
//????????:
//?????????????????
bool CDetectRectByContours_new::DetectRect_Book(Mat src, float & angle, Point* pt, Rect rect)
{
        if (src.empty())
                return false;

        //if (rect.x<0 || rect.y<0 ||rect.width>src.cols || rect.height>src.rows || src.cols<20 ||src.rows<20)
        //	return false;
        //medianBlur(src,src, 5);


        int width = src.cols;
        int height = src.rows;

        bool isOriginalRect  = false;
        if(rect.x < 0 || rect.y < 0 || rect.width < 1 || rect.height < 1 ||
                rect.x > src.cols || rect.y > src.rows || rect.width > src.cols || rect.height > src.rows )
        {
                rect.x = 0;
                rect.y = 0;

                isOriginalRect =true;
        }
        else
        {
                src = src(rect).clone();
                isOriginalRect  = true;
        }

        float fRiao = 1;
        Mat gray = Resize(src,fRiao);

        Mat bw = im2bw(gray);

        vector<Point2f> pts;
        bool type = findMaxConyours(bw, pts, 2);
        if (type)
        {
                float width1 = 2*int(norm(pts[0]-pts[1])/2);
                float height1 = 2*int(norm(pts[2]-pts[1])/2);
                if (width1>50 && height1>40)
                {
                        for (int ii = 0;ii<pts.size();ii++)
                        {
                                /*if (ii<=1)
                                {
                                        if(pts[ii].y>2)
                                                pts[ii].y += 3;
                                }
                                else
                                {
                                        if(pts[ii].y<bw.rows-3)
                                                pts[ii].y -= 2;
                                }
                                if (ii==0 ||ii==3)
                                {
                                        if(pts[ii].x>2)
                                                pts[ii].x += 3;
                                }
                                else
                                {
                                        if(pts[ii].x<bw.cols-3)
                                                pts[ii].x -= 2;
                                }
                                */
                                pt[ii].x = pts[ii].x/fRiao + rect.x;
                                pt[ii].y = pts[ii].y/fRiao + rect.y;

                                //pt[ii].x = pt[ii].x < 0? 0:(pt[ii].x > width-1 ? width-1:pt[ii].x);
                                //pt[ii].y = pt[ii].y < 0? 0:(pt[ii].y > height-1 ? height-1:pt[ii].y);
                        }
                        //return true;
                }
                else
                {
                        /*pt[0].x = 0;
                        pt[0].y = 0;
                        pt[1].x = src.cols-1;
                        pt[1].y = 0;
                        pt[2].x = src.cols-1;
                        pt[2].y = src.rows-1;
                        pt[3].x = 0;
                        pt[3].y = src.rows-1;
                        angle = 0;*/
                        return false;
                }
        }
        else
        {
                /*pt[0].x = 0;
                pt[0].y = 0;
                pt[1].x = src.cols-1;
                pt[1].y = 0;
                pt[2].x = src.cols-1;
                pt[2].y = src.rows-1;
                pt[3].x = 0;
                pt[3].y = src.rows-1;
                angle = 0;*/
                return false;
        }
        return true;
}

//???งเ??
//???????:
//[in]src??????
//[out]angle????งา???
//[out]pt?? ????CvPoint pt[4]
//[in]rect?????????
//????????:
//?????????????????
bool CDetectRectByContours_new::DetectRect_Multi(Mat src, MRectRArray &mRectArray, Rect rect)
{
        if (src.empty())
                return false;

        int width = src.cols;
        int height = src.rows;

        bool isOriginalRect  = false;
        if(rect.x < 0 || rect.y < 0 || rect.width < 1 || rect.height < 1 ||
                rect.x > src.cols || rect.y > src.rows || rect.width > src.cols || rect.height > src.rows )
        {
                rect.x = 0;
                rect.y = 0;

                isOriginalRect =true;
        }
        else
        {
                src = src(rect).clone();
                isOriginalRect  = true;
        }

        float fRiao = 1;
        Mat gray = Resize(src,fRiao);

        Mat bw = im2bw(gray);

        vector<Point2f> ptss;
        if (findMaxConyours(bw, ptss,1))
        {
                vector<Point2f> pts;
                for (int idx = 0 ;idx<int(int(ptss.size())/4);idx++)
                {
                        pts.clear();
                        pts.push_back(ptss[4*idx]);
                        pts.push_back(ptss[4*idx+1]);
                        pts.push_back(ptss[4*idx+2]);
                        pts.push_back(ptss[4*idx+3]);

                        Point pt[4];

                        float width1 = 2*int(norm(pts[0]-pts[1])/2);
                        float height1 = 2*int(norm(pts[2]-pts[1])/2);
                        if (width1>50 && height1>40)
                        {
                                for (int ii = 0;ii<pts.size();ii++)
                                {
                                        pt[ii].x = pts[ii].x/fRiao + rect.x;
                                        pt[ii].y = pts[ii].y/fRiao + rect.y;

                                        pt[ii].x = pt[ii].x < 0? 0:(pt[ii].x > width-1 ? width-1:pt[ii].x);
                                        pt[ii].y = pt[ii].y < 0? 0:(pt[ii].y > height-1 ? height-1:pt[ii].y);
                                }
                                //return true;
                        }
                        else
                        {
                                continue;
                                pt[0].x = 0;
                                pt[0].y = 0;
                                pt[1].x = src.cols-1;
                                pt[1].y = 0;
                                pt[2].x = src.cols-1;
                                pt[2].y = src.rows-1;
                                pt[3].x = 0;
                                pt[3].y = src.rows-1;

                        }

                        MRectR rrect;
                        for (int idx1=0;idx1 <4;idx1++)
                        {
                                rrect.m_pt[idx1].x = pt[idx1].x;
                                rrect.m_pt[idx1].y = pt[idx1].y;
                        }
                        mRectArray.m_nCount +=1;
                        mRectArray.m_rectR[idx]=rrect;
                }
        }
        else
        {
                return false;
                Point pt[4];
                pt[0].x = 0;
                pt[0].y = 0;
                pt[1].x = src.cols-1;
                pt[1].y = 0;
                pt[2].x = src.cols-1;
                pt[2].y = src.rows-1;
                pt[3].x = 0;
                pt[3].y = src.rows-1;


                MRectR rrect;
                for (int idx1=0;idx1 <4;idx1++)
                {
                        rrect.m_pt[idx1].x = pt[idx1].x;
                        rrect.m_pt[idx1].y = pt[idx1].y;
                }
                mRectArray.m_nCount =1;
                mRectArray.m_rectR[0]=rrect;
        }
        if (mRectArray.m_nCount>0)
                return true;
        else
                return false;
}

bool CDetectRectByContours_new::DetectRect_seg(Mat &src, Point* pt)
{

        vector<Point> pts;

        vector<Point> contour;
    contour.push_back(Point(pt[0].x,pt[0].y));
    contour.push_back(Point(pt[1].x,pt[1].y));
    contour.push_back(Point(pt[2].x,pt[2].y));
    contour.push_back(Point(pt[3].x,pt[3].y));
    RotatedRect rRect;
    rRect = minAreaRect(contour);
        Point2f pt1[4];
        rRect.points(pt1);

        for (int ii = 0;ii<4;ii++)
        {
                pts.push_back(pt1[ii]);
        }

        for (int ii = 0; ii < pts.size() - 1; ii++)
        {
                for (int jj = ii + 1; jj < pts.size(); jj++)
                {
                        if (pts[ii].y > pts[jj].y)
                        {
                                Point temp = pts[ii];
                                pts[ii] = pts[jj];
                                pts[jj] = temp;
                        }
                }
        }
        if (pts[0].x > pts[1].x)
        {
                Point temp = pts[0];
                pts[0] = pts[1];
                pts[1] = temp;
        }
        if (pts[3].x > pts[2].x)
        {
                Point temp = pts[2];
                pts[2] = pts[3];
                pts[3] = temp;
        }

        float width = 2*int(norm(pts[0]-pts[1])/2);
        float height = 2*int(norm(pts[2]-pts[1])/2);

        vector<Point2f> Points1;
        Points1.push_back(Point2f(0,0));
        Points1.push_back(Point2f(width,0));
        Points1.push_back(Point2f(width,height));
        Points1.push_back(Point2f(0,height));


        Mat dst;
        Mat PerspectiveMatrix;
        PerspectiveMatrix = findHomography(pts, Points1);//????????????ศฮ????
        warpPerspective(src, dst ,PerspectiveMatrix, Size(width,height));//???ศฮ

        src = dst.clone();
        return true;

}

Mat CDetectRectByContours_new::Resize(Mat src, float &fRiao)
{
        Mat gray;
        //float fRiao = 1;
        if (min(src.cols,src.rows)>600)
        {
                fRiao = 600 / float(min(src.cols, src.rows));
                /*Size dsize;
                if (src.cols > src.rows)
                {
                        int height = 800;
                        int width = src.cols * 800 / src.rows;
                        dsize = Size(width, height);
                }
                else
                {
                        int width = 800;
                        int height = src.rows * 800 / src.cols;
                        dsize = Size(width, height);
                }*/
                if (fRiao<0.25)
                {
                        resize(src, src, Size(0,0),0.5,0.5,1);
                        resize(src, src, Size(0,0),0.5,0.5,1);
                        resize(src, src, Size(0,0),fRiao/0.25,fRiao/0.25,1);
                }
                else if(fRiao<0.5)
                {
                        resize(src, src, Size(0,0),0.5,0.5,1);
                        resize(src, src, Size(0,0),fRiao/0.5,fRiao/0.5,1);
                }
                else if (fRiao != 1)
                {
                        resize(src, src, Size(0,0),fRiao,fRiao,1);
                }
        }

        //imshow("1",src);
        //waitKey(0);
        return src;
}

Mat CDetectRectByContours_new::im2bw(Mat src)
{
        Mat mask;
        if (src.channels()==3)
        {
                clock_t start, end;
                start = clock();

                medianBlur(src,src, 3);
                blur(src,src,Size(3,3));

                vector<cv::Mat> rgbChannels(3);
                split(src, rgbChannels);

                end = clock();
                float aa7 = ((double)(end - start) / (CLOCKS_PER_SEC)) * 1000;
                start = clock();

                Mat mark;
                double avg_b,avg_g,avg_r;
                vector<float> dst;// =  CnewColorEnhance::findThreshold(src,200);
                Mat maskg;
                if (dst.size()==3)
                {
                        if (dst[0]>dst[1] && dst[0]>dst[2])
                        {
                                avg_b = mean(rgbChannels[0],rgbChannels[0]>100)[0];
                                avg_g = avg_b-2;
                                avg_r = avg_b-2;
                        }
                        else if (dst[1]>dst[0] && dst[1]>dst[2])
                        {
                                avg_g = mean(rgbChannels[1],rgbChannels[1]>100)[0];
                                avg_r = avg_g-2;
                                avg_b = avg_g-2;
                        }
                        else
                        {
                                avg_r = mean(rgbChannels[2],rgbChannels[2]>100)[0];
                                avg_b = avg_r-2;
                                avg_g = avg_r-2;
                        }
                }
                else
                {

                        Mat src1 = src.clone();
                        if(CDetectRectByContours_new::grad(src1,maskg))
                        {
                                cvtColor(src1,src1, CV_BGR2GRAY);
                                mark = src1>80;

                        }
                        else
                        {
                                //rectangle(rgbChannels[0],Rect(0,0,src.cols-1,src.rows-1),Scalar(0),1);
                                //rectangle(rgbChannels[1],Rect(0,0,src.cols-1,src.rows-1),Scalar(0),1);
                                //rectangle(rgbChannels[2],Rect(0,0,src.cols-1,src.rows-1),Scalar(0),1);
                                mark = rgbChannels[0]>80;
                                bitwise_or(mark,rgbChannels[1]>80,mark);
                                bitwise_or(mark,rgbChannels[2]>80,mark);

                                avg_b = mean(rgbChannels[0],mark)[0];
                                avg_g = mean(rgbChannels[1],mark)[0];
                                avg_r = mean(rgbChannels[2],mark)[0];

                                mark = rgbChannels[0]>0.8*avg_b;
                                bitwise_or(mark,rgbChannels[1]>0.8*avg_g,mark);
                                bitwise_or(mark,rgbChannels[2]>0.8*avg_r,mark);
                        }


                        avg_b = mean(rgbChannels[0],mark)[0];
                        avg_g = mean(rgbChannels[1],mark)[0];
                        avg_r = mean(rgbChannels[2],mark)[0];
                }

                end = clock();
                float aa0 = ((double)(end - start) / (CLOCKS_PER_SEC)) * 1000;
                start = clock();

                if ((avg_b<avg_r || avg_b<avg_g))// && avg_b<180)
                {
                        Mat mask51;
                        Mat temp = rgbChannels[0].clone();
                        CDetectRectByContours_new::grad(temp,mask51);
                        bitwise_or(mask51,maskg,maskg);
                        temp = rgbChannels[2].clone();
                        CDetectRectByContours_new::grad(temp,mask51);
                        bitwise_or(mask51,maskg,maskg);
                }

                end = clock();
                float aa2 = ((double)(end - start) / (CLOCKS_PER_SEC)) * 1000;
                start = clock();

                Rect cRect(src.cols/4,src.rows/4,src.cols/2,src.rows/2);
                double avg = 0;
                Mat mask5;
                if (1)
                {
                        Mat gray;
                        Mat bw;
                        int thresh;
                        if (avg_b>=avg_g && avg_b>=avg_r)
                        {
                                gray = rgbChannels[0];
                                bool tt = CDetectRectByContours_new::grad(gray,mask5);
                                if (tt)
                                        avg_b = avg_b>mean(gray,gray>80)[0]?avg_b:mean(gray,gray>80)[0];
                                double ak = max(80.0,0.75*avg_b);
                                thresh = ak;
                                bw = rgbChannels[0]>ak;
                        }
                        else if(avg_g>=avg_b && avg_g>=avg_r)
                        {
                                gray = rgbChannels[1];
                                bool tt = CDetectRectByContours_new::grad(gray,mask5);
                                if (tt)
                                        avg_g = avg_g>mean(gray,gray>80)[0]?avg_g:mean(gray,gray>80)[0];
                                double ak = max(60.0,0.75*avg_g);
                                thresh = ak;
                                bw = rgbChannels[1]>ak;
                        }
                        else
                        {
                                gray = rgbChannels[2];
                                bool tt = CDetectRectByContours_new::grad(gray,mask5);
                                if (tt)
                                        avg_r = avg_r>mean(gray,gray>80)[0]?avg_r:mean(gray,gray>80)[0];
                                double ak = max(60.0,0.75*avg_r);
                                thresh = ak;
                                bw = rgbChannels[2]>ak;

                        }




                        vector<Point> areaMatvPoint;
                        //???????????vvPoint?????????????
                        vector<vector<Point>> vvPoint;
                        findContours(bw.clone(),vvPoint,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);

                        if (vvPoint.size()>0)
                        {
                                //???????????????????????????
                                bool errMsg = false;
                                int rectMaxArea =800;
                                int rectArea =0;
                                RotatedRect rMaxRect;
                                for (vector<vector<Point>>::iterator itr=vvPoint.begin();itr!=vvPoint.end();itr++)
                                {
                                        RotatedRect rrect =minAreaRect(*itr);
                                        rectArea =rrect.size.area();
                                        if( rectArea >rectMaxArea && (rrect.size.width>100 && rrect.size.height>100))
                                        {

                                                Rect rRect(rrect.center.x-rrect.size.width/2,rrect.center.y-rrect.size.height/2,rrect.size.width,rrect.size.height);
                                                int deta_x = 0-rRect.x>0?0-rRect.x:0;
                                                int deta_y = 0-rRect.y>0?0-rRect.y:0;

                                                rRect.x = rRect.x<0?0:rRect.x;
                                                rRect.y = rRect.y<0?0:rRect.y;
                                                rRect.width = rRect.x+rRect.width>gray.cols-1?gray.cols-1-rRect.x-deta_x:rRect.width-deta_x;
                                                rRect.height = rRect.y+rRect.height>gray.rows-1?gray.rows-1-rRect.y-deta_y:rRect.height-deta_y;
                                                if (rRect.x<0)
                                                        rRect.x = 0;
                                                if (rRect.y<0)
                                                        rRect.y = 0;
                                                if (rRect.x+rRect.width>gray.cols)
                                                        rRect.width = gray.cols-rRect.x;
                                                if (rRect.y+rRect.height>gray.rows)
                                                        rRect.height = gray.rows-rRect.y;
                                                double avg1 = mean(gray(rRect),mark(rRect)>100)[0];
                                                if (avg1>avg)
                                                {
                                                        errMsg = true;
                                                        avg = avg1;
                                                        rMaxRect =rrect;
                                                        //rectMaxArea = rectArea;
                                                        areaMatvPoint.clear();
                                                        areaMatvPoint = *itr;
                                                }
                                        }
                                }
                                if (errMsg)
                                {
                                        Point2f pt[4];
                                        rMaxRect.points(pt);
                                        int x2 = 0;
                                        int y2 = 0;
                                        int x1 = src.cols;
                                        int y1 = src.rows;
                                        for (int ii = 0;ii<4;ii++)
                                        {
                                                if (pt[ii].x<x1)
                                                        x1 = pt[ii].x;
                                                if (pt[ii].y<y1)
                                                        y1 = pt[ii].y;
                                                if (pt[ii].x>x2)
                                                        x2 = pt[ii].x;
                                                if (pt[ii].y>y2)
                                                        y2 = pt[ii].y;
                                        }
                                        if (y1<0)
                                                y1 = 0;
                                        if(x1<0)
                                                x1 = 0;
                                        if (y2>src.rows-1)
                                                y2 = src.rows-1;
                                        if (x2>src.cols-1)
                                                x2 = src.cols-1;
                                        cRect.x = x1;
                                        cRect.y = y1;
                                        cRect.width = x2-cRect.x;
                                        cRect.height = y2-cRect.y;
                                }
                        }
                }


                end = clock();
                float aa3 = ((double)(end - start) / (CLOCKS_PER_SEC)) * 1000;
                start = clock();

                Mat mask = Mat::zeros(src.size(),CV_8UC1);



                Mat mm = mask.clone();
                Mat mm1 = Mat::ones(Size(cRect.width,cRect.height),CV_8UC1)*255;
                mm1.copyTo(mm(cRect));

                Mat src1 = src.clone();
                src1.setTo(0,mm);
                vector<cv::Mat> hsvChannels(3);
                split(src1, hsvChannels);
                Point pt_B = CBrightnessBalance::findMaxThreshold1(hsvChannels[0],0.01,0.01);
                Point pt_G = CBrightnessBalance::findMaxThreshold1(hsvChannels[1],0.01,0.01);
                Point pt_R = CBrightnessBalance::findMaxThreshold1(hsvChannels[2],0.01,0.01);
                double ba = 1-mean(mm)[0]/255;

                vector<double> aa;
                vector<double> aa1;
                for(int ii = 0;ii<3;ii++)
                {
                        double aa_1 = mean(rgbChannels[ii],mm)[0];
                        double aa_2 = mean(rgbChannels[ii],~mm)[0];
                        aa.push_back(aa_1);
                        aa1.push_back(aa_2);
                }
                double bb1 = max(pt_B.x,max(pt_G.x,pt_R.x));


                double aa_2 = avg_b;//aa[0];

                if (bb1<20 || ba<0.05)
                {
                        if (aa_2<128)
                                aa_2 = max(35.0,aa1[0]*1.5);
                        aa_2 = max(80.0,0.9*aa_2);
                }
                else
                {
                        aa_2 =max(80.0,0.9*aa_2);
                }
                Mat bw2 = rgbChannels[0]>aa_2;
                if (1)
                {
                        double aa_3 = avg_g;//aa[0];
                        if (bb1<20 || ba<0.05)
                        {
                                if (aa_3<128)
                                        aa_3 = max(35.0,aa1[0]*1.5);
                                aa_3 = max(80.0,0.9*aa_3);
                        }
                        else
                        {
                                aa_3 = max(80.0,0.9*aa_3);
                        }
                        Mat bw = rgbChannels[1]>aa_3;
                        bitwise_and(bw, bw2, bw2);

                        aa_3 = avg_r;//aa[0];
                        if (bb1<20 || ba<0.05)
                        {
                                if (aa_3<128)
                                        aa_3 = max(35.0,aa1[0]*1.5);
                                aa_3 = max(80.0,0.9*aa_3);
                        }
                        else
                        {
                                aa_3 = max(80.0,0.9*aa_3);
                        }
                        bw = rgbChannels[1]>aa_3;
                        bitwise_and(bw, bw2, bw2);
                }

                if (avg_b>=avg_g && avg_b>=avg_r)
                {
                        /*double aa_3 = avg;//aa[0];
                        if (bb1<20 || ba<0.05)
                        {
                                aa_3 = max(35.0,aa1[0]*1.5);
                                aa_3 = min(130.0,0.95*aa_3);
                        }
                        else
                        {
                                aa_3 = max(130.0,0.95*aa_3);
                        }
                        bw2 = rgbChannels[0]>aa_3;*/

                        Mat gray = rgbChannels[0];

                        Mat tt;
                        blur(gray,tt,Size(20,1));
                        Mat bw1 = gray>1.1*tt;
                        bitwise_or(mask, bw1, mask);

                        blur(gray,tt,Size(1,20));
                        Mat bw = gray>1.1*tt;

                        //bitwise_or(temp1, temp2, mask);
                        bitwise_or(mask, bw, mask);
                        bitwise_and(mask, gray>80, mask);
                        bitwise_or(mask, bw2, mask);
                }
                else if (avg_g>=avg_b && avg_g>=avg_r)
                {
                        /*double aa_3 = avg;//aa[1];
                        if (bb1<20 || ba<0.05)
                        {
                                aa_3 = max(35.0,aa1[1]*1.5);
                                aa_3 = min(130.0,0.95*aa_3);
                        }
                        else
                        {
                                aa_3 = max(130.0,0.95*aa_3);
                        }
                        bw2 = rgbChannels[1]>aa_3;*/

                        Mat gray = rgbChannels[1];

                        Mat tt;
                        blur(gray,tt,Size(20,1));
                        Mat bw1 = gray>1.1*tt;
                        bitwise_or(mask, bw1, mask);

                        blur(gray,tt,Size(1,20));
                        Mat bw = gray>1.1*tt;

                        //bitwise_or(temp1, temp2, mask);
                        bitwise_or(mask, bw, mask);
                        bitwise_and(mask, gray>80, mask);
                        bitwise_or(mask, bw2, mask);
                }
                else
                {
                        /*double aa_3 = avg;//aa[2];
                        if (bb1<20 || ba<0.05)
                        {
                                aa_3 = max(35.0,aa1[2]*1.5);
                                aa_3 = min(130.0,0.95*aa_3);
                        }
                        else
                        {
                                aa_3 = max(130.0,0.95*aa_3);
                        }
                        bw2 = rgbChannels[2]>aa_3;*/

                        Mat gray = rgbChannels[2];

                        Mat tt;
                        blur(gray,tt,Size(20,1));
                        Mat bw1 = gray>1.1*tt;
                        bitwise_or(mask, bw1, mask);

                        blur(gray,tt,Size(1,20));
                        Mat bw = gray>1.1*tt;

                        //bitwise_or(temp1, temp2, mask);
                        bitwise_or(mask, bw, mask);
                        bitwise_and(mask, gray>80, mask);
                        bitwise_or(mask, bw2, mask);
                }


                Mat bw;
                if (0)
                {
                        if (avg_b>=avg_g && avg_b>=avg_r)
                        {
                                Mat gray = rgbChannels[0];
                                double ak = aa[0];
                                double aa_1 = aa1[0];
                                if (bb1<20 || ba<0.05)
                                {
                                        ak = max(35.0,aa_1*1.5);
                                }
                                else
                                        ak = max(80.0,0.8*ak);
                                bw = rgbChannels[0]>ak;
                        }
                        else if(avg_g>=avg_b && avg_g>=avg_r)
                        {
                                Mat gray = rgbChannels[1];
                                double ak = aa[1];
                                double aa_1 = aa1[1];
                                if (bb1<20 || ba<0.05)
                                {
                                        ak = max(35.0,aa_1*1.5);
                                }
                                else
                                        ak = max(80.0,0.8*ak);
                                bw = rgbChannels[1]>ak;
                        }
                        else
                        {
                                Mat gray = rgbChannels[2];
                                double ak = aa[2];
                                double aa_1 = aa1[2];
                                if (bb1<20 || ba<0.05)
                                {
                                        ak = max(35.0,aa_1*1.5);
                                }
                                else
                                        ak = max(80.0,0.8*ak);
                                bw = rgbChannels[2]>ak;
                        }
                }
                else
                {
                        double ak = max(80.0,0.65*avg);
                        bw = rgbChannels[0]>ak;
                        for(int ii = 1;ii<3;ii++)
                        {
                                Mat bw1 = rgbChannels[ii]>ak;
                                bitwise_or(bw1,bw,bw);
                        }
                }

                end = clock();
                float aa4 = ((double)(end - start) / (CLOCKS_PER_SEC)) * 1000;
                start = clock();

                //imshow("2",mask);
                bitwise_and(bw, mask, mask);

                //imshow("4",mask);
                bitwise_or(maskg, mask5, maskg);
                double ak = min(80.0,max(30.0,0.7*mean(rgbChannels[0])[0]));
                bw = rgbChannels[0]>ak;
                for(int ii = 1;ii<3;ii++)
                {
                        ak = min(80.0,max(30.0,0.7*mean(rgbChannels[ii])[0]));
                        Mat bw1 = rgbChannels[ii]>ak;
                        bitwise_or(bw1,bw,bw);
                }
                //Mat element = getStructuringElement(MORPH_RECT,Size(2,2));
                //erode(bw,bw,element);
                bitwise_and(bw, maskg, maskg);

                bitwise_or(maskg, mask, mask);

                end = clock();
                float aa5 = ((double)(end - start) / (CLOCKS_PER_SEC)) * 1000;
                start = clock();

                double men = mean(maskg)[0];
                vector<vector<Point> > vvPoint;
                findContours(maskg.clone(),vvPoint,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
                if (vvPoint.size()>0)
                {
                        //???????????????????????????
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
                        float Area = rMaxRect.size.area();
                        float men1 = mask.cols*mask.rows*men/pow(max(rMaxRect.size.width,rMaxRect.size.height),2);
                        if(!(((rMaxRect.size.width>70 || rMaxRect.size.height>70) && (men1>10 || men>2)) || men>15))
                        {
                                return Mat::zeros(mask.size(),mask.type());
                        }
                }
                else
                {
                        return Mat::zeros(mask.size(),mask.type());
                }
                //imshow("1",mask);
                //waitKey(0);

                end = clock();
                float aa6 = ((double)(end - start) / (CLOCKS_PER_SEC)) * 1000;
                start = clock();

                return mask;
        }

        if (src.channels()==3)
        {
                Mat src_hsv;
                //cvtColor(src,src_hsv, CV_BGR2HSV);
                vector<cv::Mat> rgbChannels(3);
                vector<cv::Mat> rgbratio(3);
                //split(src_hsv, rgbChannels);
                split(src, rgbChannels);

                //double avg1 = mean(rgbChannels[1])[0];
                mask = rgbChannels[2]>80;//0.45*255;
                Mat bw = rgbChannels[1]>80;
                bitwise_and(bw,mask,mask);
                bw = rgbChannels[0]>80;
                bitwise_and(bw,mask,mask);

                for(int ii = 0;ii<3;ii++)
                {
                        bw = rgbChannels[ii]>120;
                        bitwise_or(mask,bw,mask);
                }

                cvtColor(src,src, CV_BGR2GRAY);
                bw = src>90;
                bitwise_or(bw,mask,mask);

        }
        else
        {
                mask = src>80;
        }
        return mask;
}

bool CDetectRectByContours_new::grad(Mat &src ,Mat &mask)
{
        if (src.empty())
                return false;

        Mat gray = src.clone();
        if (src.channels()==3)
        {
                //vector<cv::Mat> rgbChannels(3);
                //split(src, rgbChannels);
                //gray = rgbChannels[0].mul(0.34)+rgbChannels[1].mul(0.33)+rgbChannels[2].mul(0.33);
                //addWeighted(rgbChannels[0], 0.33, rgbChannels[1], 0.33, 0, gray);
                cvtColor(src,gray, CV_BGR2GRAY);
        }
        int scale = 1;
        int delta = 0;
        int ddepth = CV_16S;
        Mat grad;
        if (1)
        {
                Mat abs_grad_x, abs_grad_y;
                /// ?? X???????
                //Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
                Sobel(gray, grad, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
                convertScaleAbs(grad, abs_grad_x);

                /// ??Y???????
                //Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
                Sobel(gray, grad, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
                convertScaleAbs(grad, abs_grad_y);

                /// ??????(????)
                addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);
        }
        double aaa = mean(grad)[0];
        double avg = (mean(grad)[0]*2>60?mean(grad)[0]*2:60);
        mask = grad>avg;

        vector<vector<Point>> vvPoint;
        vector<vector<Point>> vvPoint1;
        Rect cRect;
        int x1 = mask.cols;
        int x2 = 0;
        int y1 = mask.rows;
        int y2 = 0;
        findContours(mask.clone(),vvPoint,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
        if (vvPoint.size()>0)
        {
                bool errMsg = false;
                //???????????????????????????
                int rectMaxArea =400;
                int rectArea =0;
                RotatedRect rMaxRect;
                for (vector<vector<Point>>::iterator itr=vvPoint.begin();itr!=vvPoint.end();itr++)
                {
                        RotatedRect rrect =minAreaRect(*itr);
                        rectArea =rrect.size.area();
                        int Area = contourArea(*itr);
                        if( rectArea >400)
                        {
                                if (rrect.size.width>70 || rrect.size.height>70)
                                {
                                        Point2f pt[4];
                                        Point2f pf(0,0);
                                        rrect.points(pt);
                                        for (int ii = 0;ii<4;ii++)
                                        {
                                                if (pt[ii].x<x1)
                                                        x1 = pt[ii].x;
                                                if (pt[ii].x>x2)
                                                        x2 = pt[ii].x;
                                                if (pt[ii].y<y1)
                                                        y1 = pt[ii].y;
                                                if (pt[ii].y>y2)
                                                        y2 = pt[ii].y;
                                        }
                                }

                        }
                }
                if ((x2<x1 || y2<y1) ||( x2-x1<100 || y2-y1<100))
                        return false;
                //x1 -= 10;
                //y1 -= 10;
                //x2 += 10;
                //y2 += 10;
                if (x1<0)
                        x1 = 0;
                if (y1<0)
                        y1 = 0;
                if (x2>mask.cols)
                        x2 = mask.cols;
                if (y2 >mask.rows)
                        y2 = mask.rows;
                cRect = Rect(x1,y1,x2-x1,y2-y1);
        }
        else
        {
                return false;
        }

        Mat temp = Mat::zeros(src.size(),src.type());
        Mat tmp = src(cRect);
        tmp.copyTo(temp(cRect));
        src = temp.clone();
        //imshow("11",src);
        //imshow("12",mask);
        //waitKey(0);
        return true;
}


bool CDetectRectByContours_new::findMaxConyours_scanner(Mat bw, Mat bw1, vector<Point2f> &PT4)
{
        if (bw.empty())
                return false;


        vector<Point> areaMatvPoint;
        //???????????vvPoint?????????????
        vector<vector<Point>> vvPoint;
        findContours(bw.clone(),vvPoint,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
        if (vvPoint.size()>0)
        {
                bool errMsg = false;
                //???????????????????????????
                int rectMaxArea =0;
                int rectArea =0;
                RotatedRect rMaxRect;
                vector<Point> areaMatvPoint1;
                vector<Point> areaMatvPoint2;
                for (vector<vector<Point>>::iterator itr=vvPoint.begin();itr!=vvPoint.end()-1;itr++)
                {
                        RotatedRect rrect =minAreaRect(*itr);
                        areaMatvPoint1 =*itr;
                        rectArea =rrect.size.area();
                        if( rectArea < 200)
                                continue;
                        for (vector<vector<Point>>::iterator itr1=vvPoint.begin()+1;itr1!=vvPoint.end();itr1++)
                        {
                                RotatedRect rrect1 =minAreaRect(*itr1);
                                areaMatvPoint2 = *itr1;
                                int rectArea1 =rrect1.size.area();
                                if( rectArea1 < 200)
                                        continue;
                                line(bw,areaMatvPoint1[0],areaMatvPoint2[0],Scalar(255),2);
                        }
                }


                vvPoint.clear();
                findContours(bw.clone(),vvPoint,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
                if (vvPoint.size()>0)
                {
                        for (vector<vector<Point>>::iterator itr=vvPoint.begin();itr!=vvPoint.end();itr++)
                        {
                                RotatedRect rrect =minAreaRect(*itr);
                                rectArea =rrect.size.area();
                                if (rectArea>rectMaxArea)
                                {
                                        rectMaxArea = rectArea;
                                        areaMatvPoint = *itr;
                                        rMaxRect = rrect;
                                }
                        }
                        if (rectMaxArea>1000)
                        {
                                Point2f pt[4];
                                Point2f pf(0,0);
                                rMaxRect.center.x += 0.5;
                                rMaxRect.center.y += 0.5;
                                rMaxRect.points(pt);
                                float min_dist = 20000;
                                int idx = 0;
                                for (int jj = 0;jj<4;jj++)
                                {
                                        float dist = norm(pf-pt[jj]);
                                        if (dist<min_dist)
                                        {
                                                min_dist = dist;
                                                idx = jj;
                                        }
                                }

                                vector<Point2f> pts;
                                for (int jj = 0;jj<4;jj++)
                                {
                                        PT4.push_back(pt[(jj+idx)%4]);
                                }

                                if (rMaxRect.size.area()/(bw.cols*bw.rows)>0.95 || (PT4[0].x<2 && PT4[1].x>bw.cols-3) || (PT4[0].y<2 && PT4[3].y>bw.rows-3))
                                {
                                        float avg = mean(bw1)[0];
                                        if (avg>0.01)
                                        {
                                                PT4[0] = Point2f(0,0);
                                                PT4[1] = Point2f(bw.cols-1,0);
                                                PT4[2] = Point2f(bw.cols-1,bw.rows-1);
                                                PT4[3] = Point2f(0,bw.rows-1);
                                                PT4.clear();
                                                return false;
                                        }
                                        //bitwise_or(bw,bw1,bw);
                                        //imshow("1",bw1);
                                        //waitKey(0);
                                        vvPoint.clear();
                                        rectMaxArea = 0;
                                        findContours(bw.clone(),vvPoint,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
                                        if (vvPoint.size()>0)
                                        {
                                                for (vector<vector<Point>>::iterator itr=vvPoint.begin();itr!=vvPoint.end();itr++)
                                                {
                                                        RotatedRect rrect =minAreaRect(*itr);
                                                        rectArea =rrect.size.area();
                                                        if (rectArea>rectMaxArea)
                                                        {
                                                                rectMaxArea = rectArea;
                                                                areaMatvPoint = *itr;
                                                                rMaxRect = rrect;
                                                        }
                                                }
                                        }

                                        vector<int> index;
                                        vector<Point> pts;
                                        pts.push_back(Point(-10,-10));
                                        pts.push_back(Point(bw.cols+10,-10));
                                        pts.push_back(Point(bw.cols+10,bw.rows+10));
                                        pts.push_back(Point(-10,bw.rows+10));
                                        for(int jj = 0;jj<4;jj++)
                                        {
                                                float min_dist = 2000;
                                                for(int ii = 0;ii<areaMatvPoint.size();ii++)
                                                {
                                                        float dist = norm(areaMatvPoint[ii]-pts[jj]);
                                                        if (min_dist>dist)
                                                        {
                                                                min_dist = dist;
                                                                idx = ii;
                                                        }
                                                }
                                                index.push_back(idx);
                                                //PT4[jj] = areaMatvPoint[idx];
                                        }

                                        float KK = 0;
                                        float count = 0;
                                        vector<vector<Point>> PTs;
                                        int list[4] = {0,0,0,0};
                                        vector<int> Idex(list, list+4);
                                        for(int jj = 0;jj<4;jj++)
                                        {
                                                vector<Point> pts1;
                                                if(index[jj]>index[(jj+1)%4])
                                                {
                                                        for(int ii = index[(jj+1)%4];ii<index[jj];ii++)
                                                        {
                                                                pts1.push_back(areaMatvPoint[ii]);
                                                        }
                                                }
                                                else
                                                {
                                                        for(int ii = index[(jj+1)%4];ii<areaMatvPoint.size();ii++)
                                                        {
                                                                pts1.push_back(areaMatvPoint[ii]);
                                                        }
                                                        for(int ii = 0;ii<index[jj];ii++)
                                                        {
                                                                pts1.push_back(areaMatvPoint[ii]);
                                                        }
                                                }
                                                PTs.push_back(pts1);

                                                if(jj==0)
                                                {
                                                        Point Pt2 = pts1[100];
                                                        Point Pt1 = pts1[pts1.size()-100];
                                                        bool type = true;
                                                        if (pts1[100].y<2 && pts1[pts1.size()-100].y<2)
                                                                continue;
                                                        if(pts1[100].y>pts1[pts1.size()-100].y)
                                                        {
                                                                //Pt2 = pts1[100];
                                                                for(int ii = 100;ii<pts1.size()-100;ii++)
                                                                {
                                                                        if (pts1[ii].y>1 && pts1[ii+1].y == 1)
                                                                        {
                                                                                Pt1 = pts1[ii+1];
                                                                                continue;
                                                                        }
                                                                }
                                                        }
                                                        else
                                                        {
                                                                //Pt1 = pts1[pts1.size()-100];
                                                                for(int ii = pts1.size()-100;ii>=100;ii--)
                                                                {
                                                                        if (pts1[ii].y>1 && pts1[ii-1].y == 1)
                                                                        {
                                                                                Pt2 = pts1[ii-1];
                                                                                continue;
                                                                        }
                                                                }
                                                        }
                                                        if (abs(Pt2.x-Pt1.x)<30 || abs(Pt2.y-Pt1.y)==1)
                                                                continue;
                                                        float K = float(Pt2.y-Pt1.y)/float(Pt2.x-Pt1.x);
                                                        KK += K;
                                                        count += 1;
                                                        Idex[0] = -1;
                                                }
                                                else if(jj==1)
                                                {
                                                        Point Pt2 = pts1[100];
                                                        Point Pt1 = pts1[pts1.size()-100];
                                                        bool type = true;
                                                        if (pts1[100].x>bw.cols-3 && pts1[pts1.size()-100].x>bw.cols-3)
                                                                continue;
                                                        if(pts1[100].x<pts1[pts1.size()-100].x)
                                                        {
                                                                //Pt2 = pts1[100];
                                                                for(int ii = 100;ii<pts1.size()-100;ii++)
                                                                {
                                                                        if (pts1[ii].x<bw.cols-2 && pts1[ii+1].x == bw.cols-2)
                                                                        {
                                                                                Pt1 = pts1[ii+1];
                                                                                continue;
                                                                        }
                                                                }
                                                        }
                                                        else
                                                        {
                                                                //Pt1 = pts1[pts1.size()-100];
                                                                for(int ii = pts1.size()-100;ii>=100;ii--)
                                                                {
                                                                        if (pts1[ii].x<bw.cols-2 && pts1[ii-1].x == bw.cols-2)
                                                                        {
                                                                                Pt2 = pts1[ii-1];
                                                                                continue;
                                                                        }
                                                                }
                                                        }
                                                        if (abs(Pt2.y-Pt1.y)<30 || abs(Pt2.x-Pt1.x)==1)
                                                                continue;
                                                        float K = float(Pt2.x-Pt1.x)/float(Pt2.y-Pt1.y);
                                                        if (count==0)
                                                        {
                                                                KK -= K;
                                                                count += 1;
                                                                Idex[1] = -1;
                                                        }
                                                        else
                                                        {
                                                                if (abs(-K-KK/count)<KK/count)
                                                                {
                                                                        KK -= K;
                                                                        count += 1;
                                                                        Idex[1] = -1;
                                                                }
                                                        }
                                                }
                                                else if(jj==2)
                                                {
                                                        Point Pt2 = pts1[100];
                                                        Point Pt1 = pts1[pts1.size()-100];
                                                        bool type = true;
                                                        if (pts1[100].y>bw.rows-3 && pts1[pts1.size()-100].y>bw.rows-3)
                                                                continue;
                                                        if(pts1[100].y<pts1[pts1.size()-100].y)
                                                        {
                                                                //Pt2 = pts1[100];
                                                                for(int ii = 100;ii<pts1.size()-100;ii++)
                                                                {
                                                                        if (pts1[ii].y<bw.rows-2 && pts1[ii+1].y == bw.rows-2)
                                                                        {
                                                                                Pt1 = pts1[ii+1];
                                                                                continue;
                                                                        }
                                                                }
                                                        }
                                                        else
                                                        {
                                                                //Pt1 = pts1[pts1.size()-100];
                                                                for(int ii = pts1.size()-100;ii>=100;ii--)
                                                                {
                                                                        if (pts1[ii].y<bw.rows-2 && pts1[ii-1].y == bw.rows-2)
                                                                        {
                                                                                Pt2 = pts1[ii-1];
                                                                                continue;
                                                                        }
                                                                }
                                                        }
                                                        if (abs(Pt2.x-Pt1.x)<30 || abs(Pt2.y-Pt1.y)==1)
                                                                continue;
                                                        float K = float(Pt2.y-Pt1.y)/float(Pt2.x-Pt1.x);
                                                        if (count==0)
                                                        {
                                                                KK += K;
                                                                count += 1;
                                                                Idex[2] = -1;
                                                        }
                                                        else
                                                        {
                                                                if (abs(K-KK/count)<KK/count)
                                                                {
                                                                        KK += K;
                                                                        count += 1;
                                                                        Idex[2] = -1;
                                                                }
                                                        }
                                                }
                                                else if(jj==3)
                                                {
                                                        Point Pt2 = pts1[100];
                                                        Point Pt1 = pts1[pts1.size()-100];
                                                        bool type = true;
                                                        if (pts1[100].x<2 && pts1[pts1.size()-100].x<2)
                                                                continue;
                                                        if(pts1[100].x>pts1[pts1.size()-100].x)
                                                        {
                                                                //Pt2 = pts1[100];
                                                                for(int ii = 100;ii<pts1.size()-100;ii++)
                                                                {
                                                                        if (pts1[ii].x>1 && pts1[ii+1].x == 1)
                                                                        {
                                                                                Pt1 = pts1[ii+1];
                                                                                continue;
                                                                        }
                                                                }
                                                        }
                                                        else
                                                        {
                                                                //Pt1 = pts1[pts1.size()-100];
                                                                for(int ii = pts1.size()-100;ii>=100;ii--)
                                                                {
                                                                        if (pts1[ii].x>1 && pts1[ii-1].x == 1)
                                                                        {
                                                                                Pt2 = pts1[ii-1];
                                                                                continue;
                                                                        }
                                                                }
                                                        }
                                                        if (abs(Pt2.y-Pt1.y)<30 || abs(Pt2.x-Pt1.x)==1)
                                                                continue;
                                                        float K = float(Pt2.x-Pt1.x)/float(Pt2.y-Pt1.y);
                                                        if (count==0)
                                                        {
                                                                KK -= K;
                                                                count += 1;
                                                                Idex[3] = -1;
                                                        }
                                                        else
                                                        {
                                                                if (abs(-K-KK/count)<KK/count)
                                                                {
                                                                        KK -= K;
                                                                        count += 1;
                                                                        Idex[3] = -1;
                                                                }
                                                        }
                                                }

                                        }
                                        if (count>0)
                                        {
                                                KK = KK/count;
                                                float angle= (180.0/3.1415)*atan(KK);
                                                if (angle> 2 && count<3)
                                                        return true;
                                                cv::Point2f center = cv::Point2f(static_cast<float>(bw.cols / 2),
                                                        static_cast<float>(bw.rows / 2));
                                                //???????????????2f??????float?????????X??Y?????????????????
                                                cv::Mat affineTrans = getRotationMatrix2D(center, angle, 1.0);
                                                //getRotationMatrix2D???????????????????????center???????????????????angle???????
                                                //??????1.0??????double scale?????????????
                                                Mat dst;
                                                cv::warpAffine(bw.clone(),dst,affineTrans,bw.size(),cv::INTER_CUBIC);//,cv::BORDER_REPLICATE,Scalar(0));
                                                /*imshow("dst",dst);
                                                imshow("bw",bw);
                                                waitKey(0);*/
                                                rectMaxArea = 0;
                                                vvPoint.clear();
                                                findContours(dst.clone(),vvPoint,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
                                                if (vvPoint.size()>0)
                                                {
                                                        for (vector<vector<Point>>::iterator itr=vvPoint.begin();itr!=vvPoint.end();itr++)
                                                        {
                                                                RotatedRect rrect =minAreaRect(*itr);
                                                                rectArea =rrect.size.area();
                                                                if (rectArea>rectMaxArea)
                                                                {
                                                                        rectMaxArea = rectArea;
                                                                        areaMatvPoint = *itr;
                                                                        rMaxRect = rrect;
                                                                }
                                                        }
                                                        if (rectMaxArea>1000)
                                                        {
                                                                Point2f pt[4];
                                                                Point2f pf(0,0);
                                                                rMaxRect.center.x += 1;
                                                                rMaxRect.center.y += 1;
                                                                rMaxRect.size.width -=2;
                                                                rMaxRect.size.height -=2;
                                                                rMaxRect.points(pt);
                                                                float min_dist = 20000;
                                                                int idx = 0;
                                                                for (int jj = 0;jj<4;jj++)
                                                                {
                                                                        float dist = norm(pf-pt[jj]);
                                                                        if (dist<min_dist)
                                                                        {
                                                                                min_dist = dist;
                                                                                idx = jj;
                                                                        }
                                                                }
                                                                PT4.clear();
                                                                angle = -(3.14*angle) / 180;
                                                                for (int jj = 0; jj < 4; jj++)
                                                                {

                                                                        float x = pt[(jj+idx)%4].x - rMaxRect.center.x;
                                                                        float y = pt[(jj+idx)%4].y - rMaxRect.center.y;
                                                                        pt[(jj+idx)%4].x = x * cos(angle) + y * sin(angle) + rMaxRect.center.x;
                                                                        pt[(jj+idx)%4].y = -x * sin(angle) + y * cos(angle) + rMaxRect.center.y;
                                                                }


                                                                for(int jj = 0; jj < 4; jj++)
                                                                {
                                                                        vector<Point> points = PTs[jj];
                                                                        float K = float(pt[(jj+idx)%4].y-pt[(jj+idx+1)%4].y)/float(pt[(jj+idx)%4].x-pt[(jj+idx+1)%4].x+0.000001);
                                                                        float B = pt[(jj+idx)%4].y-pt[(jj+idx)%4].x*K;
                                                                        if (jj==0)
                                                                        {
                                                                                float max_y = 0;
                                                                                for(int ii = 50;ii<points.size()-50;ii++)
                                                                                {
                                                                                        float yy = K*points[ii].x+B;
                                                                                        float deta_yy = yy-points[ii].y;
                                                                                        if (max_y<deta_yy)
                                                                                        {
                                                                                                max_y = deta_yy;
                                                                                        }
                                                                                }
                                                                                pt[(jj+idx)%4].y -= max_y;
                                                                                pt[(jj+idx+1)%4].y -= max_y;
                                                                        }
                                                                        else if (jj==1)
                                                                        {
                                                                                float max_x = 0;
                                                                                for(int ii = 50;ii<points.size()-50;ii++)
                                                                                {
                                                                                        float xx = (points[ii].y-B)/K;
                                                                                        float deta_xx = points[ii].x-xx;
                                                                                        if (max_x<deta_xx)
                                                                                        {
                                                                                                max_x = deta_xx;
                                                                                        }
                                                                                }
                                                                                pt[(jj+idx)%4].x += max_x;
                                                                                pt[(jj+idx+1)%4].x += max_x;
                                                                        }
                                                                        else if (jj==2)
                                                                        {
                                                                                float max_y = 0;
                                                                                for(int ii = 50;ii<points.size()-50;ii++)
                                                                                {
                                                                                        float yy = points[ii].x*K+B;
                                                                                        float deta_yy = points[ii].y-yy;
                                                                                        if (max_y<deta_yy)
                                                                                        {
                                                                                                max_y = deta_yy;
                                                                                        }
                                                                                }
                                                                                pt[(jj+idx)%4].y += max_y;
                                                                                pt[(jj+idx+1)%4].y += max_y;
                                                                        }
                                                                        else
                                                                        {
                                                                                float max_x = 0;
                                                                                for(int ii = 50;ii<points.size()-50;ii++)
                                                                                {
                                                                                        float xx = (points[ii].y-B)/K;
                                                                                        float deta_xx = xx-points[ii].x;
                                                                                        if (max_x<deta_xx)
                                                                                        {
                                                                                                max_x = deta_xx;
                                                                                        }
                                                                                }
                                                                                pt[(jj+idx)%4].x -= max_x;
                                                                                pt[(jj+idx+1)%4].x -= max_x;
                                                                        }
                                                                }


                                                                for(int jj = 0; jj < 4; jj++)
                                                                {
                                                                        /*if (Idex[(4+jj-1)%4]>=0)
                                                                        {
                                                                                if (jj==0)
                                                                                {
                                                                                        pt[(4+jj+idx-1)%4].x -= max(pt[(jj+idx)%4].x,pt[(4+jj+idx-1)%4].x);
                                                                                        pt[(jj+idx)%4].x -= max(pt[(jj+idx)%4].x,pt[(4+jj+idx-1)%4].x);
                                                                                }
                                                                                else if(jj==1)
                                                                                {
                                                                                        pt[(4+jj+idx-1)%4].y -= max(pt[(jj+idx)%4].y,pt[(4+jj+idx-1)%4].y);
                                                                                        pt[(jj+idx)%4].y -= max(pt[(jj+idx)%4].y,pt[(4+jj+idx-1)%4].y);
                                                                                }
                                                                                else if(jj==2)
                                                                                {
                                                                                        pt[(4+jj+idx-1)%4].x += max(bw.cols-pt[(jj+idx)%4].x,bw.cols-pt[(4+jj+idx-1)%4].x);
                                                                                        pt[(jj+idx)%4].x += max(bw.cols-pt[(jj+idx)%4].x,bw.cols-pt[(4+jj+idx-1)%4].x);
                                                                                }
                                                                                else
                                                                                {
                                                                                        pt[(4+jj+idx-1)%4].y += max(bw.rows-pt[(jj+idx)%4].y,bw.rows-pt[(4+jj+idx-1)%4].y);
                                                                                        pt[(jj+idx)%4].y += max(bw.rows-pt[(jj+idx)%4].y,bw.rows-pt[(4+jj+idx-1)%4].y);
                                                                                }
                                                                                if (jj>0)
                                                                                        PT4[PT4.size()-1] = pt[(4+jj+idx-1)%4];
                                                                        }*/
                                                                        PT4.push_back(pt[(jj+idx)%4]);
                                                                }

                                                        }
                                                        else
                                                                return false;
                                                }
                                                else
                                                        return false;
                                        }
                                        else
                                        {
                                                Point2f pt[4];
                                                Point2f pf(0,0);
                                                rMaxRect.center.x += 0.5;
                                                rMaxRect.center.y += 0.5;
                                                rMaxRect.points(pt);
                                                float min_dist = 20000;
                                                int idx = 0;
                                                for (int jj = 0;jj<4;jj++)
                                                {
                                                        float dist = norm(pf-pt[jj]);
                                                        if (dist<min_dist)
                                                        {
                                                                min_dist = dist;
                                                                idx = jj;
                                                        }
                                                }

                                                PT4.clear();
                                                for (int jj = 0;jj<4;jj++)
                                                {
                                                        PT4.push_back(pt[(jj+idx)%4]);
                                                }
                                        }
                                }
                                return true;
                        }
                }
        }
        return false;
}



bool CDetectRectByContours_new::findMaxConyours(Mat bw, vector<Point2f> &PT4, int type)
{
        if (bw.empty())
                return false;

        bool err = true;
        while (true)
        {
                vector<Point> areaMatvPoint;
                //???????????vvPoint?????????????
                vector<vector<Point>> vvPoint;
                vector<vector<Point>> vvPoint1;
                vector<RotatedRect> cRect;
                findContours(bw.clone(),vvPoint,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
                if (vvPoint.size()>0)
                {
                        bool errMsg = false;
                        //???????????????????????????
                        int rectMaxArea =1000;
                        int rectArea =0;
                        RotatedRect rMaxRect;
                        for (vector<vector<Point>>::iterator itr=vvPoint.begin();itr!=vvPoint.end();itr++)
                        {
                                RotatedRect rrect =minAreaRect(*itr);
                                rectArea =rrect.size.area();
                                int Area = contourArea(*itr);
                                if( rectArea >1000)
                                {
                                        float fRiao = 1.0*Area/rectArea;
                                        /*if (rectArea>bw.cols*bw.rows*0.9 && fRiao<0.7)
                                        {
                                                continue;
                                        }*/
                                        if ((fRiao>0.5 || (fRiao>0.05 && rectArea>40000)) && (rrect.size.width>100 || rrect.size.height>100))
                                        {
                                                errMsg = true;

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
                        }

                        if (cRect.size()>1 && err)
                        {
                                vector<Mat> Matchannels;
                                for (int ii = 0;ii<cRect.size();ii++)
                                {
                                        Point2f pt[4];
                                        cRect[ii].points(pt);
                                        Mat mask = Mat::zeros(bw.size(),bw.type());
                                        vector<Point> pts;
                                        pts.push_back(pt[0]);
                                        pts.push_back(pt[1]);
                                        pts.push_back(pt[2]);
                                        pts.push_back(pt[3]);
                                        vector<vector<Point>> contours;
                                        contours.push_back(pts);
                                        drawContours(mask,contours,-1,Scalar(255),-1);
                                        drawContours(mask,contours,-1,Scalar(255),4);
                                        Matchannels.push_back(mask);
                                }

                                bool err_type = false;
                                for (int ii = 0;ii<cRect.size()-1;ii++)
                                {
                                        Mat mask1 = Matchannels[ii];
                                        double avg1 = mean(mask1)[0];

                                        //Rect Rect1 = boundingRect(Mat(vvPoint1[ii]));
                                        for(int jj = ii+1;jj<cRect.size();jj++)
                                        {
                                                //Rect Rect2 = boundingRect(Mat(vvPoint1[jj]));

                                                Mat mask2 = Matchannels[jj].clone();
                                                double avg2 = mean(mask2)[0];

                                                bitwise_or(mask1,mask2,mask2);
                                                double avg = mean(mask2)[0];
                                                double avg3 = avg1+avg2;
                                                //imshow("5",mask2);
                                                //waitKey(0);
                                                //int deta_w = max(Rect1.x+Rect1.width,Rect2.x+Rect2.width)-min(Rect1.x,Rect2.x);
                                                //int deta_h = max(Rect1.y+Rect1.height,Rect2.y+Rect2.height)-min(Rect1.y,Rect2.y);
                                                if (avg<avg3-0.001)//Rect1.height+Rect2.height>deta_h || Rect1.width+Rect2.width>deta_w)
                                                {
                                                        int idx1 = 0;
                                                        int idx2 = 0;
                                                        float len = max(bw.cols,bw.rows);
                                                        for (int kk = 0;kk<vvPoint1[ii].size();kk++)
                                                        {
                                                                for (int zz = 0;zz<vvPoint1[jj].size();zz++)
                                                                {
                                                                        float dist = norm(vvPoint1[ii][kk]-vvPoint1[jj][zz]);
                                                                        if (dist<len)
                                                                        {
                                                                                len = dist;
                                                                                idx1 = kk;
                                                                                idx2 = zz;
                                                                        }
                                                                }
                                                        }
                                                        //line(bw,Point(cRect[ii].center.x,cRect[ii].center.y),Point(cRect[jj].center.x,cRect[jj].center.y),Scalar(255),3);
                                                        line(bw,vvPoint1[ii][idx1],vvPoint1[jj][idx2],Scalar(255),3);
                                                        err_type = true;
                                                }
                                        }
                                }
                                err = false;
                                if(err_type)
                                {
                                        continue;
                                }
                        }

                        if (errMsg)
                        {
                                if(type==1)
                                {
                                        for (int ii = 0;ii<cRect.size();ii++)
                                        {
                                                if (cRect[ii].size.width<70 || cRect[ii].size.height<70)
                                                        continue;
                                                Point2f pt[4];
                                                Point2f pf(0,0);

                                                bool type1 = false;
                                                bool type2 = false;
                                                bool type3 = false;
                                                bool type4 = false;
                                                if(1)
                                                {
                                                        cRect[ii].points(pt);
                                                        float min_dist = 20000;
                                                        int idx1 = 0;
                                                        for (int jj = 0;jj<4;jj++)
                                                        {
                                                                float dist = norm(pf-pt[jj]);
                                                                if (dist<min_dist)
                                                                {
                                                                        min_dist = dist;
                                                                        idx1 = jj;
                                                                }
                                                        }

                                                        vector<Point> pts;
                                                        for (int jj = 0;jj<4;jj++)
                                                        {
                                                                pts.push_back(pt[(jj+idx1)%4]);
                                                        }

                                                        if(pts[0].y>2 || pts[1].y>2)
                                                                type1 = true;

                                                        if(pts[2].y<bw.rows-3 || pts[3].y<bw.rows-3)
                                                                        type2 = true;


                                                        if(pts[0].x>2 || pts[3].x>2)
                                                                type3 = true;

                                                        if(pts[1].x<bw.cols-3 || pts[2].x<bw.cols-3)
                                                                type4 = true;
                                                }
                                                if(type1 && type2 && type3 && type4)
                                                {
                                                        cRect[ii].size.width -= 5;
                                                        cRect[ii].size.height -= 5;
                                                        cRect[ii].center.x += 0.5;
                                                        cRect[ii].center.y += 0.5;
                                                }
                                                else
                                                {
                                                        if(type1)
                                                        {
                                                                cRect[ii].size.height -= 2;
                                                                cRect[ii].center.y += 1;
                                                                //rMaxRect.center.x += 1;
                                                                cRect[ii].center.y += 0.5;
                                                        }
                                                        if(type2)
                                                        {
                                                                cRect[ii].size.height -= 2;
                                                                cRect[ii].center.y -= 0.5;
                                                        }
                                                        if(type3)
                                                        {
                                                                cRect[ii].size.width -= 2;
                                                                cRect[ii].center.x += 1;
                                                                cRect[ii].center.x += 0.5;
                                                                //rMaxRect.center.y += 1;
                                                        }
                                                        if(type4)
                                                        {
                                                                cRect[ii].size.width -= 2;
                                                                cRect[ii].center.x -= 0.5;
                                                        }
                                                }

                                                cRect[ii].points(pt);
                                                float min_dist = 20000;
                                                int idx = 0;
                                                for (int jj = 0;jj<4;jj++)
                                                {
                                                        float dist = norm(pf-pt[jj]);
                                                        if (dist<min_dist)
                                                        {
                                                                min_dist = dist;
                                                                idx = jj;
                                                        }
                                                }

                                                vector<Point2f> pts;
                                                for (int jj = 0;jj<4;jj++)
                                                {
                                                        pts.push_back(pt[(jj+idx)%4]);
                                                        //line(bw,pt[(ii+idx)%4],pt[(ii+idx+1)%4],Scalar(255),3);
                                                }

                                                //imshow("5",bw);
                                                //waitKey(0);
                                                if (cRect[ii].size.area()>90000)
                                                {
                                                        if (pts.size()==4)
                                                        {
                                                                for (int jj = 0;jj<pts.size();jj++)
                                                                        PT4.push_back(pts[jj]);
                                                        }
                                                        continue;
                                                }

                                                vector<float> length;
                                                length.push_back(2000);
                                                length.push_back(2000);
                                                length.push_back(2000);
                                                length.push_back(2000);
                                                for (int kk = 0;kk<vvPoint1[ii].size();kk++)
                                                {
                                                        for(int jj = 0;jj<4;jj++)
                                                        {
                                                                Point2f pf = vvPoint1[ii][kk];
                                                                float dist = norm(pts[jj]-pf);
                                                                if (dist<length[jj])
                                                                        length[jj] = dist;
                                                        }
                                                }
                                                int count = 0;
                                                bool index = true;
                                                for(int jj = 0;jj<4;jj++)
                                                {
                                                        if (length[jj]<min(cRect[ii].size.width,cRect[ii].size.height)*0.15)
                                                        {
                                                                count+=1;
                                                                if (count==2 && length[jj-1]<min(cRect[ii].size.width,cRect[ii].size.height)*0.15)
                                                                        index = false;
                                                        }

                                                }
                                                if (count<2)
                                                {
                                                        pts.clear();
                                                        //return false;
                                                }
                                                else if (count==2 && index)
                                                {
                                                        pts.clear();
                                                        //return false;
                                                }
                                                if (pts.size()==4)
                                                {
                                                        for (int jj = 0;jj<pts.size();jj++)
                                                                PT4.push_back(pts[jj]);
                                                }
                                        }
                                        if ((PT4.size())%4==0 && PT4.size()>1)
                                                return true;
                                        else
                                                return false;
                                }
                                else
                                {
                                        int idex = 0;
                                        while (idex<cRect.size())
                                        {
                                                if (cRect[idex].size.width<70 || cRect[idex].size.height<70)
                                                {
                                                        idex += 1;
                                                        continue;
                                                }

                                                rMaxRect = cRect[idex];
                                                areaMatvPoint = vvPoint1[idex];
                                                idex += 1;

                                                Point2f pt[4];
                                                Point2f pf(0,0);

                                                bool type1 = false;
                                                bool type2 = false;
                                                bool type3 = false;
                                                bool type4 = false;
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

                                                        if(pts[0].y>2 || pts[1].y>2)
                                                                type1 = true;

                                                        if(pts[2].y<bw.rows-3 || pts[3].y<bw.rows-3)
                                                                        type2 = true;


                                                        if(pts[0].x>2 || pts[3].x>2)
                                                                type3 = true;

                                                        if(pts[1].x<bw.cols-3 || pts[2].x<bw.cols-3)
                                                                type4 = true;
                                                }
                                                if (type==0)
                                                {
                                                        if(type1 && type2 && type3 && type4)
                                                        {
                                                                rMaxRect.size.width -= 5;
                                                                rMaxRect.size.height -= 5;
                                                                rMaxRect.center.x += 0.5;
                                                                rMaxRect.center.y += 0.5;
                                                        }
                                                        else
                                                        {
                                                                if(type1)
                                                                {
                                                                        rMaxRect.size.height -= 2;
                                                                        rMaxRect.center.y += 1;
                                                                        //rMaxRect.center.x += 1;
                                                                        rMaxRect.center.y += 0.5;
                                                                }
                                                                if(type2)
                                                                {
                                                                        rMaxRect.size.height -= 2;
                                                                        rMaxRect.center.y -= 0.5;
                                                                }
                                                                if(type3)
                                                                {
                                                                        rMaxRect.size.width -= 2;
                                                                        rMaxRect.center.x += 1;
                                                                        rMaxRect.center.x += 0.5;
                                                                        //rMaxRect.center.y += 1;
                                                                }
                                                                if(type4)
                                                                {
                                                                        rMaxRect.size.width -= 2;
                                                                        rMaxRect.center.x -= 0.5;
                                                                }
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

                                                int aa = 0;
                                                for (int jj = 0;jj<4;jj++)
                                                {
                                                        PT4.push_back(pt[(jj+idx)%4]);
                                                        if (jj==0)
                                                        {
                                                                if (PT4[jj].x < 3 && PT4[jj].y <3)
                                                                        aa += 1;
                                                        }
                                                        else if(jj==1)
                                                        {
                                                                if (PT4[jj].x >bw.cols-4 && PT4[jj].y <3)
                                                                        aa += 1;
                                                        }
                                                        else if(jj==2)
                                                        {
                                                                if (PT4[jj].x >bw.cols-4 && PT4[jj].y > bw.rows-4)
                                                                        aa += 1;
                                                        }
                                                        else
                                                        {
                                                                if (PT4[jj].x <3 && PT4[jj].y > bw.rows-4)
                                                                        aa += 1;
                                                        }
                                                }
                                                if (aa ==4)
                                                        return true;


                                                if (type == 2)
                                                {
                                                        //???ฆฬ???
                                                        for(int jj = 0;jj<4;jj++)
                                                        {
                                                                if (jj==0)
                                                                {
                                                                        PT4[jj].x -= 10;
                                                                        PT4[jj].y -= 10;
                                                                }
                                                                else if(jj==1)
                                                                {
                                                                        PT4[jj].x += 10;
                                                                        PT4[jj].y -= 10;
                                                                }
                                                                else if(jj==2)
                                                                {
                                                                        PT4[jj].x += 10;
                                                                        PT4[jj].y += 10;
                                                                }
                                                                else
                                                                {
                                                                        PT4[jj].x -= 10;
                                                                        PT4[jj].y += 10;
                                                                }

                                                        }
                                                }

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

                                                if (type ==2)
                                                {
                                                        vector<Point2f> pts = PT4;
                                                        int ck = 0;
                                                        for (int jj = 0;jj<4;jj++)
                                                        {
                                                                if (length[jj]<0.3*max(rMaxRect.size.width,rMaxRect.size.height) &&
                                                                        ( areaMatvPoint[indx[jj]].x>1 && areaMatvPoint[indx[jj]].x<bw.cols-2 && areaMatvPoint[indx[jj]].y>1 && areaMatvPoint[indx[jj]].y<bw.rows-2))
                                                                                pts[jj] = areaMatvPoint[indx[jj]];
                                                                else
                                                                {
                                                                        ck += 1;
                                                                        if (jj==0)
                                                                        {
                                                                                pts[jj].x -= 10;
                                                                                pts[jj].y -= 10;
                                                                        }
                                                                        else if(jj==1)
                                                                        {
                                                                                pts[jj].x += 10;
                                                                                pts[jj].y -= 10;
                                                                        }
                                                                        else if(jj==2)
                                                                        {
                                                                                pts[jj].x += 10;
                                                                                pts[jj].y += 10;
                                                                        }
                                                                        else
                                                                        {
                                                                                pts[jj].x -= 10;
                                                                                pts[jj].y += 10;
                                                                        }
                                                                }
                                                        }
                                                        if (ck>1)
                                                        {
                                                                PT4 = pts;
                                                        }
                                                        else
                                                        {
                                                                Mat mask = Mat::zeros(bw.size(),bw.type());
                                                                vector<vector<Point>> contours;
                                                                vector<Point> pt;
                                                                pt.push_back(Point(pts[0].x,pts[0].y));
                                                                pt.push_back(Point(pts[1].x,pts[1].y));
                                                                pt.push_back(Point(pts[2].x,pts[2].y));
                                                                pt.push_back(Point(pts[3].x,pts[3].y));
                                                                contours.push_back(pt);
                                                                drawContours(mask,contours,-1,Scalar(255),-1);
                                                                drawContours(mask,contours,-1,Scalar(255),4);

                                                                Mat mask1 = Mat::zeros(bw.size(),bw.type());
                                                                contours.clear();
                                                                contours.push_back(areaMatvPoint);
                                                                drawContours(mask1,contours,-1,Scalar(255),-1);
                                                                drawContours(mask1,contours,-1,Scalar(255),4);

                                                                double aa = mean(mask)[0];
                                                                double aa2 = mean(mask1)[0];
                                                                if (aa2/aa<0.5)
                                                                {
                                                                        for (int jj = 0;jj<4;jj++)
                                                                        {
                                                                                if (length[jj]<0.3*max(rMaxRect.size.width,rMaxRect.size.height) &&
                                                                                        ( areaMatvPoint[indx[jj]].x>1 && areaMatvPoint[indx[jj]].x<bw.cols-2 && areaMatvPoint[indx[jj]].y>1 && areaMatvPoint[indx[jj]].y<bw.rows-2))
                                                                                                PT4[jj] = areaMatvPoint[indx[jj]];
                                                                                else
                                                                                {
                                                                                        Point2f pf = PT4[jj];
                                                                                        int idx1 = indx[(jj+1)%4];
                                                                                        int idx2 = indx[(4+jj-1)%4];

                                                                                        int index1 = 0;
                                                                                        int index2 = 0;
                                                                                        int count = 0;
                                                                                        int min_dist = 20000;
                                                                                        for (int zz = idx1;zz<idx1+areaMatvPoint.size();zz++)
                                                                                        {
                                                                                                int aa = zz%areaMatvPoint.size();
                                                                                                Point2f pf2 = areaMatvPoint[aa];
                                                                                                float dist = norm(pf-pf2);
                                                                                                if (dist<min_dist)
                                                                                                {
                                                                                                        min_dist = dist;
                                                                                                        index1 = aa;
                                                                                                        count = 0;
                                                                                                }
                                                                                                else
                                                                                                {
                                                                                                        count += 1;
                                                                                                }
                                                                                                if ( count>10)
                                                                                                        break;
                                                                                        }
                                                                                        count = 0;
                                                                                        min_dist = 20000;
                                                                                        for (int zz = areaMatvPoint.size()+idx2;zz<idx2+2*areaMatvPoint.size();zz--)
                                                                                        {
                                                                                                int aa = (zz+areaMatvPoint.size())%areaMatvPoint.size();
                                                                                                Point2f pf2 = areaMatvPoint[aa];
                                                                                                float dist = norm(pf-pf2);
                                                                                                if (dist<min_dist)
                                                                                                {
                                                                                                        min_dist = dist;
                                                                                                        index2 = aa;
                                                                                                        count = 0;
                                                                                                }
                                                                                                else
                                                                                                {
                                                                                                        count += 1;
                                                                                                }
                                                                                                if ( count>10)
                                                                                                        break;
                                                                                        }
                                                                                        if (index1 != 0 && index2 != 0)
                                                                                        {
                                                                                                Point2f pf1 = areaMatvPoint[index1];
                                                                                                Point2f pf2 = areaMatvPoint[index2];
                                                                                                Point2f pf3 = areaMatvPoint[indx[(4+jj-1)%4]];
                                                                                                Point2f pf4 = areaMatvPoint[indx[(jj+1)%4]];
                                                                                                float K1 = (pf4.y-pf1.y)/(pf4.x-pf1.x+0.00001);
                                                                                                float K2 = (pf3.y-pf2.y)/(pf3.x-pf2.x+0.00001);
                                                                                                float b1 = pf1.y-K1*pf1.x;
                                                                                                float b2 = pf2.y-K2*pf2.x;
                                                                                                float x = (b1-b2)/(K2-K1);
                                                                                                float y = K1*x+b1;
                                                                                                PT4[jj] = Point2f(x,y);
                                                                                        }
                                                                                        else
                                                                                        {
                                                                                                if (jj==0)
                                                                                                {
                                                                                                        pts[jj].x -= 10;
                                                                                                        pts[jj].y -= 10;
                                                                                                }
                                                                                                else if(jj==1)
                                                                                                {
                                                                                                        pts[jj].x += 10;
                                                                                                        pts[jj].y -= 10;
                                                                                                }
                                                                                                else if(jj==2)
                                                                                                {
                                                                                                        pts[jj].x += 10;
                                                                                                        pts[jj].y += 10;
                                                                                                }
                                                                                                else
                                                                                                {
                                                                                                        pts[jj].x -= 10;
                                                                                                        pts[jj].y += 10;
                                                                                                }
                                                                                        }
                                                                                }
                                                                        }
                                                                }
                                                                else
                                                                {
                                                                        /*bitwise_and(mask,~mask1,mask);
                                                                        double aa1 = mean(mask)[0];
                                                                        if (aa1/aa<0.02)
                                                                                PT4 = pts;
                                                                        else*/
                                                                        if (1)
                                                                        {
                                                                                for (int jj = 0;jj<4;jj++)
                                                                                {
                                                                                        //if (length[jj]<0.1*max(rMaxRect.size.width,rMaxRect.size.height) &&
                                                                                        //	( PT4[jj].x>0 && PT4[jj].x<bw.cols-1 && PT4[jj].y>0 && PT4[jj].y<bw.rows-1))
                                                                                        if (length[jj]<0.15*max(rMaxRect.size.width,rMaxRect.size.height) &&
                                                                                        ( areaMatvPoint[indx[jj]].x>1 && areaMatvPoint[indx[jj]].x<bw.cols-2 && areaMatvPoint[indx[jj]].y>1 && areaMatvPoint[indx[jj]].y<bw.rows-2))
                                                                                                        PT4[jj] = areaMatvPoint[indx[jj]];
                                                                                        else
                                                                                        {
                                                                                                Point2f pf = PT4[jj];
                                                                                                Point2f pf_1 = areaMatvPoint[indx[jj]];
                                                                                                bool ty = false;
                                                                                                int min_dist = abs(pf.x-pf_1.x)+2;
                                                                                                if (abs(pf.x-pf_1.x)<1.5 || (pf.y>bw.rows-1 || pf.y<1))
                                                                                                {
                                                                                                        min_dist = abs(pf.y-pf_1.y)+2;
                                                                                                        ty = true;
                                                                                                }
                                                                                                int idx1 = indx[(jj+1)%4];
                                                                                                int idx2 = indx[(4+jj-1)%4];
                                                                                                int idx3 = indx[jj];

                                                                                                int index1 = 0;
                                                                                                int index2 = 0;
                                                                                                int count = 0;

                                                                                                for (int zz = idx1;zz<idx1+areaMatvPoint.size();zz++)
                                                                                                {
                                                                                                        int aa = (zz+areaMatvPoint.size())%areaMatvPoint.size();
                                                                                                        //if (aa<idx1)
                                                                                                        //	break;
                                                                                                        Point2f pf2 = areaMatvPoint[aa];
                                                                                                        float dist;
                                                                                                        if (ty)
                                                                                                                dist = abs(pf2.y-pf.y);
                                                                                                        else
                                                                                                                dist = abs(pf2.x-pf.x);
                                                                                                        if (dist<min_dist)
                                                                                                        {
                                                                                                                min_dist = dist;
                                                                                                                index1 = aa;
                                                                                                                count = 0;
                                                                                                        }
                                                                                                        else if(dist<min_dist+2)
                                                                                                        {
                                                                                                                count += 1;
                                                                                                        }
                                                                                                        if ( count>10)
                                                                                                                break;
                                                                                                }
                                                                                                int count2 = 0;
                                                                                                if (ty)
                                                                                                        min_dist = abs(pf.y-pf_1.y)+2;
                                                                                                else
                                                                                                        min_dist = abs(pf.x-pf_1.x)+2;
                                                                                                for (int zz = areaMatvPoint.size()+idx2;zz<idx2+2*areaMatvPoint.size();zz--)
                                                                                                {
                                                                                                        int aa = zz%areaMatvPoint.size();
                                                                                                        //if(aa>idx2)
                                                                                                        //	break;
                                                                                                        Point2f pf2 = areaMatvPoint[aa];
                                                                                                        float dist;
                                                                                                        if (ty)
                                                                                                                dist = abs(pf2.y-pf.y);
                                                                                                        else
                                                                                                                dist = abs(pf2.x-pf.x);
                                                                                                        if (dist<min_dist)
                                                                                                        {
                                                                                                                min_dist = dist;
                                                                                                                index2 = aa;
                                                                                                                count2 = 0;
                                                                                                        }
                                                                                                        else if(dist<min_dist+2)
                                                                                                        {
                                                                                                                count2 += 1;
                                                                                                        }
                                                                                                        if ( count2>10)
                                                                                                                break;
                                                                                                }
                                                                                                if (count >5 && count2 >5 )
                                                                                                {
                                                                                                        Point2f pf1 = areaMatvPoint[index1];
                                                                                                        Point2f pf2 = areaMatvPoint[index2];
                                                                                                        Point2f pf3 = areaMatvPoint[indx[(4+jj-1)%4]];
                                                                                                        Point2f pf4 = areaMatvPoint[indx[(jj+1)%4]];
                                                                                                        float K1 = (pf4.y-pf1.y)/(pf4.x-pf1.x+0.00000001);
                                                                                                        float K2 = (pf3.y-pf2.y)/(pf3.x-pf2.x+0.00000001);
                                                                                                        float b1 = pf1.y-K1*pf1.x;
                                                                                                        float b2 = pf2.y-K2*pf2.x;
                                                                                                        float x = (b1-b2)/(K2-K1+0.000000001);
                                                                                                        float y = K1*x+b1;
                                                                                                        PT4[jj] = Point2f(x,y);
                                                                                                }
                                                                                                else
                                                                                                {
                                                                                                        if (jj==0)
                                                                                                        {
                                                                                                                pts[jj].x -= 10;
                                                                                                                pts[jj].y -= 10;
                                                                                                        }
                                                                                                        else if(jj==1)
                                                                                                        {
                                                                                                                pts[jj].x += 10;
                                                                                                                pts[jj].y -= 10;
                                                                                                        }
                                                                                                        else if(jj==2)
                                                                                                        {
                                                                                                                pts[jj].x += 10;
                                                                                                                pts[jj].y += 10;
                                                                                                        }
                                                                                                        else
                                                                                                        {
                                                                                                                pts[jj].x -= 10;
                                                                                                                pts[jj].y += 10;
                                                                                                        }
                                                                                                }
                                                                                        }
                                                                                }
                                                                        }
                                                                }
                                                        }
                                                        length.clear();
                                                        length.push_back(2000);
                                                        length.push_back(2000);
                                                        length.push_back(2000);
                                                        length.push_back(2000);
                                                        //int indx[4];
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

                                                                if (abs(len-num)/len<0.15 && (length[jj]<max(20.0,0.05*min(rMaxRect.size.width,rMaxRect.size.height)) &&
                                                                        length[(jj+1)%4]<max(20.0,0.05*min(rMaxRect.size.width,rMaxRect.size.height))))
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
                                                        if (length[jj]<max(20.0,min(rMaxRect.size.width,rMaxRect.size.height)*0.05))
                                                        {
                                                                /*if (PT4[jj].x<3 || PT4[jj].x>bw.cols-4 || PT4[jj].y<3 || PT4[jj].y>bw.rows-4)
                                                                {
                                                                        count += 0.5;
                                                                        continue;
                                                                }*/
                                                                count+=1;
                                                                if ((count==2 || count==2.5) && length[jj-1]<max(20.0,min(rMaxRect.size.width,rMaxRect.size.height)*0.05))
                                                                        index = false;
                                                        }

                                                }

                                                if (cc==1)
                                                {
                                                        if (count<3)
                                                        {
                                                                PT4.clear();
                                                                //return false;
                                                        }
                                                        else
                                                                return true;
                                                }
                                                else if (cc<1)
                                                {
                                                        PT4.clear();
                                                }
                                                else if ( count<2)
                                                {
                                                        PT4.clear();
                                                        //return false;
                                                }
                                                else if (count<2.6 && index )
                                                {
                                                        PT4.clear();
                                                        //return false;
                                                }
                                                else
                                                        return true;
                                        }
                                        return false;
                                }
                        }
                        else
                                return false;
                }
                else
                {
                        return false;
                }
        }
}

//???????????????????
//uthreshold??vthreshold??????????????????????????
//type????????????????0????????1???????
void CDetectRectByContours_new::delete_jut(Mat& src, Mat& dst, int uthreshold, int vthreshold, int type)
{

        int threshold;
        src.copyTo(dst);
        rectangle(dst, Point(0, 0), Point(dst.cols,dst.rows), Scalar(0), 2);
        int height = dst.rows;
        int width = dst.cols;
        int k;  //??????????????????
        for (int i = 0; i < height - 1; i++)
        {
                uchar* p = dst.ptr<uchar>(i);
                for (int j = 0; j < width - 1; j++)
                {
                        if (type == 0)
                        {
                                //??????
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
                                //??????
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
                                //??????
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
                                //??????
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

void  CDetectRectByContours_new::fillHole(Mat srcBw, Mat &dstBw)
{
        Size m_Size = srcBw.size();
        Mat Temp=Mat::zeros(m_Size.height+2,m_Size.width+2,srcBw.type());//??????
        srcBw.copyTo(Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));

        cv::floodFill(Temp, Point(0, 0), Scalar(255));

        Mat cutImg;//?จน?????????
        Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);

        dstBw = srcBw | (~cutImg);
}
