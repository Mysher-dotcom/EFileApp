#include "deviceinfohelper.h"
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDir>
#include <QTextCodec>

DeviceInfoHelper::DeviceInfoHelper()
{

}

//获取设备列表信息配置文件路径
QString DeviceInfoHelper::getDeviceListInfoFilePath()
{
    QString docPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString tmpFolder = docPath + "/EFile_config/device";
    QDir newFolder;
    if(newFolder.exists(tmpFolder) == false)
    {
        newFolder.mkpath(tmpFolder);
    }
    return tmpFolder + "/device.ini";
}

//获取设备具体配置信息文件路径
QString DeviceInfoHelper::getDeviceInfoFilePath(QString fileName)
{
    QString docPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString tmpFolder = docPath + "/EFile_config/device/info";
    QDir newFolder;
    if(newFolder.exists(tmpFolder) == false)
    {
        newFolder.mkpath(tmpFolder);
    }
    return tmpFolder + "/"+fileName+".ini";
}

//获取默认设备组名
QString DeviceInfoHelper::getDefaultDeviceGroupName()
{
    QSettings setting(getDeviceListInfoFilePath(),QSettings::IniFormat);
    setting.setIniCodec(QTextCodec::codecForName("GB2312"));
    QString qvar = setting.value(QString("%1/%2").arg("default").arg("name")).toString();
    return qvar;
}

//获取设备列表信息
QList<DeviceInfoData> DeviceInfoHelper::getDeviceListInfo()
{
    QList<DeviceInfoData> list;

    QSettings * psettings = new QSettings (getDeviceListInfoFilePath(),QSettings::IniFormat);
    psettings->setIniCodec(QTextCodec::codecForName("GB2312"));
    QStringList allGroup = psettings->childGroups(); //从配置文件中读全部的Group
    for(int i=0;i<allGroup.length();i++)
    {
        QString groupName = allGroup[i];
        if(groupName == "default")
        {
            continue;
        }
        psettings->beginGroup(allGroup[i]);
        DeviceInfoData devInfo;
        devInfo.type = psettings->value("type").toInt();//设备类型，0=扫描仪，1=拍摄仪
        devInfo.name = psettings->value("name").toString();//设备名
        devInfo.model = psettings->value("model").toString();//设备类型
        devInfo.status = psettings->value("status").toInt();//设备状态，0=空闲，1=占用，2=未连接
        devInfo.config = psettings->value("config").toString();//设备具体配置信息文件路径
        devInfo.index = psettings->value("index").toInt();//设备具体配置信息文件路径
        devInfo.isLicense = psettings->value("license").toInt();//是否有授权（0=普通设备，1=授权设备，2=设备没有usb权限）
        list.append(devInfo);
        psettings->endGroup();
    }
    delete  psettings;

    return list;
}

//设置配置文件中指定的组名的键值
void DeviceInfoHelper::writeValue(QString filePath,QString groupName,QString keyName,QString keyValue)
{
    QSettings setting(filePath,QSettings::IniFormat);
    setting.setIniCodec(QTextCodec::codecForName("GB2312"));
    setting.beginGroup(groupName);
    setting.setValue(keyName,keyValue);
}

//读取配置文件中指定的节点键值
QString DeviceInfoHelper::readValue(QString filePath,QString groupName,QString keyName)
{
    QSettings setting(filePath,QSettings::IniFormat);
    setting.setIniCodec(QTextCodec::codecForName("GB2312"));
    QString qvar = setting.value(QString("%1/%2").arg(groupName).arg(keyName)).toString();
    return qvar;
}
