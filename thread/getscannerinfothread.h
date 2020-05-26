#ifndef GETDEVICEINFOTHREAD_H
#define GETDEVICEINFOTHREAD_H

#include <QObject>
#include <QMap>
#include <QMetaType>
#include <QVariant>

class GetScannerInfoThread: public QObject
{
    Q_OBJECT
public:
    explicit GetScannerInfoThread(int devIndex,QObject *parent = nullptr);

    void closeThread();//停止线程

    /*
     * 设备参数键值对集合
     * int=设备类型，0=拍摄仪，1=扫描仪
     * QStringList=设备信息的值集合
     * 如：<1,"扫描仪,UNIS F3135,空闲">
     */
    QMap<int ,QStringList> devInfoMap;

    /*
     * 设备参数键值对集合
     * int=参数的下标
     * QMap=<参数名称，参数的值集合>
     * 如：<1,<色彩模式,"Color24,Gary8,Lineart">>
     */
    QMap<int ,QMap<QString ,QStringList>> devParMap;

private:
    volatile bool isStop;


signals:
    //信号
    void signalNoScanner();//无设备信号

    //设备信息信号(信号槽不可以传复杂类型的参数，需要转化成QVariant作为参数)
    void signalScannerInfo(QVariant, const QString &);

    //设备的参数信号
    void signalScannerParInfo(QVariant,const QString &);

public slots:
    //槽
    void slotStartThread();//开始线程，读取设备信息
    void slotGetDevicePar(); //获取设备的参数

};


#endif // GETDEVICEINFOTHREAD_H
