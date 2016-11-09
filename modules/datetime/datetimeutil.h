#ifndef DATETIMEUTIL_H
#define DATETIMEUTIL_H

#include <QString>
#include <QTimeZone>

static const char *const timezone_database = "timezone";

struct Timezone
{
    explicit Timezone(bool valid = true);
    bool operator ==(const Timezone& tz) const;

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

    static const QStringList city2UTC(const QString& city);

    // 某年某月有多少天
    static int dayOfMonth(int year, int month);

    // 两个时区之间相差几个小时
    static float hoursBetweenTwoTimeZone(const QTimeZone& tz, const QTimeZone& cur = QTimeZone());
    static float hoursBetweenTwoTimeZone(const QString& tz, const QString& cur = QString());
};

#endif // DATETIMEUTIL_H
