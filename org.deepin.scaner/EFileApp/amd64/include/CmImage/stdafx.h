#ifndef STDAFX_H
#define STDAFX_H

/*#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>*/
#include "opencv2/opencv.hpp"
using namespace cv;

#include <iostream>
#include <time.h>
#include <vector>
using namespace std;


#include "./Model/MImageModel/MImageDataPool.h"
#include "MImage.h"
#include "./Model/MImageModel/Adapter.h"
//#include "./Model/MImageModel/Jpeg.h"

#include "./ImgProc/ColorSpace/AdaptiveThreshold.h"

#include "./ImgProc/Enhancement/ColorSpace.hpp"
#include "./ImgProc/Enhancement/Curves.hpp"
#include "./ImgProc/Enhancement/DocumentBackgroundSeparation.h"
#include "./ImgProc/Enhancement/FilterEffect.h"
#include "./ImgProc/Enhancement/Histogram.h"
#include "./ImgProc/Enhancement/HSL.hpp"
#include "./ImgProc/Enhancement/SharpImage.h"
#include "./ImgProc/Enhancement/Halftone.h"
#include "./ImgProc/Enhancement/AutoLevel.h"
#include "./ImgProc/Enhancement/BrightnessBalance.h"
#include "./ImgProc/Enhancement/newColorEnhance.h"
#include "./ImgProc/Enhancement/ImageEnhancement.h"
#include "./ImgProc/Enhancement/ImgAdjust.h"
#include "./ImgProc/Enhancement/ColorEnhance.h"
#include "./ImgProc/Enhancement/FilterEffect.h"
#include "./ImgProc/Enhancement/Noise.h"
#include "./ImgProc/Enhancement/RemoveNoise.h"
#include "./ImgProc/Enhancement/DeleteNoise_BW.h"
#include "./ImgProc/Enhancement/DetectionWhitePage.h"
#include "./ImgProc/Enhancement/ImageDecoder.h"
#include "./ImgProc/Enhancement/Compare.h"
//#include "./ImgProc/Enhancement/WaterMark.h"

#include "./ImgProc/Feature/AdjustSkew.h"
#include "./ImgProc/Feature/DetectEdge.h"
#include "./ImgProc/Feature/DetectRectByContours.h"
#include "./ImgProc/Feature/DetectRectByContours_new.h"
#include "./ImgProc/Feature/DetectRectBySegmation.h"
#include "./ImgProc/Feature/FillBorder.h"
#include "./ImgProc/Feature/PunchHold.h"

#include "./ImgProc/Transform/CropRectByCV.h"
#include "./ImgProc/Transform/MergerImage.h"
#include "./ImgProc/Transform/Rotate.h"


#include "./ImgProc/WaterMark/CvText.h"


//#include "Model/MImageModel/MImageDataPool.h"


#include "./ImgProc/BookProcess/BookProcess.h"
#include "./ImgProc/BookProcess/Test_Book.h"


#endif // STDAFX_H
