#include "uploadfilethread.h"
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QFile>
#include "helper/webhelper.h"

UploadFileThread::UploadFileThread(QObject *parent) : QObject(parent)
{

}

void UploadFileThread::setFileList(QStringList list)
{
    fileList = list;
}


//开始上传
void UploadFileThread::startUpload()
{
    for(int i = 0 ;i < fileList.length() ; i ++)
    {
        imgFile = new QFile(fileList.at(i));
        if(!imgFile->exists())
        {
            emit signalSingleUploadOver(imgFile->fileName(),false);
            return;
        }
        QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
        QHttpPart imagePart;
        imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/jpeg"));
        imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\""+imgFile->fileName()+"\""));
        imgFile->open(QIODevice::ReadOnly);
        imagePart.setBodyDevice(imgFile);
        imgFile->setParent(multiPart);
        multiPart->append(imagePart);

        //添加需要携带的参数，比如用户名，或者token之类的，每一个参数对应一个QHttpPart对象
        QHttpPart dataPart;
        dataPart.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("text/plain"));
        dataPart.setHeader(QNetworkRequest::ContentDispositionHeader,QVariant("form-data; name=\"params\"")); dataPart.setBody(WebHelper::getToken().toUtf8());
        multiPart->append(dataPart);

        QUrl url(WebHelper::getUploadAddress());
        QNetworkRequest request(url);

        QNetworkAccessManager m_httpNAM;
        QNetworkReply *uploadFileReply = m_httpNAM.post(request, multiPart);
        multiPart->setParent(uploadFileReply);
        connect(uploadFileReply,SIGNAL(finished()),this,SLOT(slotSinglaUploadOver()));

        //线程中使用上传，一定要加QEventLoop，等待返回上传结果 vector 2020-05-29
        QEventLoop eventLoop;
        connect(&m_httpNAM, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
        eventLoop.exec();

        QByteArray response;
        if(uploadFileReply->error() == QNetworkReply::NoError)
        {
            response = uploadFileReply->readAll();
            qDebug() << " ok:"<<response ;
        }
        else
        {
            qDebug() << " no:"<<response ;
        }

    }
    emit signalOver();
}


void UploadFileThread::slotSinglaUploadOver()
{
   qDebug() << "upload finish" ;
   if(imgFile != NULL)
   {
       emit  signalSingleUploadOver(imgFile->fileName(),true);
       imgFile->flush();
       imgFile->close();
       delete  imgFile;
       imgFile = NULL;
   }
}
