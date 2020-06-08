#include "globalhelper.h"
#include <QString>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDebug>
#include <QTextCodec>
#include <QFile>
#include <QDir>
#include <QWidget>
#include <QColor>
#include <QPalette>
#include <QtMultimedia/QSound>

GlobalHelper::GlobalHelper()
{

}

GlobalHelper::~GlobalHelper()
{
}

//获取设备信息是否结束
bool GlobalHelper::getDeviceInfoIsOver;

//获取扫描存放文件夹路径
QString GlobalHelper::getScanFolder()
{
    return readSettingValue("set","savepath");
}

//获取空的扫描临时存放路径
QString GlobalHelper::getScanTempFoler()
{
    QString tmpFolder = "/tmp/viisanTmp/";
    QDir newFolder;
    if(newFolder.exists(tmpFolder) == false)
    {
        newFolder.mkpath(tmpFolder);
    }
    return tmpFolder;
}

//清空文件夹
bool GlobalHelper::deleteDirectory(const QString &path)
{
    if (path.isEmpty())
        return false;

    QDir dir(path);
    if(!dir.exists())
        return true;

    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    foreach (QFileInfo fi, fileList)
    {
        if (fi.isFile())
            fi.dir().remove(fi.fileName());
        else
            deleteDirectory(fi.absoluteFilePath());
    }
    return dir.rmpath(dir.absolutePath());
}

//获取设置文件全路径
QString GlobalHelper::getSettingFilePath()
{
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString docPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString appPath = QCoreApplication::applicationDirPath();
    return appPath + "/setting.ini";
}

//写配置文件,初始值
void GlobalHelper::writeSettingFile()
{
    QString mode,Color24,Gray8,Lineart;
    QString source,ADF_FRONT , ADF_DUPLEX, FLAT_FRONT;
    QString resolution;

    QSettings * psettings = new QSettings (getSettingFilePath(),QSettings::IniFormat);
    psettings->setIniCodec(QTextCodec::codecForName("GB2312"));
    //语言
    psettings->beginGroup("lan");
    psettings->setValue("mode","色彩模式");
    psettings->setValue("Color24","彩色图");
    psettings->setValue("Gray8","灰度图");
    psettings->setValue("Lineart","黑白图");
    psettings->setValue("source","扫描模式");
    psettings->setValue("ADF_FRONT","ADF正面");
    psettings->setValue("ADF_DUPLEX","ADF双面");
    psettings->setValue("FLAT_FRONT","平板");
    psettings->setValue("resolution","分辨率");
    psettings->setValue("format","视频格式");
    //psettings->setValue("format_c","视频格式");
    //psettings->setValue("resolution_c","分辨率");
    psettings->endGroup();

    //是否显示,1=true 0=false
    psettings->beginGroup("show");
    psettings->setValue("mode","1");
    psettings->setValue("source","1");
    psettings->setValue("resolution","1");
    //psettings->setValue("preview","0");
    //psettings->setValue("threshold","0");
    //psettings->setValue("gamma-value","0");
    //psettings->setValue("tl-x","0");
    //psettings->setValue("tl-y","0");
    //psettings->setValue("br-x","0");
    //psettings->setValue("br-y","0");
    psettings->endGroup();

    //设置
    psettings->beginGroup("set");
    psettings->setValue("savepath","/tmp/viisan/");//存储路径
    psettings->setValue("fileover","0");//文件覆盖警告开关
    psettings->setValue("nodevice","1");//无设备警告开关
    psettings->setValue("finishbeep","0");//完成后提示音开关
    psettings->setValue("finishexit","1");//完成后关闭开关
    psettings->setValue("iconList","0");//缩略图模式
    psettings->setValue("dzda","0");//电子档案显示模式
    psettings->setValue("classification","1");//是否分类显示
    psettings->endGroup();


    delete  psettings;
   //QString preview,threshold,gamma-value,tl-x,tl-y,br-x,br-y;

}

/*
 * 读取配置文件中指定的节点键值
 * groupName=组名
 * keyName=键名
 */
QString GlobalHelper::readSettingValue(QString groupName,QString keyName)
{
    QSettings setting(getSettingFilePath(),QSettings::IniFormat);
    setting.setIniCodec(QTextCodec::codecForName("GB2312"));
    //QString mode = setting.value("lan/mode").toString();
    QString qvar = setting.value(QString("%1/%2").arg(groupName).arg(keyName)).toString();
    return qvar;
}

/*
 * 设置配置文件中指定的组名的键值
 * groupName=组名
 * keyName=键名
 * keyValue=键值
 */
void GlobalHelper::writeSettingValue(QString groupName, QString keyName, QString keyValue)
{
    QSettings setting(getSettingFilePath(),QSettings::IniFormat);
    setting.setIniCodec(QTextCodec::codecForName("GB2312"));
    setting.beginGroup(groupName);
    setting.setValue(keyName,keyValue);
}


/*
 * 设置Widget背景色
 */
void GlobalHelper::setWidgetBackgroundColor(QWidget *parwidget,QColor color)
{
    parwidget->setFixedHeight(48);
    //参数行容器设置背景
    parwidget->setGeometry(0, 0, 300, 100);
    QPalette pal(parwidget->palette());
    pal.setColor(QPalette::Background, color);
    parwidget->setAutoFillBackground(true);
    parwidget->setPalette(pal);
}

//播放声音
void GlobalHelper::playSound(QString soundPath)
{
    int nIsSound= GlobalHelper::readSettingValue("set","finishbeep").toInt();
    if(nIsSound == 0)
    {
        QSound::play(":/sound/finishSound.wav");
    }
}

//图片名超长，用...
QString GlobalHelper::setShowString(QString str,int limitWidth)
{
    //获得字符串所占的像素宽度
    QFontMetrics fm(str);
    int width=fm.width(str);
    if(width > limitWidth)
    {
        QString pro = str.mid(0,10);
        QString end = str.mid(str.length()-5,5);
        str = pro + "..." ;//+ end;
    }
    return str;

}
