#include "doscanthread.h"
#include "globalhelper.h"
#include <QDebug>
#include <QThread>
#include "camscansdk.h"
#include "cmimage.h"
#include "MImage.h"
#include "errortype.h"
#include <DDialog>
#include <QIcon>
#include <QDateTime>


QMap<int ,QString> scanParMap;//扫描需要的参数

DoScanThread *DoScanThread::g_doScanThread = NULL;//全局线程对象，供扫描窗口绑定信号槽

DoScanThread::DoScanThread(QMap<int ,QString> map,QObject *parent) : QObject(parent)
{
    g_doScanThread = this;
    isStop=false;
    scanParMap = map;
}

void DoScanThread::closeThread()
{
    isStop = true;
}

//扫描过程中存图回调
int DoScanThread::doScanReceiveCB(uchar*data,int size,int w,int h,int nBpp,int nDPI)
{
    qDebug("DoScanThread::doScanReceiveCB start\n");
    emit g_doScanThread->signalScanSaveImage((char*)data,w,h,NULL,size,nDPI);
    qDebug("DoScanThread::doScanReceiveCB end\n");
    return 0;
}

//扫描过程中错误信息回调
int DoScanThread::doScanStatuCB(int nStatus)
{
    qDebug()<<"扫描错误信息:"<<nStatus;
    QString msg="未知错误";
    if(nStatus == STATUS_NO_DOCS)
    {
        msg="请正确放入纸张！";
    }
    else if(nStatus == STATUS_DEVICE_BUSY)
    {
        msg="扫描过程中卡纸！";
    }
    else if(nStatus == STATUS_JAMMED)
    {
        msg="文档进纸器卡纸！";
    }
    else
    {
        msg="未知错误";
    }

    emit g_doScanThread->signalScanError(msg);
    return  0;
}

//开始扫描
void DoScanThread::startScanSlot()
{
    GlobalHelper::writeSettingValue("imgEdit","nIsLineart","1");//先将黑白二值图处理关闭
    //设置扫描参数
    QMap<int,QString>::iterator it;
    for(it = scanParMap.begin();it!=scanParMap.end();it++)
    {
        QString value =it.value();
        QByteArray baValue = value.toLatin1();

        qDebug()<<"设置扫描仪参数:"<<it.key()<<","<<baValue.data();
        SetCapCtrl(it.key(),baValue.data());//设置扫描仪参数
        if(value.toLower()=="lineart")//如果发现是黑白图，打开开关
        {
            GlobalHelper::writeSettingValue("imgEdit","nIsLineart","0");
        }
    }

   int devIndex = 0;//str.toInt();
   Scan(devIndex,doScanReceiveCB,doScanStatuCB);//扫描

   qDebug("scan finish\n");

   emit g_doScanThread->signalScanOver();//扫描结束
}

//新增图像
void DoScanThread::addPicSlot(QString str)
{
    emit g_doScanThread->signalAddPic(str);
}


