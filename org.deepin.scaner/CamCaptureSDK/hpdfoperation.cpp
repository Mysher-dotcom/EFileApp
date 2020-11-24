#include "errortype.h"
#include "hpdfoperation.h"
jmp_buf env;

void error_handler_mz(HPDF_STATUS   error_no, HPDF_STATUS   detail_no,void   *user_data)
{
    printf("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,(HPDF_UINT)detail_no);
    longjmp(env, 1);
}

hpdfoperation::hpdfoperation()
{
    pdf = NULL;
}
int hpdfoperation::jpeg2pdf(char  *jpeg_file_name,char * pdf_file_name,_HPDF_PageSizes PAGE_SIZE_TYPE,bool multipage ,bool multipage_saveFlag )
{
    if(jpeg_file_name == NULL || pdf_file_name == NULL){
        return ERROR_ARGV;
    }

    if(!pdf){
               pdf = HPDF_New (error_handler_mz, NULL);
    }

    if (!pdf) {
        printf ("ERROR: cannot create pdf object.\n");
        return ERROR_MALLOC;
    }

    if (setjmp(env)) {
           HPDF_Free (pdf);
           return ERROR_STATE;
       }

        HPDF_Page page = NULL;
        HPDF_Destination dst = NULL;
        HPDF_Image hpdfImage = NULL;

        /*load picture*/
        hpdfImage = HPDF_LoadJpegImageFromFile(pdf, "/home/viisan/Desktop/imm.jpg");
        if(!hpdfImage && !multipage){
                HPDF_Free (pdf);
                return ERROR_READ;
        }
           /*Create new page*/
        page = HPDF_AddPage (pdf);
        /*Set page object*/
       HPDF_Page_SetSize (page, PAGE_SIZE_TYPE, HPDF_PAGE_LANDSCAPE);
        dst = HPDF_Page_CreateDestination (page);

        HPDF_Destination_SetXYZ (dst, 0, HPDF_Page_GetHeight (page), 1);
        HPDF_SetOpenAction(pdf, dst);

        //将页的宽高设置为image的宽高
        HPDF_Page_SetHeight (page, HPDF_Image_GetHeight(hpdfImage));
        HPDF_Page_SetWidth (page, HPDF_Image_GetWidth(hpdfImage));

        int x = 0;
        int y = 0;
        HPDF_Page_DrawImage(page, hpdfImage, x, y, HPDF_Image_GetWidth(hpdfImage),HPDF_Image_GetHeight(hpdfImage));
        if(multipage && multipage_saveFlag){
            /* save the document to a file */
                HPDF_SaveToFile (pdf, pdf_file_name);
                /* clean up */
                HPDF_Free (pdf);
                pdf = NULL;
        }

        if(!multipage && !multipage_saveFlag){
            /* save the document to a file */
                HPDF_SaveToFile (pdf, pdf_file_name);
                /* clean up */
                HPDF_Free (pdf);
                pdf = NULL;

        }
        return ERROR_GOOD;
}

int hpdfoperation::png2pdf(char *png_file_name, char *pdf_file_name, _HPDF_PageSizes PAGE_SIZE_TYPE, bool multipage, bool multipage_saveFlag)
{

    if(png_file_name == NULL || pdf_file_name == NULL){
        return ERROR_ARGV;
    }

    if(!pdf){
               pdf = HPDF_New (error_handler_mz, NULL);
    }

    if (!pdf) {
        printf ("ERROR: cannot create pdf object.\n");
        return ERROR_MALLOC;
    }

    if (setjmp(env)) {
           HPDF_Free (pdf);
           return ERROR_STATE;
       }

        HPDF_Page page = NULL;
        HPDF_Destination dst = NULL;
        HPDF_Image hpdfImage = NULL;

        /*load picture*/
        hpdfImage = HPDF_LoadPngImageFromFile(pdf, png_file_name);
        if(!hpdfImage && !multipage){
                HPDF_Free (pdf);
                return ERROR_READ;
        }

           /*Create new page*/
        page = HPDF_AddPage (pdf);
        /*Set page object*/
       HPDF_Page_SetSize (page, PAGE_SIZE_TYPE, HPDF_PAGE_LANDSCAPE);
        dst = HPDF_Page_CreateDestination (page);

        HPDF_Destination_SetXYZ (dst, 0, HPDF_Page_GetHeight (page), 1);
        HPDF_SetOpenAction(pdf, dst);

        //将页的宽高设置为image的宽高
        HPDF_Page_SetHeight (page, HPDF_Image_GetHeight(hpdfImage));
        HPDF_Page_SetWidth (page, HPDF_Image_GetWidth(hpdfImage));

        int x = 0;
        int y = 0;
        HPDF_Page_DrawImage(page, hpdfImage, x, y, HPDF_Image_GetWidth(hpdfImage),HPDF_Image_GetHeight(hpdfImage));
        if(multipage && multipage_saveFlag){
            /* save the document to a file */
                HPDF_SaveToFile (pdf, pdf_file_name);
                /* clean up */
                HPDF_Free (pdf);
                pdf = NULL;
        }

        if(!multipage && !multipage_saveFlag){
            /* save the document to a file */
                HPDF_SaveToFile (pdf, pdf_file_name);
                /* clean up */
                HPDF_Free (pdf);
                pdf = NULL;

        }
        return ERROR_GOOD;

}

int hpdfoperation::rgb2pdf(unsigned char *srcData, int srcDataWidth, int srcDataHeight,char *destFile ,int colorType)
{
    if(srcData==NULL ||srcDataWidth < 0 || srcDataHeight < 0 ||destFile == NULL){
            return ERROR_ARGV;
    }

    if(!pdf){
               pdf = HPDF_New (error_handler_mz, NULL);
    }

    if (!pdf) {
        printf ("ERROR: cannot create pdf object.\n");
        return ERROR_MALLOC;
    }

    if (setjmp(env)) {
           HPDF_Free (pdf);
           return ERROR_STATE;
       }

    HPDF_Page page = NULL;
    HPDF_Destination dst = NULL;
    HPDF_Image hpdfImage = NULL;

    /*load picture*/
    if(colorType == 0){
         hpdfImage = HPDF_LoadRawImageFromMem(pdf, srcData,srcDataWidth,srcDataHeight,HPDF_CS_DEVICE_RGB,8);
    }else {
          hpdfImage = HPDF_LoadRawImageFromMem(pdf, srcData,srcDataWidth,srcDataHeight,HPDF_CS_DEVICE_GRAY,8);
    }

    /*Create new page*/
 page = HPDF_AddPage (pdf);
 /*Set page object*/
HPDF_Page_SetSize (page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_LANDSCAPE);
 dst = HPDF_Page_CreateDestination (page);

 HPDF_Destination_SetXYZ (dst, 0, HPDF_Page_GetHeight (page), 1);
 HPDF_SetOpenAction(pdf, dst);

 //将页的宽高设置为image的宽高
 HPDF_Page_SetHeight (page, HPDF_Image_GetHeight(hpdfImage));
 HPDF_Page_SetWidth (page, HPDF_Image_GetWidth(hpdfImage));

 int x = 0;
 int y = 0;
 HPDF_Page_DrawImage(page, hpdfImage, x, y, HPDF_Image_GetWidth(hpdfImage),HPDF_Image_GetHeight(hpdfImage));

 /* save the document to a file */
     HPDF_SaveToFile (pdf, destFile);
     /* clean up */
     //HPDF_Free (pdf);
       return ERROR_GOOD;
}
