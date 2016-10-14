#ifndef DATETIMEUTIL_H
#define DATETIMEUTIL_H

#include <QString>

class DatetimeUtil
{
public:
    DatetimeUtil();

    QStringList city2UTC(const QString& city);
};

#endif // DATETIMEUTIL_H
