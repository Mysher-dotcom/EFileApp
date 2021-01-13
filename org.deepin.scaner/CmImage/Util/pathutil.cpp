#include "pathutil.h"

#include <QString>
#include <QStandardPaths>
#include <QDebug>
#include <QApplication>

PathUtil::PathUtil()
{

}

/**获取系统桌面路径
 * @return: desktopPath
 */
QString PathUtil::getDesktopPath(){

    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    qDebug()<< "desktopPath:" << desktopPath;
    return desktopPath;

}

/**获取的执行文件exe所在的路径
 * @return: applicationDirPath
 */
QString PathUtil::getApplicationDirPath(){

    // QString QApplication::applicationDirPath()
    QString applicationDirPath = QApplication::applicationDirPath();
    qDebug()<< "applicationDirPath:" << applicationDirPath;
    return applicationDirPath;

}
