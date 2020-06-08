#include "webhelper.h"
#include <QSettings>
#include <QDir>
#include <QCoreApplication>
#include <QTextCodec>

WebHelper::WebHelper()
{

}

//写默认网络配置文件
void WebHelper::writeWebDefault()
{
    QString webFilePath = QCoreApplication::applicationDirPath()+ "/web.ini";

    QFileInfo settingFile(webFilePath);
    if(settingFile.exists()) return;

    QSettings * psettings = new QSettings (webFilePath,QSettings::IniFormat);
    psettings->setIniCodec(QTextCodec::codecForName("GB2312"));

    psettings->beginGroup("set");
    psettings->setValue("ip","39.97.163.109");
    psettings->setValue("port","8080");
    psettings->setValue("webname","");
    psettings->setValue("token","360");
    psettings->setValue("upload","/fupload/client");

    psettings->endGroup();

    delete  psettings;
}

//获取上传接口地址
QString WebHelper::getUploadAddress()
{
    QString webFilePath = QCoreApplication::applicationDirPath()+ "/web.ini";
    QSettings setting(webFilePath,QSettings::IniFormat);
    setting.setIniCodec(QTextCodec::codecForName("GB2312"));
    QString ip = setting.value(QString("%1/%2").arg("set").arg("ip")).toString();
    QString port = setting.value(QString("%1/%2").arg("set").arg("port")).toString();
    QString webname = setting.value(QString("%1/%2").arg("set").arg("webname")).toString();
    QString upload = setting.value(QString("%1/%2").arg("set").arg("upload")).toString();
    return "http://" + ip + ":" + port + "/" + webname + upload;
}

//获取Token授权码
QString WebHelper::getToken()
{
    QString webFilePath = QCoreApplication::applicationDirPath()+ "/web.ini";
    QSettings setting(webFilePath,QSettings::IniFormat);
    setting.setIniCodec(QTextCodec::codecForName("GB2312"));
    return setting.value(QString("%1/%2").arg("set").arg("token")).toString();
}
