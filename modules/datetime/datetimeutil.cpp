#include "datetimeutil.h"
#include <cmath>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QTimeZone>
#include <QDebug>


namespace dcc {
namespace datetime {

DatetimeUtil::DatetimeUtil()
{

}

float DatetimeUtil::hoursBetweenTwoTimeZone(const QTimeZone &tz, const QTimeZone &cur)
{
    QDateTime dt = QDateTime::currentDateTime().toTimeZone(tz);
    QDateTime curDt = QDateTime::currentDateTime();

    QDateTime sys = cur.isValid() ? curDt.toTimeZone(cur) : curDt;

    float utc1 = dt.offsetFromUtc()/3600.0;
    float utc2 = sys.offsetFromUtc()/3600.0;

    return (utc1 - utc2);
}

float DatetimeUtil::hoursBetweenTwoTimeZone(const QString &tz, const QString &cur)
{
    QTimeZone one = QTimeZone(tz.toStdString().c_str());
    if(cur.isNull())
    {
        return hoursBetweenTwoTimeZone(one);
    }
    else
    {
        QTimeZone two = QTimeZone(cur.toStdString().c_str());
        return hoursBetweenTwoTimeZone(one, two);
    }
}
}
}
