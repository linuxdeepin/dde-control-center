#include "datetimeutil.h"
#include <cmath>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QTimeZone>
#include <QDebug>

Timezone::Timezone(bool valid)
    :m_valid(valid)
{

}

bool Timezone::operator ==(const Timezone &tz)
{
    return (this->m_city == tz.m_city);
}

void Timezone::millerTranstion(int w, int h)
{
    double PI = 3.141592653589793;
    double y = m_lon / 180;
    double rlat = m_lat * PI / 180;
    double x = 1.25 * log( tan( 0.25 * PI + 0.4 * rlat ) );
    x = x / 2.3034125433763912;

    m_x =  w/2*(y + 1);
    m_y =  h/2*(1 - x);
}

DatetimeUtil::DatetimeUtil()
{

}

QStringList DatetimeUtil::city2UTC(const QString &city)
{
    QSqlDatabase db;
    if(QSqlDatabase::contains(timezone_database))
    {
        db = QSqlDatabase::database(QLatin1String(timezone_database));
    }
    else
    {
        db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), timezone_database);
    }

    db.setDatabaseName(CITIES_DATABASE_PATH + QString(timezone_database) + ".db");

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
