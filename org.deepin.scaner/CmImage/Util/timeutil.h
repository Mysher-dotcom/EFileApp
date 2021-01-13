#ifndef TIMEUTIL_H
#define TIMEUTIL_H

class QString;
class QDateTime;

class TimeUtil
{
public:
    TimeUtil();

    //获取当前时间，按yy-dd-mm格式
    static QString getCurrentDate();

    //从文本内容创建对象，按yy-dd-mm格式
    static QDateTime createDateFromContent(QString content);

    //计算day1-day2时间差，day1/day2按yy-dd-mm格式,时间差按天计算
    static int getDayDiff(QString day1, QString day2);




};

#endif // TIMEUTIL_H
