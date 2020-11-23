#ifndef UPLOADFILETHREAD_H
#define UPLOADFILETHREAD_H

#include <QObject>
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QFile>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QEventLoop>

class UploadFileThread: public QObject
{
    Q_OBJECT
public:
    explicit UploadFileThread(QObject *parent = nullptr);
    void setFileList(QStringList);//设置需要上传的文件

    QStringList fileList;

    QFile *imgFile;
    //QNetworkAccessManager *httpNAM;
    //QNetworkReply *uploadFileReply ;

    //信号
signals:
    void signalSingleUploadOver(QString filepath,bool uploadResult);//单个文件上传完成
    void signalOver();//上传结束


    //槽
public slots:
    void startUpload();//开始上传
    void slotSinglaUploadOver();

};

#endif // UPLOADFILETHREAD_H
