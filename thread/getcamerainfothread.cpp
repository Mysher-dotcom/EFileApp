#include "getcamerainfothread.h"
#include <QDebug>
#include "camcapturesdk.h"

int _devIndex;
GetCameraInfoThread::GetCameraInfoThread(int devIndex,QObject *parent) : QObject(parent)
{
    _devIndex = devIndex;
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
    // 0=正常，2=非法设备 ,3=USB无权限
    long nGetCountResult = Cam_GetDevCount(nDevCount);
    if(nGetCountResult != 0)
    {
        QString msg;
        if(nGetCountResult == 2) msg = "请连接授权设备！";
        else msg = "请检查USB权限，请执行:sudo chmod 777 -R /dev/bus/usb";
        emit signalCameraError(msg);//拍摄仪发生错误
        return;
    }

    qDebug("GetDevCount is %d\n",nDevCount);
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
        qDebug("Get DevName is %s\n",szDevNodeName);

        //如：<0,"拍摄仪,SD544,空闲,设备下标">
        QStringList cameraInfo;
        cameraInfo<<QString("%1 %2").arg("拍摄仪").arg(QString::number(i+1))<<szDevNodeName<<"空闲"<<QString::number(i);
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

   long nFPS = 0;
   long nResolutionCount = 0;
   long nWidth=0,nHeight=0;
   long nFormatCount = 0;
   char szFormat[256]={0};

   QStringList parFormatValue ;
   Cam_GetCameraFormatCount(_devIndex,nFormatCount);
   qDebug("GetCameraFormatCount is %d\n",nFormatCount);
   for (int m = 0;m<nFormatCount;m++)
   {
       Cam_GetCameraFormatName(m,szFormat);
       qDebug("GetCameraFormatName Width is %s\n",szFormat);
       parFormatValue<<szFormat;
   }
   //视频格式参数加入集合
   QMap<QString,QStringList> tmpFormatMap ;
   tmpFormatMap.insert("format_c",parFormatValue);
   cameraParMap.insert(cameraParMap.count(),tmpFormatMap);


   QStringList parResolutionValue ;
   Cam_GetCameraResolutionCount(_devIndex,0,nResolutionCount);
   qDebug("GetCameraResolution is %d\n",nResolutionCount);
   for (int j = 0;j<nResolutionCount;j++)
   {
       Cam_GetCameraResolutionName(j,nWidth,nHeight);
       qDebug("GetCameraResolutionName Width is %d,Height is %d\n",nWidth,nHeight);
       QString Width = QString::number(nWidth);
       QString Height = QString::number(nHeight);
       Width.append("x");
       Width.append(Height);
       parResolutionValue<<Width;
   }
   //分辨率参数加入集合
   QMap<QString,QStringList> tmpResolutionMap ;
   tmpResolutionMap.insert("resolution_c",parResolutionValue);
   cameraParMap.insert(cameraParMap.count(),tmpResolutionMap);


    //需要传递的数据
    QVariant DataVar; //申明通用数据对象
    DataVar.setValue(cameraParMap); //数据包装
    emit signalCameraParInfo(DataVar,"abc");//发送信号给主线程，主线程绘制设备信息和参数UI
}
