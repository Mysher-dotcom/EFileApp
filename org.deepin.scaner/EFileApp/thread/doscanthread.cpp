#include "doscanthread.h"
#include "helper/globalhelper.h"
#include <QDebug>
#include <QThread>
#include "camscansdk.h"
#include "cmimage.h"
#include "MImage.h"
#include "errortypescanner.h"
#include <DDialog>
#include <QIcon>
#include <QDateTime>

int deviceIndex;
QMap<int ,QString> scanParMap;//扫描需要的参数

DoScanThread *DoScanThread::g_doScanThread = NULL;//全局线程对象，供扫描窗口绑定信号槽

DoScanThread::DoScanThread(int devIndex,QMap<int ,QString> map,QObject *parent) : QObject(parent)
{
    g_doScanThread = this;
    isStop=false;
    deviceIndex = devIndex;
    scanParMap = map;
}

void DoScanThread::closeThread()
{
    isStop = true;
}

//扫描过程中存图回调
int DoScanThread::doScanReceiveCB(const uchar*data,int size,int w,int h,int nBpp,int nDPI)
{

    emit g_doScanThread->signalScanSaveImage((char*)data,size,w,h,nBpp,nDPI);

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

        msg="未知错误,"+nStatus;
    }

    emit g_doScanThread->signalScanError(msg);
    return  0;
}

//开始扫描
void DoScanThread::startScanSlot()
{
    int nResult = OpenDev(deviceIndex);//打开设备
    if(nResult != 0)
    {
        CloseDev(deviceIndex);//关闭设备
        qDebug()<<"打开扫描仪设备出错:"<<nResult;
        emit g_doScanThread->signalScanOver();//扫描结束
        return;
    }
    GlobalHelper::writeSettingValue("imgEdit","nIsLineart","1");//先将黑白二值图处理关闭
    //设置扫描参数
    QMap<int,QString>::iterator it;
    for(it = scanParMap.begin();it!=scanParMap.end();it++)
    {
        QString value =it.value();

        //QString curvalue = value;

        if(value=="Black & White")
        {
            value="Gray";
        }
        QByteArray baValue = value.toLatin1();

        qDebug()<<"设置扫描仪参数:"<<it.key()<<","<<baValue.data();

        SetCapCtrl(it.key(),baValue.data());//设置扫描仪参数

        if(value.toLower()=="lineart"||it.value()=="Black & White")//如果发现是黑白图，打开开关
        {
            GlobalHelper::writeSettingValue("imgEdit","nIsLineart","0");
        }
    }

    Scan(deviceIndex,doScanReceiveCB,doScanStatuCB);//扫描

    qDebug()<<"scan finish";
    emit g_doScanThread->signalScanOver();//扫描结束

    CloseDev(deviceIndex);//关闭设备
}

//新增图像
void DoScanThread::addPicSlot(QString str)
{
    emit g_doScanThread->signalAddPic(str);
}


