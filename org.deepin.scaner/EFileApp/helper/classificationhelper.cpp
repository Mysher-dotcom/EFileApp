#include "classificationhelper.h"
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDir>
#include <QTextCodec>
#include "globalhelper.h"

QStringList ClassificationHelper::captureFilePath;//拍摄保存的文件列表

ClassificationHelper::ClassificationHelper()
{

}

ClassificationHelper::~ClassificationHelper()
{

}

//修改文件名(当前目录)
QString ClassificationHelper::renameFile(QString path,QString newName)
{
    QFileInfo fileinfo;
    fileinfo = QFileInfo(path);
    QString folderPath = fileinfo.path();//文件夹路径
    QString imgSuffix = fileinfo.suffix(); //文件后缀

    QString newPath = folderPath +"/"+ newName+"."+imgSuffix;
    QFile file;
    bool bResult = file.rename(path,newPath);
    if(bResult)
    {
        return newPath;
    }
    return path;
}

//修改文件名
QString ClassificationHelper::renameFile(QString oldFilePath,QString newFolder,QString newName)
{
    QString newNameTmp = newName;
    if(newName.isEmpty())
    {
        newNameTmp = "未知";
    }
    QFileInfo fileinfo;
    fileinfo = QFileInfo(oldFilePath);
    QString imgSuffix = fileinfo.suffix(); //文件后缀
    QString newPath = newFolder +"/"+ newNameTmp+"."+imgSuffix;

    int imgIndex = 1;
    while(QFile::exists(newPath))
    {
        QString strImgIndex = QString("%1").arg(imgIndex,5,10,QLatin1Char('0'));//编号补齐5位，如：00001
        newNameTmp = newName + "(" + strImgIndex + ")";
        newPath = newFolder +"/"+ newNameTmp+"."+imgSuffix;
        imgIndex++;
    }


    QFile file;
    bool bResult = file.copy(oldFilePath,newPath);
    if(bResult)
    {
        return newPath;
    }
    return oldFilePath;
}

//获取分类规则配置文件路径
QString ClassificationHelper::getClassificationFilePath()
{
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString docPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString appPath = QCoreApplication::applicationDirPath();

    QString tmpFolder = docPath + "/EFile_config/classification";
    QDir newFolder;
    if(newFolder.exists(tmpFolder) == false)
    {
        newFolder.mkpath(tmpFolder);
    }
    return tmpFolder + "/rule.ini";
}

//写分类规则配置文件,默认值
void ClassificationHelper::writeClassficationFileDefault()
{
    QSettings * psettings = new QSettings (getClassificationFilePath(),QSettings::IniFormat);
    psettings->setIniCodec(QTextCodec::codecForName("GB2312"));

    //规则1,5-12位条码
    psettings->beginGroup("Group1");
    psettings->setValue("groupName","姓名");
    psettings->setValue("length","4-12");
    psettings->setValue("values","1001:张三;1002:李四;1004:苏佳俊;1005:李腾;1006:黄维发");
    psettings->endGroup();

    //规则2,1-4位条码
    psettings->beginGroup("Group2");
    psettings->setValue("groupName","档案类型");
    psettings->setValue("length","0-3");
    psettings->setValue("values","1018:干部履历表;1019:中共党员需有自传材料;1020:参加工作以来的年度考核表;1022:高校招生考生报名登记表;1023:高校学生登记表;"
                                 "1024:高校毕业生登记表;1048:具备干部履历表所记载的对应培训材料;1049:入党政审材料;1050	:入团志愿书;1051:入党志愿书;1052:入党申请书;"
                                 "1053:转正申请书;1054:中国共产党党员登记表;1055:民主党派成员入党材料;1056:具备最新任免表所记载奖励材料;1057:处分通知材料;1059:外单位工作期间工资材料;"
                                 "1060:本校工作期间工资材料;1058:转正定级材料;1061:干部任免审批表;1064:毕业生就业通知书");
    psettings->endGroup();

    delete  psettings;
}

//获取分类规则信息
QList<ClassRuleData> ClassificationHelper::getClassRuleInfo()
{
    QList<ClassRuleData> list ;

    QSettings * psettings = new QSettings (getClassificationFilePath(),QSettings::IniFormat);
    psettings->setIniCodec(QTextCodec::codecForName("GB2312"));
    QStringList allGroup = psettings->childGroups(); //从配置文件中读全部的Group
    for(int i=0;i<allGroup.length();i++)
    {
        psettings->beginGroup(allGroup[i]);
        QString p1 = psettings->value("length").toString();
        QString p2 = psettings->value("values").toString();
        ClassRuleData crd;
        crd.groupName = psettings->value("groupName").toString();
        crd.length = psettings->value("length").toString();
        crd.values = psettings->value("values").toString();
        list.append(crd);
        psettings->endGroup();
    }
    delete  psettings;

    return list;
}

//根据条码信息创建文件夹
QString ClassificationHelper::createFolderByCodeInfo(QString rootFolder,QStringList codeInfo)
{
    QString folderPath = rootFolder + "/bacode";
    if(codeInfo.length() <= 0)
    {
        folderPath += "/未识别";
        QDir newFolder;
        if(newFolder.exists(folderPath) == false)
        {
            newFolder.mkpath(folderPath);
        }
    }

    for(int i=0;i<codeInfo.length();i++)
    {
        folderPath += "/" + codeInfo.at(i);
        QDir newFolder;
        if(newFolder.exists(folderPath) == false)
        {
            newFolder.mkpath(folderPath);
        }
    }
    return folderPath;
}

