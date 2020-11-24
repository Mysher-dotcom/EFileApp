#include "stdafx.h"
#include "RemoveNoise.h"

//去噪
bool CRemoveNoise::removeNoise(Mat src,Mat &dst,int reps /* =1 */,int kernel /* =3 */)
{
	//形态学运算核
	Mat se(kernel,kernel,CV_8U,Scalar(1));
	//形态学运算次数
	for(int idx =0;idx <reps;idx++)
		morphologyEx(src,dst,cv::MORPH_CLOSE,se);

	return true;
}

