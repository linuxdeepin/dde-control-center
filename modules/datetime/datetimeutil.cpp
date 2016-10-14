#include "datetimeutil.h"
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
    QSqlDatabase db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"), "timezone");
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
    return timezones;
}
