#ifndef PATHUTIL_H
#define PATHUTIL_H

class QString;

class PathUtil
{
public:
    PathUtil();

    /**获取系统桌面路径
     * @return: desktopPath
     */
    static QString getDesktopPath();

    /**获取的执行文件exe所在的路径
     * @return: applicationDirPath
     */
    static QString getApplicationDirPath();
};

#endif // PATHUTIL_H
