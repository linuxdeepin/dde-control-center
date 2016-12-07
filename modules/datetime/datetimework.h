#ifndef DATETIMEWORK_H
#define DATETIMEWORK_H

#include "datetimemodel.h"

#include <QObject>
#include <com_deepin_daemon_timedate.h>

using Timedate = com::deepin::daemon::Timedate;

class DatetimeWork : public QObject
{
    Q_OBJECT
public:
    explicit DatetimeWork(DatetimeModel* model, QObject *parent = 0);

    void setTimezone(const QString& timezone);

signals:
    void NTPChanged(bool  value) const;

public slots:
    void setDatetime(int year, int month, int day, int hour, int minute);
    void setNTP(bool ntp);
    void onProperty(const QString &propName, const QVariant &value);

private:
    DatetimeModel* m_model;
    Timedate* m_timedateInter;
};

#endif // DATETIMEWORK_H
