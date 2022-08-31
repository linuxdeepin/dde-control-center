// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DATETIMEWORK_H
#define DATETIMEWORK_H

#include "datetimemodel.h"

#include <com_deepin_daemon_timedate.h>
#include <com_deepin_daemon_timedated.h>

#include <QObject>

namespace dcc {
namespace datetime {

using Timedate = com::deepin::daemon::Timedate;
using Timedated = com::deepin::daemon::Timedated;

class DatetimeWork : public QObject
{
    Q_OBJECT
public:
    explicit DatetimeWork(DatetimeModel *model, QObject *parent = nullptr);
    ~DatetimeWork();

    void activate();
    void deactivate();
    Timedate *getTimedate();
    static DatetimeWork &getInstance();
    DatetimeModel *model() { return m_model; }
    void setFormat(QString property, QString value);

Q_SIGNALS:
    void requestSetAutoHide(const bool visible) const;

public Q_SLOTS:
    void seteFormatSlot(QDBusMessage msg);
    void setNTP(bool ntp);
    void setDatetime(const QDateTime &time);
    void set24HourType(bool state);
#ifndef DCC_DISABLE_TIMEZONE
    void setTimezone(const QString &timezone);
    void removeUserTimeZone(const ZoneInfo &info);
    void addUserTimeZone(const QString &zone);
#endif
    void setNtpServer(QString server);

    void setWeekdayFormat(int type);
    void setShortDateFormat(int type);
    void setLongDateFormat(int type);
    void setLongTimeFormat(int type);
    void setShortTimeFormat(int type);
    void setWeekStartDayFormat(int type);
private Q_SLOTS:
#ifndef DCC_DISABLE_TIMEZONE
    void onTimezoneListChanged(const QStringList &timezones);
#endif
private:
    void refreshNtpServerList();
    ZoneInfo GetZoneInfo(const QString &zoneId);

private:
    DatetimeModel *m_model;
    Timedate *m_timedateInter;
    Timedated *m_systemtimedatedInter;
    QStringList m_formatList;
};
}
}
#endif // DATETIMEWORK_H
