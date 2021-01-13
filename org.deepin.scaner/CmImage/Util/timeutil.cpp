#include "timeutil.h"
#include <QString>

#include <QDate>

TimeUtil::TimeUtil()
{

}


//获取当前时间，按yy-dd-mm格式
QString TimeUtil::getCurrentDate()
{
    QString strBuffer;
    QDateTime time;
    time = QDateTime::currentDateTime();
    //strBuffer = time.toString("yyyy-MM-dd hh:mm:ss");
    strBuffer = time.toString("yyyy-MM-dd");
    return  strBuffer;
}

//从文本内容创建对象，按yy-dd-mm格式
QDateTime TimeUtil::createDateFromContent(QString content)
{
    QString strDateTime = content;
    QDateTime time;
    time = QDateTime::fromString(content, "yyyy-MM-dd");
    return time;
}

//计算day1-day2时间差，day1/day2按yy-dd-mm格式,时间差按天计算
int TimeUtil::getDayDiff(QString day1, QString day2)
{
    QDateTime date1 = TimeUtil::createDateFromContent(day1);
    QDateTime date2 = TimeUtil::createDateFromContent(day2);

    int diff = date2.daysTo(date1);
    return diff;
}
