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

    // 两个时区之间相差几个小时
    static float hoursBetweenTwoTimeZone(const QTimeZone& tz, const QTimeZone& cur = QTimeZone());
    static float hoursBetweenTwoTimeZone(const QString& tz, const QString& cur = QString());
};
}
}
#endif // DATETIMEUTIL_H
