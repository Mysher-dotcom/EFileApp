#ifndef WEBHELPER_H
#define WEBHELPER_H

#include <QString>

class WebHelper
{
public:
    WebHelper();

    static void writeWebDefault();//写默认网络配置文件
    static QString getUploadAddress();//获取上传接口地址
    static QString getToken();//获取Token授权码

};

#endif // WEBHELPER_H
