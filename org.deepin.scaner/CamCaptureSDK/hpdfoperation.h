#ifndef HPDFOPERATION_H
#define HPDFOPERATION_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include "hpdf.h"
#include"datatype.h"



class hpdfoperation
{
public:
    HPDF_Doc pdf;
    hpdfoperation();
    int jpeg2pdf(char  *jpeg_file_name,char * pdf_file_name,_HPDF_PageSizes PAGE_SIZE_TYPE = HPDF_PAGE_SIZE_A4,
                            bool multipage = false,bool multipage_saveFlag = false);
    int png2pdf(char  *png_file_name,char * pdf_file_name,_HPDF_PageSizes PAGE_SIZE_TYPE = HPDF_PAGE_SIZE_A4,
                            bool multipage = false,bool multipage_saveFlag = false);
    int rgb2pdf(unsigned char *srcData, int srcDataWidth,  int srcDataHeight,char *destFile,int colorType);
};

#endif // HPDFOPERATION_H
