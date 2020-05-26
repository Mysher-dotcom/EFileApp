#ifndef DOSCANTHREAD_H
#define DOSCANTHREAD_H

#include <QObject>
#include <QMap>
#include <QMetaType>
#include <QVariant>
#include "Jpeg.h"
class DoScanThread;

class DoScanThread : public QObject
{
    Q_OBJECT
public:
    explicit DoScanThread(QMap<int ,QString>,QObject *parent = nullptr);

    static DoScanThread *g_doScanThread;

    void closeThread();
    static int doScanReceiveCB(uchar*data,int size,int w,int h,int nBpp,int nDPI);
    static int doScanStatuCB(int nStatus);

private:
    volatile bool isStop;

signals:
    //信号
    void signalAddPic(QString); //新增图像信号
    void signalScanError(QString);//扫描中错误信号
    void signalScanOver();//扫描结束信号
    void signalScanSaveImage(char*,int,int,char*,int,int);//扫描保存图像

public slots:
    //槽
    void startScanSlot();//开始扫描
    void addPicSlot(QString); //新增图像
    //void scanOverSlot();//扫描结束

};

#endif // DOSCANTHREAD_H
