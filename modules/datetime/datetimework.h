#ifndef DATETIMEWORK_H
#define DATETIMEWORK_H

#include "datetimemodel.h"

#include <QObject>
#include <com_deepin_daemon_timedate.h>

namespace dcc {
namespace datetime {

using Timedate = com::deepin::daemon::Timedate;

class DatetimeWork : public QObject
{
    Q_OBJECT
public:
    explicit DatetimeWork(DatetimeModel* model, QObject *parent = 0);

    void activate();
    void deactivate();

public slots:
    void setTimezone(const QString& timezone);
    void setDatetime(const QDateTime &time);
    void setNTP(bool ntp);
    void removeUserTimeZone(const ZoneInfo &info);

private slots:
    void onTimezoneListChanged(const QStringList &timezones);

private:
    DatetimeModel* m_model;
    Timedate* m_timedateInter;
};
}
}
#endif // DATETIMEWORK_H
