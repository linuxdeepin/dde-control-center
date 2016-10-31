#ifndef DATETIMEUTIL_H
#define DATETIMEUTIL_H

#include <QString>
#include <QTimeZone>

static const char* timezone_database = "timezone";

struct Timezone
{
    explicit Timezone(bool valid = true);
    bool operator ==(const Timezone& tz);

    void millerTranstion(int w, int h);

    double m_lon;
    double m_lat;
    double m_x;
    double m_y;
    bool m_valid;
    QString m_city;
    QString m_timezone;
};

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
