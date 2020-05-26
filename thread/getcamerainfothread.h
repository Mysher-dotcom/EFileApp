#ifndef GETCAMERAINFOTHREAD_H
#define GETCAMERAINFOTHREAD_H

#include <QObject>
#include <QMap>
#include <QMetaType>
#include <QVariant>

class GetCameraInfoThread: public QObject
{
        Q_OBJECT
public:
    GetCameraInfoThread(int devIndex,QObject *parent = nullptr);

    /*
     * 设备参数键值对集合
     * int=设备类型，0=拍摄仪，1=扫描仪
     * QStringList=设备信息的值集合
     * 如：<0,"拍摄仪,SD544,空闲,设备下标">
     */
    QMap<int ,QStringList> cameraInfoMap;
    /*
     * 参数键值对集合
     * int=参数的下标
     * QMap=<参数名称，参数的值集合>
     * 如：<1,<图片类型,"彩色,黑白,灰度">>
     */
    QMap<int ,QMap<QString ,QStringList>> cameraParMap;

signals:
    //信号
    void signalCameraError(QString);//拍摄仪发生错误
    void signalNoCamera();//无设备信息
    void signalCameraInfo(QVariant, const QString &);//设备信息信息(信号槽不可以传复杂类型的参数，需要转化成QVariant作为参数)
    void signalCameraParInfo(QVariant,const QString &);//设备的参数信息

public slots:
    //槽
    void slotStartCameraThread();//开始线程，读取设备信息
    void slotGetCameraPar(); //获取设备的参数

};

#endif // GETCAMERAINFOTHREAD_H
