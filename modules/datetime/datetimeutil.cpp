#include "datetimeutil.h"
#include <cmath>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QTimeZone>
#include <QDebug>

DatetimeUtil::DatetimeUtil()
{

}

QStringList DatetimeUtil::city2UTC(const QString &city)
{
    QSqlDatabase db;
    if(QSqlDatabase::contains("timezone"))
    {
        db = QSqlDatabase::database(QLatin1String("timezone"));
    }
    else
    {
        db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), "timezone");
    }

    db.setDatabaseName(CITIES_DATABASE_PATH);

    if (!db.open())
    {
        qWarning() << QString("Failed to open %1 database").arg(db.databaseName());
        qWarning() << db.lastError();
        return QStringList();
    }

    QSqlQuery query(db);

    QString schema = QString("SELECT timezone from tzcity where name='%1'").arg(city);

    if(!query.exec(schema))
    {
        qDebug()<<query.lastError();
        return QStringList();
    }

    QStringList timezones;
    while(query.next())
    {
        QSqlRecord record = query.record();
        for(int i = 0; i<record.count(); i++)
        {
            QSqlField field = record.field(i);
            if(field.name() == "timezone")
            {
                QTimeZone tz(field.value().toByteArray());
                timezones<<tz.displayName(QTimeZone::StandardTime, QTimeZone::OffsetName);
            }
        }
    }

    db.close();
    return timezones;
}

int DatetimeUtil::dayOfMonth(int year, int month)
{
    QDate date(year, month, 1);
    return date.daysInMonth();
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
