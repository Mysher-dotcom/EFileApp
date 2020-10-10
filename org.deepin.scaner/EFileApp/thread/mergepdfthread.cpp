#include "mergepdfthread.h"
#include "hpdfoperation.h"
#include <QImage>
#include "cpng.h"
CJpeg merge_jpg;//CJPEG对象
CPNG m_png;
MergePDFThread::MergePDFThread(QObject *parent): QObject(parent)
{

}

void MergePDFThread::setFileList(QStringList list)
{
    fileList = list;
}

void MergePDFThread::startMerge()
{
    if(fileList.size() <= 0)
    {
        emit signalOver();
        return;
    }
    char pdfPath[256]={0};
    strncpy(pdfPath,fileList.at(0).toUtf8().data(),strlen(fileList.at(0).toUtf8().data())-3);
    strcat(pdfPath,"pdf");
    qDebug("pdf path is %s\n",pdfPath);
    hpdfoperation pdfop;
    for(int i=0;i<fileList.size();i++)
    {
        unsigned char* dstBuf = NULL;
        JPEGInfo jpgInfo;
        int dstW,dstH;
        char jpgPath[256] = {0};
        strcpy(jpgPath,fileList.at(i).toUtf8().data());

        QString tt = fileList.at(i).right(3);
        if(QString::compare(tt,"jpg")==0)
        {
            merge_jpg.readBufFromJpeg(jpgPath,&dstBuf,jpgInfo,dstW,dstH);

            if(jpgInfo.colorSpace == 1)
            {
                pdfop.rgb2pdf(dstBuf,jpgInfo.width,jpgInfo.height,pdfPath,1,true,i == fileList.size() - 1);
            }
            else
            {
                pdfop.rgb2pdf(dstBuf,jpgInfo.width,jpgInfo.height,pdfPath,0,true,i == fileList.size() - 1);
            }

            //pdfop.jpeg2pdf(jpgPath,pdfPath,HPDF_PAGE_SIZE_A4,true,i == fileList.size() - 1);
        }
        else if (QString::compare(tt,"png")==0) {
            pic_data out;

            m_png.decode_png(fileList.at(i).toUtf8().data(),&out);

            if(out.color_type == 0)
            {
                pdfop.rgb2pdf(out.rgba,out.width,out.height,pdfPath,1,true,i == fileList.size() - 1);
            }
            else
            {
                pdfop.rgb2pdf(out.rgba,out.width,out.height,pdfPath,0,true,i == fileList.size() - 1);
            }
            //pdfop.png2pdf(jpgPath,pdfPath,HPDF_PAGE_SIZE_A4,true,i == fileList.size() - 1);
        }
        else
        {
            char strPath[256]={0};
            strcpy(strPath,fileList.at(i).toUtf8().data());
            QImage* img2 = new QImage(strPath);
            dstBuf = img2->bits ();
            pdfop.rgb2pdf(dstBuf,img2->width(),img2->height(),pdfPath,0,true,i == fileList.size() - 1);
        }
        emit signalSingleFileMergeOver(fileList.at(i),i);
    }
    emit signalOver();
}
