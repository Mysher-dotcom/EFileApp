#include "recognizethread.h"
#include <QFileInfo>
#include <QImage>
#include "camcapturesdk.h"
#include "helper/classificationhelper.h"

CJpeg m_jpg;//CJPEG对象v

RecognizeThread::RecognizeThread(QObject *parent) : QObject(parent)
{

}

void RecognizeThread::startRecognize()
{
    //没有图像
    if(ClassificationHelper::captureFilePath.length() <= 0)
    {
        emit signalOver();
        return;
    }

    //读取分类规则
    QList<ClassRuleData> ruleList = ClassificationHelper::getClassRuleInfo();
    if(ruleList.length() <= 0)
    {
        emit signalOver();
        return;
    }

    //循环识别图像列表
    for(int i=0;i<ClassificationHelper::captureFilePath.length();i++)
    {
        QString codes = regcognizeBarcode(ClassificationHelper::captureFilePath.at(i));//识别条码
        QStringList codeInfo;//条码信息

        if(codes.length() > 1)
        {
            for(int i=0;i<ruleList.length();i++)
            {
                QStringList codeLength = ruleList.at(i).length.split("-");//条码截取长度
                QString code1 = codes.mid(codeLength.at(0).toInt(),codeLength.at(1).toInt()+1);//截取的条码

                //匹配截取的条码值
                QStringList valuesList = ruleList.at(i).values.split(";");
                for(int j=0;j<valuesList.length();j++)
                {
                    QString value1 = valuesList.at(j).split(":")[0];
                    if(code1 == value1)
                    {
                        codeInfo<<valuesList.at(j).split(":")[1];
                        break;
                    }
                }
            }
        }

        emit signalSendCode(ClassificationHelper::captureFilePath.at(i),
                            codes,
                            codeInfo);
    }

    emit signalOver();
}

//识别条码
QString RecognizeThread::regcognizeBarcode(QString imgPath)
{
    QFileInfo fileinfo;
    fileinfo = QFileInfo(imgPath);
    QString imgSuffix = fileinfo.suffix(); //文件后缀

    if(imgSuffix.isNull() || imgSuffix.isEmpty() || !imgSuffix.contains("jpg"))
    {
        QImage* img2 = new QImage(imgPath);
        unsigned char *data = img2->bits ();
        long lWidth=img2->width(),lHeight=img2->height();
        barcodeInfoList infos;
        Cam_BarcodeRecognizeBuffer((char*)data,lWidth,lHeight,3,infos);
        //qDebug("barcode is ::%s\n",infos.infos[0].cbarcodeRet);
        return infos.infos[0].cbarcodeRet;
    }
    else
    {
        //QString 转 char*
        char *folderPath;
        QByteArray qba = imgPath.toLatin1();
        folderPath = qba.data();

        unsigned char *des_buff=NULL;
        JPEGInfo jpgInfo;
        int desW=0,desH=0;
        m_jpg.readBufFromJpeg(folderPath,&des_buff,jpgInfo,desW,desH);
        //Cam_readBufFromJpeg(folderPath,&des_buff,jpgInfo,desW,desH);

        barcodeInfoList infos;
        Cam_BarcodeRecognizeBuffer((char*)des_buff,jpgInfo.width,jpgInfo.height,3,infos);
        //qDebug("barcode is: %s\n",infos.infos[0].cbarcodeRet);
        return infos.infos[0].cbarcodeRet;
    }
    return "";
}
