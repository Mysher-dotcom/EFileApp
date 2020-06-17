#include "getcamerainfothread.h"
#include <QDebug>
#include "camcapturesdk.h"

GetCameraInfoThread::GetCameraInfoThread(QStringList devIndexList,QObject *parent) : QObject(parent)
{
    _devIndexList = devIndexList;
}

void GetCameraInfoThread::slotStartCameraThread()
{
    //清空参数集合
    QMap<int ,QStringList>::iterator it;
    for(it = cameraInfoMap.begin();it!=cameraInfoMap.end();it++)
    {
        if((it.key()%2)==0)
            cameraInfoMap.erase(it);
    }

    //Init();
    long nDevCount  = 0;
    Cam_GetDevCount(nDevCount);
    qDebug("Camera线程GetDevCount is %d\n",nDevCount);
    if(nDevCount==0)
    {
        emit signalNoCamera();//发送无拍摄仪信号
        return;
    }

    char szDevNodeName[256]={0};
    for (int i = 0;i < nDevCount;i++)
    {
        memset(szDevNodeName,0,sizeof(szDevNodeName));
        Cam_GetDevName(i,szDevNodeName);

        int isLicense = 0;
        Cam_GetDevLicense(i,isLicense);

        qDebug()<<"Camera线程，设备名："<<szDevNodeName<<"，是否有授权（0=普通设备，1=授权设备，2=设备没有usb权限）:"<<isLicense;

        //如：<0,"拍摄仪,SD544,空闲,设备下标,是否有授权">
        QStringList cameraInfo;
        cameraInfo<<QString("%1 %2").arg("拍摄仪").arg(QString::number(i+1))<<szDevNodeName<<"0"<<QString::number(i)<<QString::number(isLicense);
        cameraInfoMap.insert(i,cameraInfo);//参数加入集合

    }

    //需要传递的数据
    QVariant qvCameraInfoMap; //申明通用数据对象
    qvCameraInfoMap.setValue(cameraInfoMap); //数据包装

    //发送信号给主线程，主线程绘制设备信息和参数UI
    emit signalCameraInfo(qvCameraInfoMap,"123");

}

void GetCameraInfoThread::slotGetCameraPar()
{
    //清空参数集合
    QMap<int ,QMap<QString ,QStringList>>::iterator it;
    for(it = cameraParMap.begin();it!=cameraParMap.end();it++)
    {
       if((it.key()%2)==0)
            cameraParMap.erase(it);
    }

    if(_devIndexList.size() <= 0)
    {
        QVariant DataVar;
        emit signalCameraParInfo(DataVar,"-1");
        return;
    }
    for (int i=0;i<_devIndexList.size();i++)
    {
        int _devIndex = _devIndexList.at(i).toInt();

        long nFPS = 0;
        long nResolutionCount = 0;
        long nWidth=0,nHeight=0;
        long nFormatCount = 0;
        char szFormat[256]={0};

        QStringList parFormatValue ;
        Cam_GetCameraFormatCount(_devIndex,nFormatCount);
        qDebug("Camera线程GetCameraFormatCount is %d\n",nFormatCount);
        for (int m = 0;m<nFormatCount;m++)
        {
           Cam_GetCameraFormatName(m,szFormat);
           qDebug("Camera线程GetCameraFormatName Width is %s\n",szFormat);
           parFormatValue<<szFormat;
        }
        //视频格式参数加入集合
        QMap<QString,QStringList> tmpFormatMap ;
        tmpFormatMap.insert("format",parFormatValue);
        cameraParMap.insert(cameraParMap.count(),tmpFormatMap);

        QStringList parResolutionValue ;
        Cam_GetCameraResolutionCount(_devIndex,0,nResolutionCount);
        qDebug("Camera线程GetCameraResolution is %d\n",nResolutionCount);
        for (int j = 0;j<nResolutionCount;j++)
        {
           Cam_GetCameraResolutionName(j,nWidth,nHeight);
           qDebug("Camera线程GetCameraResolutionName Width is %d,Height is %d\n",nWidth,nHeight);
           QString Width = QString::number(nWidth);
           QString Height = QString::number(nHeight);
           Width.append("x");
           Width.append(Height);
           parResolutionValue<<Width;
        }
        //分辨率参数加入集合
        QMap<QString,QStringList> tmpResolutionMap ;
        tmpResolutionMap.insert("resolution",parResolutionValue);
        cameraParMap.insert(cameraParMap.count(),tmpResolutionMap);

        //需要传递的数据
        QVariant DataVar; //申明通用数据对象
        DataVar.setValue(cameraParMap); //数据包装
        emit signalCameraParInfo(DataVar,QString::number(_devIndex));//发送信号给主线程，主线程绘制设备信息和参数UI
    }
    emit signalParOver();

}
