#include "stdafx.h"
#include "Halftone.h"
//  15-210
int HalftoneDarkerPhoto8x8[8][8] = { { 89, 46, 52, 95, 123, 160, 167, 130}, 
                  { 40, 15, 21, 58, 154, 198, 204, 173 }, 
                  { 83, 34, 27, 65, 148, 191, 210, 179},
                  { 108, 77, 71, 102, 117, 142, 185, 136},
                  {120, 157, 164, 126, 92, 49, 55, 99},
                  {151, 195, 201, 170, 43, 18, 24, 61},
                  {145, 188, 207, 176, 86, 37, 30, 68},
                  {114, 139, 182, 133, 111, 80, 74, 105},
                };
       

//  15-230
int HalftoneDarkerPhotoText8x8[8][8] = { { 97, 49, 56, 104, 134, 175, 182, 141},
                  { 42, 15, 22, 63, 169, 216, 223, 189},
                  { 90, 35, 29, 70, 162, 210, 230, 196},
                  {117, 83, 76, 111, 128, 155, 203, 148},
                  {131, 172, 179, 138, 100, 53, 59, 107},
                  {165, 213, 220, 186, 46, 18, 25, 66},
                  {158, 206, 227, 192, 93, 39, 32, 73},
                  {124, 152, 199, 145, 121, 87, 80, 114},
                };
      
//  1-210
int HalftoneLighterPhoto8x8[8][8] = { { 81, 34, 41, 87, 117, 157, 164, 124},
                  { 28,  1,  8, 47, 150, 197, 203, 170},
                  { 74, 21, 14, 54, 144, 190, 210, 177},
                  {101, 67, 61, 94, 110, 137, 183, 130},
                  {114, 154, 160, 120, 84, 37, 44, 91},
                  {147, 193, 200, 167, 31,  4, 11, 51},
                  {140, 187, 207, 174, 77, 24, 18, 57},
                  {107, 134, 180, 127, 104, 71, 64, 97},
                };

// 30-235
int HalftoneLighterPhotoText8x8[8][8] = { { 108, 63, 69, 115, 144, 183, 189, 150},
                  { 56, 30, 37, 76, 176, 222, 228, 196},
                  { 102, 50, 43, 82, 170, 215, 235, 202},
                  { 128, 95, 89, 121, 137, 163, 209, 157},
                  {141, 180, 186, 147, 111, 66, 72, 118},
                  {173, 219, 225, 193,  59, 33, 40, 79},
                  {167, 212, 232, 199, 105, 53, 46, 85},
                  {134, 160, 206, 154, 131, 98, 92, 124},
                };

double Prewitt3x3Horizontal[3][3] = { { -1,  0,  1, }, 
                  { -1,  0,  1, }, 
                  { -1,  0,  1, }, };

double Prewitt3x3Vertical[3][3] = { {  1,  1,  1, }, 
                  {  0,  0,  0, }, 
                  { -1, -1, -1, }, };

//Convert to 1 bit by Halftone
//参数说明:
//input:
//nFilterMatrix:
// 0 - DarkerPhoto
// 1 - DarkerPhoto + Text
// 2 - LighterPhoto
// 3 - LighterPhot + Text
// 4 - ErrorDiffusion
//nEdgeThreshold:
//取值范围:<40
//nFilterThreshold:
//取值范围:[0,255]
//返回值:
//二值化后的灰度图
Mat Halftone::ConvertTo1BitHalftone(Mat input, int nColorChannel,int nFilterMatrix, int nEdgeThreshold, int nFilterThreshold)
{
	if (nColorChannel > 3 || nColorChannel < 0)
        nColorChannel = 3;
    if (nFilterThreshold > 40)
        nFilterThreshold = 40;
    if (nEdgeThreshold > 255) nEdgeThreshold = 255;
    if (nEdgeThreshold < 0) nEdgeThreshold = 0;

	Mat resultBmp;// = input.clone();

    bool bIsColorBmp = true;
    if (input.channels() == 1)
        bIsColorBmp = false;

	if (nEdgeThreshold > 0)
    {
        if (bIsColorBmp)
		{
            input = EdgeEnhanceConvolutionFilter(input, nColorChannel, Prewitt3x3Horizontal, Prewitt3x3Vertical, nEdgeThreshold, nFilterThreshold);
		}
		else
		{
            input = EdgeEnhanceConvolutionFilterWithGray(input, Prewitt3x3Horizontal, Prewitt3x3Vertical, nEdgeThreshold);
		}
        nFilterThreshold = 0;
    }


	switch (nFilterMatrix)
    {
        case 0:  // Darker Photo
		default:
            if (bIsColorBmp)
			{
                resultBmp = ColorConvertTo1BitWithHalfTone8x8(input, nColorChannel, HalftoneDarkerPhoto8x8, nFilterThreshold);
			}
			else
			{
				resultBmp = GrayConvertTo1BitWithHalfTone8x8(input, HalftoneDarkerPhoto8x8);
			}
			break;
		case 1:  // Darker Photo+Text
			if (bIsColorBmp)
			{
                resultBmp = ColorConvertTo1BitWithHalfTone8x8(input, nColorChannel, HalftoneDarkerPhotoText8x8, nFilterThreshold);
			}
			else
			{
				resultBmp = GrayConvertTo1BitWithHalfTone8x8(input, HalftoneDarkerPhotoText8x8);
			}
			break;
		case 2:  // Lighter Photo
			if (bIsColorBmp)
			{
                resultBmp = ColorConvertTo1BitWithHalfTone8x8(input, nColorChannel, HalftoneLighterPhoto8x8, nFilterThreshold);
			}
			else
			{
				resultBmp = GrayConvertTo1BitWithHalfTone8x8(input, HalftoneLighterPhoto8x8);
			}
			break;
		case 3:  // Lighter Photo+Text
			if (bIsColorBmp)
			{
                resultBmp = ColorConvertTo1BitWithHalfTone8x8(input, nColorChannel, HalftoneLighterPhotoText8x8, nFilterThreshold);
			}
			else
			{
				resultBmp = GrayConvertTo1BitWithHalfTone8x8(input, HalftoneLighterPhotoText8x8);
			}
			break;
		case 4://ErrorDiffusion
			if (bIsColorBmp)
			{
                resultBmp = ColorConvertTo1BitWithHalfTone8x8(input, nColorChannel, HalftoneLighterPhotoText8x8, nFilterThreshold);
			}
			else
			{
				resultBmp = GrayConvertTo1BitWithErrorDiffusion(input);
			}
			break;
	};

	return resultBmp;
}

Mat Halftone::ColorConvertTo1BitWithHalfTone8x8(const Mat &input, int nColorChannel, int filterMatrix[8][8], int nFilterThreshold)
{
	if (nColorChannel < 0 || nColorChannel > 3)
                nColorChannel = 3;

    if (nFilterThreshold > 40)
        nFilterThreshold = 40;

    int nColor1 = 0;
    int nColor2 = 2;
    switch (nColorChannel)
    {
        case 0:
            nColor1 = 1;
            nColor2 = 2;
            break;
        case 1:
            nColor1 = 0;
            nColor2 = 2;
            break;
        case 2:
            nColor1 = 0;
            nColor2 = 1;
            break;
    }
    int intColor1;
    int intColor2;

	Mat output = Mat(input.rows, input.cols, CV_8UC1);

    long lIDXA = 0;
    //long lIDXB = 0;
    // Ordered Dither
    int intY = 0;
    int intX = 0;
    int intODY = 0;
    int intODX = 0;
    int intGray = 0;
    //int  intOD    = 0;
    // Ordered Dither Matrix
    int aryOrderedDither[8][8];
	::memcpy(aryOrderedDither, filterMatrix,sizeof(aryOrderedDither));

    uchar* inputBmpPtr = input.data;
    uchar* outputBmpPtr = output.data;
    int width = input.cols;
    int height = input.rows;
	int inputStride = input.step;
	int outputStride = output.step;

    //var data = new schar[input.Width, input.Height];
    // The height of the image. But we are stepping to three once.
    for (intY = 0; intY < height; intY += 8)
    {
        // The width of the image. But we are stepping to three once.
        for (intX = 0; intX < width; intX += 8)
        {
            // The height of the matrix.
            for (intODY = 0; intODY < 8 && intODY < (height - intY); intODY++)
            {
                // The width of the matrix.
                for (intODX = 0; intODX < 8 && intODX < (width - intX); intODX++)
                {
                    // The index of pixel, because we use the three depth bit to present one pixel of color,
                    // Therefore, we have to multiple three.
                    lIDXA = ((intX + intODX) * 3) + ((intY + intODY) * inputStride);
                    if (nColorChannel == 3)
                    {
                        intGray = (int)((double)inputBmpPtr[lIDXA + 0] * 0.114 + (double)inputBmpPtr[lIDXA + 1] * 0.587 + (double)inputBmpPtr[lIDXA + 2] * 0.299);
                        if (intGray > 255) intGray = 255;
                    }
                    else
                    {
                        // To get the pixel depth of the blue channel,
                        intGray = inputBmpPtr[lIDXA + nColorChannel];  // green Channel
                    }

                    intColor1 = inputBmpPtr[lIDXA + nColor1];
                    intColor2 = inputBmpPtr[lIDXA + nColor2];

                    if (nFilterThreshold > 0
                        && intGray - intColor1 > nFilterThreshold && intGray - intColor2 > nFilterThreshold
                        && nColorChannel != 3)
                    {
                        //outputBmpPtr[((intX + intODX) / 8) + (intY + intODY) * outputStride] |= masks[(intX + intODX) % 8];
						outputBmpPtr[((intX + intODX)) + (intY + intODY) * outputStride]= 255;
                    }
                    // If the gray depth more than the matrix, it should be white.
                    else if (intGray > aryOrderedDither[intODY][intODX])
                    {
                        //intOD = 255;
                        //outputBmpPtr[((intX + intODX) / 8) + (intY + intODY) * outputStride] |= masks[(intX + intODX) % 8];
						outputBmpPtr[((intX + intODX)) + (intY + intODY) * outputStride]= 255;
					}
                    // Otherwise, it should be black.
                    else
                    {
                        //intOD = 0;
                        //outputBmpPtr[((intX + intODX) / 8) + (intY + intODY) * outputStride] |= 0;
						outputBmpPtr[((intX + intODX)) + (intY + intODY) * outputStride]= 0;
                    }
                } // for ( intODX = 0; intODX < 3; intODX++ )
            } // for ( intODY = 0; intODY < 3; intODY++ )
        } // for ( intX = 0; intX < imageA->DibInfo->bmiHeader.biWidth - 3; intX+=3 )
    } // for ( intY = 0; intY < imageA->DibInfo->bmiHeader.biHeight - 3; intY+=3 )
                
                    
            

	return output;
}

Mat Halftone::ColorConvertTo1BitWithErrorDiffusion(const Mat &input, int nColorChannel, int nFilterThreshold)
{
	if (nColorChannel < 0 || nColorChannel > 3)
        nColorChannel = 3;

    if (nFilterThreshold > 40)
        nFilterThreshold = 40;

    int nColor1 = 0;
    int nColor2 = 2;
    switch (nColorChannel)
    {
        case 0:
            nColor1 = 1;
            nColor2 = 2;
            break;
        case 1:
            nColor1 = 0;
            nColor2 = 2;
            break;
        case 2:
            nColor1 = 0;
            nColor2 = 1;
            break;
    }
    int intColor1;
    int intColor2;

	Mat output = Mat(input.rows, input.cols, CV_8UC1);
              
    uchar* inputBmpPtr = input.data;
    uchar* outputBmpPtr = output.data;
    int width = input.cols;
    int height = input.rows;
	int inputStride = input.step;
	int outputStride = output.step;
    double dbGray = 0.0f;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (nColorChannel == 3)
            {
                dbGray = 0.0;
                dbGray += (double)inputBmpPtr[x * 3 + 0 + y * inputStride] * 0.114;
                dbGray += (double)inputBmpPtr[x * 3 + 1 + y * inputStride] * 0.587;
                dbGray += (double)inputBmpPtr[x * 3 + 2 + y * inputStride] * 0.299;

                if (dbGray - 128 > 0)
                    outputBmpPtr[x + y * outputStride] = 255;
                else outputBmpPtr[x + y * outputStride] = 0;

                double error;
                if (dbGray - 128 > 0)
                    error = (dbGray - 255);// - (j ? 32 : -32));
                else
                    error = (dbGray - 0);// - (j ? 32 : -32));

                if (x < width - 1)
                {
                    dbGray = 0;
                    dbGray += (double)inputBmpPtr[(x + 1) * 3 + 0 + y * inputStride] * 0.114;
                    dbGray += (double)inputBmpPtr[(x + 1) * 3 + 1 + y * inputStride] * 0.587;
                    dbGray += (double)inputBmpPtr[(x + 1) * 3 + 2 + y * inputStride] * 0.299;

                    if ((dbGray + 7 * error / 16) > 255)
                    {
                        inputBmpPtr[(x + 1) * 3 + 0 + y * inputStride] = 255;
                        inputBmpPtr[(x + 1) * 3 + 1 + y * inputStride] = 255;
                        inputBmpPtr[(x + 1) * 3 + 2 + y * inputStride] = 255;
                    }
                    else if ((dbGray + 7 * error / 16) < 0)
                    {
                        inputBmpPtr[(x + 1) * 3 + 0 + y * inputStride] = 0;
                        inputBmpPtr[(x + 1) * 3 + 1 + y * inputStride] = 0;
                        inputBmpPtr[(x + 1) * 3 + 2 + y * inputStride] = 0;
                    }
                    else
                    {
                        inputBmpPtr[(x + 1) * 3 + 0 + y * inputStride] = (char)(dbGray + 7 * error / 16);
                        inputBmpPtr[(x + 1) * 3 + 1 + y * inputStride] = (char)(dbGray + 7 * error / 16);
                        inputBmpPtr[(x + 1) * 3 + 2 + y * inputStride] = (char)(dbGray + 7 * error / 16);
                    }
                }
                if (y < height - 1)
                {
                    if (x > 0)
                    {
                        dbGray = 0;
                        dbGray += (double)inputBmpPtr[(x - 1) * 3 + 0 + (y + 1) * inputStride] * 0.114;
                        dbGray += (double)inputBmpPtr[(x - 1) * 3 + 1 + (y + 1) * inputStride] * 0.587;
                        dbGray += (double)inputBmpPtr[(x - 1) * 3 + 2 + (y + 1) * inputStride] * 0.299;

                        if ((dbGray + 3 * error / 16) > 255)
                        {
                            inputBmpPtr[(x - 1) * 3 + 0 + (y + 1) * inputStride] = 255;
                            inputBmpPtr[(x - 1) * 3 + 1 + (y + 1) * inputStride] = 255;
                            inputBmpPtr[(x - 1) * 3 + 2 + (y + 1) * inputStride] = 255;
                        }
                        else if ((dbGray + 3 * error / 16) < 0)
                        {
                            inputBmpPtr[(x - 1) * 3 + 0 + (y + 1) * inputStride] = 0;
                            inputBmpPtr[(x - 1) * 3 + 1 + (y + 1) * inputStride] = 0;
                            inputBmpPtr[(x - 1) * 3 + 2 + (y + 1) * inputStride] = 0;
                        }
                        else
                        {
                            inputBmpPtr[(x - 1) * 3 + 0 + (y + 1) * inputStride] = (char)(dbGray + 3 * error / 16);
                            inputBmpPtr[(x - 1) * 3 + 1 + (y + 1) * inputStride] = (char)(dbGray + 3 * error / 16);
                            inputBmpPtr[(x - 1) * 3 + 2 + (y + 1) * inputStride] = (char)(dbGray + 3 * error / 16);
                        }
                    }

                    dbGray = 0;
                    dbGray += (double)inputBmpPtr[x * 3 + 0 + (y + 1) * inputStride] * 0.114;
                    dbGray += (double)inputBmpPtr[x * 3 + 1 + (y + 1) * inputStride] * 0.587;
                    dbGray += (double)inputBmpPtr[x * 3 + 2 + (y + 1) * inputStride] * 0.299;

                    if ((dbGray + 5 * error / 16) > 255)
                    {
                        inputBmpPtr[x * 3 + 0 + (y + 1) * inputStride] = 255;
                        inputBmpPtr[x * 3 + 1 + (y + 1) * inputStride] = 255;
                        inputBmpPtr[x * 3 + 2 + (y + 1) * inputStride] = 255;
                    }
                    else if ((dbGray + 5 * error / 16) < 0)
                    {
                        inputBmpPtr[x * 3 + 0 + (y + 1) * inputStride] = 0;
                        inputBmpPtr[x * 3 + 1 + (y + 1) * inputStride] = 0;
                        inputBmpPtr[x * 3 + 2 + (y + 1) * inputStride] = 0;
                    }
                    else
                    {
                        inputBmpPtr[x * 3 + 0 + (y + 1) * inputStride] = (char)(dbGray + 5 * error / 16);
                        inputBmpPtr[x * 3 + 1 + (y + 1) * inputStride] = (char)(dbGray + 5 * error / 16);
                        inputBmpPtr[x * 3 + 2 + (y + 1) * inputStride] = (char)(dbGray + 5 * error / 16);
                    }

                    if (x < width - 1)
                    {
                        dbGray = 0;
                        dbGray += (double)inputBmpPtr[(x + 1) * 3 + 0 + (y + 1) * inputStride] * 0.114;
                        dbGray += (double)inputBmpPtr[(x + 1) * 3 + 1 + (y + 1) * inputStride] * 0.587;
                        dbGray += (double)inputBmpPtr[(x + 1) * 3 + 2 + (y + 1) * inputStride] * 0.299;

                        if ((dbGray + 1 * error / 16) > 255)
                        {
                            inputBmpPtr[(x + 1) * 3 + 0 + (y + 1) * inputStride] = 255;
                            inputBmpPtr[(x + 1) * 3 + 1 + (y + 1) * inputStride] = 255;
                            inputBmpPtr[(x + 1) * 3 + 2 + (y + 1) * inputStride] = 255;
                        }
                        else if ((dbGray + 1 * error / 16) < 0)
                        {
                            inputBmpPtr[(x + 1) * 3 + 0 + (y + 1) * inputStride] = 0;
                            inputBmpPtr[(x + 1) * 3 + 1 + (y + 1) * inputStride] = 0;
                            inputBmpPtr[(x + 1) * 3 + 2 + (y + 1) * inputStride] = 0;
                        }
                        else
                        {
                            inputBmpPtr[(x + 1) * 3 + 0 + (y + 1) * inputStride] = (char)(dbGray + 1 * error / 16);
                            inputBmpPtr[(x + 1) * 3 + 1 + (y + 1) * inputStride] = (char)(dbGray + 1 * error / 16);
                            inputBmpPtr[(x + 1) * 3 + 2 + (y + 1) * inputStride] = (char)(dbGray + 1 * error / 16);
                        }
                    }
                }
            }
            else
            {
                intColor1 = inputBmpPtr[x * 3 + nColor1 + y * inputStride];
                intColor2 = inputBmpPtr[x * 3 + nColor2 + y * inputStride];
                if (nFilterThreshold > 0
                    && inputBmpPtr[x * 3 + nColorChannel + y * inputStride] - intColor1 > nFilterThreshold
                    && inputBmpPtr[x * 3 + nColorChannel + y * inputStride] - intColor2 > nFilterThreshold
                    && nColorChannel != 3)
                {
                    outputBmpPtr[(x) + y * outputStride] |= 255;
                }
                else if (inputBmpPtr[x * 3 + nColorChannel + y * inputStride] - 128 > 0)
                    outputBmpPtr[(x) + y * outputStride] |= 255;
                else outputBmpPtr[(x) + y * outputStride] |= 0;

                char error;
                if (inputBmpPtr[x * 3 + nColorChannel + y * inputStride] - 128 > 0)
                    error = (char)(inputBmpPtr[x * 3 + nColorChannel + y * inputStride] - 255);// - (j ? 32 : -32));
                else if (nFilterThreshold > 0
                    && inputBmpPtr[x * 3 + nColorChannel + y * inputStride] - inputBmpPtr[x * 3 + nColor1 + y * inputStride] > nFilterThreshold
                    && inputBmpPtr[x * 3 + nColorChannel + y * inputStride] - inputBmpPtr[x * 3 + nColor2 + y * inputStride] > nFilterThreshold)
                {
                    error = 0;
                }
                else
                    error = (char)(inputBmpPtr[x * 3 + nColorChannel + y * inputStride] - 0);// - (j ? 32 : -32));

                if (x < width - 1)
                {
                    if (nFilterThreshold > 0
                        && inputBmpPtr[(x + 1) * 3 + nColorChannel + y * inputStride] - inputBmpPtr[(x + 1) * 3 + nColor1 + y * inputStride] > nFilterThreshold
                        && inputBmpPtr[(x + 1) * 3 + nColorChannel + y * inputStride] - inputBmpPtr[(x + 1) * 3 + nColor2 + y * inputStride] > nFilterThreshold)
                        inputBmpPtr[(x + 1) * 3 + nColorChannel + y * inputStride] = 255;
                    else if ((inputBmpPtr[(x + 1) * 3 + nColorChannel + y * inputStride] + 7 * error / 16) > 255)
                        inputBmpPtr[(x + 1) * 3 + nColorChannel + y * inputStride] = 255;
                    else if ((inputBmpPtr[(x + 1) * 3 + nColorChannel + y * inputStride] + 7 * error / 16) < 0)
                        inputBmpPtr[(x + 1) * 3 + nColorChannel + y * inputStride] = 0;
                    else
                        inputBmpPtr[(x + 1) * 3 + nColorChannel + y * inputStride] = (char)(inputBmpPtr[(x + 1) * 3 + nColorChannel + y * inputStride] + 7 * error / 16);
                }
                if (y < height - 1)
                {
                    if (x > 0)
                    {
                        if (nFilterThreshold > 0
                            && inputBmpPtr[(x - 1) * 3 + nColorChannel + (y + 1) * inputStride] - inputBmpPtr[(x - 1) * 3 + nColor1 + (y + 1) * inputStride] > nFilterThreshold
                            && inputBmpPtr[(x - 1) * 3 + nColorChannel + (y + 1) * inputStride] - inputBmpPtr[(x - 1) * 3 + nColor2 + (y + 1) * inputStride] > nFilterThreshold)
                            inputBmpPtr[(x - 1) * 3 + nColorChannel + (y + 1) * inputStride] = 255;
                        else if ((inputBmpPtr[(x - 1) * 3 + nColorChannel + (y + 1) * inputStride] + 3 * error / 16) > 255)
                            inputBmpPtr[(x - 1) * 3 + nColorChannel + (y + 1) * inputStride] = 255;
                        else if ((inputBmpPtr[(x - 1) * 3 + nColorChannel + (y + 1) * inputStride] + 3 * error / 16) < 0)
                            inputBmpPtr[(x - 1) * 3 + nColorChannel + (y + 1) * inputStride] = 0;
                        else
                            inputBmpPtr[(x - 1) * 3 + nColorChannel + (y + 1) * inputStride] = (char)(inputBmpPtr[(x - 1) * 3 + nColorChannel + (y + 1) * inputStride] + 3 * error / 16);
                    }
                    if (nFilterThreshold > 0
                        && inputBmpPtr[x * 3 + nColorChannel + (y + 1) * inputStride] - inputBmpPtr[x * 3 + nColor1 + (y + 1) * inputStride] > nFilterThreshold
                        && inputBmpPtr[x * 3 + nColorChannel + (y + 1) * inputStride] - inputBmpPtr[x * 3 + nColor2 + (y + 1) * inputStride] > nFilterThreshold)
                        inputBmpPtr[x * 3 + nColorChannel + (y + 1) * inputStride] = 255;
                    else if ((inputBmpPtr[x * 3 + nColorChannel + (y + 1) * inputStride] + 5 * error / 16) > 255)
                        inputBmpPtr[x * 3 + nColorChannel + (y + 1) * inputStride] = 255;
                    else if ((inputBmpPtr[x * 3 + nColorChannel + (y + 1) * inputStride] + 5 * error / 16) < 0)
                        inputBmpPtr[x * 3 + nColorChannel + (y + 1) * inputStride] = 0;
                    else
                        inputBmpPtr[x * 3 + nColorChannel + (y + 1) * inputStride] = (char)(inputBmpPtr[x * 3 + nColorChannel + (y + 1) * inputStride] + 5 * error / 16);

                    if (x < width - 1)
                    {
                        if (nFilterThreshold > 0
                            && inputBmpPtr[(x + 1) * 3 + nColorChannel + (y + 1) * inputStride] - inputBmpPtr[(x + 1) * 3 + nColor1 + (y + 1) * inputStride] > nFilterThreshold
                            && inputBmpPtr[(x + 1) * 3 + nColorChannel + (y + 1) * inputStride] - inputBmpPtr[(x + 1) * 3 + nColor2 + (y + 1) * inputStride] > nFilterThreshold)
                            inputBmpPtr[(x + 1) * 3 + nColorChannel + (y + 1) * inputStride] = 255;
                        if ((inputBmpPtr[(x + 1) * 3 + nColorChannel + (y + 1) * inputStride] + 1 * error / 16) > 255)
                            inputBmpPtr[(x + 1) * 3 + nColorChannel + (y + 1) * inputStride] = 255;
                        else if ((inputBmpPtr[(x + 1) * 3 + nColorChannel + (y + 1) * inputStride] + 1 * error / 16) < 0)
                            inputBmpPtr[(x + 1) * 3 + nColorChannel + (y + 1) * inputStride] = 0;
                        else
                            inputBmpPtr[(x + 1) * 3 + nColorChannel + (y + 1) * inputStride] = (char)(inputBmpPtr[(x + 1) * 3 + nColorChannel + (y + 1) * inputStride] + 1 * error / 16);
                    }
                }
            }
        }
    }

    return output;
}

Mat Halftone::GrayConvertTo1BitWithHalfTone8x8(const Mat &input, int filterMatrix[8][8])
{
	Mat output = Mat(input.rows, input.cols, CV_8UC1);
	long lIDXA = 0;
    //long lIDXB = 0;
    // Ordered Dither
    int intY = 0;
    int intX = 0;
    int intODY = 0;
    int intODX = 0;
    int intGray = 0;
    //int  intOD    = 0;
    // Ordered Dither Matrix
    int aryOrderedDither[8][8];
	::memcpy(aryOrderedDither, filterMatrix,sizeof(aryOrderedDither));

    uchar* inputBmpPtr = input.data;
    uchar* outputBmpPtr = output.data;
    int width = input.cols;
    int height = input.rows;
	int inputStride = input.step;
	int outputStride = output.step;

    //var data = new schar[input.Width, input.Height];
    // The height of the image. But we are stepping to three once.
    for (intY = 0; intY < height; intY += 8)
    {
        // The width of the image. But we are stepping to three once.
        for (intX = 0; intX < width; intX += 8)
        {
            // The height of the matrix.
            for (intODY = 0; intODY < 8 && intODY < (height - intY); intODY++)
            {
                // The width of the matrix.
                for (intODX = 0; intODX < 8 && intODX < (width - intX); intODX++)
                {
                    // The index of pixel, because we use the three depth bit to present one pixel of color,
                    // Therefore, we have to multiple three.
                    lIDXA = ((intX + intODX)) + ((intY + intODY) * inputStride);
                    // To get the pixel depth of the blue channel,
                    intGray = inputBmpPtr[lIDXA];  // green Channel

                    // If the gray depth more than the matrix, it should be white.
                    if (intGray >(int) aryOrderedDither[intODY][intODX])
                    {
                        //intOD = 255;
                        //outputBmpPtr[((intX + intODX) / 8) + (intY + intODY) * outputStride] |= masks[(intX + intODX) % 8];
						outputBmpPtr[((intX + intODX)) + (intY + intODY) * outputStride]= 255;
					}
                    // Otherwise, it should be black.
                    else
                    {
                        //intOD = 0;
                        //outputBmpPtr[((intX + intODX) / 8) + (intY + intODY) * outputStride] |= 0;
						outputBmpPtr[((intX + intODX)) + (intY + intODY) * outputStride]= 0;
                    }
                } // for ( intODX = 0; intODX < 3; intODX++ )
            } // for ( intODY = 0; intODY < 3; intODY++ )
        } // for ( intX = 0; intX < imageA->DibInfo->bmiHeader.biWidth - 3; intX+=3 )
    } // for ( intY = 0; intY < imageA->DibInfo->bmiHeader.biHeight - 3; intY+=3 )

	return output;
}

Mat Halftone::GrayConvertTo1BitWithErrorDiffusion(Mat input)
{
	Mat output = Mat(input.rows, input.cols, CV_8UC1);
	uchar* outputData = 0;
	uchar* inputData = 0;
	int y,x;
    for( y = 0; y < input.rows; ++y)
    {
        for ( x = 0; x < input.cols; ++x)
        {
			inputData = input.ptr<uchar>(y);
			outputData = output.ptr<uchar>(y);

            if (inputData[x] - 128 > 0)
			{
				//outputData[(x / 8)] |= masks[x % 8];
				outputData[x]= 255;
			}
            else 
			{
				//outputData[(x / 8)] |= 0;
				outputData[x]= 0;
			}

            uchar error;
            if (inputData[x] - 128 > 0)
				error = (inputData[x] - 255);// - (j ? 32 : -32));
            else
                error = (inputData[x] - 0);// - (j ? 32 : -32));

			if (x < input.cols - 1)
            {
                if ((inputData[(x + 1)] + 7 * error / 16) > 255)
                    inputData[(x + 1)] = 255;
                else if ((inputData[(x + 1)] + 7 * error / 16) < 0)
                    inputData[(x + 1)] = 0;
                else
                    inputData[(x + 1)] = (inputData[(x + 1)] + 7 * error / 16);
            }
			if (y < input.rows - 1)
            {
				inputData = input.ptr<uchar>(y+1);
				outputData = output.ptr<uchar>(y+1);
                if (x > 0)
                {
                    if ((inputData[(x - 1)] + 3 * error / 16) > 255)
                        inputData[(x - 1)] = 255;
                    else if ((inputData[(x - 1)] + 3 * error / 16) < 0)
                        inputData[(x - 1)] = 0;
                    else
                        inputData[(x - 1)] = (inputData[(x - 1)] + 3 * error / 16);
                }
                if ((inputData[x] + 5 * error / 16) > 255)
                    inputData[x] = 255;
                else if ((inputData[x] + 5 * error / 16) < 0)
                    inputData[x] = 0;
                else
                    inputData[x] = (inputData[x] + 5 * error / 16);

                if (x < input.cols - 1)
                {
                    if ((inputData[(x + 1)] + 1 * error / 16) > 255)
                        inputData[(x + 1)] = 255;
                    else if ((inputData[(x + 1)] + 1 * error / 16) < 0)
                        inputData[(x + 1)] = 0;
                    else
                        inputData[(x + 1)] = (inputData[(x + 1)] + 1 * error / 16);
                }
            }

        }//end for
    }//end for

	return output;
    
}

//利用一阶导，对原图进行卷积滤波，实现边缘增强，背景平滑
Mat Halftone::EdgeEnhanceConvolutionFilter(const Mat &input, int nColorChannel, double xFilterMatrix[3][3], double yFilterMatrix[3][3], int nEdgeThreshold, int nFilterThreshold)
{
	if (nColorChannel < 0 || nColorChannel > 3)
       nColorChannel = 3;

    if (nFilterThreshold > 40)
        nFilterThreshold = 40;

    int nColor1 = 0;
    int nColor2 = 2;
    switch (nColorChannel)
    {
        case 0:
            nColor1 = 1;
            nColor2 = 2;
            break;
        case 1:
            nColor1 = 0;
            nColor2 = 2;
            break;
        case 2:
            nColor1 = 0;
            nColor2 = 1;
            break;
    }

	int intColor1;
    int intColor2;

	Mat output = Mat(input.rows, input.cols, input.type());
              
    uchar* inputBmpPtr = input.data;
    uchar* outputBmpPtr = output.data;
    int width = input.cols;
    int height = input.rows;
	int inputStride = input.step;
	int outputStride = output.step;

    double gray_X = 0.0;
    double gray_Y = 0.0;

    int filterWidth = 3;
    int filterHeight = 3;

    int filterOffset = (filterWidth - 1) / 2;

    int charOffset = 0;

    for (int offsetY = filterOffset; offsetY < height - filterOffset; offsetY++)
    {
        for (int offsetX = filterOffset; offsetX < width - filterOffset; offsetX++)
        {
            gray_X = 0;
            gray_Y = 0;

            charOffset = offsetY * inputStride + offsetX * 3;

            if (nColorChannel == 3)//
            {
                gray_X += (double)(inputBmpPtr[(offsetX - 1) * 3 + 0 + offsetY * inputStride] - inputBmpPtr[(offsetX + 1) * 3 + 0 + offsetY * inputStride]) * 0.114;
                gray_X += (double)(inputBmpPtr[(offsetX - 1) * 3 + 1 + offsetY * inputStride] - inputBmpPtr[(offsetX + 1) * 3 + 1 + offsetY * inputStride]) * 0.587;
                gray_X += (double)(inputBmpPtr[(offsetX - 1) * 3 + 2 + offsetY * inputStride] - inputBmpPtr[(offsetX + 1) * 3 + 2 + offsetY * inputStride]) * 0.299;

                gray_Y += (double)(inputBmpPtr[offsetX * 3 + 0 + (offsetY - 1) * inputStride] - inputBmpPtr[offsetX * 3 + 0 + (offsetY + 1) * inputStride]) * 0.114;
                gray_Y += (double)(inputBmpPtr[offsetX * 3 + 1 + (offsetY - 1) * inputStride] - inputBmpPtr[offsetX * 3 + 1 + (offsetY + 1) * inputStride]) * 0.587;
                gray_Y += (double)(inputBmpPtr[offsetX * 3 + 2 + (offsetY - 1) * inputStride] - inputBmpPtr[offsetX * 3 + 2 + (offsetY + 1) * inputStride]) * 0.299;
            }
            else
            {
                if (inputBmpPtr[offsetX * 3 + nColorChannel + offsetY * inputStride] - inputBmpPtr[offsetX * 3 + nColor1 + offsetY * inputStride] > nFilterThreshold &&
                    inputBmpPtr[offsetX * 3 + nColorChannel + offsetY * inputStride] - inputBmpPtr[offsetX * 3 + nColor2 + offsetY * inputStride] > nFilterThreshold &&
                    nFilterThreshold > 0)
                {
                    gray_X = 0;
                    gray_Y = 0;
                }
                else
                {
                    gray_X = (double)(inputBmpPtr[(offsetX - 1) * 3 + nColorChannel + offsetY * inputStride] - inputBmpPtr[(offsetX + 1) * 3 + nColorChannel + offsetY * inputStride]);
                    gray_Y = (double)(inputBmpPtr[offsetX * 3 + nColorChannel + (offsetY - 1) * inputStride] - inputBmpPtr[offsetX * 3 + nColorChannel + (offsetY + 1) * inputStride]);
                }
            }

            if (gray_X > 50 || gray_X < -50 || gray_Y > 50 || gray_Y < -50)
            {
                outputBmpPtr[charOffset + 0] = (char)nEdgeThreshold;
                outputBmpPtr[charOffset + 1] = (char)nEdgeThreshold;
                outputBmpPtr[charOffset + 2] = (char)nEdgeThreshold;
            }
            else
            {
                outputBmpPtr[charOffset + 0] = inputBmpPtr[charOffset + 0];
                outputBmpPtr[charOffset + 1] = inputBmpPtr[charOffset + 1];
                outputBmpPtr[charOffset + 2] = inputBmpPtr[charOffset + 2];
            }
        }
    }
    


	 return output;
}

Mat Halftone::EdgeEnhanceConvolutionFilterWithGray(const Mat &input, double xFilterMatrix[3][3], double yFilterMatrix[3][3], int nEdgeThreshold)
{
	Mat output = Mat(input.rows, input.cols, CV_8UC1);
	uchar* inputBmpPtr = input.data;
    uchar* outputBmpPtr = output.data;
    int width = input.cols;
    int height = input.rows;
	int inputStride = input.step;
	int outputStride = output.step;

	double gray_X = 0.0;
    double gray_Y = 0.0;

    int filterWidth = 3;
    int filterHeight = 3;

    int filterOffset = (filterWidth - 1) / 2;
    //int calcOffset = 0;

    int charOffset = 0;

	for (int offsetY = filterOffset; offsetY < height - filterOffset; offsetY++)
    {
        for (int offsetX = filterOffset; offsetX < width - filterOffset; offsetX++)
        {
            gray_X = 0;
            gray_Y = 0;

            charOffset = offsetY * inputStride + offsetX;

            for (int filterY = -filterOffset; filterY <= filterOffset; filterY++)
            {
                for (int filterX = -filterOffset;
                    filterX <= filterOffset; filterX++)
                {
                    gray_X += (double)(inputBmpPtr[(offsetX + filterX) + (offsetY + filterY) * inputStride]) *
                                xFilterMatrix[filterY + filterOffset][ filterX + filterOffset];

                    gray_Y += (double)(inputBmpPtr[(offsetX + filterX) + (offsetY + filterY) * inputStride]) *
                                yFilterMatrix[filterY + filterOffset][ filterX + filterOffset];
                }
            }

            if (gray_X > 150 || gray_X < -150 || gray_Y > 150 || gray_Y < -150)
            {
                outputBmpPtr[charOffset] = nEdgeThreshold;
            }
            else
            {
                outputBmpPtr[charOffset] = inputBmpPtr[charOffset + 0];
            }
        }
    }

	return output;
}