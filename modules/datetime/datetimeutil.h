#ifndef DATETIMEUTIL_H
#define DATETIMEUTIL_H

#include <QString>
#include <QTimeZone>

class DatetimeUtil
{
public:
    DatetimeUtil();

    QStringList city2UTC(const QString& city);

    // 某年某月有多少天
    int dayOfMonth(int year, int month);

    // 两个时区之间相差几个小时
    float hoursBetweenTwoTimeZone(const QTimeZone& tz, const QTimeZone& cur = QTimeZone());
    float hoursBetweenTwoTimeZone(const QString& tz, const QString& cur = QString());
};

#endif // DATETIMEUTIL_H
