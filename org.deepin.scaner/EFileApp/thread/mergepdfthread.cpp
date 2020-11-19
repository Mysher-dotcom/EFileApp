#include "mergepdfthread.h"
#include "hpdfoperation.h"
#include <QImage>
#include "cpng.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>

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
    //遇到重名的PDF，追加编号
    QString filePath = fileList.at(0);
    int index = 1;
    QFileInfo *fi = new QFileInfo(filePath);
    QString fileSuffix = "pdf";//fi->suffix().trimmed();
    QString fileName = fi->fileName();
    QString fileFolder = fi->absolutePath();
    fileName = fileName.mid(0,fileName.indexOf("."));
    filePath = fileFolder+"/"+fileName+"."+fileSuffix;
    QFile newFile(filePath);
    while(newFile.exists() == true)
    {
       filePath = fileFolder+"/"+fileName+"("+QString::number(index)+")."+fileSuffix;
       newFile.setFileName(filePath);
       index++;
    }
    delete  fi;

    //char pdfPath[256]={0};
   // strncpy(pdfPath,filePath.toUtf8().data(),);
   // strcat(pdfPath,"pdf");
    qDebug("pdf path is %s\n",filePath.toUtf8().data());
    qDebug()<<"pdf path lenght:"<<filePath.toLocal8Bit().length()<<","<<filePath.toUtf8().length();
    hpdfoperation pdfop;
    for(int i=0;i<fileList.size();i++)
    {
        int nMergeResult = 0;
        unsigned char* dstBuf = NULL;
        JPEGInfo jpgInfo;
        int dstW,dstH;
        char jpgPath[2048] = {0};
        strcpy(jpgPath,fileList.at(i).toUtf8().data());
         qDebug("jpgPath path is %s\n",jpgPath);
        QString tt = fileList.at(i).right(3);
        if(QString::compare(tt,"jpg")==0)
        {
            merge_jpg.readBufFromJpeg(jpgPath,&dstBuf,jpgInfo,dstW,dstH);

            if(jpgInfo.colorSpace == 1)
            {
                nMergeResult = pdfop.rgb2pdf(dstBuf,jpgInfo.width,jpgInfo.height,filePath.toUtf8().data(),1,true,i == fileList.size() - 1);
            }
            else
            {
                nMergeResult =pdfop.rgb2pdf(dstBuf,jpgInfo.width,jpgInfo.height,filePath.toUtf8().data(),0,true,i == fileList.size() - 1);
            }

            //pdfop.jpeg2pdf(jpgPath,pdfPath,HPDF_PAGE_SIZE_A4,true,i == fileList.size() - 1);
        }
        else if (QString::compare(tt,"png")==0) {
            pic_data out;

            m_png.decode_png(fileList.at(i).toUtf8().data(),&out);

            if(out.color_type == 0)
            {
                nMergeResult = pdfop.rgb2pdf(out.rgba,out.width,out.height,filePath.toUtf8().data(),1,true,i == fileList.size() - 1);
            }
            else
            {
                nMergeResult = pdfop.rgb2pdf(out.rgba,out.width,out.height,filePath.toUtf8().data(),0,true,i == fileList.size() - 1);
            }
            //pdfop.png2pdf(jpgPath,pdfPath,HPDF_PAGE_SIZE_A4,true,i == fileList.size() - 1);
        }
        else
        {
            char strPath[256]={0};
            strcpy(strPath,fileList.at(i).toUtf8().data());
            QImage* img2 = new QImage(strPath);
             QImage imgColor;
            if(img2->format() == QImage::Format_RGB32)
            {
                imgColor = img2->convertToFormat(QImage::Format_RGB888);
                dstBuf = imgColor.bits ();
            }
            else
            {
                dstBuf = img2->bits ();
            }
            if(img2->format() == QImage::Format_RGB32)
            {
               nMergeResult = pdfop.rgb2pdf((unsigned char*)dstBuf,img2->width(),img2->height(),filePath.toUtf8().data(),0,true,i == fileList.size() - 1);
            }
            if(img2->format() == QImage::Format_Indexed8 || img2->format() == QImage::Format_Grayscale8 )
            {
               nMergeResult =pdfop.rgb2pdf((unsigned char*)dstBuf,img2->width(),img2->height(),filePath.toUtf8().data(),1,true,i == fileList.size() - 1);
            }
            delete img2;
        }

        qDebug()<<"合并PDF函数返回结果："<<nMergeResult;
        emit signalSingleFileMergeOver(fileList.at(i),i);
    }
    emit signalOver();
}
