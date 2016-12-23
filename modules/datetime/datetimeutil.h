#ifndef DATETIMEUTIL_H
#define DATETIMEUTIL_H

#include <QString>
#include <QTimeZone>

namespace dcc {
namespace datetime {

static const char *const timezone_database = "timezone";

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
}
}
#endif // DATETIMEUTIL_H
