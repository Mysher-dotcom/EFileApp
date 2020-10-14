#include "doscanthread.h"
#include "helper/globalhelper.h"
#include "helper/deviceinfohelper.h"
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
    QString msg=tr("Unknown error");//未知错误
    if(nStatus == STATUS_NO_DOCS)
    {
        msg=tr("Out of paper");//请正确放入纸张！
        emit g_doScanThread->signalScanError(msg);
    }
    else if(nStatus == STATUS_DEVICE_BUSY)
    {
        msg=tr("Paper jam when scanning");//扫描过程中卡纸！
        emit g_doScanThread->signalScanError(msg);
    }
    else if(nStatus == STATUS_JAMMED)
    {
        msg=tr("Paper jam in the document feeder");//文档进纸器卡纸！
        emit g_doScanThread->signalScanError(msg);
    }
    /*
    else if(nStatus == STATUS_JAMMED)
        msg=tr("Unknown error");//不支持该操作
    else if(nStatus == STATUS_CANCELLED)
        msg=tr("Unknown error");//操作被取消
    else if(nStatus == STATUS_INVAL)
        msg=tr("Unknown error");//数据或参数无效
    else if(nStatus == STATUS_EOF)
        msg=tr("Unknown error");//没有更多可用数据（文件结束）
    else if(nStatus == STATUS_COVER_OPEN)
        msg=tr("Unknown error");//扫描仪盖板已打开
    else if(nStatus == STATUS_IO_ERROR)
        msg=tr("Unknown error");//设备I/O期间出错
    else if(nStatus == STATUS_NO_MEM)
        msg=tr("Unknown error");//内存不足
    else if(nStatus == STATUS_ACCESS_DENIED)
        msg=tr("Unknown error");//对资源的访问已被拒绝。
    else if(nStatus == STATUS_INIT_FAILED)
        msg=tr("Unknown error");//初始化失败
    else if(nStatus == STATUS_OPEN_FAILED)
        msg=tr("Unknown error");//打开失败
    else if(nStatus == STATUS_CONNECT_FAILED)
        msg=tr("Unknown error");//连接失败
    */

    return  0;
}

//开始扫描
void DoScanThread::startScanSlot()
{
    QString defaultDeviceModelFilePath =  DeviceInfoHelper::readValue(DeviceInfoHelper::getDeviceListInfoFilePath(),"default","config");
    int nResult = OpenDev(deviceIndex);//打开设备
    if(nResult != 0)
    {
        emit g_doScanThread->signalOpenDevError(tr("Unknown error"));//打开扫描仪设备出错
        qDebug()<<"打开扫描仪设备出错:"<<nResult;
        ExitDev();//CloseDev(deviceIndex);//关闭设备
        emit g_doScanThread->signalScanOver();//扫描结束
        return;
    }
    //GlobalHelper::writeSettingValue("imgEdit","nIsLineart","1");//先将黑白二值图处理关闭
    DeviceInfoHelper::writeValue(defaultDeviceModelFilePath,"imgset","nIsLineart","1");

    //设置扫描参数
    QMap<int,QString>::iterator it;
    for(it = scanParMap.begin();it!=scanParMap.end();it++)
    {
        QString value =it.value();
        if(value=="Black & White")
        {
            value="Gray";
        }
        QByteArray baValue = value.toLatin1();
        qDebug()<<"设置扫描仪参数:"<<it.key()<<","<<baValue.data();
        SetCapCtrl(it.key(),baValue.data());//设置扫描仪参数
        if(value.toLower()=="lineart"||it.value()=="Black & White")//如果发现是黑白图，打开开关
        {
            //GlobalHelper::writeSettingValue("imgEdit","nIsLineart","0");
            DeviceInfoHelper::writeValue(defaultDeviceModelFilePath,"imgset","nIsLineart","0");
        }
    }

    nResult = Scan(deviceIndex,doScanReceiveCB,doScanStatuCB);//扫描    
    if(nResult != 0)
    {
        qDebug()<<"***************扫描失败*********"<<nResult;
        if(nResult != STATUS_NO_DOCS
          && nResult != STATUS_DEVICE_BUSY
          && nResult != STATUS_JAMMED)
        {
            emit g_doScanThread->signalOpenDevError(tr("Unknown error"));//扫描出错
        }
    }
    qDebug()<<"scan finish";
    emit g_doScanThread->signalScanOver();//扫描结束
    CloseDev(deviceIndex);//关闭设备
}

//新增图像
void DoScanThread::addPicSlot(QString str)
{
    emit g_doScanThread->signalAddPic(str);
}


