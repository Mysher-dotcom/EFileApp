#include "stdafx.h"
#include "SharpImage.h"


//输入形参为pass by conference-const，保证输入的图像不会被修改，并且为传递引用
void CSharpImage::sharpenImage0(const cv::Mat &image, cv::Mat &result)
{
    //为输出图像分配内存
    result.create(image.size(),image.type());
    
    /*滤波核为拉普拉斯核3x3：
                             0 -1 0
                            -1 5 -1
                             0 -1 0   
    */
    for(int j= 1; j<image.rows-1; ++j)
    {
        const uchar *previous = image.ptr<const uchar>(j-1);
        const uchar *current = image.ptr<const uchar>(j);
        const uchar *next = image.ptr<const uchar>(j+1);
        uchar *output = result.ptr<uchar>(j);
        for(int i= 1; i<image.cols-1; ++i)
        {
            *output++ = cv::saturate_cast<uchar>(5*current[i]-previous[i]-next[i]-current[i-1]-current[i+1]);  //saturate_cast<uchar>()保证结果在uchar范围内
        }
    }
    result.row(0).setTo(cv::Scalar(0));
    result.row(result.rows-1).setTo(cv::Scalar(0));
    result.col(0).setTo(cv::Scalar(0));
    result.col(result.cols-1).setTo(cv::Scalar(0));
}

void CSharpImage::sharpenImage1(const cv::Mat &image, cv::Mat &result)
{
    //创建并初始化滤波模板
    cv::Mat kernel(3,3,CV_32F,cv::Scalar(0));
    kernel.at<float>(1,1) = 5.0;
    kernel.at<float>(0,1) = -1.0;
    kernel.at<float>(1,0) = -1.0;
    kernel.at<float>(1,2) = -1.0;
    kernel.at<float>(2,1) = -1.0;

    result.create(image.size(),image.type());
    
    //对图像进行滤波
    cv::filter2D(image,result,image.depth(),kernel);
}

