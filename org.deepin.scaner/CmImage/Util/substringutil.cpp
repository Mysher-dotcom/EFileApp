#include "substringutil.h"

#include <QString>

SubStringUtil::SubStringUtil()
{

}


//获取子字符串
QString SubStringUtil::getSbuString(QString content, QString start, QString end, int firstIndex, int & findIndex)
{
    QString sub = "";

    int startIndex = content.indexOf(start,firstIndex);
    if(startIndex == -1)
    {
        return  sub;
    }

    int endIndex = content.indexOf(end,startIndex+start.size());
    if(endIndex == -1)
    {
        return  sub;
    }

    sub = content.mid(startIndex+1,endIndex);
    findIndex = endIndex;


    return sub;
}
