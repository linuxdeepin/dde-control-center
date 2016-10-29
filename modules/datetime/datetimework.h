#ifndef DATETIMEWORK_H
#define DATETIMEWORK_H

#include <QObject>
#include <com_deepin_daemon_timedate.h>

using Timedate = com::deepin::daemon::Timedate;

class DatetimeWork : public QObject
{
    Q_OBJECT
public:
    explicit DatetimeWork(QObject *parent = 0);

    bool nTP() const;
signals:
    void NTPChanged(bool  value) const;

public slots:
    void setDatetime(int year, int month, int day, int hour, int minute);
    void setNTP(bool ntp);
    void onProperty(const QString &propName, const QVariant &value);

private:
    Timedate* m_timedateInter;
};

#endif // DATETIMEWORK_H
