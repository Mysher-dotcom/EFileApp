#ifndef DEVICEINFOHELPER_H
#define DEVICEINFOHELPER_H

#include <QString>
#include <QSettings>
#include <QMetaType>
#include <qstring.h>
#include <QList>

//分类规则数据
typedef struct{
    int type;//设备类型，0=扫描仪，1=拍摄仪
    QString name;//设备名
    QString model;//设备类型
    int status;//设备状态，0=空闲，1=占用，2=未连接
    QString config;//设备具体配置信息文件路径
    int index;//设备下标
    int isLicense;//是否有授权（0=普通设备，1=授权设备，2=设备没有usb权限）
}DeviceInfoData;

Q_DECLARE_METATYPE(DeviceInfoData)


class DeviceInfoHelper
{
public:
    DeviceInfoHelper();

    static QString getDeviceListInfoFilePath();//获取设备列表信息配置文件路径
    static QString getDeviceInfoFilePath(QString fileName);//获取设备具体配置信息文件路径
    static QString getDefaultDeviceGroupName();//获取默认设备组名
    static QList<DeviceInfoData> getDeviceListInfo();//获取设备列表信息
    static void writeValue(QString filePath,QString groupName,QString keyName,QString keyValue);//设置配置文件中指定的组名的键值
    static QString readValue(QString filePath,QString groupName,QString keyName);//读取配置文件中指定的节点键值

};

#endif // DEVICEINFOHELPER_H
