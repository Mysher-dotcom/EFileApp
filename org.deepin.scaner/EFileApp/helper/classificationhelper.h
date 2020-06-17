#ifndef CLASSIFICATIONHELPER_H
#define CLASSIFICATIONHELPER_H

#include <QString>
#include <QSettings>
#include <QMetaType>
#include <qstring.h>
#include <QList>

//分类规则数据
typedef struct{
    QString groupName;//组名
    QString length;//条码长度
    QString values;//值
}ClassRuleData;
Q_DECLARE_METATYPE(ClassRuleData)


class ClassificationHelper
{
public:
    ClassificationHelper();
    ~ClassificationHelper();

    static QStringList captureFilePath;//拍摄保存的文件列表

    //修改文件名(当前目录)
    static QString renameFile(QString path,QString newName);

    //修改文件名
    static QString renameFile(QString oldFilePath,QString newFolder,QString newName);

    //获取分类规则配置文件路径
    static QString getClassificationFilePath();

    //写分类规则配置文件,默认值
    static void writeClassficationFileDefault();

    //获取分类规则信息
    static QList<ClassRuleData> getClassRuleInfo();

    //根据条码信息创建文件夹
    static QString createFolderByCodeInfo(QString rootFolder,QStringList codeInfo);

};

#endif // CLASSIFICATIONHELPER_H
