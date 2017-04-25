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
    ~DatetimeWork();

    void activate();
    void deactivate();

public slots:
    void setNTP(bool ntp);
    void setDatetime(const QDateTime &time);
#ifndef DCC_DISABLE_TIMEZONE
    void setTimezone(const QString& timezone);
    void removeUserTimeZone(const ZoneInfo &info);
    void addUserTimeZone(const QString &zone);
#endif

private slots:
#ifndef DCC_DISABLE_TIMEZONE
    void onTimezoneListChanged(const QStringList &timezones);
#endif

private:
    DatetimeModel* m_model;
    Timedate* m_timedateInter;
};
}
}
#endif // DATETIMEWORK_H
