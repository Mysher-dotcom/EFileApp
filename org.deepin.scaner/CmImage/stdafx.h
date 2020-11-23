#ifndef STDAFX_H
#define STDAFX_H

#ifndef RGBQUAD
    typedef struct tagRGBQUAD{
        unsigned char rgbBlue;
        unsigned char rgbGreen;
        unsigned char rgbRed;
        unsigned char rgbReserved;
    }RGBQUAD;
#endif
#include "opencv2/opencv.hpp"
using namespace cv;

#include <iostream>
#include <time.h>
#include <vector>
using namespace std;


#include "./ImgProc/MImageDataPool.h"
#include "MImage.h"
#include "./ImgProc/Adapter.h"

#include "./ImgProc/AdaptiveThreshold.h"

#include "./ImgProc/ImageDecoder.h"
#include "./ImgProc/DetectRectByContours_new.h"

#include "./ImgProc/Rotate.h"

#endif // STDAFX_H
