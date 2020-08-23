#include "mergepdfthread.h"
#include "hpdfoperation.h"
#include <QImage>

CJpeg merge_jpg;//CJPEG对象

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
            pdfop.rgb2pdf(dstBuf,jpgInfo.width,jpgInfo.height,pdfPath,0,fileList.size());

        }
        else
        {
            char strPath[256]={0};
            strcpy(strPath,fileList.at(i).toUtf8().data());
            QImage* img2 = new QImage(strPath);
            dstBuf = img2->bits ();
            pdfop.rgb2pdf(dstBuf,img2->width(),img2->height(),pdfPath,0,i+1);
        }
        emit signalSingleFileMergeOver(fileList.at(i),i);
    }
    emit signalOver();
}
