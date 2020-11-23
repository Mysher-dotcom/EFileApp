#include "stdafx.h"
#include "AdaptiveThreshold.h"
//V0.0.1


//局部阈值
//参数:
//[in] src		:	源图像	
//[in] nMethod	:	求阈值方式
//			0	:	采用后面4种方式加权值（对于颜色较淡的图像有奇效）
//			1	:	Ostu 方法【XZhang 01】【Xia 99】
//			2	:	Kittler 和 Illingworth 方法【Kittler and Illingworth 86】
//			3	:	最大熵
//			4	:	位差
//[in] pBox		:	区域
//返回值:
//二值化后图像
Mat CAdaptiveThreshold::AdaptiveThreshold(const Mat &src, int method, int nBoxSize, int nBias, float fGlobalLocalBalance)
{
	Mat dst;
	if(!src.data)
	{
		return dst;
	}

    //IplImage *iplSrc =  &(IplImage)src;
    IplImage iplTmp = (IplImage)src;
    IplImage *iplSrc = &iplTmp;
	
	IplImage *iplDst =  cvCreateImage(cvSize(src.cols,src.rows),8,1);
	if(AdaptiveThreshold(iplSrc, iplDst, method, nBoxSize, nBias, fGlobalLocalBalance))
	{
		dst = cv::cvarrToMat(iplDst,true);  
	}

	if(iplDst!= NULL)
	{
		cvReleaseImage(&iplDst);
	}

	return dst;
}

//最佳阈值
int  CAdaptiveThreshold::OptimalThreshold(IplImage * src,int method,RECT * pBox)
{
    if(src==NULL||src->nChannels!=1)
    {
        return 128;
    }
    double p[256];
    memset(p,  0, 256*sizeof(double));

    int xmin,xmax,ymin,ymax;
    if (pBox){
        xmin = pBox->left>0?pBox->left:0;
        xmax = pBox->right<src->width?pBox->right:src->width;
        ymin = pBox->bottom>0?pBox->bottom:0;
        ymax = pBox->top<src->height?pBox->top:src->height;
    } else {
        xmin = ymin = 0;
        xmax = src->width;
        ymax=src->height;
    }

    if (xmin>=xmax || ymin>=ymax)
        return -1;

    //build histogram
    for (int y = ymin; y<ymax; y++){
        uchar* pGray = (uchar*)src->imageData+y*src->widthStep + xmin;
        uchar* pContr = 0;
        for (int x = xmin; x<xmax; x++){
            uchar n = *pGray++;
            if (pContr){
                if (*pContr) p[n]++;
                pContr++;
            } else {
                p[n]++;
            }
        }
    }

    //find histogram limits
    int gray_min = 0;
    while (gray_min<255 && p[gray_min]==0) gray_min++;
    int gray_max = 255;
    while (gray_max>0 && p[gray_max]==0) gray_max--;
    if (gray_min > gray_max)
        return -1;
    if (gray_min == gray_max){
        if (gray_min == 0)
            return 0;
        else
            return gray_max-1;
    }

    //compute total moments 0th,1st,2nd order
    int i,k;
    double w_tot = 0;
    double m_tot = 0;
    double q_tot = 0;
    for (i = gray_min; i <= gray_max; i++){
        w_tot += p[i];
        m_tot += i*p[i];
        q_tot += i*i*p[i];
    }

    double L, L1max, L2max, L3max, L4max; //objective functions
    int th1,th2,th3,th4; //optimal thresholds
    L1max = L2max = L3max = L4max = 0;
    th1 = th2 = th3 = th4 = -1;

    double w1, w2, m1, m2, q1, q2, s1, s2;
    w1 = m1 = q1 = 0;
    for (i = gray_min; i < gray_max; i++){
        w1 += p[i];
        w2 = w_tot - w1;
        m1 += i*p[i];
        m2 = m_tot - m1;
        q1 += i*i*p[i];
        q2 = q_tot - q1;
        s1 = q1/w1-m1*m1/w1/w1; //s1 = q1/w1-pow(m1/w1,2);
        s2 = q2/w2-m2*m2/w2/w2; //s2 = q2/w2-pow(m2/w2,2);

        //Otsu
        L = -(s1*w1 + s2*w2); //implemented as definition
        //L = w1 * w2 * (m2/w2 - m1/w1)*(m2/w2 - m1/w1); //implementation that doesn't need s1 & s2
        if (L1max < L || th1<0){
            L1max = L;
            th1 = i;
        }

        //Kittler and Illingworth
        if (s1>0 && s2>0){
            L = w1*log(w1/sqrt(s1))+w2*log(w2/sqrt(s2));
            //L = w1*log(w1*w1/s1)+w2*log(w2*w2/s2);
            if (L2max < L || th2<0){
                L2max = L;
                th2 = i;
            }
        }

        //max entropy
        L = 0;
        for (k=gray_min;k<=i;k++) if (p[k] > 0)	L -= p[k]*log(p[k]/w1)/w1;
        for (k;k<=gray_max;k++) if (p[k] > 0)	L -= p[k]*log(p[k]/w2)/w2;
        if (L3max < L || th3<0){
            L3max = L;
            th3 = i;
        }

        //potential difference (based on Electrostatic Binarization method by J. Acharya & G. Sreechakra)
        // L=-fabs(vdiff/vsum); ?molto selettivo, sembra che L=-fabs(vdiff) o L=-(vsum)
        // abbiano lo stesso valore di soglia... il che semplificherebbe molto la routine
        double vdiff = 0;
        for (k=gray_min;k<=i;k++)
            vdiff += p[k]*(i-k)*(i-k);
        double vsum = vdiff;
        for (k;k<=gray_max;k++){
            double dv = p[k]*(k-i)*(k-i);
            vdiff -= dv;
            vsum += dv;
        }
        if (vsum>0) L = -fabs(vdiff/vsum); else L = 0;
        if (L4max < L || th4<0){
            L4max = L;
            th4 = i;
        }
    }

    int threshold;
    switch (method){
    case 1: //Otsu
        threshold = th1;
        break;
    case 2: //Kittler and Illingworth
        threshold = th2;
        break;
    case 3: //max entropy
        threshold = th3;
        break;
    case 4: //potential difference
        threshold = th4;
        break;
    default: //auto
        {
            int nt = 0;
            threshold = 0;
            if (th1>=0) { threshold += th1; nt++;}
            if (th2>=0) { threshold += th2; nt++;}
            if (th3>=0) { threshold += th3; nt++;}
            if (th4>=0) { threshold += th4; nt++;}
            if (nt)
                threshold /= nt;
            else
                threshold = (gray_min+gray_max)/2;

            /*better(?) but really expensive alternative:
            n = 0:255;
            pth1 = c1(th1)/sqrt(2*pi*s1(th1))*exp(-((n - m1(th1)).^2)/2/s1(th1)) + c2(th1)/sqrt(2*pi*s2(th1))*exp(-((n - m2(th1)).^2)/2/s2(th1));
            pth2 = c1(th2)/sqrt(2*pi*s1(th2))*exp(-((n - m1(th2)).^2)/2/s1(th2)) + c2(th2)/sqrt(2*pi*s2(th2))*exp(-((n - m2(th2)).^2)/2/s2(th2));
            ...
            mse_th1 = sum((p-pth1).^2);
            mse_th2 = sum((p-pth2).^2);
            ...
            select th# that gives minimum mse_th#
            */

        }
    }

    if (threshold <= gray_min || threshold >= gray_max)
        threshold = (gray_min+gray_max)/2;

    return threshold;
}


//根据mask，对每个像素进行阈值化
//mask尺寸和原图相同
bool CAdaptiveThreshold::Threshold(IplImage* src,IplImage* dst,IplImage* mask)
{
    if(src==NULL||dst==NULL||dst->nChannels !=1 ||mask == NULL||
        src->width!=dst->width||src->height!=dst->height)
    {
        return false;
    }

    CvSize size = cvGetSize(src);
    IplImage* gray = cvCreateImage(size,8,1);
    if(src->nChannels == 3)
    {
        cvCvtColor(src,gray,CV_BGR2GRAY);
    }
    else
    {
        cvCopy(src,gray);
    }
    uchar* pg  = (uchar*)gray->imageData;
    uchar* pd = (uchar*)dst->imageData;
    uchar* pm = (uchar*)mask->imageData;

    double dstep = dst->widthStep;

    for(int i = 0; i < dst->height; i++)
    {
        for (int j =0;j<dst->width;j++)
        {
            int npos = i*dstep+j;
            if( pg[npos] > pm[npos] )
            {
                pd[npos] = 255;
            }
            else
            {
                pd[npos] = 0;
            }
        }
    }

    cvReleaseImage(&gray);
    return true;
}

//局部阈值
bool CAdaptiveThreshold::AdaptiveThreshold(IplImage* src,IplImage* dst,int method, int nBoxSize, int nBias, float fGlobalLocalBalance)
{
    if(src==NULL||dst==NULL||dst->nChannels !=1 ||
        src->width!=dst->width||src->height!=dst->height)
    {
        return false;
    }

    if (nBoxSize<8) nBoxSize = 8;
    if (fGlobalLocalBalance<0.0f) fGlobalLocalBalance = 0.0f;
    if (fGlobalLocalBalance>1.0f) fGlobalLocalBalance = 1.0f;

    int mw = (src->width + nBoxSize - 1)/nBoxSize;
    int mh = (src->height + nBoxSize - 1)/nBoxSize;

    CvSize size = cvGetSize(src);
    IplImage* mask = cvCreateImage(cvSize(mw,mh),8,1);
    IplImage* gray = cvCreateImage(size,8,1);
    if(src->nChannels == 3)
    {
        cvCvtColor(src,gray,CV_BGR2GRAY);
    }
    else
    {
        cvCopy(src,gray);
    }

    //180,100  如果识别灰度图像要降低方差，识别二值图像要提高方差
    //CropNormalize(gray,180,40);

    int globalthreshold = OptimalThreshold(gray,method, 0);
    if (globalthreshold <0)
        return false;

    uchar* pg  = (uchar*)gray->imageData;
    uchar* pm = (uchar*)mask->imageData;

    for (int y=0; y<mh; y++){
        for (int x=0; x<mw; x++){
            int npos = y*mask->widthStep+x;
            RECT r;
            r.left = x*nBoxSize;
            r.right = r.left + nBoxSize;
            r.bottom = y*nBoxSize;
            r.top = r.bottom + nBoxSize;
            int threshold = OptimalThreshold(gray,method, &r);
            if (threshold <0) return false;

            double dt0 = nBias+((1.0f-fGlobalLocalBalance)*threshold + fGlobalLocalBalance*globalthreshold);
            double dt1 = 255<dt0?255:dt0;
            double dt = 0>dt1?0:dt1;
            pm[npos] = dt;
        }
    }

    //mask.Resample(mw*nBoxSize,mh*nBoxSize,0);
    //mask.Crop(0,head.biHeight,head.biWidth,0);

    //IplImage * mask1 = cvCreateImage(cvSize(mw*nBoxSize,mh*nBoxSize),mask->depth,mask->nChannels);

    IplImage * mask1 = cvCreateImage(cvSize(src->width,src->height),mask->depth,mask->nChannels);
    cvResize(mask,mask1,CV_INTER_CUBIC);

    ////第一步：将需要剪切的图像图像不部分设置为ROI
    //cvSetImageROI(mask1 , cvRect(0,0,src->width,src->height));
    ////第二步：新建一个与需要剪切的图像部分同样大小的新图像
    //IplImage * mask2 = cvCreateImage(cvSize(src->width,src->height),src->depth,src->nChannels);
    ////第三步：将源图像复制到新建的图像中
    //cvCopy(mask1,mask2,0);
    ////第四步：释放ROI区域
    //cvResetImageROI(mask1);

    if(!Threshold(gray,dst,mask1))
    {
        cvReleaseImage(&mask);
        cvReleaseImage(&gray);
        cvReleaseImage(&mask1);
        return false;
    }
    cvReleaseImage(&mask);
    cvReleaseImage(&gray);
    cvReleaseImage(&mask1);

    return true;
}
