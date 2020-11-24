#ifndef SUBSTRINGUTIL_H
#define SUBSTRINGUTIL_H

class QString;

class SubStringUtil
{
public:
    SubStringUtil();

    //获取子字符串
    static QString getSbuString(QString content, QString start, QString end, int firstIndex, int & findIndex);
};

#endif // SUBSTRINGUTIL_H
