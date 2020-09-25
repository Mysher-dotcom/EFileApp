#ifndef GLOBALHELPER_H
#define GLOBALHELPER_H
#include <QString>
#include <QSettings>
#include <QMap>

class GlobalHelper
{
public:

   GlobalHelper();
   ~GlobalHelper();


   static bool getDeviceInfoIsOver;//获取设备信息是否结束
   static QString softVersion;//版本号


   //获取扫描存放文件夹路径
   static QString getScanFolder();

   //获取空的扫描临时存放路径
   static QString getScanTempFoler();

   //清空文件夹
   static bool deleteDirectory(const QString &path);

   //获取配置文件全路径
   static QString getSettingFilePath();

   static void checkVersion();//匹配版本号，用于是否删除配置文件

   //写配置文件,初始值
   static void writeSettingFile();

   /*
    * 读取配置文件中指定的节点键值
    * groupName=组名
    * keyName=键名
    */
   static QString readSettingValue(QString groupName,QString keyName);

   /*
    * 设置配置文件中指定的组名的键值
    * groupName=组名
    * keyName=键名
    * keyValue=键值
    */
   static void writeSettingValue(QString groupName,QString keyName,QString keyValue);

   /*
    * 设置Widget背景色
    */
   static void setWidgetBackgroundColor(QWidget *widget,QColor color,bool isFixedHeight=true);

   //播放声音
   static void playSound(QString soundPath);

   //图片名超长，用...
   static QString setShowString(QString str,int limitWidth);

};

#endif // GLOBALHELPER_H
