#include "stdafx.h"
#include "FilterEffect.h"

//怀旧风格
Mat CFilterEffect::nostalgic(Mat src)
{
	Mat r(src.rows, src.cols, CV_8UC1);
    Mat g(src.rows, src.cols, CV_8UC1);
    Mat b(src.rows, src.cols, CV_8UC1);

    Mat out[]={b, g, r};

    split(src, out);

    Mat r_new(src.rows, src.cols, CV_8UC1);
    Mat g_new(src.rows, src.cols, CV_8UC1);
    Mat b_new(src.rows, src.cols, CV_8UC1);

    r_new=0.393*r+0.769*g+0.189*b;
    g_new=0.349*r+0.686*g+0.168*b;
    b_new=0.272*r+0.534*g+0.131*b;

    Mat rgb[]={b_new, g_new, r_new};

	Mat dst(src.size(), CV_8UC3);
    merge(rgb,3,dst);

	return dst;
}

//水波效果
Mat CFilterEffect::waterEffect(Mat src)
{
	Mat dst = src.clone();
   
    int width=src.cols;
    int height=src.rows;
    float A,B;
    A=7;
    B=2.5;

    Point Center(width/2, height/2);

    float r0,r1,new_x,new_y;
    float p,q,x1,y1,x0,y0;
    float theta;

    for (int y=0; y<height; y++)
    {
        for (int x=0; x<width; x++)
        {
            y0=Center.y-y;
            x0=x-Center.x;
            theta=atan(y0/(x0+0.00001));
            if(x0<0) theta=theta+CV_PI;
            r0=sqrt(x0*x0+y0*y0);
            r1=r0+A*width*0.01*sin(B*0.1*r0);

            new_x=r1*cos(theta);
            new_y=r1*sin(theta);

            new_x=Center.x+new_x;
            new_y=Center.y-new_y;

            if(new_x<0)         new_x=0;
            if(new_x>=width-1)  new_x=width-2;
            if(new_y<0)         new_y=0;
            if(new_y>=height-1) new_y=height-2;

            x1=(int)new_x;
            y1=(int)new_y;

            p=new_x-x1;
            q=new_y-y1;

            for (int k=0; k<3; k++)
            {
                dst.at<Vec3b>(y, x)[k]=(1-p)*(1-q)*src.at<Vec3b>(y1, x1)[k]+
                                        (p)*(1-q)*src.at<Vec3b>(y1,x1+1)[k]+
                                        (1-p)*(q)*src.at<Vec3b>(y1+1,x1)[k]+
                                        (p)*(q)*src.at<Vec3b>(y1+1,x1+1)[k];
            }

        }
    }
	return dst;
}

//波浪效果
Mat CFilterEffect::waveEffect(Mat src)
{
	Mat dst = src.clone();

    int width=src.cols;
    int height=src.rows;
    int N=30;

    Point Center(width/2, height/2);

    float new_x,new_y;
    float p,q,x1,y1,x0,y0;

    for (int y=0; y<height; y++)
    {
        for (int x=0; x<width; x++)
        {
            y0=Center.y-y;
            x0=x-Center.x;

            new_x=N*sin(2*CV_PI*y0/128.0)+x0;
            new_y=N*cos(2*CV_PI*x0/128.0)+y0;

            new_x=Center.x+new_x;
            new_y=Center.y-new_y;

            if(new_x<0)         new_x=0;
            if(new_x>=width-1)  new_x=width-2;
            if(new_y<0)         new_y=0;
            if(new_y>=height-1) new_y=height-2;

            x1=(int)new_x;
            y1=(int)new_y;

            p=new_x-x1;
            q=new_y-y1;

            for (int k=0; k<3; k++)
            {
                dst.at<Vec3b>(y, x)[k]=(1-p)*(1-q)*src.at<Vec3b>(y1, x1)[k]+
                                        (p)*(1-q)*src.at<Vec3b>(y1,x1+1)[k]+
                                        (1-p)*(q)*src.at<Vec3b>(y1+1,x1)[k]+
                                        (p)*(q)*src.at<Vec3b>(y1+1,x1+1)[k];
            }

       }
   }

   return dst;
}

//素描效果
Mat CFilterEffect::sketchEffect(Mat src)
{
    Mat Image_in;
    Image_in=src;
  
    Mat Image_out(Image_in.size(), CV_32FC3);
    Image_in.convertTo(Image_out, CV_32FC3);

    Mat I(Image_in.size(), CV_32FC1);

    cv::cvtColor(Image_out, I, CV_BGR2GRAY);
    I=I/255.0;
   
    Mat I_invert;
    I_invert=-I+1.0;
   
    Mat I_gau;
    GaussianBlur(I_invert, I_gau, Size(25,25), 0, 0);
   
    float delta=0.01;
    I_gau=-I_gau+1.0+delta;
   
    Mat I_dst;
    cv::divide(I, I_gau, I_dst);
    I_dst=I_dst;

    Mat b(Image_in.size(), CV_32FC1);
    Mat g(Image_in.size(), CV_32FC1);
    Mat r(Image_in.size(), CV_32FC1);

    Mat rgb[]={b,g,r};

    //float alpha=0.75;
	float alpha = 1;

    r=alpha*I_dst+(1-alpha)*200.0/255.0;
    g=alpha*I_dst+(1-alpha)*205.0/255.0;
    b=alpha*I_dst+(1-alpha)*105.0/255.0;

    cv::merge(rgb, 3, Image_out);

	Mat dst = src.clone();
	Image_out =	Image_out*255;
	Image_out.convertTo(dst,CV_8UC3);
	return dst;
}

//素描效果
Mat CFilterEffect::sketchEffect2(Mat src)
{
	Mat I_invert;  
    Mat I_gau;  
    float delta=0.01;  
    float alpha=0;  
     
    Mat I_dst;
    vector<Mat> channels;
    IplImage tmp1_1, tmp1_2, tmp1_3;
    IplImage tmp2_1, tmp2_2, tmp2_3;
    IplImage tmp3_1, tmp3_2, tmp3_3;
 
    Mat mat = src;
 
    split(mat, channels);
    tmp2_1 = channels.at(0);
    tmp2_2 = channels.at(1);
    tmp2_3 = channels.at(2);
 
    Mat Image_out(mat.size(), CV_32FC3);
    mat.convertTo(Image_out, CV_32FC3);
    Mat I(mat.size(), CV_32FC1);
    cv::cvtColor(Image_out, I, CV_BGR2GRAY);
    I=I/255.0;
    I_invert=-I+1.0;
    GaussianBlur(I_invert, I_gau, Size(25, 25), 0, 0);
    I_gau=-I_gau+1.0+delta;
 
    cv::divide(I, I_gau, I_dst);  
    I_dst = I_dst * 255.0;
    tmp1_1 = I_dst;
 
    Mat b(mat.size(), CV_32FC1);  
    Mat g(mat.size(), CV_32FC1);  
    Mat r(mat.size(), CV_32FC1);
 
    Mat rgb[]={b,g,r};  
    tmp3_1 = b;
    tmp3_2 = g;
    tmp3_3 = r;
     
    alpha = 1.0;//原0.75
 
    cvAddWeighted(&tmp1_1, alpha, &tmp2_1, (1-alpha), 0, &tmp3_1);
    cvAddWeighted(&tmp1_1, alpha, &tmp2_2, (1-alpha), 0, &tmp3_2);
    cvAddWeighted(&tmp1_1, alpha, &tmp2_3, (1-alpha), 0, &tmp3_3);
    cv::merge(rgb, 3, Image_out);

	Mat dst = src.clone();
    Image_out.convertTo(dst, CV_8UC3);
	return dst;
}

//浮雕效果
Mat CFilterEffect::reliefEffect(Mat src)
{
    Mat Image_in(src);

    Mat Image_out(Image_in.size(), CV_32FC3);
    Image_in.convertTo(Image_out, CV_32FC3);

    Mat Image_2(Image_in.size(), CV_32FC3);
    Image_in.convertTo( Image_2, CV_32FC3);


     Mat kernel;
     Point anchor;
     double delta;
     int ddepth;
     int kernel_size;

     ddepth=-1;
     anchor=Point(-1,-1);
     delta=0;

     kernel_size=3;

     Mat K_1;

     K_1=Mat::zeros(kernel_size, kernel_size, CV_32F);

     float p;

     p=3;

     K_1.at<float>(0,2)=p;
     K_1.at<float>(2,0)=-p;


     Mat Image_x(Image_in.size(), CV_32FC3);

     cv::filter2D(Image_2, Image_x, ddepth, K_1);

     cv::add(Image_x, Scalar(128.0, 128.0, 128.0), Image_out);

    Image_out=Image_out/255;

    Mat dst = src.clone();
	Image_out =	Image_out*255;
	Image_out.convertTo(dst,CV_8UC3);
	return dst;

}

//渐变映射 
Mat CFilterEffect::shadeEffect(Mat src)
{
    Mat Image_in(src);

    Mat Image_out(Image_in.size(), CV_32FC3);
    Image_in.convertTo(Image_out, CV_32FC3);

    Mat Image_2(Image_in.size(), CV_32FC3);
    Image_in.convertTo(Image_2, CV_32FC3);

    Mat Map(Image_in.size(), CV_32FC3);
    Mat temp;
    float val;

    // build the mapping talbe
    for (int i=0; i<Image_2.cols; i++)
    {
        temp=Map.col(i);
        val=1-std::abs((float)(i)/((float)Image_2.cols/2)-1);
        temp.setTo(Scalar(val,val,val));
    }

    cv::multiply(Image_2, Map, Image_out);

    Image_out=Image_out/255.0;

    Mat dst = src.clone();
	Image_out =	Image_out*255;
	Image_out.convertTo(dst,CV_8UC3);
	return dst;
}

//照亮边缘
Mat CFilterEffect::glowingEdgeEffect(Mat src)
{
    Mat Image_in(src);

    Mat Image_out(Image_in.size(), CV_32FC3);
    Image_in.convertTo(Image_out, CV_32FC3);

    Mat Image_2(Image_in.size(), CV_32FC3);
    Image_in.convertTo( Image_2, CV_32FC3);

    Mat kernel;
    Point anchor;
    double delta;
    int ddepth;
    int kernel_size;

    ddepth=-1;
    anchor=Point(-1,-1);
    delta=0;

    kernel_size=3;

    Mat K_x;
    Mat K_y;

    K_x=Mat::zeros(kernel_size, kernel_size, CV_32F);
    K_y=Mat::zeros(kernel_size, kernel_size, CV_32F);

    float p,q;

    p=3; q=0;

    K_x.at<float>(0,0)=-1;  K_x.at<float>(0,1)=0; K_x.at<float>(0,2)=1;
    K_x.at<float>(1,0)=-p;  K_x.at<float>(1,1)=q;  K_x.at<float>(1,2)=p;
    K_x.at<float>(2,0)=-1;  K_x.at<float>(2,1)=0;  K_x.at<float>(2,2)=1;

    K_y.at<float>(0,0)=-1; K_y.at<float>(0,1)=-p; K_y.at<float>(0,2)=-1;
    K_y.at<float>(1,0)=0;  K_y.at<float>(1,1)=q;  K_y.at<float>(1,2)=0;
    K_y.at<float>(2,0)=1;  K_y.at<float>(2,1)=p;  K_y.at<float>(2,2)=1;


    Mat Image_x(Image_in.size(), CV_32FC3);
    Mat Image_y(Image_in.size(), CV_32FC3);

    cv::filter2D(Image_2, Image_x, ddepth, K_x);
    cv::filter2D(Image_2, Image_y, ddepth, K_y);

    float alpha=0.5;

    Image_out=alpha*abs(Image_x)+(1-alpha)*abs(Image_y);

    Image_out=Image_out/255;

	Mat dst = src.clone();
	Image_out =	Image_out*255;
	Image_out.convertTo(dst,CV_8UC3);
	return dst;
}

//色调映射 
Mat CFilterEffect::shade2Effect(Mat src)
{
    Mat Image_in(src);
   
    Mat Image_out(Image_in.size(), CV_32FC3);
    Image_in.convertTo(Image_out, CV_32FC3);

    Mat Map(50, 255, CV_32FC3);

    float val;

    Mat temp;

    // build the mapping talbe
    for (int i=0; i<Map.cols; i++)
    {
        val=i/255.0;
        temp=Map.col(i);
        temp.setTo(Scalar(3*val,3*val-1,3*val-2));
    }

    MatIterator_<Vec3f> pixel_begin, pixel_end;
    pixel_begin=Image_out.begin<Vec3f>();
    pixel_end =Image_out.end<Vec3f>();

    // I=0.299*R+0.587*G+0.144*B

    for( ; pixel_begin!=pixel_end; pixel_begin++)
    {
        val=(*pixel_begin)[0]*0.144;
        val=val+(*pixel_begin)[1]*0.587;
        val=val+(*pixel_begin)[2]*0.299;
        val=val/255.0;
        (*pixel_begin)[0]=3*val;
        (*pixel_begin)[1]=3*val-1;
        (*pixel_begin)[2]=3*val-2;
    }

  
	Mat dst = src.clone();
	Image_out =	Image_out*255;
	Image_out.convertTo(dst,CV_8UC3);
	return dst;
}

//马赛克
Mat CFilterEffect::mosaicEffect(Mat src)
{
    Mat Image_in(src);
  
    Mat Image_out(Image_in.size(), CV_32FC3);
    Image_in.convertTo(Image_out, CV_32FC3);

    int P_size=9;
    float k1, k2;

    int n_row;
    int n_col;

    float m, n;

    int h,w;

    n_row=Image_in.rows;
    n_col=Image_in.cols;

    Mat sub_mat;

    //srand( (unsigned)time(NULL) );

    for (int i=P_size; i<Image_in.rows-P_size-1; i=i+P_size)
    {
        for (int j=P_size; j<Image_in.cols-1-P_size; j=j+P_size)
        {
            k1=(double)((rand() % 100))/100.0-0.5;
            k2=(double)((rand() % 100))/100.0-0.5;

            m=(k1*(P_size*2-1));
            n=(k2*(P_size*2-1));

            h=(int)(i+m)% n_row;
            w=(int)(j+n)% n_col;

            sub_mat=Image_out.operator()(Range(i-P_size,i+P_size), Range(j-P_size,j+P_size));

            sub_mat.setTo(Scalar(Image_in.at<Vec3b>(h,w)));

        }
    }

    Image_out=Image_out/255.0;

	Mat dst = src.clone();
	Image_out =	Image_out*255;
	Image_out.convertTo(dst,CV_8UC3);
	return dst;

}

//曝光过度
Mat CFilterEffect::overExposureEffect(Mat src)
{
   
    Mat Image_in(src);
    // Show_Image(Image_in, Img_name);
    Mat Image_out(Image_in.size(), CV_32FC3);
    Image_in.convertTo(Image_out, CV_32FC3);


    int nrows=Image_in.rows;
    int ncols=Image_in.cols;

    
    Image_out=-1*Image_in+255;

    cv::min(Image_in, Image_out, Image_out);

	Mat dst = src.clone();
	//Image_out =	Image_out*255;
	Image_out.convertTo(dst,CV_8UC3);
	return dst;


}

//旋转模糊
Mat CFilterEffect::spinBlurEffect(Mat src)
{
	Mat dst;
	return dst;
}

//碎片特效
Mat CFilterEffect::fragmentEffect(Mat src)
{
    Mat Image_in(src);
  
    Mat Image_out(Image_in.size(), CV_32FC3);
    Image_in.convertTo(Image_out, CV_32FC3);

    Mat Image_2(Image_in.size(), CV_32FC3);
    Image_in.convertTo(Image_2, CV_32FC3);

    Mat Img_up(Image_in.rows, Image_in.cols, CV_32FC3);
    Mat Img_down(Image_in.rows, Image_in.cols, CV_32FC3);
    Mat Img_left(Image_in.rows, Image_in.cols, CV_32FC3);
    Mat Img_right(Image_in.rows, Image_in.cols, CV_32FC3);

    Image_2.copyTo(Img_up);
    Image_2.copyTo(Img_down);
    Image_2.copyTo(Img_left);
    Image_2.copyTo(Img_right);

    Mat temp1, temp2;

    int offset=5;

    int Rows=Image_in.rows;
    int Cols=Image_in.cols;

    // move downward
    temp1=Image_2.rowRange(0, Rows-1-offset);
    temp2=Img_up.rowRange(offset, Rows-1);
    temp1.copyTo(temp2);

    // move upward
    temp1=Image_2.rowRange(offset, Rows-1);
    temp2=Img_down.rowRange(0, Rows-1-offset);
    temp1.copyTo(temp2);

    // move left
    temp1=Image_2.colRange(offset, Cols-1);
    temp2=Img_left.colRange(0, Cols-1-offset);
    temp1.copyTo(temp2);

    // move right
    temp1=Image_2.colRange(0, Cols-1-offset);
    temp2=Img_right.colRange(offset, Cols-1);
    temp1.copyTo(temp2);

    Image_out=1/4.0*(Img_up+Img_down+Img_left+Img_right);

    Image_out=1/255.0*Image_out;

	Mat dst = src.clone();
	Image_out =	Image_out*255;
	Image_out.convertTo(dst,CV_8UC3);
	return dst;

}

//凹陷
Mat CFilterEffect::ellipsoidEffect(Mat src)
{
    Mat Img(src);
  
    Mat Img_out(Img.size(), CV_8UC3);
    Img.copyTo(Img_out);

    int width=Img.cols;
    int height=Img.rows;

    float e;
    float a,b, a0, b0, a1,b1;
    float alpha=1.0;
    float K=CV_PI/2;

    a=width/2; b=height/2;
    e=(float)width/(float)height;

    Point2i Center(width/2, height/2);

    float new_x, new_y;
    float p,q,x1,y1,x0,y0;
    float theta;

    for (int y=0; y<height; y++)
    {
        for (int x=0; x<width; x++)
        {
            y0=Center.y-y;
            x0=x-Center.x;


            theta=atan(y0*e/(x0+0.0001));
            if(x0<0)   theta=theta+CV_PI;

            a0=x0/cos(theta);
            b0=y0/sin(theta+0.0001);

            if(a0>a || b0>b)  continue;

            a1=a*sin(a0/a*K);
            b1=b*sin(b0/b*K);

            a1=(a1-a0)*(alpha)+a0;
            b1=(b1-b0)*(alpha)+b0;

            new_x=a1*cos(theta);
            new_y=b1*sin(theta);

            new_x=Center.x+new_x;
            new_y=Center.y-new_y;

            if(new_x<0)         new_x=0;
            if(new_x>=width-1)  new_x=width-2;
            if(new_y<0)         new_y=0;
            if(new_y>=height-1) new_y=height-2;

            x1=(int)new_x;
            y1=(int)new_y;

            p=new_x-x1;
            q=new_y-y1;


            for (int k=0; k<3; k++)
            {
                Img_out.at<Vec3b>(y, x)[k]=(1-p)*(1-q)*Img.at<Vec3b>(y1, x1)[k]+
                                            (p)*(1-q)*Img.at<Vec3b>(y1,x1+1)[k]+
                                            (1-p)*(q)*Img.at<Vec3b>(y1+1,x1)[k]+
                                            (p)*(q)*Img.at<Vec3b>(y1+1,x1+1)[k];

            }


        }
    }

	Mat dst = Img_out.clone();
	
	return dst;
}

//突出
Mat CFilterEffect::ellipsoid2Effect(Mat src)
{
    Mat Img(src);
 
    Mat Img_out(Img.size(), CV_8UC3);
    Img.copyTo(Img_out);

    int width=Img.cols;
    int height=Img.rows;

    float e;
    float a,b, a0, b0, a1,b1;
    float alpha=1.0;
    float K=CV_PI/2;

    a=width/2; b=height/2;
    e=(float)width/(float)height;

    Point2i Center(width/2, height/2);

    float new_x, new_y;
    float p,q,x1,y1,x0,y0;
    float theta;

    for (int y=0; y<height; y++)
    {
        for (int x=0; x<width; x++)
        {
            y0=Center.y-y;
            x0=x-Center.x;


            theta=atan(y0*e/(x0+0.0001));
            if(x0<0)   theta=theta+CV_PI;

            a0=x0/cos(theta);
            b0=y0/sin(theta+0.0001);

            if(a0/a>1 || b0/b>1)  continue;

            a1=asin(a0/a)*a/K;
            b1=asin(b0/b)*b/K;

            a1=(a0-a1)*(1-alpha)+a1;
            b1=(b0-b1)*(1-alpha)+b1;

            new_x=a1*cos(theta);
            new_y=b1*sin(theta);

            new_x=Center.x+new_x;
            new_y=Center.y-new_y;

            if(new_x<0)         new_x=0;
            if(new_x>=width-1)  new_x=width-2;
            if(new_y<0)         new_y=0;
            if(new_y>=height-1) new_y=height-2;

            x1=(int)new_x;
            y1=(int)new_y;

            p=new_x-x1;
            q=new_y-y1;


            for (int k=0; k<3; k++)
            {
                Img_out.at<Vec3b>(y, x)[k]=(1-p)*(1-q)*Img.at<Vec3b>(y1, x1)[k]+
                                            (p)*(1-q)*Img.at<Vec3b>(y1,x1+1)[k]+
                                            (1-p)*(q)*Img.at<Vec3b>(y1+1,x1)[k]+
                                            (p)*(q)*Img.at<Vec3b>(y1+1,x1+1)[k];

            }


        }
    }

	Mat dst = Img_out.clone();
	return dst;
}

//漩涡
Mat CFilterEffect::vertexEffect(Mat src)
{
    Mat Img(src);
 
    Mat Img_out(Img.size(), CV_8UC3);
    Img.copyTo(Img_out);

    int width=Img.cols;
    int height=Img.rows;
    int N=70;

    Point Center(width/2, height/2);

    float radius,Dis,new_x,new_y;
    float p,q,x1,y1,x0,y0;
    float theta;

    for (int y=0; y<height; y++)
    {
        for (int x=0; x<width; x++)
        {
            y0=Center.y-y;
            x0=x-Center.x;
            Dis=x0*x0+y0*y0;

                theta=atan(y0/(x0+0.00001));
                if(x0<0) theta=theta+CV_PI;

                radius=sqrt(Dis);
                theta=theta+radius/N;
                new_x=radius*cos(theta);
                new_y=radius*sin(theta);
                new_x=Center.x+new_x;
                new_y=Center.y-new_y;

                if(new_x<0)         new_x=0;
                if(new_x>=width-1)  new_x=width-2;
                if(new_y<0)         new_y=0;
                if(new_y>=height-1) new_y=height-2;

                x1=(int)new_x;
                y1=(int)new_y;

                p=new_x-x1;
                q=new_y-y1;

                for (int k=0; k<3; k++)
                {
                    Img_out.at<Vec3b>(y, x)[k]=(1-p)*(1-q)*Img.at<Vec3b>(y1, x1)[k]+
                                            (p)*(1-q)*Img.at<Vec3b>(y1,x1+1)[k]+
                                            (1-p)*(q)*Img.at<Vec3b>(y1+1,x1)[k]+
                                            (p)*(q)*Img.at<Vec3b>(y1+1,x1+1)[k];
                }

        }
    }

    Mat dst = Img_out.clone();
	return dst;
}

//黑白
Mat CFilterEffect::blackSideEffect(Mat src)
{
    Mat Img_in(src);
  
    Mat Img_out(Img_in.size(), CV_32FC3);
    Img_in.convertTo(Img_out, CV_32FC3);

    Mat R(Img_in.size(),CV_32FC1);
    Mat G(Img_in.size(),CV_32FC1);
    Mat B(Img_in.size(),CV_32FC1);

    Mat I(Img_in.size(),CV_32FC1);

    Mat BW_out(Img_in.size(), CV_32FC1);

    Mat rgb[]={B, G, R};
    cv::split(Img_out, rgb);

    I=B+G+R;

    float maxVal, minVal, midVal;

    float color_ratio[6]={0.4,0.6,0.4,0.6,0.2,0.8};
    float r_max_mid, r_max;
    int Ind;

    for(int i=0; i<I.rows; i++)
    {
        for(int j=0; j<I.cols; j++)
        {
            maxVal=std::max(R.at<float>(i,j), std::max(G.at<float>(i,j),
                                                       B.at<float>(i,j)));
            minVal=std::min(R.at<float>(i,j), std::min(G.at<float>(i,j),
                                                       B.at<float>(i,j)));
            midVal=I.at<float>(i,j)-maxVal-minVal;

            if(minVal==R.at<float>(i,j))
            {
                Ind=0;
            }
            else if(minVal==G.at<float>(i,j))
            {
                Ind=2;
            }
            else
            {
                Ind=4;
            }
            r_max_mid=color_ratio[(Ind+3)%6+1];

            if(maxVal==R.at<float>(i,j))
            {
                Ind=1;
            }
            else if(maxVal==G.at<float>(i,j))
            {
                Ind=3;
            }
            else
            {
                Ind=5;
            }

            r_max=color_ratio[Ind];

            BW_out.at<float>(i,j)=(maxVal-midVal)*r_max+(midVal-minVal)
                      *r_max_mid+minVal;

        }
    }

    BW_out=BW_out/255;
  
	Mat dst = src.clone();
	BW_out =	BW_out*255;
	BW_out.convertTo(dst,CV_8UC3);
	return dst;

}

//油画效果
Mat CFilterEffect::painterly(Mat src)
{
	Mat OriginalImage(src);
	Mat dst(src);
	//
	//遍历所有像素  
	for(int j=0;j<OriginalImage.rows-2;j++)
	{
		//const uchar*current=OriginalImage.ptr<const uchar>(j);//当前行
		const uchar*next1=OriginalImage.ptr<const uchar>(j+1);//后1行
		const uchar*next2=OriginalImage.ptr<const uchar>(j+2);//后两行
		const uchar*next=next1;
		uchar *output=dst.ptr<uchar>(j);
		for(int i=0;i<OriginalImage.cols-2;++i)
		{
			//随机使用相邻点 2.5piexl内
			if(rand()%2)
             next=next2;
			else next=next1;
			int count=3*(i+1);
			if(rand()%2)
				count=3*(i+2);
			else count=3*(i+1);

			for (int ch=0;ch<3;++ch)
			{
				output[3*i+ch]=next[count+ch];
			}
		}//end for(int i=0;i<OriginalImage.cols-2;++i)
	}//end for(int j=0;j<OriginalImage.rows-2;j++)

	return dst;
}

Mat CFilterEffect::painterly2(Mat mat_src)
{
	Mat mat_dst;
	float fResize = 0.5;
	Mat matResize;
	Mat matEffect;
	fResize = 0.5;
	matResize;
	cv::resize(mat_src,matResize,cv::Size(mat_src.cols * fResize, mat_src.rows * fResize),0,0,CV_INTER_LINEAR);
	matEffect = CFilterEffect::painterly(matResize);
	cv::resize(matEffect,mat_dst,cv::Size(mat_src.cols, mat_src.rows ),0,0,CV_INTER_LINEAR);
	return mat_dst;
}
